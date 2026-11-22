#!/usr/bin/env python3
"""Knomira queue scanning and label bootstrap; never launches a model."""
import argparse
import datetime as dt
import json
import os
from pathlib import Path
import re
import urllib.error
import urllib.parse
import urllib.request

API = "https://api.github.com"
LABELS = ["agent:approved", "agent:ready", "agent:running",
          "agent:review", "agent:blocked", "agent:done",
          "agent:provider:gemini", "agent:provider:kilo",
          "agent:provider:opencode", "priority:P0", "priority:P1", "priority:P2"]
BLOCKED = {"agent:running", "agent:review", "agent:blocked", "agent:done"}
MANUAL_ONLY_PREFIX = "manual:"


def audit_event(event, agent, issue_number, outcome, reason):
    """Emit a metadata-only JSON audit record; never log issue text or secrets."""
    allowed = {
        ("queue.eligibility", "allow", "eligible"),
        ("queue.eligibility", "deny", "manual-label"),
        ("queue.eligibility", "deny", "policy"),
        ("runner.gate", "deny", "security-gate"),
        ("runner.dry_run", "allow", "selected"),
        ("runner.dry_run", "deny", "no-task"),
    }
    if (event, outcome, reason) not in allowed:
        raise ValueError("Unsupported audit event")
    if not isinstance(agent, str) or not re.fullmatch(r"[a-z][a-z0-9_-]{0,31}", agent):
        raise ValueError("Invalid agent for audit event")
    if issue_number is not None and (
        type(issue_number) is not int or issue_number <= 0
    ):
        raise ValueError("Invalid issue number for audit event")
    record = {
        "utc": dt.datetime.now(dt.timezone.utc).isoformat(timespec="seconds"),
        "event": event, "agent": agent, "issue": issue_number,
        "outcome": outcome, "reason": reason,
    }
    print("KNOMIRA_AUDIT " + json.dumps(record, sort_keys=True))


def api(repo, token, path, method="GET", data=None):
    payload = json.dumps(data).encode() if data is not None else None
    req = urllib.request.Request(API + "/repos/" + repo + path, method=method,
        data=payload, headers={"Authorization": "Bearer " + token,
        "Accept": "application/vnd.github+json", "User-Agent": "knomira-agent-queue",
        "Content-Type": "application/json"})
    with urllib.request.urlopen(req, timeout=25) as response:
        raw = response.read()
        return json.loads(raw) if raw else {}


def labels(issue):
    return {x["name"] for x in issue.get("labels", [])}


def field(body, name):
    m = re.search(r"(?im)^(?:###\s*)?" + re.escape(name) +
                  r":?[ \t]*(?:\n+[ \t]*)?([^\n]+)", body or "")
    return m.group(1).strip().strip(chr(96) + " ") if m else ""


def metadata(issue):
    body = issue.get("body") or ""
    return {"base": field(body, "Base branch"),
            "kind": field(body, "Task kind").lower(),
            "risk": field(body, "Risk").lower(),
            "caps": [x.strip().lower() for x in
                     field(body, "Required capabilities").split(",") if x.strip()],
            "paths": [x.strip().strip(chr(96)) for x in
                      field(body, "Allowed paths").split(",") if x.strip()]}


def valid_branch(value):
    return bool(re.fullmatch(r"[A-Za-z0-9][A-Za-z0-9_./-]{0,140}", value) and
                ".." not in value and "//" not in value and
                not value.endswith((".", "/")))


def valid_paths(paths):
    return bool(paths) and all(
        bool(re.fullmatch(r"[A-Za-z0-9_./-]+", x)) and
        not x.startswith((".", "/")) and ".." not in x
        for x in paths)


