# Earth & Sky 1.0

Release candidate — not yet published. Intended tag: `v1.0.0`.

Earth & Sky is an offline Windows/Linux desktop application for exploring Earth and the sky, with flat-map and globe views, daylight/twilight, the Sun, Moon, five planets and all 88 constellations.

## New since v0.1.1

- Direct local/UTC date and time selection (Ctrl+T), strict validation, and Live/Selected/Playing status.
- A separate Now button that restores live time while preserving the selected location and view.
- My Location information: Sun, Moon, Mercury, Venus, Mars, Jupiter and Saturn rise/set times, current horizon status, daylight duration and three twilight bands.
- Polar-day/night and daylight-saving transition handling. Event times explicitly use the computer's time zone.
- Clean PNG/JPEG export (Ctrl+S), with only tiny, subdued image credits. Confirmation clears on any click or after five seconds.
- Sharp constellation rendering at maximum flat-map zoom, subdued gray names, bright body labels, collision avoidance and no label connector lines.
- Clearer controls, keyboard navigation and consolidated documentation.
- Automated regression coverage for astronomy, navigation, observer events, rendering, screenshot export and error handling.

## Intended release assets

- `Earth-and-Sky-1.0.0-Windows-x64-Setup.exe`
- `Earth-and-Sky-1.0.0-Linux-amd64.deb`
- `Earth-and-Sky-1.0.0-Linux-x86_64.AppImage`
- `Earth-and-Sky-1.0.0-Source.zip`
- Matching `.sha256` files, a SHA256SUMS manifest and build provenance.

Only upload assets built and validated from the recorded release source. The final published notes must list only assets actually attached. Windows builds are unsigned. No macOS, ARM64 or older-Linux support is claimed.

## Notes

Rise/set and the simulation clock use the computer's time zone, not the selected geographic location's zone. Above-horizon status does not guarantee naked-eye visibility. My Horizon is geometric; event calculations use standard refraction and zero observer elevation. Terrain and weather are not modeled. Ground imagery is a fixed NASA composite, not live satellite imagery.

Application code is GPL-3.0-or-later. Third-party components retain their licenses. The application source ZIP does not include Qt/dependency source; complete the corresponding-source distribution plan before publishing.
