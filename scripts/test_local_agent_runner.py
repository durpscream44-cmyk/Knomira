"""Offline security regressions for the local runner; no provider/GitHub calls."""
import os
from pathlib import Path
import subprocess
import tempfile
import unittest
from unittest.mock import patch

import local_agent_runner as runner


class RunnerSecurityTests(unittest.TestCase):
    def test_paths_require_relative_allowlisted_contained_targets(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            (root / "scripts").mkdir()
            (root / "scripts" / "safe.py").write_text("ok", encoding="utf-8")
            allow = ["scripts/"]
            self.assertTrue(runner.permitted("scripts/safe.py", allow, [], root))
            for name in ("../secret", r"C:\secret", r"\\server\share", ".env",
                         "scripts/../secret", "scripts/name.", "scripts/bad:name"):
                self.assertFalse(runner.permitted(name, allow, [], root), name)
            self.assertFalse(runner.permitted("scripts/key.pem", allow,
                                              ["*.pem"], root))

    def test_symlink_escape_is_denied_when_windows_allows_link_creation(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory) / "repo"
            outside = Path(directory) / "outside"
            (root / "scripts").mkdir(parents=True)
            outside.mkdir()
            (outside / "secret.py").write_text("secret", encoding="utf-8")
            try:
                os.symlink(outside, root / "scripts" / "escape", target_is_directory=True)
            except (OSError, NotImplementedError) as err:
                self.skipTest("Windows symlink creation unavailable: " + str(err))
            self.assertFalse(runner.permitted("scripts/escape/secret.py",
                                              ["scripts/"], [], root))

    def test_unreadable_path_metadata_fails_closed(self):
        with patch.object(Path, "lstat", side_effect=PermissionError("ACL denied")):
            self.assertTrue(runner._is_reparse_point(Path("C:/disposable/probe")))

    def test_child_environment_drops_arbitrary_secrets(self):
        env = runner.child_environment({
            "PATH": "C:\\tools", "SystemRoot": "C:\\Windows", "GITHUB_TOKEN": "x",
            "GH_TOKEN": "y", "AWS_SECRET_ACCESS_KEY": "z", "API_KEY": "q",
        })
        self.assertEqual(env, {"PATH": "C:\\tools", "SystemRoot": "C:\\Windows"})

    def test_changed_files_never_stages_changes(self):
        with patch.object(runner.subprocess, "check_output", side_effect=[
                b"scripts/a.py\0", b"scripts/b.py\0"]) as output:
            self.assertEqual(runner.changed_files("unused", "abc"),
                             ["scripts/a.py", "scripts/b.py"])
            self.assertEqual(output.call_count, 2)
            self.assertTrue(all("add" not in call.args[0] for call in output.call_args_list))

    def test_permission_denied_git_probe_fails_without_claim_or_provider(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory) / "repo"
            root.mkdir()
            issue = {"number": 17, "title": "isolated probe", "body": ""}
            meta = {"base": "main", "paths": ["scripts/" ]}
            with patch.object(runner, "git", side_effect=PermissionError("fetch denied")), \
                 patch.object(runner, "add_label") as add_label, \
                 patch.object(runner, "remove_label") as remove_label, \
                 patch.object(runner, "comment") as comment, \
                 patch.object(runner.subprocess, "run") as process:
                runner.run_one("owner/repo", "dummy-token", root, "gemini",
                               {"protected_patterns": [], "agents": {"gemini": {
                                   "command": ["dummy"]}}}, issue, meta)
            add_label.assert_not_called()
            remove_label.assert_not_called()
            comment.assert_not_called()
            process.assert_not_called()

    def test_workflow_scopes_write_permission_to_manual_bootstrap(self):
        workflow = (Path(__file__).resolve().parents[1] /
                    ".github/workflows/agent-queue.yml").read_text(encoding="utf-8")
        self.assertIn("issues: read", workflow)
        self.assertIn("issues: write", workflow)
        self.assertIn("if: github.event_name == 'schedule' || inputs.mode == 'scan'", workflow)
        self.assertIn("if: github.event_name == 'workflow_dispatch' && inputs.mode == 'bootstrap'",
                      workflow)
        self.assertNotIn("issues: write", workflow.split("jobs:", 1)[0])


if __name__ == "__main__":
    unittest.main()
