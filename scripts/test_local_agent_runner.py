"""COORD-01A: mock-only coordinator safety probes.

Known security gaps are marked expectedFailure; this is NOT security
acceptance. Tests do not invoke real coding CLIs, Git, GitHub, or model APIs.
Production security corrections require a separate reviewed branch.
"""
import contextlib
import io
import os
from pathlib import Path
import subprocess
import sys
import tempfile
import unittest
from unittest.mock import patch

import local_agent_runner as runner

BASE_SHA = "5daa68631db9cccbcee959e7a178b9c89262e88e"
CHANGED = "scripts/test_local_agent_runner.py"
CFG = {"agents": {"kilo": {"command": ["fake-provider", "run"]}},
       "protected_patterns": [".env", "*.key", "*.pem", "*secret*", ".git"]}
META = {"base": "feature/universal-agent-onboarding", "paths": [CHANGED]}
ISSUE = {"number": 25, "title": "Mock-only safety checks", "body": "Fixture"}
class GateAndPathProbes(unittest.TestCase):
    def test_sec01_execute_refused_before_credentials_github_or_subprocess(self):
        stderr = io.StringIO()
        stdout = io.StringIO()
        with patch.object(sys, "argv", ["runner.py", "--agent", "kilo", "--execute"]), \
             patch.object(runner.os, "getenv", side_effect=AssertionError("credentials read")), \
             patch.object(runner, "git", side_effect=AssertionError("git invoked")), \
             patch.object(runner, "choose", side_effect=AssertionError("GitHub invoked")), \
             patch.object(runner, "api", side_effect=AssertionError("GitHub API invoked")), \
             patch.object(runner.subprocess, "run", side_effect=AssertionError("child invoked")), \
             patch.object(runner.subprocess, "check_output", side_effect=AssertionError("child invoked")), \
             contextlib.redirect_stderr(stderr), \
             contextlib.redirect_stdout(stdout):
            with self.assertRaises(SystemExit) as error:
                runner.main()
        self.assertEqual(error.exception.code, 2)
        self.assertIn("--execute is disabled", stderr.getvalue())
        self.assertIn('"event": "runner.gate"', stdout.getvalue())
        self.assertIn('"reason": "security-gate"', stdout.getvalue())
        self.assertNotIn("dummy-github", stdout.getvalue())

    def test_sec05_protected_and_exact_file_paths(self):
        self.assertTrue(runner.permitted(CHANGED, [CHANGED], CFG["protected_patterns"]))
        for candidate in ("../scripts/test_local_agent_runner.py",
                          "scripts/../scripts/test_local_agent_runner.py",
                          "scripts/test_local_agent_runner.py.bak",
                          "/scripts/test_local_agent_runner.py",
                          "scripts/.git/config", "scripts/secret.key"):
            with self.subTest(candidate=candidate):
                self.assertFalse(runner.permitted(candidate, [CHANGED], CFG["protected_patterns"]))

    def test_sec05_protected_patterns_with_broad_allowed_directory(self):
        # The positive control isolates protected globs from the path allowlist.
        cases = (("scripts/secret.key", "scripts/", "*.key"),
                 ("scripts/auth.pem", "scripts/", "*.pem"),
                 ("scripts/secret.txt", "scripts/", "*secret*"),
                 ("src/gui/license.h", "src/gui/", "*license*"))
        for candidate, allowed_dir, pattern in cases:
            with self.subTest(path=candidate):
                self.assertTrue(runner.permitted(candidate, [allowed_dir], []))
                self.assertFalse(runner.permitted(candidate, [allowed_dir],
                                                  [pattern]))
        self.assertFalse(runner.permitted("scripts/.env", ["scripts/"], []))
        self.assertFalse(runner.permitted("scripts/.git/config", ["scripts/"], []))

    def test_sec05_symlink_escape_from_allowlisted_directory(self):
        with tempfile.TemporaryDirectory() as temp:
            root = Path(temp) / "checkout"
            inside = root / "scripts"
            outside = Path(temp) / "outside"
            inside.mkdir(parents=True)
            outside.mkdir()
            try:
                (inside / "link").symlink_to(outside, target_is_directory=True)
            except (OSError, NotImplementedError) as error:
                self.skipTest("Symlinks unavailable: " + str(error))
            original = Path.cwd()
            try:
                os.chdir(root)
                self.assertFalse(runner.permitted("scripts/link/x.py", ["scripts/"], []))
            finally:
                os.chdir(original)


    def test_sec05_symlink_to_internal_directory_is_also_rejected(self):
        with tempfile.TemporaryDirectory() as temp:
            root = Path(temp) / "checkout"
            (root / "scripts").mkdir(parents=True)
            (root / "safe").mkdir()
            try:
                (root / "scripts" / "link").symlink_to(
                    root / "safe", target_is_directory=True)
            except (OSError, NotImplementedError) as error:
                self.skipTest("Symlinks unavailable: " + str(error))
            self.assertTrue(runner.permitted(
                "scripts/normal.py", ["scripts/"], [], root=root))
            self.assertFalse(runner.permitted(
                "scripts/link/normal.py", ["scripts/"], [], root=root))

    def test_sec05_reject_drive_unc_ads_and_dot_aliases(self):
        with tempfile.TemporaryDirectory() as temp:
            for candidate in ("C:/Windows/system.ini", "//server/share/config",
                              "scripts/notes.txt:payload", "scripts//file.py",
                              "scripts/./file.py", "scripts/dot./file.py",
                              "scripts/ending ", "scripts/ending."):
                with self.subTest(path=candidate):
                    self.assertFalse(runner.permitted(
                        candidate, [candidate, "scripts/"], [], root=temp))

    def test_sec05_explicit_root_rejects_external_link_without_chdir(self):
        with tempfile.TemporaryDirectory() as temp:
            root = Path(temp) / "checkout"
            outside = Path(temp) / "outside"
            (root / "scripts").mkdir(parents=True)
            outside.mkdir()
            try:
                (root / "scripts" / "escape").symlink_to(
                    outside, target_is_directory=True)
            except (OSError, NotImplementedError) as error:
                self.skipTest("Symlinks unavailable: " + str(error))
            self.assertFalse(runner.permitted(
                "scripts/escape/unapproved.py", ["scripts/"], [], root=root))
            self.assertTrue(runner.permitted(
                "scripts/approved.py", ["scripts/"], [], root=root))


