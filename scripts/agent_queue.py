#!/usr/bin/env python3
"""Safe GitHub Issue -> Jules dispatcher; read-only unless explicitly enabled.

Python standard library only. Review docs/agent-coordination/SETUP.md before use.
"""
import argparse
import json
import os
import re
import sys
import urllib.error
import urllib.parse
import urllib.request

GH = "https://api.github.com"
JULES = "https://jules.googleapis.com/v1alpha"
SESSION_RE = re.compile(r"<!-- knomira-jules-session: (sessions/[A-Za-z0-9_-]+) -->")
REQUIRED = {"agent:approved", "agent:ready", "agent:provider:jules"}
COLORS = {
    "agent:approved": "1d76db",
    "agent:ready": "0e8a16",
    "agent:running": "fbca04",
    "agent:review": "5319e7",
    "agent:blocked": "d93f0b",
    "agent:done": "006b75",
    "agent:provider:jules": "c5def5",
    "priority:P0": "b60205",
    "priority:P1": "ff9f1c",
    "priority:P2": "ededed",
}


def http(base, path, token, method="GET", data=None, extra_headers=None):
    payload = None if data is None else json.dumps(data).encode("utf-8")
    headers = {"Accept": "application/json", "User-Agent": "knomira-agent-queue/1.0"}
    headers["Authorization" if base == GH else "x-goog-api-key"] = (
        "Bearer " + token if base == GH else token
    )
    if payload is not None:
        headers["Content-Type"] = "application/json"
    headers.update(extra_headers or {})
    req = urllib.request.Request(base + path, data=payload, headers=headers, method=method)
    with urllib.request.urlopen(req, timeout=30) as response:
        raw = response.read()
        return json.loads(raw.decode("utf-8")) if raw else {}


def issue_labels(issue):
    return {item["name"] for item in issue.get("labels", [])}


def eligible(issue):
    labels = issue_labels(issue)
    return (not issue.get("pull_request") and issue.get("state") == "open"
            and REQUIRED.issubset(labels)
            and not labels.intersection({"agent:running", "agent:review",
                                         "agent:blocked", "agent:done"}))


def base_branch(body):
    """Accept the simple template or GitHub Issue Form heading format."""
    patterns = [
        r"(?im)^Base branch:\s*`?([^\s`]+)",
        r"(?im)^### Base branch\s*\n\s*`?([^\s`]+)",
    ]
    for pattern in patterns:
        match = re.search(pattern, body or "")
        if match:
            branch = match.group(1).strip()
            if (re.fullmatch(r"[A-Za-z0-9][A-Za-z0-9_./-]{0,140}", branch)
                    and ".." not in branch and "//" not in branch
                    and not branch.endswith(("/", "."))):
                return branch
    return None


def priority(issue):
    labels = issue_labels(issue)
    return (0 if "priority:P0" in labels else 1 if "priority:P1" in labels else 2,
            issue.get("created_at", ""), issue["number"])


def load_issues(repo, token, label, limit=100):
    query = urllib.parse.urlencode({"state": "open", "per_page": limit, "labels": label})
    path = "/repos/" + repo + "/issues?" + query
    return http(GH, path, token)


def add_label(repo, number, token, label):
    http(GH, f"/repos/{repo}/issues/{number}/labels", token, "POST", {"labels": [label]})


def remove_label(repo, number, token, label):
    path = f"/repos/{repo}/issues/{number}/labels/{urllib.parse.quote(label, safe='')}"
    try:
        http(GH, path, token, "DELETE")
    except urllib.error.HTTPError as exc:
        if exc.code != 404:
            raise


def comment(repo, number, token, body):
    http(GH, f"/repos/{repo}/issues/{number}/comments", token, "POST", {"body": body})


def session_from_comments(comments):
    for item in reversed(comments):
        match = SESSION_RE.search(item.get("body") or "")
        if match:
            return match.group(1)
    return None


def reconcile(repo, token, key):
    running = load_issues(repo, token, "agent:running")
    for issue in running:
        number = issue["number"]
        comments = http(GH, f"/repos/{repo}/issues/{number}/comments?per_page=100", token)
        session = session_from_comments(comments)
        if not session:
            print(f"#{number}: running without a session marker; investigate manually")
            continue
        info = http(JULES, "/" + session, key)
        state = info.get("state", "UNKNOWN")
        print(f"#{number}: Jules {session}: {state}")
        if state == "COMPLETED":
            links = [output.get("pullRequest", {}).get("url") for output in info.get("outputs", [])]
            links = [url for url in links if url and url.startswith(f"https://github.com/{repo}/pull/")]
            text = "Jules session completed. " + (
                "PR(s): " + ", ".join(links) if links
                else "No matching repository PR URL returned; inspect the session manually."
            )
            comment(repo, number, token, text +
                    "\nStatus: awaiting independent Codex/human review; NOT merged or verified.")
            add_label(repo, number, token, "agent:review")
            remove_label(repo, number, token, "agent:running")
        elif state == "FAILED":
            comment(repo, number, token, "Jules session FAILED. Investigate before retrying: "
                    + info.get("url", session))
            add_label(repo, number, token, "agent:blocked")
            remove_label(repo, number, token, "agent:running")
    return len(load_issues(repo, token, "agent:running"))


