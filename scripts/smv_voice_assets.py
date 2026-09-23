#!/usr/bin/env python3
"""Prepare 16 *human-recorded* Vietnamese announcement clips for the SMV board.

Usage:
  python scripts/smv_voice_assets.py --input recordings --output main/boards/smartmedivend-s3/voice

Requires recordings/smv_01.wav ... smv_16.wav. The recording must already
contain exactly the desired spoken announcement; this tool does NOT synthesize
speech or make a robotic voice sound like a human recording.
"""
from __future__ import annotations

import argparse
import json
import shutil
import subprocess
import tempfile
from pathlib import Path

COUNT = 16
MIN_SECONDS = 0.35
MAX_SECONDS = 22.0


def _run(*args: str) -> str:
    completed = subprocess.run(args, check=True, capture_output=True, text=True)
    return completed.stdout


def inspect_audio(path: Path) -> dict:
    report = json.loads(_run("ffprobe", "-v", "error", "-show_streams", "-show_format", "-of", "json", str(path)))
    streams = [s for s in report["streams"] if s.get("codec_type") == "audio"]
    if len(streams) != 1:
        raise ValueError(f"{path}: expected exactly one audio stream")
    duration = float(report["format"].get("duration", "0"))
    if not MIN_SECONDS <= duration <= MAX_SECONDS:
        raise ValueError(f"{path}: duration {duration:.2f}s outside {MIN_SECONDS}-{MAX_SECONDS}s")
    return {"duration": duration, "codec": streams[0]["codec_name"], "channels": streams[0]["channels"]}


def prepare(input_dir: Path, output_dir: Path) -> None:
    paths = [input_dir / f"smv_{i:02d}.wav" for i in range(1, COUNT + 1)]
    missing = [p.name for p in paths if not p.is_file()]
    if missing:
        raise FileNotFoundError("Missing *human-recorded* WAV files: " + ", ".join(missing))
    if shutil.which("ffmpeg") is None or shutil.which("ffprobe") is None:
        raise RuntimeError("ffmpeg and ffprobe must be installed and on PATH")
    # Validate ALL sources before touching the live firmware assets.
    for path in paths:
        info = inspect_audio(path)
        if info["codec"] not in ("pcm_s16le", "pcm_s24le", "pcm_s32le", "pcm_f32le", "pcm_f64le"):
            raise ValueError(f"{path}: supply uncompressed PCM WAV (found {info['codec']})")
    output_dir.mkdir(parents=True, exist_ok=True)
    with tempfile.TemporaryDirectory(prefix="smv_voice_") as tmp:
        staged = Path(tmp)
        for i, src in enumerate(paths, start=1):
            dst = staged / f"smv_{i:02d}.ogg"
            # Keep speech intelligibility and headroom for the MAX98357A; 
            # do NOT change pitch, tempo or apply synthetic voice processing.
            _run("ffmpeg", "-hide_banner", "-loglevel", "error", "-nostdin", "-y", "-i", str(src),
                 "-vn", "-af", "highpass=f=75,lowpass=f=9000,alimiter=limit=0.65:level=false",
                 "-ar", "24000", "-ac", "1", "-c:a", "libopus", "-b:a", "40k",
                 "-application", "voip", "-frame_duration", "60", "-vbr", "on", str(dst))
            info = inspect_audio(dst)
            if info["codec"] != "opus" or info["channels"] != 1:
                raise ValueError(f"{dst}: invalid output codec")
            blob = dst.read_bytes()
            offset = blob.find(b"OpusHead")
            if offset < 0 or int.from_bytes(blob[offset + 12:offset + 16], "little") != 24000:
                raise ValueError(f"{dst}: missing 24kHz OpusHead input sample-rate field")
            durations = _run("ffprobe", "-v", "error", "-select_streams", "a:0", "-show_packets",
                             "-show_entries", "packet=duration_time", "-of", "csv=p=0", str(dst)).splitlines()
            if not durations or any(abs(float(d.split(",", 1)[0]) - 0.06) > 0.0001 for d in durations[:-1]) or not (0 < float(durations[-1].split(",", 1)[0]) <= 0.0601):
                raise ValueError(f"{dst}: expected exclusively 60ms Opus packets")
            print(f"{dst.name}: {info['duration']:.2f}s, 24kHz/mono Opus, 60ms packets")
        # Stage all 16 before replacing any asset. Use replace to avoid torn individual files.
        for i in range(1, COUNT + 1):
            filename = f"smv_{i:02d}.ogg"
            (staged / filename).replace(output_dir / filename)
    print(f"Prepared {COUNT} local announcements in {output_dir}")


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--input", type=Path, required=True, help="folder holding smv_01.wav ... smv_16.wav")
    parser.add_argument("--output", type=Path, required=True, help="board voice/ folder")
    args = parser.parse_args()
    prepare(args.input, args.output)


if __name__ == "__main__":
    main()
