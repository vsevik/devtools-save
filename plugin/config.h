// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef _CONFIG_H_
#define _CONFIG_H_

#if defined(__linux__)
#define OS_LINUX 1
#elif defined(__APPLE__)
#define OS_MACOSX 1
#elif defined(_WIN32)
#define OS_WIN
#endif

#if defined(OS_LINUX) || defined(OS_MACOSX)
#define OS_POSIX
#endif

#endif // _CONFIG_H
