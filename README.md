# inputstream.ffmpegarchive addon for Kodi

This is a [Kodi](http://kodi.tv) input stream addon for streams that can be opened by FFmpeg's libavformat, such as plain TS, HLS and DASH streams. Note that the only DASH streams supported are those without DRM.

The addon also has support for Archive/Catchup services where there is a replay windows (usually in days) and can timeshift across that span.

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

## Using the addon

The adding can be accessed like any other inputstream in Kodi. The following example will show how to manually choose this addon for playback when using IPTV Simple Client with the following entry in the M3U file.

```
#KODIPROP:inputstreamclass=inputstream.ffmpegarchive
#KODIPROP:inputstream.ffmpegarchive.mime_type=video/mp2t
#KODIPROP:inputstream.ffmpegarchive.is_realtime_stream=true
#KODIPROP:inputstream.ffmpegarchive.is_archive_stream=false
#EXTINF:-1,MyChannel
http://127.0.0.1:3002/mystream.ts
```

Note that the appropriate mime type should always be set. Here are the some common ones:
- TS: `video/mp2t`
- HLS: `application/x-mpegURL` or `application/vnd.apple.mpegurl`
- Dash: `application/dash+xml`

If enabling archive/catchup support there are a number of other variables that needs to be set as shown in this example.

```
#KODIPROP:inputstreamclass=inputstream.ffmpegarchive
#KODIPROP:inputstream.ffmpegarchive.mime_type=application/x-mpegURL
#KODIPROP:inputstream.ffmpegarchive.is_realtime_stream=true
#KODIPROP:inputstream.ffmpegarchive.is_archive_stream=true
#KODIPROP:inputstream.ffmpegarchive.playback_as_live=true
#KODIPROP:inputstream.ffmpegarchive.catchup_start_time=1111111
#KODIPROP:inputstream.ffmpegarchive.catchup_end_time=1111111
#KODIPROP:inputstream.ffmpegarchive.timeshift_buffer_start_time=1111111
#KODIPROP:inputstream.ffmpegarchive.timeshift_buffer_offset=1111111
#EXTINF:-1,MyChannel
http://127.0.0.1:3002/mystream.m3u8
```

- `playback_as_live`: Should the playback be considerd as live tv, allowing skipping from one programme to the next over the entire catchup window, if so set to `true`. Otherwise set to `false` to treat all programmes as videos.
- `catchup_start_time`: The unix time in seconds of the start of catchup window.
- `catchup_end_time`: The unix time in seconds of the end of catchup window.
- `timeshift_buffer_start_time`: The unix time in seconds of the start of the timeshift window.
- `timeshift_buffer_offset`: The offset from the timeshift start time where playback should begin.

Note: setting `playback_as_live` to `true` only makes sense when the catchup start and end times are set to the size of the catchup windows (e.g. 3 days). If the catchup start and end times are set to the programme times then `playback_as_live` will have little effect.

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