/*
 *  Copyright (C) 2005-2018 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#if (defined TARGET_POSIX)
#include "pthreads/ThreadImpl.cpp"
#elif (defined TARGET_WINDOWS)
#include "win/ThreadImpl.cpp"
#endif

