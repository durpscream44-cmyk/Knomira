#!/usr/bin/env python3
"""Opt-in single-host Knomira worker. Dry-run by default; no automatic merges."""
import argparse
import datetime as dt
import fnmatch
import os
from pathlib import Path
import platform
import re
import shutil
import stat
import subprocess
import urllib.error
from agent_queue import api, add_label, comment, eligible, issues, metadata, profiles, remove_label

TIME_BUDGET_SECONDS = 900
CHILD_ENV_ALLOWLIST = {
    "comspec", "path", "pathext", "systemdrive", "systemroot", "temp", "tmp", "windir"
}


def git(where, *args):
    return subprocess.check_output(["git", *args], cwd=where, text=True,
                                   stderr=subprocess.PIPE).strip()


def available():
    caps = {"git", "github"}
    if platform.system() == "Windows":
        caps.add("windows")
    if shutil.which("cmake"):
        caps.add("cmake")
    qt = os.getenv("Qt6_DIR") or os.getenv("QT_ROOT_DIR")
    if qt and Path(qt).is_dir():
        caps.add("qt")
    # Native assets and desktop GUI tests are NEVER automatically certified.
    return caps


def _inside(path, parent):
    try:
        path.relative_to(parent)
        return True
    except ValueError:
        return False


def _is_reparse_point(path):
    try:
        if path.is_symlink() or (hasattr(path, "is_junction") and path.is_junction()):
            return True
        attributes = getattr(path.lstat(), "st_file_attributes", 0)
        reparse_flag = getattr(stat, "FILE_ATTRIBUTE_REPARSE_POINT", 0x400)
        return bool(attributes & reparse_flag)
    except FileNotFoundError:
        return False
    except OSError:
        # An unreadable path cannot be safely classified as an ordinary file.
        return True


def _reparse_in_path(root, parts):
    current = root
    for part in parts:
        current = current / part
        if _is_reparse_point(current):
            return True
    return False


def _matches_protected(path, protected):
    low = path.replace("\\", "/").casefold()
    return any(fnmatch.fnmatch(low, pattern.casefold()) or
               (pattern.endswith("/") and low.startswith(pattern.casefold()))
               for pattern in protected)


def permitted(path, allowed, protected, root=None):
    """Check a relative path against the task allowlist and protected paths.

    When root is supplied, also resolve existing symlinks/junctions and require
    the resolved target to remain inside both the checkout and an allowed root.
    Unknown/unreadable reparse paths fail closed.
    """
    raw = str(path).replace("\\", "/")
    if (not raw or "\x00" in raw or raw.startswith("/") or
            re.match(r"^[A-Za-z]:", raw) or "//" in raw):
        return False
    parts = raw.split("/")
    if any(part in ("", ".", "..") or part.endswith((".", " ")) or ":" in part
           for part in parts):
        return False
    if any(part.casefold() in {".env", ".ssh", ".git"} for part in parts):
        return False
    if _matches_protected(raw, protected):
        return False

    lexical_matches = []
    for entry in allowed:
        entry_raw = str(entry).replace("\\", "/")
        if not entry_raw or entry_raw.startswith("/") or re.match(r"^[A-Za-z]:", entry_raw):
            continue
        entry_parts = entry_raw.rstrip("/").split("/")
        if (not entry_parts or any(part in ("", ".", "..") or ":" in part
                                   for part in entry_parts)):
            continue
        if raw == "/".join(entry_parts):
            lexical_matches.append((entry_parts, False))
        elif entry_raw.endswith("/") and parts[:len(entry_parts)] == entry_parts:
            lexical_matches.append((entry_parts, True))
    if not lexical_matches:
        return False
    if root is None:
        return True

    try:
        root_path = Path(root).resolve(strict=True)
        target = root_path.joinpath(*parts)
        if _reparse_in_path(root_path, parts):
            return False
        resolved_target = target.resolve(strict=False)
        if not _inside(resolved_target, root_path):
            return False
        resolved_relative = resolved_target.relative_to(root_path).as_posix()
        if _matches_protected(resolved_relative, protected):
            return False
        for entry_parts, is_directory in lexical_matches:
            allowed_root = root_path.joinpath(*entry_parts)
            if _reparse_in_path(root_path, entry_parts):
                continue
            resolved_allowed = allowed_root.resolve(strict=False)
            if not _inside(resolved_allowed, root_path):
                continue
            if ((is_directory and _inside(resolved_target, resolved_allowed)) or
                    (not is_directory and resolved_target == resolved_allowed)):
                return True
    except (OSError, RuntimeError, ValueError):
        return False
    return False


