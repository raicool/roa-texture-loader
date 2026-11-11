
import shutil
import subprocess


BIN_DIRECTORY: str = "c:/opt/repository/roa-texture-loader/bin/x86-Release"
RIVALS_DIRECTORY: str = "C:/rivals_re_project/roa-offline"

def main():
	shutil.copyfile(f"{BIN_DIRECTORY}/roa-texture-loader.dll", f"{RIVALS_DIRECTORY}/mods/roa-texture-loader.dll")

if (__name__ == "__main__"):
	main()