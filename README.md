# roa-texture-loader
[![Release](https://badgen.net/github/release/raicool/roa-texture-loader)](https://github.com/raicool/roa-texture-loader/releases)
[![Maintenance](https://img.shields.io/badge/Maintained%3F-yes-green.svg)](https://github.com/raicool/roa-texture-loader/graphs/commit-activity)
[![GitHub last commit](https://badgen.net/github/last-commit/raicool/roa-texture-loader/main)](https://github.com/raicool/roa-texture-loader/commits/main)

custom texture loader for rivals of aether 1 

> Works through versions 1.4.0 - 2.1.9.1

#### Wiki page for help creating your own texture pack: https://github.com/raicool/roa-texture-loader/wiki/Getting-Started

###### requires [roa-mod-loader](https://github.com/raicool/roa-mod-loader) to be installed to Rivals of Aether's game directory

<img width="832" height="478" alt="image" src="https://github.com/user-attachments/assets/16d89cb6-dfc6-449e-9acd-107515980068" />

# planned future features
- [ ] add custom sound effects to attack windows (e.g. normals, taunts, articles, etc)

# building & installing
requires Visual Studio 2022 in order to be built (older visual studio versions most likely work but i have not tested them)

1. clone the repository as well as submodules:
```sh
	git clone https://github.com/raicool/roa-texture-loader --recurse-submodules
	cd roa-texture-loader
```
2. using Visual Studio's x86 Dev command prompt *vcvars32.bat* run
```sh
	cmake -G "Ninja" -DCMAKE_SYSTEM_PROCESSOR=i386 -B bin/
	ninja -C bin/
```
3. copy the dll `bin/roa-texture-loader.dll` into the mods directory of rivals of aether, e.g `C:/Program Files (x86)/Steam/steamapps/common/Rivals of Aether/mods/`
	- if the mods directory doesnt already exists, create one
	- copy the roa-texture-loader folder into the mods folder as well