def eligible(issue, agent, cfg):
    if issue.get("pull_request") or issue.get("state") != "open":
        return False
    ls = labels(issue)
    # An explicit manual assignment wins even if auto-approval labels coexist.
    # Deny the whole manual:* namespace, including future/manual case variants.
    if any(name.casefold().startswith(MANUAL_ONLY_PREFIX) for name in ls):
        return False
    if not {"agent:approved", "agent:ready"}.issubset(ls) or ls.intersection(BLOCKED):
        return False
    if "agent:provider:" + agent not in ls:
        return False
    profile = cfg["agents"][agent]
    data = metadata(issue)
    return (profile["execution"] == "local_cli" and
            data["kind"] in profile["allowed_kinds"] and
            data["kind"] not in cfg["auto_denied_kinds"] and
            data["risk"] == "low" and valid_branch(data["base"]) and
            valid_paths(data["paths"]) and bool(data["caps"]) and
            all(x in cfg["runtime_capabilities"] for x in data["caps"]))


def issues(repo, token, label):
    out = []
    for page in range(1, 4):
        params = urllib.parse.urlencode({"labels": label, "state": "open",
                                          "per_page": 100, "page": page})
        part = api(repo, token, "/issues?" + params)
        out.extend(part)
        if len(part) != 100:
            break
    return out


def add_label(repo, token, num, name):
    api(repo, token, "/issues/" + str(num) + "/labels", "POST", {"labels": [name]})


def remove_label(repo, token, num, name):
    path = "/issues/" + str(num) + "/labels/" + urllib.parse.quote(name, safe="")
    try:
        api(repo, token, path, "DELETE")
    except urllib.error.HTTPError as err:
        if err.code != 404:
            raise


def comment(repo, token, num, body):
    api(repo, token, "/issues/" + str(num) + "/comments", "POST", {"body": body})


def profiles():
    return json.loads((Path(__file__).resolve().parents[1] /
        "docs/agent-coordination/profiles.json").read_text(encoding="utf-8"))


def main():
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument("--mode", choices=["scan", "bootstrap"], default="scan")
    a = p.parse_args()
    repo, token = os.getenv("GITHUB_REPOSITORY", ""), os.getenv("GITHUB_TOKEN", "")
    if not re.fullmatch(r"[A-Za-z0-9_.-]+/[A-Za-z0-9_.-]+", repo) or not token:
        p.error("GITHUB_REPOSITORY and GITHUB_TOKEN required")
    if a.mode == "bootstrap":
        try:
            api(repo, token, "/issues?state=open&per_page=1")
        except urllib.error.HTTPError as err:
            if err.code == 410:
                print("GitHub Issues are disabled: enable in Settings > General > Features.")
                return
            raise
        for label in LABELS:
            try:
                api(repo, token, "/labels/" + urllib.parse.quote(label, safe=""))
            except urllib.error.HTTPError as err:
                if err.code != 404:
                    raise
                api(repo, token, "/labels", "POST",
                    {"name": label, "color": "c5def5"})
            print("label ready:", label)
        return
    try:
        queue = issues(repo, token, "agent:ready")
    except urllib.error.HTTPError as err:
        if err.code == 410:
            print("GitHub Issues are disabled in this repository. "
                  "Enable Issues under Settings > General > Features before dispatching tasks.")
            print("Read proposed assignments in docs/agent-coordination/PROPOSED_TASKS.md.")
            return
        raise
    cfg = profiles()
    print("Read-only queue scan; zero AI sessions launched")
    for agent, spec in cfg["agents"].items():
        if spec["execution"] != "local_cli":
            continue
        matching = []
        for item in queue:
            allowed = eligible(item, agent, cfg)
            reason = ("eligible" if allowed else
                      "manual-label" if any(name.casefold().startswith(MANUAL_ONLY_PREFIX)
                                             for name in labels(item)) else "policy")
            audit_event("queue.eligibility", agent, item["number"],
                        "allow" if allowed else "deny", reason)
            if allowed:
                matching.append(item)
        print(agent, "eligible:", len(matching))
        for item in matching[:10]:
            # Issue titles/bodies are untrusted; keep them out of workflow logs.
            print("  issue", item["number"])


if __name__ == "__main__":
    main()
