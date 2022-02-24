# CHOpt

A program to generate optimal Star Power paths for Clone Hero.

[![Build Status](https://ci.appveyor.com/api/projects/status/github/GenericMadScientist/CHOpt?branch=main&svg=true)](https://ci.appveyor.com/project/GenericMadScientist/CHOpt)

## Install

Download the latest version from the [Releases page](../../releases). If you're
on Windows, you will need to have installed the latest Visual Studio
Redistributable whose installer can be obtained
[here](https://aka.ms/vs/17/release/vc_redist.x64.exe).

## Usage

If you are unfamiliar with the intricacies of reading paths, read
[this guide](misc/How-to-read-paths.md).

CHOpt has two versions, a command-line version and a graphical version. The
graphical version has the same options as the command-line version and is
self-explanatory enough. As for the command-line version, an example usage to
path Trogdor on Hard and save the output to trogdor_path.png is

```bat
> CHOpt.exe -f trogdor.chart -d hard -o trogdor_path.png
```

Only the -f parameter is required, the difficulty defaults to Expert and the
path is by default saved to path.png. The full list of arguments can be found
by passing -h or --help to CHOpt, or by consulting the table below.

| Arguments               | Action                                                           |
| ----------------------- | ---------------------------------------------------------------- |
| -h, --help              | List optional arguments                                          |
| -f, --file              | Chart filename                                                   |
| -o, --output            | Filename of output image (.bmp or .png)                          |
| -d, --diff              | Difficulty (easy/medium/hard/expert)                             |
| -i, --instrument        | Instrument (guitar/coop/bass/rhythm/keys/ghl/ghlbass/drums)      |
| --sqz, --squeeze        | Set squeeze %                                                    |
| --ew, --early-whammy    | Set early whammy %                                               |
| --lazy, --lazy-whammy   | Set number of ms of whammy lost per sustain                      |
| --delay, --whammy-delay | Amount of ms after each activation before whammy can be obtained |
| --lag, --video-lag      | Video calibration, in ms                                         |
| -s, --speed             | Set speed the song is played at                                  |
| --no-double-kick        | Disable 2x kick (drums only)                                     |
| --no-kick               | Disable non-2x kicks (drums only)                                |
| --no-pro-drums          | Disable pro drums (drums only)                                   |
| --enable-dynamics       | Enables double points from ghost and accent notes (drums only)   |
| --engine                | Choose the engine (ch/rb/rb3)                                    |
| -p, --precision-mode    | Enable precision mode (CH only)                                  |
| -b, --blank             | Output a blank image without pathing                             |
| --no-image              | Do not create an image                                           |
| --no-bpms               | Do not draw BPMs                                                 |
| --no-solos              | Do not draw solo sections                                        |
| --no-time-sigs          | Do not draw time signatures                                      |
| --act-opacity           | Set opacity of activations in images                             |

If you would like to conveniently run CHOpt on a setlist and you happen to be
on Windows, I made a PowerShell script that I've put [here](misc/setlist.ps1).
Change the four variables then run the script. The simplest way to do that is
probably to open the folder the script is in, double click the top bar and type
in cmd then enter to open a command prompt in that folder, then run the command

```bat
> powershell -ExecutionPolicy Bypass -File setlist.ps1
```

## Dependencies

* [Boost](https://www.boost.org) for JSON, Nowide, Program Options and Test
* [Cairo](https://www.cairographics.org) for rasterisation
* [libpng](http://libpng.org/pub/png/libpng.html) to save pngs
* [Pango](https://pango.gnome.org) for drawing UTF-8 text
* [Qt 6](https://www.qt.io) for the GUI

In addition, we depend on their dependencies (e.g. zlib for libpng). libpng and
zlib need to be set up so that
[FindPNG](https://cmake.org/cmake/help/latest/module/FindPNG.html) can find
them, and the same is true for Boost and Qt (see
[this](https://cmake.org/cmake/help/latest/module/FindBoost.html) and
[this](https://cmake.org/cmake/help/latest/manual/cmake-qt.7.html) page for
details). Pango, Cairo, and their dependencies GLib and HarfBuzz do not have
modules included with CMake to find them. Look at [appveyor.yml](appveyor.yml)
to see how to find them.

## Acknowledgements

* FireFox2000000's Moonscraper .chart and .mid parsers were very helpful for
getting an initial idea of Clone Hero's parsing behaviour.
* Dinoguy1000 and shadoweh helped me make sure CHOpt runs on other peoples'
machines.
* Various users for feedback and testing, including 3-UP, CyclopsDragon,
DNelson, Generalguy26, GHNerd, Haggis, Joel, Jrh, Kyleruth, LightlessWalk,
Littlejth, Lucretio, RandomDays, RileyTheFox, Taka, Venxm, and Zantor.

## Contact

If you have any bug reports I would prefer they be reported on the GitHub page,
but feel free to send them to me on Discord at GMS#5303.
