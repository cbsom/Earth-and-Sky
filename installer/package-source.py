#!/usr/bin/env python3
# SPDX-License-Identifier: GPL-3.0-or-later
"""Archive current application sources, assets, notices, and build instructions."""
from pathlib import Path
from zipfile import ZipFile, ZIP_DEFLATED
import hashlib
import re
import subprocess

root = Path(__file__).resolve().parent.parent
version = re.search(r'^#define AppVersion "([^\"]+)"', (root / "installer/Earth-and-Sky.iss").read_text(encoding="utf-8"), re.M)[1]
files = set()
for name in ["LICENSE", "THIRD-PARTY-NOTICES.txt", "README.md", "RELEASE-CHECKLIST.md", ".gitattributes"]:
    files.add(root / name)
for name in ["outputs/Earth-and-Sky/source", "outputs/Earth-and-Sky/screenshots", "installer", ".github/workflows"]:
    for path in (root / name).rglob("*"):
        rel = path.relative_to(root / name)
        if any(part.startswith(("build", ".", "__pycache__")) for part in rel.parts):
            continue
        if path.is_file():
            files.add(path)
for name in ["README.md", "LICENSE", "THIRD-PARTY-NOTICES.txt"]:
    files.add(root / "outputs/Earth-and-Sky" / name)
# The prefix rule above excludes build scripts too; explicitly retain them.
files.update(p for p in (root / "installer").rglob("build*") if p.is_file() and p.suffix in {".cmd", ".sh"})
files.add(root / "outputs/Earth-and-Sky/source/build.cmd")
output = root / "outputs/installers" / f"Earth-and-Sky-{version}-Source.zip"
output.parent.mkdir(parents=True, exist_ok=True)
try:
    commit = subprocess.check_output(["git", f"--git-dir={root / '.git'}", f"--work-tree={root}", "rev-parse", "HEAD"], stderr=subprocess.DEVNULL, text=True).strip()
except (OSError, subprocess.CalledProcessError):
    commit = "unavailable"
prefix = f"Earth-and-Sky-{version}-Source/"
manifest = []
with ZipFile(output, "w", ZIP_DEFLATED) as archive:
    for path in sorted(files):
        relative = path.relative_to(root).as_posix()
        data = path.read_bytes()
        archive.writestr(prefix + relative, data)
        manifest.append(f"{hashlib.sha256(data).hexdigest()}  {relative}")
    archive.writestr(prefix + "SOURCE-MANIFEST.sha256", "\n".join(manifest) + "\n")
    archive.writestr(prefix + "SOURCE-SNAPSHOT.txt", f"Base commit: {commit}\nThis archives current working files, including local changes.\nUse the hashes to identify the snapshot; matching an older binary is not implied.\nQt/dependency source must be supplied separately where required.\n")
checksum = hashlib.sha256(output.read_bytes()).hexdigest()
output.with_suffix(".zip.sha256").write_text(f"{checksum}  {output.name}\n", encoding="utf-8")
print(output)