def child_environment(source=None):
    """Return only process variables needed to locate native tools on Windows."""
    source = os.environ if source is None else source
    return {key: value for key, value in source.items()
            if key.casefold() in CHILD_ENV_ALLOWLIST}


def choose(repo, token, agent, cfg, root=None):
    for issue in sorted(issues(repo, token, "agent:ready"), key=lambda x: x["number"]):
        if not eligible(issue, agent, cfg):
            continue
        m = metadata(issue)
        if not set(m["caps"]).issubset(available()):
            print("Skipped issue", issue["number"], "missing runtime capability",
                  sorted(set(m["caps"]) - available()))
            continue
        if any(not permitted(x.rstrip("/") + ("/placeholder" if x.endswith("/") else ""),
                             [x], cfg["protected_patterns"], root=root) for x in m["paths"]):
            print("Skipped issue", issue["number"], "unsafe allowed path")
            continue
        try:
            api(repo, token, "/git/ref/heads/" + m["base"])
        except urllib.error.HTTPError:
            print("Skipped issue", issue["number"], "base branch missing")
            continue
        return issue, m
    return None, None


def changed_files(where, start_sha):
    tracked = subprocess.check_output(["git", "diff", "--name-only", "--no-renames",
                                       "-z", start_sha], cwd=where)
    untracked = subprocess.check_output(["git", "ls-files", "--others",
                                         "--exclude-standard", "-z"], cwd=where)
    paths = [x.decode("utf-8") for x in tracked.split(b"\x00") if x]
    paths.extend(x.decode("utf-8") for x in untracked.split(b"\x00") if x)
    return list(dict.fromkeys(paths))


def quota_failure(output):
    text = output.casefold()
    return any(marker in text for marker in (
        "quota exceeded", "resource_exhausted", "rate limit exceeded",
        "too many requests", "http 429", "status code 429"))


