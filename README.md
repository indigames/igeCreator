# igeCreator - The Game Editor from Indi Games

## Requirements
### Windows
1. Visual Studio 2019
1. CMake

### MacOS
1. XCode
2. CMake

## Source code

The project includes sub-modules from other repository. When cloning, be sure to pass `--recursive` in the command.

```sh
    git clone https://github.com/indigames/igeCreator --recursive
```

Also, to update the sub-modules, run this

```sh
    git submodule update --init --recursive --remote
```

## Build instruction
### Windows
1. Build a Release version
Run command below:
    ```dos
        .\scripts\build-pc.bat
    ```
The release version of the application will be found in `app` folder.

2. Debugging
Run command below:
    ```dos
        .\script\gen-pc.bat
    ```
Open `build\pc\x64\igeCreator.sln` with Visual Studio 2019.

### MacOS
Support in next version, stay tunned.

### Linux
Support in next version, stay tunned.
