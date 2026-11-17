import unittest

from agent_queue import base_branch, eligible, issue_labels, priority, session_from_comments


class QueueTests(unittest.TestCase):
    def test_base_branch_simple(self):
        self.assertEqual(base_branch("Base branch: `completion/play01e-colony-roundtrip`"),
                         "completion/play01e-colony-roundtrip")

    def test_base_branch_issue_form(self):
        self.assertEqual(base_branch("### Base branch\n\nfeature/playable-db-embark-integration\n"),
                         "feature/playable-db-embark-integration")

    def test_bad_branches_rejected(self):
        for value in ("../master", "a//b", ".hidden", "name with spaces", ""):
            self.assertIsNone(base_branch("Base branch: " + value))

    def test_only_approved_and_ready(self):
        issue = {"number": 3, "state": "open", "labels": [
            {"name": n} for n in ("agent:approved", "agent:ready", "agent:provider:jules")
        ]}
        self.assertTrue(eligible(issue))
        issue["labels"].append({"name": "agent:running"})
        self.assertFalse(eligible(issue))
        issue["labels"].pop()
        issue["state"] = "closed"
        self.assertFalse(eligible(issue))

    def test_unapproved_not_ready(self):
        issue = {"state": "open", "labels": [{"name": "agent:ready"}]}
        self.assertFalse(eligible(issue))

    def test_jules_session_marker(self):
        self.assertEqual(session_from_comments([
            {"body": "not a Jules marker"},
            {"body": "<!-- knomira-jules-session: sessions/123-xyz -->\nClaimed"},
        ]), "sessions/123-xyz")

    def test_priority(self):
        base = {"number": 2, "created_at": "2026-11-01", "labels": []}
        urgent = {"number": 3, "created_at": "2026-11-02",
                  "labels": [{"name": "priority:P0"}]}
        self.assertLess(priority(urgent), priority(base))


if __name__ == "__main__":
    unittest.main()
