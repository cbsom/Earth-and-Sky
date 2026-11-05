# Release preparation

## Source and licensing

- Original application: GPL-3.0-or-later. Retain LICENSE and THIRD-PARTY-NOTICES.txt in source and packages.
- City data: Simplemaps free Basic, CC BY 4.0, with attribution and reduced-column change notice. Other component licenses remain in force.
- Build from a recorded source commit and record any local changes. `python installer/package-source.py` archives current application source, assets, vendor code, notices and build/package scripts with file hashes. A new snapshot does not establish a match with older published binaries.
- For bundled Qt and other copyleft dependencies, arrange compliant access to exact corresponding source, patches and necessary build information. For online downloads, one option is hosting corresponding source archives alongside binaries with clear links. An upstream-only link or license text is not a complete source-distribution plan. Inspect all Qt SBOM/component notices and retain Qt replacement/relinking rights. The application source archive does not include Qt source or the SDK.
- Record the Qt kit version, compiler, Linux base, dependency patches and packaging-tool hashes for each binary.

## v0.1.0 audit — 2026-11-05

The GitHub release targets `676dea0fff72493baaf92d32da5e6c9dd9b0d42a` and is titled `Release v2026.11.03-11`. It contains the Windows installer, Linux .deb, AppImage, Linux checksums, and older compiler-specific archives.

The Linux validation report identifies a later working tree based on `fb90df4`. Verify exact source snapshots for each published binary: the automatic GitHub source ZIP at the tag is not yet established as a match for every asset. Do not move the tag or silently replace assets. Prefer a clearly identified follow-up release built from one committed source snapshot with these notices.

## Download experience

Use title `Earth & Sky v0.1.0` and the prepared `installer/RELEASE-NOTES-v0.1.0.md` after confirming it matches the assets. Put setup, .deb and AppImage first. Explain that older compiler-specific archives are developer builds. Provide checksums and unambiguous matching-source links.

## Desktop validation

- Windows installation, startup, upgrades, saved settings, uninstall and unchanged Qt DLL handling.
- Mint 22 / Ubuntu 24.04 .deb installation/menu launch/removal; AppImage, settings, animations, zoom and graphics.
- At least one clean machine without Qt SDK. Older Linux and ARM64 remain unvalidated.

These local preparation files do not alter any published release, tag or asset.
