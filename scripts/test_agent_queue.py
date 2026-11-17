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


if __name__ == "__main__":
    unittest.main()
