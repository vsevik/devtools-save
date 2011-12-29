// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

function BackgroundPage() {
  this._plugin = document.getElementById("save-plugin");
  this._mapping = new FileMapping();
  chrome.extension.onRequest.addListener(this._onRequest.bind(this));
  window.addEventListener("storage", this._onStorageUpdated.bind(this), false);
}

BackgroundPage.prototype._onRequest = function(request, sender, sendResponse) {
  if (this._mapping.isEmpty()) {
    this._issueConfigurationNotice();
    return;
  }
  var target = this._mapping.map(request.url);
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
  else if (/^file:\/\//.test(target))
    this._saveLocally(target.replace(/^file:\/\//, ""), content, callback.bind(this));
}

BackgroundPage.prototype._saveLocally = function(target, content, callback) {
  if (!this._plugin.save) {
    callback(false, "No devtools-save plugin installed -- perhaps, unsupported platform? " +
        "Only linux is supported so far.");
    return;
  }
  var saved = false;
  var error;
  try {
    this._plugin.save(target, content);
    saved = true;
  } catch (e) {
    error = e.toString();
  }
  callback(saved, error);
}

BackgroundPage.prototype._saveByDAV = function(target, content, callback) {
  callback(false, "not implemented yet");
}

BackgroundPage.prototype._onStorageUpdated = function(event) {
  if (event.storageArea === window.localStorage && event.key === "files-map")
    this._mapping.load();
}

BackgroundPage.prototype._notifyError = function(filename, error) {
  var notification = webkitNotifications.createNotification("img/error.png", "Error saving file!", filename + ": " + error);
  notification.show();
}

BackgroundPage.prototype._notifySuccess = function(filename) {
  var notification = webkitNotifications.createNotification("img/saved-ok.png", "Saved ok", "Successfuly saved " + filename);
  notification.addEventListener("display", BackgroundPage._onDisplayNotification, false);
  notification.show();
}

BackgroundPage.prototype._issueConfigurationNotice = function(filename) {
  var key = "config-notification-given";
  if (localStorage.getItem(key))
    return;
  var notification = webkitNotifications.createHTMLNotification("config-notification.html");
  notification.show();
  localStorage.setItem(key, "yes");
}

BackgroundPage._onDisplayNotification = function(event) {
  window.setTimeout(function() { event.target.cancel(); }, 2000);
}

window.backgroundPage = new BackgroundPage();
