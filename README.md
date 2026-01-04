# roa-texture-loader
[![Release](https://badgen.net/github/release/raicool/roa-texture-loader)](https://github.com/raicool/roa-texture-loader/releases)
[![Maintenance](https://img.shields.io/badge/Maintained%3F-yes-green.svg)](https://github.com/raicool/roa-texture-loader/graphs/commit-activity)
[![GitHub last commit](https://badgen.net/github/last-commit/raicool/roa-texture-loader/main)](https://github.com/raicool/roa-texture-loader/commits/main)

custom texture loader for rivals of aether 1 
###### requires [roa-mod-loader](https://github.com/raicool/roa-mod-loader) to be installed to Rivals of Aether's game directory

<img width="2391" height="1205" alt="RivalsofAether_GVG7mwkqPg" src="https://github.com/user-attachments/assets/b9a4a9a4-7ed2-470e-9bfc-3c869ef7f09e"/>

# building & installing
requires Visual Studio 2022 in order to be built (older visual studio versions most likely work but i have not tested them)

1. clone the repository as well as submodules:
```
	git clone https://github.com/raicool/roa-texture-loader --recurse-submodules
	cd roa-texture-loader
```
2. using Visual Studio's x86 Dev command prompt *vcvars32.bat* run
```
	cmake -G "Ninja" -DCMAKE_SYSTEM_PROCESSOR=i386 -B bin/
	ninja -C bin/
```
3. copy the dll `bin/roa-texture-loader-XXXXXX-MSVC.dll` into the mods directory of rivals of aether, e.g `C:/Program Files (x86)/Steam/steamapps/common/Rivals of Aether/mods/`
	- if the mods directory doesnt already exists, create one
	- copy the roa-hook folder into the mods folder as well
