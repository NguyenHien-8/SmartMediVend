"""Run the real MUX relay driver against GPIO/timer doubles on the host."""

import os
import shlex
import subprocess
import tempfile
import unittest
from pathlib import Path


ROOT = Path(__file__).resolve().parents[2]
TESTS = Path(__file__).resolve().parent


class SmvRelayTest(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.temp = tempfile.TemporaryDirectory()
        cls.addClassCleanup(cls.temp.cleanup)
        cls.executable = Path(cls.temp.name) / "smv_relay_test.exe"
        command = shlex.split(os.environ.get("CXX", "c++")) + [
            "-std=c++17", "-Wall", "-Wextra", "-Werror", "-pthread",
            f"-I{TESTS / 'smv_relay_stubs'}",
            f"-I{ROOT / 'main/boards/smartmedivend-s3'}",
            str(TESTS / "smv_relay_host_test.cpp"), "-o", str(cls.executable),
        ]
        subprocess.run(command, check=True)

    def run_case(self, case):
        result = subprocess.run([str(self.executable), case], capture_output=True, text=True)
        self.assertEqual(result.returncode, 0, result.stdout + result.stderr)

    def test_all_16_channels_receive_low_for_500ms_then_high(self):
        self.run_case("channels")

    def test_invalid_or_uninitialized_requests_do_not_drive_outputs(self):
        self.run_case("invalid")

    def test_busy_request_does_not_switch_channel_or_extend_pulse(self):
        self.run_case("busy")

    def test_timer_start_failure_immediately_turns_relay_off(self):
        self.run_case("start_failure")

    def test_timer_creation_failure_keeps_relay_off(self):
        self.run_case("create_failure")


if __name__ == "__main__":
    unittest.main()
