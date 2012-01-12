// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Poor man's logging -- for our use case, it appeared to be more practical
// to implement logging on our own rather than bring in some library that builds
// well with gyp.

#ifndef _LOGGING_H
#pragma once
#include <sstream>

class LogMessage {
 public:
  enum Severity {
    LOG_INFO,
    LOG_WARNING,
    LOG_ERROR,
    LOG_FATAL
  };

  LogMessage(Severity, const char* file, int line);
  ~LogMessage();
  
  std::ostream& stream() { return stream_; }
 private:
  std::ostringstream stream_;

  LogMessage(const LogMessage&);
  LogMessage& operator=(const LogMessage&);
};

class VoidLogMessage {
 public:
  VoidLogMessage() {}
  template<typename T> VoidLogMessage& operator <<(T) { return *this; }
 private:
  VoidLogMessage(const VoidLogMessage&);
  VoidLogMessage& operator=(const VoidLogMessage&);
};

#define LOG(level) LogMessage(LogMessage::LOG_##level, __FILE__, __LINE__).stream()

#ifdef _DEBUG
#define DLOG(level) LOG(level)
#else
#define DLOG(level) VoidLogMessage()
#endif

#endif // _LOGGING_H
