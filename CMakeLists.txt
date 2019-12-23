cmake_minimum_required(VERSION 3.5)
project(inputstream.ffmpegarchive)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR})

find_package(PkgConfig)
find_package(p8-platform REQUIRED)
find_package(Kodi REQUIRED)
find_package(FFMPEG REQUIRED)
find_package(ZLIB REQUIRED)
find_package(BZip2 REQUIRED)

set(ARCHIVE_SOURCES src/StreamManager.cpp
                    src/stream/DemuxStream.cpp
                    src/stream/FFmpegStream.cpp
                    src/stream/FFmpegArchiveStream.cpp
                    src/stream/commons/Exception.cpp
                    src/stream/platform/posix/XTimeUtils.cpp
                    src/stream/threads/Atomics.cpp
                    src/stream/threads/Event.cpp
                    src/stream/threads/Thread.cpp
                    src/stream/threads/Timer.cpp
                    src/stream/threads/TimeUtils.cpp
                    src/stream/threads/SystemClock.cpp
                    src/stream/ffmpegutils/FFmpeg.cpp
                    src/stream/ffmpegutils/URL.cpp
                    src/stream/ffmpegutils/UrlOptions.cpp
                    src/stream/ffmpegutils/Variant.cpp)

set(ARCHIVE_HEADERS src/StreamManager.h
                    src/stream/BaseStream.h
                    src/stream/DemuxStream.h
                    src/stream/FFmpegStream.h
                    src/stream/FFmpegArchiveStream.h
                    src/stream/IManageDemuxPacket.h
                    src/stream/commons/Exception.h
                    src/stream/platform/posix/PlatformDefs.h
                    src/stream/platform/posix/XTimeUtils.h
                    src/stream/threads/Atomics.h
                    src/stream/threads/Condition.h
                    src/stream/threads/CriticalSection.h
                    src/stream/threads/Event.h
                    src/stream/threads/Helpers.h
                    src/stream/threads/Lockables.h
                    src/stream/threads/SharedSection.h
                    src/stream/threads/SingleLock.h
                    src/stream/threads/SystemClock.h
                    src/stream/threads/Thread.h
                    src/stream/threads/Timer.h
                    src/stream/threads/TimeUtils.h
                    src/stream/threads/platform/ThreadImpl.h
                    src/stream/ffmpegutils/FFmpeg.h
                    src/utils/Log.h
                    src/stream/ffmpegutils/URL.h
                    src/stream/ffmpegutils/UrlOptions.h
                    src/stream/ffmpegutils/Variant.h)

include_directories(${p8-platform_INCLUDE_DIRS}
                    ${INCLUDES}
                    ${FFMPEG_INCLUDE_DIRS}
                    ${ZLIB_INCLUDE_DIRS}
                    ${BZIP2_INCLUDE_DIRS}
                    ${KODI_INCLUDE_DIR}/..) # Hack way with "/..", need bigger Kodi cmake rework to match right include ways (becomes done in future)

list(APPEND DEPLIBS ${p8-platform_LIBRARIES} ${FFMPEG_LIBRARIES} ${ZLIB_LIBRARIES} ${BZIP2_LIBRARIES})

# to see linker output
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -v")

if(CORE_SYSTEM_NAME MATCHES "windows")
	list(APPEND DEPLIBS Winmm.lib WS2_32.lib)
endif()

if(CORE_SYSTEM_NAME MATCHES "osx")
	find_library(AUDIOTOOLBOX AudioToolbox)
	find_library(COREFOUNDATION CoreFoundation)
	find_library(COREMEDIA CoreMedia)
	find_library(VIDEOTOOLBOX VideoToolbox)

	include_directories(${AVFOUNDATION}
		${COCOA}
		${COREFOUNDATION}
		${COREMEDIA}
		${COREVIDEO}
		${COREMEDIAIO}
		${VIDEOTOOLBOX}
		${COCOA})

    list(APPEND DEPLIBS ${AUDIOTOOLBOX} ${COREFOUNDATION} ${COREMEDIA} ${VIDEOTOOLBOX})
endif()

addon_version(inputstream.ffmpegarchive ARCHIVE)
add_definitions(-DARCHIVE_VERSION=${ARCHIVE_VERSION})

build_addon(inputstream.ffmpegarchive ARCHIVE DEPLIBS)

if(CORE_SYSTEM_NAME STREQUAL windowsstore)
    # fix linking
    set_target_properties(inputstream.ffmpegarchive PROPERTIES LINK_FLAGS_RELWITHDEBINFO "/LTCG /defaultlib:vccorlib.lib /defaultlib:msvcrt.lib")
    set_target_properties(inputstream.ffmpegarchive PROPERTIES LINK_FLAGS_RELEASE "/LTCG /defaultlib:vccorlib.lib /defaultlib:msvcrt.lib")
    set_target_properties(inputstream.ffmpegarchive PROPERTIES LINK_FLAGS_DEBUG "/LTCG /defaultlib:vccorlibd.lib /defaultlib:msvcrtd.lib")
endif()

include(CPack)
