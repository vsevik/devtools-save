// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "logging.h"
#include <iostream>

std::ostream& operator <<(std::ostream& ostr, LogMessage::Severity severity) {
  switch (severity) {
  case LogMessage::LOG_INFO:
    ostr << "[INFO]";
    break;
  case LogMessage::LOG_WARNING:
    ostr << "[WARNING]";
    break;
  case LogMessage::LOG_ERROR:
    ostr << "[ERROR]";
    break;
  case LogMessage::LOG_FATAL:
    ostr << "[FATAL]";
    break;
  default:
    ostr << "[unknown]";
  }
  return ostr;
}

LogMessage::LogMessage(Severity severity, const char* file, int line) {
  stream_ << severity << ' ' << file << ':' << line << ": ";
}

LogMessage::~LogMessage() {
  std::cerr << stream_.str() << std::endl;
}