def run_one(repo, token, root, agent, cfg, issue, m):
    number = issue["number"]
    stamp = dt.datetime.now(dt.timezone.utc).strftime("%Y%m%d%H%M%S")
    branch = "agent/" + agent + "/issue-" + str(number) + "-" + stamp
    worktree = root.parent / ("Knomira-agent-" + agent + "-" + str(number) + "-" + stamp)
    logdir = root.parent / "Knomira-agent-run-logs"
    logdir.mkdir(exist_ok=True)
    logpath = logdir / (agent + "-" + str(number) + "-" + stamp + ".log")
    baseline = ""
    endsha = ""
    returncode = "NOT STARTED"
    paths = []
    prurl = ""
    pushed = False
    claim_started = False
    status = "BLOCKED"
    note = ""

    try:
        # Complete all Git reads/worktree setup before claiming the live issue.
        git(root, "fetch", "origin", m["base"])
        baseline = git(root, "rev-parse", "FETCH_HEAD")
        latest_issue = api(repo, token, "/issues/" + str(number))
        if not eligible(latest_issue, agent, cfg):
            raise RuntimeError("Issue is no longer eligible; no claim or worker started")
        git(root, "worktree", "add", "-b", branch, str(worktree), baseline)

        # A claim can be uncertain if the HTTP request fails after GitHub applies it.
        claim_started = True
        add_label(repo, token, number, "agent:running")
        remove_label(repo, token, number, "agent:ready")
        comment(repo, token, number,
                "Claimed by local " + agent + " runner; WIP branch " + branch +
                "; base " + m["base"] + " at " + baseline +
                ". Tests and result not yet verified.")

        prompt = (
            "Read AGENTS.md, docs/agent-coordination/profiles.json, "
            "AGENT_ROLES.md, CODEX_DELEGATION.md and the current branch checkpoint. "
            "Check current GitHub draft PRs and issue history; do not duplicate work. "
            "You must not push, merge, create PRs, or change branches yourself: "
            "the controller will validate and push allowed paths after you exit. "
            "Do not edit unapproved paths, real game saves, proprietary assets, or secrets. "
            "Keep work small and create a local WIP commit early if possible. "
            "At 50-70% of budget, leave a concise RESULT_TEMPLATE.md-style report; "
            "stop on uncertainty or quota warnings. "
            "Report commands actually run and do not claim a gameplay pass from compilation. "
            "Your time budget is 15 minutes. "
            "Base " + m["base"] + " at " + baseline + "; allowed paths: " +
            ", ".join(m["paths"]) + ". Issue #" + str(number) + ": " +
            issue["title"] + "\n" + (issue.get("body") or "")[:12000])
        cmd = cfg["agents"][agent]["command"] + [prompt]
        returncode = 1
        try:
            with logpath.open("w", encoding="utf-8", errors="replace") as output:
                try:
                    proc = subprocess.run(cmd, cwd=worktree, env=child_environment(),
                                          stdout=output, stderr=subprocess.STDOUT,
                                          timeout=TIME_BUDGET_SECONDS)
                    returncode = proc.returncode
                except subprocess.TimeoutExpired:
                    returncode = 124
                    note = "Provider timed out after 15 minutes"
                except FileNotFoundError as err:
                    returncode = 127
                    note = "Provider executable missing: " + str(err)
        except OSError as err:
            note = "Could not start or record provider process: " + str(err)

        log_text = logpath.read_text(encoding="utf-8", errors="replace") if logpath.exists() else ""
        if quota_failure(log_text):
            note = "Provider quota/rate limit detected in local log"
            raise RuntimeError(note)
        if returncode != 0:
            if not note:
                note = "Provider process exited " + str(returncode)
            raise RuntimeError(note)

        paths = changed_files(worktree, baseline)
        bad = [path for path in paths if not permitted(
            path, m["paths"], cfg["protected_patterns"], root=worktree)]
        if bad:
            raise RuntimeError("Out-of-scope paths; no push performed: " + ", ".join(bad))
        if not paths:
            raise RuntimeError("No changed files; no push or PR created")

        # Stage only the files already checked against the task allowlist.
        git(worktree, "add", "--", *paths)
        staged = git(worktree, "diff", "--cached", "--name-only").splitlines()
        if set(staged) != set(paths):
            raise RuntimeError("Staged file set changed after allowlist validation")
        git(worktree, "commit", "-m", "WIP: issue " + str(number) + " " + agent)
        endsha = git(worktree, "rev-parse", "HEAD")
        git(worktree, "push", "-u", "origin", branch)
        pushed = True
        try:
            pr = api(repo, token, "/pulls", "POST", {
                "title": "WIP issue #" + str(number) + ": " + issue["title"],
                "head": branch, "base": m["base"], "draft": True,
                "body": ("Agent task: #" + str(number) + "\nProvider: " + agent +
                         "\nBase SHA: " + baseline + "\nHead SHA: " + endsha +
                         "\nCLI exit code: 0\nTests NOT independently verified; "
                         "Codex/human review required.")})
            prurl = pr.get("html_url", "")
            if not prurl:
                raise RuntimeError("GitHub did not return a draft PR URL")
            status = "PARTIAL; awaiting review"
        except Exception as err:
            note = "Branch PUSHED at " + endsha + "; draft PR creation failed: " + str(err)
            status = "PUSHED; NO PR"
    except Exception as err:
        if not note:
            note = type(err).__name__ + ": " + str(err)
        status = "PUSHED; NO PR" if pushed else "BLOCKED; NOT PUSHED"
        if baseline and worktree.exists():
            try:
                paths = changed_files(worktree, baseline)
            except Exception as inspect_err:
                note += "; local diff inspection failed: " + str(inspect_err)
    finally:
        if claim_started:
            try:
                add_label(repo, token, number, "agent:review" if prurl else "agent:blocked")
            except Exception as err:
                note += "; final label failed: " + str(err)
            try:
                remove_label(repo, token, number, "agent:running")
            except Exception as err:
                note += "; running-label cleanup failed: " + str(err)

            pushed_text = "YES" if pushed else "NO"
            body = "\n".join([
                "Automated task report: " + status,
                "Provider: " + agent, "Issue: #" + str(number),
                "Base: " + m["base"] + (" at " + baseline if baseline else " (unavailable)"),
                "Working branch: " + branch,
                "Last local commit SHA: " + (endsha or "none"),
                "Branch pushed: " + pushed_text,
                "Draft PR: " + (prurl or "NONE"),
                "Changed files: " + (", ".join(paths) if paths else "none"),
                "CLI exit code: " + str(returncode),
                "Tests: NOT independently verified by runner; review local log and CI.",
                "Local recovery worktree: " + str(worktree),
                "Local log: " + str(logpath),
                "Reason/blockers: " + (note or "none"),
                "Next: inspect branch/diff, verify tests/Windows CI, update checkpoint.",
                "No merge, gameplay completion or save-integrity claim is implied."])
            try:
                comment(repo, token, number, body)
            except Exception as err:
                # If the handoff itself failed, leave a blocking state for recovery.
                try:
                    add_label(repo, token, number, "agent:blocked")
                    if prurl:
                        remove_label(repo, token, number, "agent:review")
                except Exception:
                    pass
                note += "; final handoff comment failed: " + str(err)

    print("Issue", number, "status:", status, "PR:", prurl or "none",
          "branch pushed:", pushed, "local recovery:", worktree)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--agent", choices=["gemini", "opencode", "kilo"], required=True)
    parser.add_argument("--once", action="store_true", help="At most one task")
    parser.add_argument("--execute", action="store_true", help="Allow actual CLI and Git writes")
    args = parser.parse_args()
    if args.execute:
        parser.error("--execute is disabled pending issue #25 security acceptance and explicit maintainer approval")
    root = Path.cwd().resolve()
    repo, token = os.getenv("GITHUB_REPOSITORY", ""), os.getenv("GITHUB_TOKEN", "")
    if not re.fullmatch(r"[A-Za-z0-9_.-]+/[A-Za-z0-9_.-]+", repo) or not token:
        parser.error("Set GITHUB_REPOSITORY and GITHUB_TOKEN")
    cfg = profiles()
    if not shutil.which(cfg["agents"][args.agent]["command"][0]):
        parser.error("Missing local agent executable")
    if git(root, "status", "--porcelain"):
        parser.error("Checkout must be clean before creating worker worktree")
    if repo.lower() not in git(root, "remote", "get-url", "origin").lower():
        parser.error("Git remote origin does not match selected repository")
    issue, m = choose(repo, token, args.agent, cfg)
    if not issue:
        print("No safe, approved, available task found for this agent")
        return
    print("Selected issue", issue["number"], issue["title"], "base", m["base"])
    if not args.execute:
        print("DRY RUN. Nothing claimed, run, committed or pushed.")
        return
    if issues(repo, token, "agent:running"):
        parser.error("Another agent issue is running; do not race")
    common = Path(git(root, "rev-parse", "--git-common-dir"))
    if not common.is_absolute():
        common = root / common
    lock = common / "knomira-agent-runner.lock"
    try:
        fd = os.open(lock, os.O_CREAT | os.O_EXCL | os.O_WRONLY, 0o600)
    except FileExistsError:
        parser.error("Local worker lock exists; inspect stale run before retrying")
    try:
        os.close(fd)
        run_one(repo, token, root, args.agent, cfg, issue, m)
    finally:
        lock.unlink(missing_ok=True)


if __name__ == "__main__":
    main()
