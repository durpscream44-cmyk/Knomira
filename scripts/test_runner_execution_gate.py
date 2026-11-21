"""The worker must reject execution before reading credentials or selecting a task."""
import contextlib
import io
import sys
import unittest
from unittest.mock import patch

import local_agent_runner


class ExecutionGateTests(unittest.TestCase):
    def test_execute_fails_closed_before_environment_or_github(self):
        stderr = io.StringIO()
        with patch.object(sys, "argv", ["local_agent_runner.py", "--agent", "kilo", "--execute"]), \
             patch.object(local_agent_runner, "git", side_effect=AssertionError("git called")), \
             patch.object(local_agent_runner, "choose", side_effect=AssertionError("GitHub called")), \
             contextlib.redirect_stderr(stderr):
            with self.assertRaises(SystemExit) as result:
                local_agent_runner.main()
        self.assertEqual(result.exception.code, 2)
        self.assertIn("--execute is disabled", stderr.getvalue())


if __name__ == "__main__":
    unittest.main()
