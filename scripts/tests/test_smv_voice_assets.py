"""Minimal host tests for validating staged human-recorded announcements."""
import importlib.util
import shutil
import subprocess
import tempfile
import unittest
from pathlib import Path

MODULE = Path(__file__).resolve().parents[1] / "smv_voice_assets.py"
spec = importlib.util.spec_from_file_location("smv_voice_assets", MODULE)
voice = importlib.util.module_from_spec(spec)
spec.loader.exec_module(voice)


class VoiceAssetsTest(unittest.TestCase):
    def test_missing_inputs_do_not_touch_existing_assets(self):
        with tempfile.TemporaryDirectory() as td:
            root = Path(td)
            output = root / "voice"
            output.mkdir()
            unchanged = output / "smv_01.ogg"
            unchanged.write_bytes(b"unmodified")
            with self.assertRaises(FileNotFoundError):
                voice.prepare(root / "input", output)
            self.assertEqual(unchanged.read_bytes(), b"unmodified")

    @unittest.skipUnless(shutil.which("ffmpeg") and shutil.which("ffprobe"), "requires ffmpeg/ffprobe")
    def test_16_recordings_produce_24k_mono_60ms_opus(self):
        with tempfile.TemporaryDirectory() as td:
            root = Path(td)
            sources, output = root / "records", root / "voice"
            sources.mkdir()
            sample = root / "example.wav"
            subprocess.run(["ffmpeg", "-v", "error", "-y", "-f", "lavfi", "-i",
                            "sine=frequency=440:duration=0.48", "-ar", "32000", "-ac", "1", str(sample)], check=True)
            for i in range(1, voice.COUNT + 1):
                shutil.copyfile(sample, sources / f"smv_{i:02d}.wav")
            voice.prepare(sources, output)
            self.assertEqual(len(list(output.glob("*.ogg"))), voice.COUNT)
            for i in range(1, voice.COUNT + 1):
                path = output / f"smv_{i:02d}.ogg"
                self.assertEqual(voice.inspect_audio(path)["codec"], "opus")
                self.assertGreater(path.stat().st_size, 100)


if __name__ == "__main__":
    unittest.main()
