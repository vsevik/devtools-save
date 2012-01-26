// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
#include "devtools_save.h"

#include <errno.h>
#include <fcntl.h>
#if defined(OS_WIN)
#include <io.h>
#endif
#include <stddef.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "logging.h"

#include "nputils.h"

struct PathUtilities {
  static bool IsSlash(char c) {
#if defined(OS_POSIX)
    return c == '/';
#elif defined(OS_WIN)
    return c == '/' || c == '\\';
#else
#error FIXME: this is not implemented for your OS yet.
#endif
  }

  static bool IsAbsolutePath(const char* filename) {
#if defined(OS_POSIX)
    return filename[0] == '/';
#elif defined(OS_WIN)
	  return isalpha(filename[0]) && filename[1] == ':' && IsSlash(filename[2]);
#else
#error FIXME: this needs to be implemented for your OS yet.
#endif
  }

  static bool HasBackreferences(const char* filename) {
#if defined(OS_POSIX)
    return strstr("/../", filename) >= 0;
#elif defined(OS_WIN)
    for (; *filename; ++filename) {
      if (IsSlash(*filename) &&
          filename[1] == '.' && filename[2] == '.' &&
          IsSlash(filename[3]))
        return true;
    }
    return false;
#else
#error FIXME: this needs to be implemented for your OS yet.
#endif
  }

  static bool IsDevToolsSaveAllowed(const char* filename) {
    static const char allow_flag_file[] = ".allow-devtools-save";
    std::string path = filename;
    bool found = false;
    if (path.empty())
      return false;
    for (size_t pos = path.length() - 1; pos && !found; --pos) {
      if (!IsSlash(path[pos]))
        continue;
      path.replace(path.begin() + pos + 1, path.end(), allow_flag_file);
      struct stat sb;
      if (stat(path.c_str(), &sb) < 0)
        continue;
#if !defined(OS_WIN)
      if (!S_ISREG(sb.st_mode))
        continue;
#endif
      found = true;
    }
    return found;
  }
};

DevToolsSave::ResultCode DevToolsSave::TestPath(const char* pathname) {
  std::string path = pathname;
  if (path.empty())
    return ERR_NOT_FOUND;
  if (!PathUtilities::IsAbsolutePath(path.c_str()))
    return ERR_RELATIVE_PATH;
  if (!PathUtilities::IsSlash(path[path.length() - 1]))
    path += "/";
  if (!PathUtilities::IsDevToolsSaveAllowed(path.c_str()))
    return ERR_MISSING_ALLOW_DEVTOOLS;
  path.erase(path.length() - 1);
  struct stat stat_info;
  if (stat(path.c_str(), &stat_info) < 0) {
    LOG(ERROR) << "stat(" << path << "): " << errno;
    return ERR_NOT_FOUND;
  }
#if defined(OS_POSIX)
  if (S_ISREG(stat_info.st_mode) || S_ISDIR(stat_info.st_mode))
    return ERR_OK;
  return ERR_NO_ACCESS;
#else
  return ERR_OK;
#endif
}

DevToolsSave::ResultCode DevToolsSave::Save(const char* filename,
                                            const char* content) {
  if (!PathUtilities::IsAbsolutePath(filename))
    return ERR_RELATIVE_PATH;
  if (!PathUtilities::IsDevToolsSaveAllowed(filename))
    return ERR_MISSING_ALLOW_DEVTOOLS;
  size_t size = strlen(content);
  DLOG(INFO) << "Saving to " << filename << " " << size << " bytes";
#if defined(OS_WIN)
  int fd = _open(filename, _O_WRONLY | _O_TRUNC);
#else
  int fd = open(filename, O_WRONLY | O_NOFOLLOW);
#endif
  if (fd < 0) {
    LOG(ERROR) << "open(" << filename << "): " << errno;
    return ERR_NOT_FOUND;
  }

#if defined(OS_POSIX)
  ResultCode rc = ERR_OK;
  // Be paranoid -- only write to regular, non-executable files that we own.
  struct stat stat_info;

  if (fstat(fd, &stat_info) < 0) {
    LOG(ERROR) << "Failed to stat " << filename << ", error: " << errno;
    rc = ERR_INTERNAL;
  } else if (!S_ISREG(stat_info.st_mode))
    rc = ERR_NO_ACCESS;
  else if (stat_info.st_uid != getuid())
    rc = ERR_NO_ACCESS;
  else if (stat_info.st_mode & (S_IXUSR | S_IXGRP | S_IXOTH))
    rc = ERR_EXECUTABLE;

  if (rc != ERR_OK) {
    close(fd);
    return rc;
  }

  ftruncate(fd, 0);
#endif

  do {
#if defined(OS_WIN)
    int written = _write(fd, content, size);
#else
    ssize_t written = write(fd, content, size);
#endif
    if (written < 0)
      break;
    size -= written;
    content += written;
  } while (size > 0);

  if (size > 0) {
    LOG(ERROR) << "Write to " << filename << " failed, " << errno;
    close(fd);
    return ERR_WRITE_FAILED;
  } else {
    DLOG(INFO) << "file written ok";
    close(fd);
  }
  return ERR_OK;
}
