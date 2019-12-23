# inputstream.ffmpegarchive addon for Kodi

This is a [Kodi](http://kodi.tv) input stream addon for FFmpeg Archive.

[![Build Status](https://travis-ci.org/xbmc/inputstream.ffmpegarchive.svg?branch=master)](https://travis-ci.org/xbmc/inputstream.ffmpegarchive)
[![Build Status](https://ci.appveyor.com/api/projects/status/github/xbmc/inputstream.ffmpegarchive?svg=true)](https://ci.appveyor.com/project/xbmc/inputstream-ffmpegarchive)

## Build instructions

### Linux

1. `git clone --branch master https://github.com/xbmc/xbmc.git`
2. `git clone https://github.com/phunkyfish/inputstream.ffmpegarchive.git`
3. `cd inputstream.ffmpegarchive && mkdir build && cd build`
4. `cmake -DADDONS_TO_BUILD=inputstream.ffmpegarchive -DADDON_SRC_PREFIX=../.. -DCMAKE_BUILD_TYPE=Debug -DCMAKE_INSTALL_PREFIX=../../xbmc/build/addons -DPACKAGE_ZIP=1 ../../xbmc/cmake/addons`
5. `make`

The addon files will be placed in `../../xbmc/build/addons` so if you build Kodi from source and run it directly the addon will be available as a system addon.

### Mac OSX

In order to build the addon on mac the steps are different to Linux and Windows as the cmake command above will not produce an addon that will run in kodi. Instead using make directly as per the supported build steps for kodi on mac we can build the tools and just the addon on it's own. Following this we copy the addon into kodi. Note that we checkout kodi to a separate directory as this repo will only only be used to build the addon and nothing else.

#### Build tools and initial addon build

1. Get the repos
 * `cd $HOME`
 * `git clone https://github.com/xbmc/xbmc xbmc-addon`
 * `git clone https://github.com/phunkyfish/inputstream.ffmpegarchive`
2. Build the kodi tools
 * `cd $HOME/xbmc-addon/tools/depends`
 * `./bootstrap`
 * `./configure --host=x86_64-apple-darwin`
 * `make -j$(getconf _NPROCESSORS_ONLN)`
3. Build the addon
 * `cd $HOME/xbmc-addon`
 * `make -j$(getconf _NPROCESSORS_ONLN) -C tools/depends/target/binary-addons ADDONS="inputstream.ffmpegarchive" ADDON_SRC_PREFIX=$HOME`

Note that the steps in the following section need to be performed before the addon is installed and you can run it in Kodi.

#### To rebuild the addon and copy to kodi after changes (after the initial addon build)

1. `cd $HOME/inputstream.ffmpegarchive`
2. `./build-install-mac.sh ../xbmc-addon`

If you would prefer to run the rebuild steps manually instead of using the above helper script check the appendix [here](#manual-steps-to-rebuild-the-addon-on-macosx)

## Appendix

### Manual Steps to rebuild the addon on MacOSX

The following steps can be followed manually instead of using the `build-install-mac.sh` in the root of the addon repo after the [initial addon build](#build-tools-and-initial-addon-build) has been completed.

**To rebuild the addon after changes**

1. `rm tools/depends/target/binary-addons/.installed-macosx*`
2. `make -j$(getconf _NPROCESSORS_ONLN) -C tools/depends/target/binary-addons ADDONS="inputstream.ffmpegarchive" ADDON_SRC_PREFIX=$HOME`

or

1. `cd tools/depends/target/binary-addons/macosx*`
2. `make`

**Copy the addon to the Kodi addon directory on Mac**

1. `rm -rf "$HOME/Library/Application Support/Kodi/addons/inputstream.ffmpegarchive"`
2. `cp -rf $HOME/xbmc-addon/addons/inputstream.ffmpegarchive "$HOME/Library/Application Support/Kodi/addons"`