def dispatch(repo, token, key, source):
    if load_issues(repo, token, "agent:running"):
        print("Another Jules issue is running; maximum concurrency is one.")
        return
    ready = [i for i in load_issues(repo, token, "agent:ready") if eligible(i)]
    ready.sort(key=priority)
    for issue in ready:
        number = issue["number"]
        branch = base_branch(issue.get("body"))
        if not branch:
            print(f"#{number}: no valid Base branch, refusing to start")
            continue
        try:
            http(GH, f"/repos/{repo}/git/ref/heads/{branch}", token)
        except urllib.error.HTTPError:
            print(f"#{number}: base branch {branch} not found; refusing to start")
            continue
        prompt = (
            f"Knomira task issue #{number}: {issue['title']}\n"
            "Read AGENTS.md and docs/agent-coordination/README.md before editing. "
            "Read current GitHub PRs/checkpoint first; avoid duplicate or overlapping work. "
            "Do NOT merge, alter real player saves, use proprietary assets, or claim unrun tests. "
            f"Target EXACT base branch {branch}. Open a reviewable PR; include 'Agent task: #{number}' "
            "and evidence (starting SHA, changed files, tests actually run, untested behavior). "
            "Avoid rewriting the engine or making unrequested gameplay changes.\n\n"
            "Approved issue description:\n" + (issue.get("body") or "")[:12000]
        )
        # Claim before remote request: if the network times out, do NOT launch twice.
        add_label(repo, number, token, "agent:running")
        remove_label(repo, number, token, "agent:ready")
        try:
            session = http(JULES, "/sessions", key, "POST", {
                "title": f"Knomira #{number}: {issue['title']}"[:100],
                "prompt": prompt,
                "sourceContext": {"source": source,
                                  "githubRepoContext": {"startingBranch": branch}},
                "automationMode": "AUTO_CREATE_PR",
                "requirePlanApproval": False,
            })
            name = session.get("name", "")
            if not re.fullmatch(r"sessions/[A-Za-z0-9_-]+", name):
                raise ValueError("Jules response did not contain an identifiable session")
            url = session.get("url") or "https://jules.google.com"
            comment(repo, number, token, f"<!-- knomira-jules-session: {name} -->\n"
                    f"Claimed by automated Jules dispatcher. Session: {url}\n"
                    f"Starting branch: `{branch}`. PR/review pending. No tests yet verified.")
            print(f"#{number}: dispatched {name}")
        except Exception as exc:
            # Network request MAY have succeeded; leave the claim blocked to prevent duplicates.
            print(f"#{number}: dispatch uncertain/failed: {type(exc).__name__}: {exc}")
            try:
                add_label(repo, number, token, "agent:blocked")
                comment(repo, number, token,
                        "Dispatch failed or timed out. Claim retained to prevent duplicate "
                        "sessions. Check Jules sessions and resolve manually before retrying.")
            except Exception:
                print("Unable to annotate issue; inspect it before retrying.", file=sys.stderr)
            raise
        return
    print("No approved, ready, provider:jules issues with verified base branch.")


def bootstrap(repo, token):
    for name, color in COLORS.items():
        path = f"/repos/{repo}/labels/{urllib.parse.quote(name, safe='')}"
        try:
            http(GH, path, token)
            print(f"Exists: {name}")
        except urllib.error.HTTPError as exc:
            if exc.code != 404:
                raise
            http(GH, f"/repos/{repo}/labels", token, "POST",
                 {"name": name, "color": color, "description": "Knomira agent queue"})
            print(f"Created: {name}")


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--mode", choices=["scan", "bootstrap", "run"], default="scan")
    args = parser.parse_args()
    repo = os.environ.get("GITHUB_REPOSITORY", "")
    token = os.environ.get("GITHUB_TOKEN", "")
    if not re.fullmatch(r"[A-Za-z0-9_.-]+/[A-Za-z0-9_.-]+", repo) or not token:
        parser.error("GITHUB_REPOSITORY and GITHUB_TOKEN must be configured")
    if args.mode == "bootstrap":
        bootstrap(repo, token)
        return
    ready = [i for i in load_issues(repo, token, "agent:ready") if eligible(i)]
    print(f"Approved Jules-ready issues: {len(ready)}")
    for issue in sorted(ready, key=priority)[:20]:
        print(f"  #{issue['number']}: {issue['title']} [base={base_branch(issue.get('body'))}]")
    if args.mode == "scan":
        print("SCAN ONLY; no session launched or issue modified")
        return
    key = os.environ.get("JULES_API_KEY", "")
    source = os.environ.get("JULES_SOURCE", "")
    enabled = os.environ.get("AGENT_JULES_ENABLED", "").lower() == "true"
    if not enabled or not key or not source or not source.startswith("sources/"):
        print("Jules dispatcher is disabled or unconfigured; no write actions performed.")
        return
    if reconcile(repo, token, key) == 0:
        dispatch(repo, token, key, source)


if __name__ == "__main__":
    main()
