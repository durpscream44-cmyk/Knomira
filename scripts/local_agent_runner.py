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
import subprocess
import urllib.error
from agent_queue import api, add_label, audit_event, comment, eligible, issues, metadata, profiles, remove_label

TIME_BUDGET_SECONDS = 900
# Startup-only values; model/provider authentication must never use inherited
# controller credentials. This is NOT an OS sandbox or runtime authorization.
CHILD_ENV_ALLOWLIST = ("PATH", "SYSTEMROOT")


def child_environment():
    """Construct a fresh child environment from a fixed startup allowlist."""
    return {key: os.environ[key] for key in CHILD_ENV_ALLOWLIST if key in os.environ}


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


def permitted(path, allowed, protected, root=None):
    """Conservative post-run path check, NOT a filesystem or process sandbox."""
    path = path.replace("\\", "/")
    parts = path.split("/")
    low = path.lower()
    # Forbid Windows drive/ADS syntax, UNC paths, dot aliases, and empty parts.
    # Deny rather than trying to canonicalize potentially ambiguous filenames.
    if (not path or path.startswith("/") or ":" in path or "\x00" in path or
            any(part in ("", ".", "..") or part.endswith((".", " "))
                for part in parts)):
        return False
    if any(fnmatch.fnmatch(low, p.lower()) or
           (p.endswith("/") and low.startswith(p.lower()))
           for p in protected):
        return False
    if any(x in (".env", ".ssh", ".git") for x in low.split("/")):
        return False
    if not any(path == x.rstrip("/") or
               (x.endswith("/") and path.startswith(x)) for x in allowed):
        return False
    try:
        checkout = Path(root if root is not None else Path.cwd()).resolve(strict=True)
        candidate = checkout
        for part in parts:
            candidate = candidate / part
            # Reject links even if their target currently stays inside the root.
            # A later retargeting must not change the authorization decision.
            if candidate.is_symlink() or (hasattr(candidate, "is_junction") and
                                          candidate.is_junction()):
                return False
        resolved = candidate.resolve(strict=False)
        return resolved.is_relative_to(checkout)
    except (OSError, RuntimeError, ValueError):
        return False


def choose(repo, token, agent, cfg):
    for issue in sorted(issues(repo, token, "agent:ready"), key=lambda x: x["number"]):
        if not eligible(issue, agent, cfg):
            continue
        m = metadata(issue)
        if not set(m["caps"]).issubset(available()):
            print("Skipped issue", issue["number"], "missing runtime capability",
                  sorted(set(m["caps"]) - available()))
            continue
        if any(not permitted(x.rstrip("/") + ("/placeholder" if x.endswith("/") else ""),
                             [x], cfg["protected_patterns"]) for x in m["paths"]):
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
    subprocess.run(["git", "add", "-A"], cwd=where, check=True)
    data = subprocess.check_output(["git", "diff", "--name-only", "--no-renames",
                                    "-z", start_sha], cwd=where)
    return [x.decode("utf-8") for x in data.split(b"\x00") if x]


