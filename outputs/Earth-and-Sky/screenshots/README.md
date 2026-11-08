# Documentation screenshots

Captured from the Windows v0.1.1 application (file version 0.1.1.0) on 2026-11-08.
All images are 1280 x 752 captures of the application content, including its controls and version label.

| File | View | Constellations |
| --- | --- | --- |
| Earth-and-Sky-Screenshot-1.jpg | Flat map | Zodiac |
| Earth-and-Sky-Screenshot-2.jpg | Globe | Zodiac |
| flat-map.png | Flat map | All constellations |
| globe.png | Globe | All constellations |

Moon, planets, latitude lines, and labels are enabled. No home city is selected in these captures. The displayed time is live, so subsequent captures will show different sky positions and lighting.

The application supports `Earth-and-Sky.exe --capture PATH` to save its rendered window and exit; `--globe` selects globe mode. Flat-map captures use the saved flat-map view. The file extension selects the output image format. Use the normal Windows Qt platform plugin supplied with the package. Preserve and restore personal preferences when preparing new captures.
