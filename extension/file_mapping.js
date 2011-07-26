// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

function FileMapEntry(webPath, publishTo) {
  this.webPath = webPath || "";
  this.publishTo = publishTo || "";
}

FileMapEntry.prototype._map = function(webPath) {
  var matchAgainst = this.webPath;
  if (!matchAgainst)
    return;
  var index = webPath.indexOf(matchAgainst);
  if (index)
    return;
  // require matchAgainst to specify a full file name or directory name, not just
  // arbitrary string prefix.
  if (matchAgainst.length < webPath.length && !/\/$/.test(matchAgainst)) {
      if (webPath.charAt(matchAgainst.length) === "/")
        matchAgainst += "/";
      else
        return;
  }
  return webPath.replace(this.webPath, this.publishTo);
}

FileMapEntry.prototype._save = function() {
  return {
    webPath: this.webPath,
    publishTo: this.publishTo
  }
}

FileMapEntry._load = function(obj) {
  return new FileMapEntry(obj.webPath, obj.publishTo);
}

function FileMapping() {
  this._entries = [];
  this.load();
}

FileMapping.prototype.isEmpty = function() {
  return !this._entries.length;
}

FileMapping.prototype.getMap = function() {
  return this._entries;
}

FileMapping.prototype.update = function(entry) {
  if (this._entries.indexOf(entry) < 0)
    this._entries.push(entry);
  this._save();
}

FileMapping.prototype.erase = function(entry) {
  var index = this._entries.indexOf(entry);
  this._entries.splice(index, 1);
  this._save();
}

FileMapping.prototype.load = function() {
  this._entries = [];
  var loaded = [];
  try {
    var filesMap = localStorage.getItem("files-map");
    if (!filesMap) {
      console.warn("Failed to load file mapping. First run?");
      return;
    }
    loaded = JSON.parse(filesMap);
  } catch (e) {
    console.error("Failed to load file mapping: " + e.toString());
    return;
  }
  for (var i = 0; i < loaded.length; ++i)
    this._entries.push(FileMapEntry._load(loaded[i]));
}

FileMapping.prototype._save = function() {
  var result = [];
  for (var i = 0; i < this._entries.length; ++i)
    result.push(this._entries[i]._save());
  localStorage.setItem("files-map", JSON.stringify(result));
}

FileMapping.prototype.map = function(file) {
  for (var i = 0; i < this._entries.length; ++i) {
    var publishTo = this._entries[i]._map(file);
    if (publishTo)
      return publishTo;
  }
}