def run_one(repo, token, root, agent, cfg, issue, m):
    number = issue["number"]
    stamp = dt.datetime.now(dt.timezone.utc).strftime("%Y%m%d%H%M%S")
    branch = "agent/" + agent + "/issue-" + str(number) + "-" + stamp
    worktree = root.parent / ("Knomira-agent-" + agent + "-" + str(number) + "-" + stamp)
    git(root, "fetch", "origin", m["base"])
    baseline = git(root, "rev-parse", "FETCH_HEAD")
    git(root, "worktree", "add", "-b", branch, str(worktree), baseline)
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
    logdir = root.parent / "Knomira-agent-run-logs"
    logdir.mkdir(exist_ok=True)
    logpath = logdir / (agent + "-" + str(number) + "-" + stamp + ".log")
    # Do not inherit model/API/cloud/SSH credentials from the controller.
    # OS account/profile files remain accessible unless independently sandboxed.
    child_env = child_environment()
    returncode, note, prurl, paths, endsha = 1, "", "", [], baseline
    timed_out = False
    try:
        with logpath.open("w", encoding="utf-8", errors="replace") as output:
            try:
                proc = subprocess.run(cmd, cwd=worktree, env=child_env, stdout=output,
                                      stderr=subprocess.STDOUT, timeout=TIME_BUDGET_SECONDS)
                returncode = proc.returncode
                note = "Agent process exited " + str(returncode)
            except subprocess.TimeoutExpired:
                timed_out = True
                returncode = 124
                note = "Agent process exceeded 15-minute budget"
    except Exception as err:
        note = "Agent execution failed: " + type(err).__name__ + ": " + str(err)
    try:
        paths = changed_files(worktree, baseline)
        bad = [p for p in paths if not permitted(
            p, m["paths"], cfg["protected_patterns"], root=worktree)]
        if bad:
            raise RuntimeError("Out-of-scope paths; no push performed: " + ", ".join(bad))
        if not paths:
            raise RuntimeError("No changed files; no PR")
        if git(worktree, "diff", "--cached", "--name-only"):
            git(worktree, "commit", "-m", "WIP: issue " + str(number) + " " + agent)
        endsha = git(worktree, "rev-parse", "HEAD")
        git(worktree, "push", "-u", "origin", branch)
        pr = api(repo, token, "/pulls", "POST", {
            "title": (("UNSUCCESSFUL (TIMED_OUT) " if timed_out else "WIP issue #") +
                      str(number) + ": " + issue["title"]),
            "head": branch, "base": m["base"], "draft": True,
            "body": ("Agent task: #" + str(number) + "\nProvider: " + agent +
                     "\nBase SHA: " + baseline + "\nHead SHA: " + endsha +
                     "\nCLI exit code: " + str(returncode) +
                     ("\nOutcome: TIMED_OUT; unsuccessful partial-work checkpoint for recovery." if timed_out else "") +
                     "\nTests NOT independently verified; Codex/human review required.")})
        prurl = pr.get("html_url", "")
    except Exception as err:
        note += "; checkpoint/PR failure: " + type(err).__name__ + ": " + str(err)
    body = ("\n".join([
        "Automated task report: " + ("PARTIAL; awaiting review" if prurl else "BLOCKED"),
        "Provider: " + agent, "Issue: #" + str(number),
        "Base: " + m["base"] + " at " + baseline,
        "Working branch: " + branch, "Last local commit SHA: " + endsha,
        "Draft PR (pushed): " + (prurl or "NONE; NOT CONFIRMED PUSHED"),
        "Changed files: " + (", ".join(paths) if paths else "none"),
        "CLI exit code: " + str(returncode),
        "Tests: NOT independently verified by runner; review local log and CI.",
        "Local recovery worktree: " + str(worktree),
        "Local log: " + str(logpath),
        "Reason/blockers: " + note,
        "Next: inspect branch/diff, verify actual tests/Windows CI, update checkpoint.",
        "Do not redo changes already present at the reported Git SHA.",
        "No merge, gameplay completion or save-integrity claim is implied."]))
    comment(repo, token, number, body)
    add_label(repo, token, number, "agent:review" if prurl else "agent:blocked")
    remove_label(repo, token, number, "agent:running")
    print("Issue", number, "PR:", prurl or "none", "local recovery:", worktree)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--agent", choices=["gemini", "opencode", "kilo"], required=True)
    parser.add_argument("--once", action="store_true", help="At most one task")
    parser.add_argument("--execute", action="store_true", help="Allow actual CLI and Git writes")
    args = parser.parse_args()
    if args.execute:
        # Record the refused attempt without fetching credentials or starting a CLI.
        audit_event("runner.gate", args.agent, None, "deny", "security-gate")
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
        audit_event("runner.dry_run", args.agent, None, "deny", "no-task")
        print("No safe, approved, available task found for this agent")
        return
    audit_event("runner.dry_run", args.agent, issue["number"], "allow", "selected")
    print("Selected issue", issue["number"], "base", m["base"])
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
