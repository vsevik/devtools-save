// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

function BackgroundPage() {
  this._plugin = document.getElementById("save-plugin");
  this._initMessages();
  this._mapping = new FileMapping();
  chrome.extension.onRequest.addListener(this._onRequest.bind(this));
  window.addEventListener("storage", this._onStorageUpdated.bind(this), false);
}

BackgroundPage.prototype._initMessages = function(plugin) {
  this._messages = [];
  var plugin = this._plugin;
  if (!plugin)
    return;
  this._messages[plugin.ERR_NOT_FOUND] =
      "File or path not found. NOTE: we only write to existing files.";
  this._messages[plugin.ERR_NO_ACCESS] =
      "Access denied.";
  this._messages[plugin.ERR_EXECUTABLE] =
      "Refusing to write to an executable file.";
  this._messages[plugin.ERR_BACKREFERENCE] =
      "Path contains backreferences.";
  this._messages[plugin.ERR_RELATIVE_PATH] =
      "Path is relative. Please specify absolute path.";
  this._messages[plugin.ERR_MISSING_ALLOW_DEVTOOLS] =
      "Missing .allow-devtools-save file in path from file to root.";
  this._messages[plugin.ERR_WRITE_FAILED] =
      "Failed while writing to file. Perhaps, disk full or network error?";
  this._messages[plugin.ERR_INTERNAL] =
      "Internal error, please check logs and report to developers.";
}

BackgroundPage.prototype._onRequest = function(request, sender, sendResponse) {
  if (this._mapping.isEmpty()) {
    this._issueConfigurationNotice();
    return;
  }
  var url = request.url.replace(/[?#].*/, "");
  url = decodeURIComponent(url);
  var target = this._mapping.map(url);
  if (!target) {
    sendResponse({ saved: false });
    return;
  }
  this._save(target, request.content, sendResponse);
}

BackgroundPage.prototype._save = function(target, content, sendResponse) {
  function callback(success, error) {
    if (error)
      this._notifyError(target, error);
    else
      this._notifySuccess(target);
    sendResponse({
      saved: success,
      error: error,
      actualTarget: target
    });
  }
  if (/^https?:/.test(target))
    this._saveByDAV(target, content, callback.bind(this));
  else
    this._saveLocally(target, content, callback.bind(this));
}

BackgroundPage.prototype._saveLocally = function(target, content, callback) {
  if (!this._plugin.save) {
    callback(false, "No devtools-save plugin installed -- perhaps, " + 
        "unsupported platform?");
    return;
  }
  var saved = false;
  var error;
  try {
    var rc = this._plugin.save(target, content);
    if (!rc)
      saved = true;
    else
      error = this._messages[rc] || ("Uknown error while saving file: " + rc);
  } catch (e) {
    error = e.toString();
  }
  callback(saved, error);
}

BackgroundPage.prototype.testLocalPath = function(path) {
  if (!this._plugin || !this._plugin.testPath) {
    return "Unable to save locally, missing plugin object " +
        "(perhaps, an unsupported platform).";
  }
  var rc = this._plugin.testPath(path);
  if (!rc)
    return;
  return this._messages[rc] || ("Uknown error accessing path: " + rc);
}

BackgroundPage.prototype._saveByDAV = function(target, content, callback) {
  callback(false, "not implemented yet");
}

BackgroundPage.prototype._onStorageUpdated = function(event) {
  if (event.storageArea === window.localStorage && event.key === "files-map")
    this._mapping.load();
}

BackgroundPage.prototype._notifyError = function(filename, error) {
  var notification = webkitNotifications.createNotification("img/error.png",
      "Error saving file!", filename + ": " + error);
  notification.show();
}

BackgroundPage.prototype._notifySuccess = function(filename) {
  var notification = webkitNotifications.createNotification(
      "img/saved-ok.png", "Saved ok", "Successfuly saved " + filename);
  notification.addEventListener("display",
      BackgroundPage._onDisplayNotification, false);
  notification.show();
}

BackgroundPage.prototype._issueConfigurationNotice = function(filename) {
  var key = "config-notification-given";
  if (localStorage.getItem(key))
    return;
  var notification = webkitNotifications.createHTMLNotification(
      "config_notification.html");
  notification.show();
  localStorage.setItem(key, "yes");
}

BackgroundPage._onDisplayNotification = function(event) {
  window.setTimeout(function() { event.target.cancel(); }, 2000);
}

window.backgroundPage = new BackgroundPage();
