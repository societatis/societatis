#### Master Build Status
[![Build Status](https://github.com/societatis/societatis/workflows/Build/badge.svg?branch=master)](https://github.com/societatis/societatis/actions)


# Table of contents
1. [Project Specs](#coinspecs)
2. [How to Compile Societatis](#howtocompile)
    1. [Societatis for Linux](#build-linux)
    2. [Societatis for Windows](#build-windows)
    3. [Societatis for macOS](#build-apple)
    4. [Societatis for Android](#build-android)
    5. [Societatis for FreeBSD](#build-freebsd)
3. [Downloads](#downloads)
4. [Useful Links](#usefullinks)

## Installing <a name="installing"></a>
We offer binary images of the latest releases here: https://releases.societatis.io

If you would like to compile yourself, read on.

### Downloads <a name="downloads"></a>
| Operating System   | Download                                 |
| ----------------   | ---------------------------------------- |
| Windows 32         | https://releases.societatis.io/get/cli/latest/win32 |
| Windows 64         | https://releases.societatis.io/get/cli/latest/win64 |
| macOS 10.8 & Later | https://releases.societatis.io/get/cli/latest/macos |
| Linux DEB          | https://releases.societatis.io/get/cli/latest/linux/deb |
| Linux RPM          | https://releases.societatis.io/get/cli/latest/linux/rpm |
| Android            | https://releases.societatis.io/get/cli/latest/arm64 |
| Other platforms    | https://releases.societatis.io|

### Coin Specs <a name="coinspecs"></a>
<table>
<tr><td>Ticker Symbol</td><td>SCTS</td></tr>
<tr><td>Algorithm</td><td>Cryptonight</td></tr>
<tr><td>Type</td><td>Proof of Work (PoW)</td></tr>
<tr><td>Block Time</td><td>60 Seconds</td></tr>
<tr><td>Premine</td><td>0.0 %</td></tr>
<tr><td>Decimals</td><td>8 Digits</td></tr>
<tr><td>Block Reward</td><td>Decrease by each block</td></tr>
<tr><td>Max Coin Supply </td><td>xxx,xxx,xxx,xxx SCTS</td></tr>
<tr><td>P2P | RPC Port</td><td>7294 | 7295</td></tr>
</table>

More information at [societatis.io](https://societatis.io/)

# How To Compile <a name="howtocompile"></a>

#### Linux  <a name="build-linux"></a>

##### Prerequisites

- You will need the following packages: build-essential, [cmake (3.10 or higher)](https://docs.qwertycoin.org/developer/compiling-from-source/untitled) and git;
- Most of these should already be installed on your system. For example on Ubuntu by running:
```
sudo apt-get install build-essential cmake git
```

##### Building

- After installing dependencies run simple script:
```
git clone --recurse-submodules https://github.com/societatis/societatis
cd ./societatis
mkdir ./build
cd ./build
cmake -DBUILD_ALL:BOOL=TRUE ..
cmake --build . --config Release
```
- If all went well, it will complete successfully, and you will find all your binaries in the `./build/src` directory.

#### Windows 10 <a name="build-windows"></a>

##### Prerequisites

- Install [Visual Studio 2017 Community Edition](https://www.visualstudio.com/thank-you-downloading-visual-studio/?sku=Community&rel=15&page=inlineinstall);
- When installing Visual Studio, it is **required** that you install **Desktop development with C++** and the **VC++ v140 toolchain** when selecting features. The option to install the v140 toolchain can be found by expanding the "Desktop development with C++" node on the right. You will need this for the project to build correctly;
- Make sure that bundled cmake version is 3.10 or higher.

##### Building

- From the start menu, open "x64 Native Tools Command Prompt for vs2017";
- And the run the following commands:
```
git clone https://github.com/societatis/societatis
cd societatis
md build
cd build
cmake -G "Visual Studio 16 2019" -X x64 -DBUILD_WITH_TOOLS:BOOL=TRUE .. 
cmake --build . --config Release
```
- If all went well, it will complete successfully, and you will find all your binaries in the `.\build\src\Release` directory;
- Additionally, a `.sln` file will have been created in the `build` directory. If you wish to open the project in Visual Studio with this, you can.

#### Apple macOS <a name="build-apple"></a>

##### Prerequisites

- Install Xcode and Developer Tools;
- Install [cmake](https://cmake.org/). See [here](https://stackoverflow.com/questions/23849962/cmake-installer-for-mac-fails-to-create-usr-bin-symlinks) if you are unable to call `cmake` from the terminal after installing;
- Install git.

##### Building

- After installing dependencies run simple script:
```
git clone https://github.com/societatis/societatis
cd ./societatis
mkdir ./build
cd ./build
cmake -DBUILD_ALL:BOOL=TRUE ..
cmake --build . --config Release
```
- If all went well, it will complete successfully, and you will find all your binaries in the `./build/src` directory.

#### Android (building on Linux) <a name="build-android"></a>

##### Prerequisites

- You will need the following packages: build-essential, cmake (3.10 or higher), git, unzip and wget;
- Most of these should already be installed on your system. For example on Ubuntu by running:
```
sudo apt-get install build-essential cmake git unzip wget
```
- Download and extract Android NDK:
```
mkdir -p "$HOME/.android"
wget -O "$HOME/.android/android-ndk-r18b-linux-x86_64.zip" "https://dl.google.com/android/repository/android-ndk-r18b-linux-x86_64.zip"
unzip -qq "$HOME/.android/android-ndk-r18b-linux-x86_64.zip" -d "$HOME/.android"
export ANDROID_NDK_r18b="$HOME/.android/android-ndk-r18b"
```

##### Building

- After installing dependencies run simple script:
```
git clone https://github.com/societatis/societatis
cd ./societatis
mkdir ./build
cd ./build
cmake -DCMAKE_TOOLCHAIN_FILE=cmake/polly/android-ndk-r18b-api-21-x86-clang-libcxx.cmake -DBUILD_ALL:BOOL=TRUE -DBUILD_WITH_TESTS:BOOL=FALSE -DSTATIC=ON -DBUILD_64=OFF -DANDROID=true -DBUILD_TAG="android" ..
cmake --build . --config Release
```
- If all went well, it will complete successfully, and you will find all your binaries in the `./build/src` directory.

#### FreeBSD  <a name="build-freebsd"></a>

##### Prerequisites

- You will need the following packages: cmake (3.10 or higher) and git;
- Most of these should already be installed on your system. For example on FreeBSD by running:
```
sudo pkg install cmake git
```

##### Building

- After installing dependencies run simple script:
```
git clone --recurse-submodules https://github.com/societatis/societatis
cd ./societatis
mkdir ./build
cd ./build
cmake -DBUILD_ALL:BOOL=TRUE ..
cmake --build . --config Release
```
- If all went well, it will complete successfully, and you will find all your binaries in the `./build/src` directory.
