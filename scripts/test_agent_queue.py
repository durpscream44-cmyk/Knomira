import unittest
from agent_queue import field, metadata, valid_branch, valid_paths, eligible

CFG = {"agents": {"gemini": {"execution": "local_cli",
       "allowed_kinds": ["docs", "tests"]},
       "jules": {"execution": "disabled", "allowed_kinds": []}},
       "auto_denied_kinds": ["engine-integration", "windows-runtime", "art-asset"],
       "runtime_capabilities": ["git", "github", "windows", "cmake", "qt", "assets", "gui"]}


def task():
    return {"number": 2, "state": "open",
       "labels": [{"name": x} for x in ("agent:approved", "agent:ready",
                                       "agent:provider:gemini")],
       "body": ("### Base branch\n\ncompletion/play01e-colony-roundtrip\n"
                "### Task kind\n\ntests\n"
                "### Required capabilities\n\ngit, github\n"
                "### Allowed paths\n\ntests/, scripts/\n"
                "### Risk\n\nlow\n")}


class Tests(unittest.TestCase):
    def test_issue_form(self):
        self.assertEqual(metadata(task())["caps"], ["git", "github"])
        self.assertEqual(metadata(task())["base"],
                         "completion/play01e-colony-roundtrip")
        self.assertTrue(eligible(task(), "gemini", CFG))

    def test_plain_text(self):
        self.assertEqual(field("Base branch: master", "Base branch"), "master")

    def test_blocked(self):
        item = task()
        item["labels"].append({"name": "agent:running"})
        self.assertFalse(eligible(item, "gemini", CFG))
        self.assertFalse(eligible(task(), "jules", CFG))

    def test_input_validation(self):
        self.assertFalse(valid_branch("../master"))
        self.assertFalse(valid_branch(".hidden"))
        self.assertTrue(valid_branch("feature/test"))
        self.assertTrue(valid_paths(["tests/", "scripts/a.py"]))
        self.assertFalse(valid_paths(["../secret"]))
        self.assertFalse(valid_paths([".env"]))


# COORD-01A deterministic zero-network and manual-only dispatch probes.
import contextlib
import io
import os
from pathlib import Path
import sys
from unittest.mock import patch
import agent_queue as queue

QUEUE_CFG = {"agents": {"gemini": {"execution": "local_cli",
              "allowed_kinds": ["docs", "tests"]},
              "jules": {"execution": "disabled", "allowed_kinds": []}},
             "auto_denied_kinds": ["engine-integration", "windows-runtime"],
             "runtime_capabilities": ["git", "github", "windows", "cmake"]}


def queue_task():
    return {"number": 28, "title": "Mock queue issue", "state": "open",
            "labels": [{"name": label} for label in
                       ("agent:approved", "agent:ready", "agent:provider:gemini")],
            "body": ("### Base branch\n\nmaster\n### Task kind\n\ntests\n"
                     "### Required capabilities\n\ngit, github\n"
                     "### Allowed paths\n\nscripts/\n### Risk\n\nlow\n")}



class QueueProbes(unittest.TestCase):
    def test_sec07_missing_automatic_approval_is_ineligible(self):
        # Only checks the missing-approval gate, not a manual-label override.
        for number in (28, 29):
            with self.subTest(issue=number):
                issue = queue_task()
                issue["number"] = number
                issue["labels"] = [{"name": "manual:kilo"}, {"name": "manual:ready"}]
                self.assertFalse(queue.eligible(issue, "gemini", QUEUE_CFG))

    def test_sec07_automatic_label_baselines_are_eligible(self):
        # Positive control MUST fail normally (not be swallowed by expectedFailure).
        # Both numbered fixtures represent otherwise eligible automatic tasks.
        for number in (28, 29):
            with self.subTest(issue=number):
                issue = queue_task()
                issue["number"] = number
                self.assertTrue(queue.eligible(issue, "gemini", QUEUE_CFG))

    def _assert_manual_label_blocks_auto_dispatch(self, number, manual_label):
        # Expected-failure methods below contain ONLY the disputed policy
        # assertion. Every fixture's automatic baseline is separately guarded.
        issue = queue_task()
        issue["number"] = number
        issue["labels"].append({"name": manual_label})
        self.assertFalse(queue.eligible(issue, "gemini", QUEUE_CFG))

    @unittest.expectedFailure
    def test_sec07_manual_ready_blocks_auto_dispatch_issue_28(self):
        self._assert_manual_label_blocks_auto_dispatch(28, "manual:ready")

    @unittest.expectedFailure
    def test_sec07_manual_ready_blocks_auto_dispatch_issue_29(self):
        self._assert_manual_label_blocks_auto_dispatch(29, "manual:ready")

    @unittest.expectedFailure
    def test_sec07_manual_kilo_blocks_auto_dispatch_issue_28(self):
        self._assert_manual_label_blocks_auto_dispatch(28, "manual:kilo")

    @unittest.expectedFailure
    def test_sec07_manual_kilo_blocks_auto_dispatch_issue_29(self):
        self._assert_manual_label_blocks_auto_dispatch(29, "manual:kilo")

    def test_sec08_queue_scan_is_get_only(self):
        calls = []
        def fake_api(repo, token, path, method="GET", data=None):
            calls.append((path, method, data))
            if path.startswith("/issues?"):
                return [queue_task()]
            self.fail("Unexpected API path: " + path)
        with patch.object(sys, "argv", ["agent_queue.py", "--mode", "scan"]), \
             patch.dict(os.environ, {"GITHUB_REPOSITORY": "example/Knomira",
                                     "GITHUB_TOKEN": "dummy-token"}), \
             patch.object(queue, "api", side_effect=fake_api), \
             patch.object(queue, "profiles", return_value=QUEUE_CFG), \
             contextlib.redirect_stdout(io.StringIO()):
            queue.main()
        self.assertTrue(calls)
        self.assertTrue(all(method == "GET" and data is None
                            for _, method, data in calls))

    @unittest.expectedFailure
    def test_sec08_scheduled_scan_token_does_not_have_issue_write_permission(self):
        workflow = (Path(__file__).resolve().parents[1] /
                    ".github/workflows/agent-queue.yml").read_text(encoding="utf-8")
        self.assertNotIn("issues: write", workflow)

    def test_sec13_bad_issue_or_unknown_capability_is_ineligible(self):
        cases = [("closed", lambda x: x.update(state="closed")),
                 ("unknown capability", lambda x: x.update(
                     body=x["body"].replace("git, github", "git, github, unknown-cap"))),
                 ("path traversal", lambda x: x.update(
                     body=x["body"].replace("scripts/", "../private/"))),
                 ("high risk", lambda x: x.update(
                     body=x["body"].replace("### Risk\n\nlow", "### Risk\n\nhigh"))),
                 ("missing approval", lambda x: x.update(
                     labels=[{"name": "agent:ready"}])),
                 ("already running", lambda x: x["labels"].append(
                     {"name": "agent:running"}))]
        for name, mutate in cases:
            with self.subTest(case=name):
                issue = queue_task()
                mutate(issue)
                self.assertFalse(queue.eligible(issue, "gemini", QUEUE_CFG))




if __name__ == "__main__":
    unittest.main()