class MockedRunOneProbes(unittest.TestCase):
    def setUp(self):
        self.tmp = tempfile.TemporaryDirectory()
        self.addCleanup(self.tmp.cleanup)
        self.root = Path(self.tmp.name) / "repo"
        self.root.mkdir()
        self.git_history = []
        self.deny_push = False
        self.cached = CHANGED
        self.end_sha = "f" * 40
        patches = [
            patch.object(runner, "git", side_effect=self.fake_git),
            patch.object(runner, "changed_files", return_value=[CHANGED]),
            patch.object(runner.subprocess, "run",
                         return_value=subprocess.CompletedProcess([], 0)),
            patch.object(runner, "api",
                         return_value={"html_url": "https://github.test/draft/1"}),
            patch.object(runner, "add_label"),
            patch.object(runner, "remove_label"),
            patch.object(runner, "comment"),
        ]
        self.git, self.changed, self.proc, self.api, self.add, self.remove, self.comment = [
            p.start() for p in patches]
        for p in patches:
            self.addCleanup(p.stop)

    def fake_git(self, where, *args):
        self.git_history.append(args)
        if args[:2] == ("rev-parse", "FETCH_HEAD"):
            return BASE_SHA
        if args[:2] == ("rev-parse", "HEAD"):
            return self.end_sha
        if args[:2] == ("worktree", "add"):
            Path(args[4]).mkdir(parents=True, exist_ok=True)
        if args[:3] == ("diff", "--cached", "--name-only"):
            return self.cached
        if args[0] == "push" and self.deny_push:
            raise PermissionError("simulated push denied")
        return ""

    def run_mock(self, issue=None, task_meta=None):
        with contextlib.redirect_stdout(io.StringIO()):
            runner.run_one("example/Knomira", "dummy-token", self.root,
                           "kilo", CFG, issue or ISSUE, task_meta or META)

    def report(self):
        return self.comment.call_args.args[3]

    @unittest.expectedFailure
    def test_sec02_timeout_requires_explicit_unsuccessful_draft(self):
        self.proc.side_effect = subprocess.TimeoutExpired(["fake-provider"], 900)
        self.run_mock()
        self.assertIn("TIMED_OUT", self.api.call_args.args[4]["body"])
        self.assertIn("Local recovery worktree:", self.report())

    @unittest.expectedFailure
    def test_sec03_nonzero_exit_must_identify_unsuccessful_draft(self):
        self.proc.return_value = subprocess.CompletedProcess([], 17)
        self.run_mock()
        self.assertIn("UNSUCCESSFUL", self.api.call_args.args[4]["body"])

    def test_sec04_child_env_is_explicitly_allowlisted(self):
        dummy = {"PATH": "dummy-path", "SYSTEMROOT": "dummy-system",
                 "GITHUB_TOKEN": "dummy-github", "GH_TOKEN": "dummy-gh",
                 "JULES_API_KEY": "dummy-jules", "OPENAI_API_KEY": "dummy-openai",
                 "ANTHROPIC_API_KEY": "dummy-anthropic",
                 "AWS_SECRET_ACCESS_KEY": "dummy-aws",
                 "SSH_AUTH_SOCK": "dummy-socket", "SOME_SECRET": "dummy-secret",
                 "HOME": "sensitive-home", "USERPROFILE": "sensitive-profile",
                 "APPDATA": "sensitive-appdata", "GIT_CONFIG_GLOBAL": "secret-git"}
        with patch.dict(os.environ, dummy, clear=True):
            self.run_mock()
            # Reading the parent's environment for the child must not mutate it.
            self.assertEqual(os.environ["GITHUB_TOKEN"], "dummy-github")
        env = self.proc.call_args.kwargs["env"]
        self.assertEqual(env, {"PATH": "dummy-path", "SYSTEMROOT": "dummy-system"})
        self.assertTrue(set(env).isdisjoint(set(dummy) - {"PATH", "SYSTEMROOT"}))

    def test_sec04_child_env_absent_startup_vars_stays_empty(self):
        dummy = {"GITHUB_TOKEN": "dummy-github",
                 "OPENAI_API_KEY": "dummy-openai", "HOME": "sensitive-home"}
        with patch.dict(os.environ, dummy, clear=True):
            self.assertEqual(runner.child_environment(), {})

    @unittest.expectedFailure
    def test_sec06_preclaimed_issue_does_not_start_a_second_worker(self):
        self.run_mock(dict(ISSUE, labels=[{"name": "agent:running"}]))
        self.proc.assert_not_called()

    @unittest.expectedFailure
    def test_sec06_interrupted_cleanup_releases_running_state(self):
        self.comment.side_effect = [None, OSError("simulated report failure")]
        try:
            self.run_mock()
        except OSError:
            pass
        self.assertIn("agent:blocked",
                      [c.args[3] for c in self.add.call_args_list])
        self.assertIn("agent:running",
                      [c.args[3] for c in self.remove.call_args_list])

    @unittest.expectedFailure
    def test_sec09_missing_cli_does_not_create_pr(self):
        self.proc.side_effect = FileNotFoundError("fake-provider")
        self.run_mock()
        self.api.assert_not_called()
        self.assertIn("BLOCKED", self.report())

    @unittest.expectedFailure
    def test_sec09_provider_quota_failure_is_detected(self):
        def quota(*args, **kwargs):
            kwargs["stdout"].write("Simulated provider quota exceeded\n")
            return subprocess.CompletedProcess([], 17)
        self.proc.side_effect = quota
        self.run_mock()
        self.assertIn("quota", self.report().lower())
        self.assertIn("BLOCKED", self.report())

    def test_sec10_push_failure_preserves_local_checkpoint(self):
        self.deny_push = True
        self.run_mock()
        self.api.assert_not_called()
        self.assertIn("BLOCKED", self.report())
        self.assertIn("Local recovery worktree:", self.report())
        self.assertIn("NOT CONFIRMED PUSHED", self.report())
        self.assertIn("push denied", self.report())

    @unittest.expectedFailure
    def test_sec11_pr_creation_failure_preserves_actual_push_status(self):
        self.api.side_effect = OSError("simulated PR API failure")
        self.run_mock()
        self.assertIn("PUSHED branch", self.report())
        self.assertIn("Draft PR (pushed): NONE", self.report())

    @unittest.expectedFailure
    def test_sec12_uncommitted_diff_does_not_push_or_open_pr(self):
        self.cached = ""
        self.end_sha = BASE_SHA
        self.run_mock()
        self.assertNotIn("push", [call[0] for call in self.git_history])
        self.api.assert_not_called()

    def test_sec12_protected_change_is_not_pushed_when_directory_is_allowed(self):
        # scripts/ is in the task allowlist; only the protected glob can block.
        self.assertTrue(runner.permitted("scripts/secret.key", ["scripts/"], []))
        self.changed.return_value = [CHANGED, "scripts/secret.key"]
        self.run_mock(task_meta=dict(META, paths=["scripts/"]))
        self.assertNotIn("push", [call[0] for call in self.git_history])
        self.api.assert_not_called()
        self.assertIn("Out-of-scope paths", self.report())

    def test_sec14_remote_fetch_permission_denial_stops_before_claim(self):
        # Distinct from SEC-10 push failure. No durable checkpoint is proven.
        with patch.object(runner, "git",
                          side_effect=PermissionError("mock 403 remote fetch")):
            with self.assertRaises(PermissionError):
                self.run_mock()
        self.add.assert_not_called()
        self.proc.assert_not_called()
        self.api.assert_not_called()

    @unittest.expectedFailure
    def test_sec14_failed_final_handoff_never_leaves_running_label(self):
        self.comment.side_effect = [None, ConnectionError("simulated report outage")]
        try:
            self.run_mock()
        except ConnectionError:
            pass
        self.assertIn("agent:blocked",
                      [c.args[3] for c in self.add.call_args_list])


if __name__ == "__main__":
    unittest.main()
