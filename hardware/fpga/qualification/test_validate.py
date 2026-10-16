"""Smoke tests for qualification status handling."""
import json
import subprocess
import sys
import tempfile
import unittest
from pathlib import Path

HERE = Path(__file__).resolve().parent

class QualificationTests(unittest.TestCase):
    def run_validator(self, changes):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            for filename in ("candidates.json", "requirements.json", "validate.py"):
                (root / filename).write_bytes((HERE / filename).read_bytes())
            path = root / "candidates.json"
            data = json.loads(path.read_text())
            data["candidates"][0].update(changes)
            path.write_text(json.dumps(data))
            return subprocess.run([sys.executable, str(root / "validate.py")],
                                  capture_output=True, text=True)

    def test_missing_evidence_is_pending(self):
        result = self.run_validator({})
        self.assertEqual(result.returncode, 0)
        self.assertIn("artix7: PENDING", result.stdout)

    def test_insufficient_pins_fails(self):
        result = self.run_validator({"usable_io": 200})
        self.assertNotEqual(result.returncode, 0)
        self.assertIn("artix7: FAIL", result.stdout)

    def test_explicit_timing_failure_fails(self):
        result = self.run_validator({"measurements": {"timing_pass": False}})
        self.assertNotEqual(result.returncode, 0)
        self.assertIn("artix7: FAIL", result.stdout)

if __name__ == "__main__":
    unittest.main()
