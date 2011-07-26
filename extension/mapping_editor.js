// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

function MappingEditor(fileMapping, parent) {
  this._fileMapping = fileMapping;
  this._boundOnDelete = this._onDelete.bind(this);
  this._dataGrid = new DataGrid(this._createRow.bind(this));
  this._dataGrid.setColumns(["", "Web URL prefix", "Local directory to save to"]);
  this._dataGrid.setRowFactory(this._createRow.bind(this));
  this._dataGrid.element.setAttribute("spellcheck", false);
  this._dataGrid.setCommitCallback(this._onCommitCallback.bind(this));

  var map = this._fileMapping.getMap();
  for (var i = 0; i < map.length; ++i)
    this._dataGrid.appendRow(this._createRow(map[i]));

  parent.appendChild(this._dataGrid.element);
}

MappingEditor.prototype._createRow = function(mapEntry) {
  if (!mapEntry) // new row
    mapEntry = new FileMapEntry();

  var deleteButton = document.createElement("button");
  deleteButton.className = "delete-button";
  deleteButton.title = "Delete map entry";
  deleteButton.addEventListener("click", this._boundOnDelete, false);
  deleteButton._mapEntry = mapEntry;

  return [
    deleteButton,
    this._createCell(mapEntry.webPath),
    this._createCell(mapEntry.publishTo)
  ];
}

MappingEditor.prototype._onDelete = function(event) {
  var entry = event.target._mapEntry;
  this._dataGrid.deleteRow(this._dataGrid.findPosition(event.target).row);
  this._fileMapping.erase(entry);
}

MappingEditor.prototype._onCommitCallback = function(target) {
  var position = this._dataGrid.findPosition(target);
  var entry = this._dataGrid.getData(position.row, 0)._mapEntry;
  var webPathCell = this._dataGrid.getData(position.row, 1);
  var webPath = this._normalizeWebPath(webPathCell.textContent);
  var publishToCell = this._dataGrid.getData(position.row, 2);
  var publishTo = this._normalizePublishTo(publishToCell.textContent);

  webPathCell.textContent = webPath;
  publishToCell.textContent = publishTo;
  var updated = entry.webPath !== webPath || entry.publishTo !== publishTo;
  if (updated) {
    entry.webPath = webPath;
    entry.publishTo = publishTo;
    this._fileMapping.update(entry);
  }
}

MappingEditor.prototype._normalizeWebPath = function(path) {
  path.trim();
  if (!path)
    return path;
  if (!/^[a-zA-Z+.-]+:\//.test(path))
    path = "http://" + path;
  return path;
}

MappingEditor.prototype._normalizePublishTo = function(path) {
  path.trim();
  if (!path)
    return path;
  if (!/^(file:|https?:)/.test(path))
    path = "file://" + path;
  return path;
}

MappingEditor.prototype._createCell = function(cellContent) {
  var span = document.createElement("span");
  span.className = "editable";
  span.textContent = cellContent || "";
  return span;
}

window.mappingEditor = new MappingEditor(new FileMapping(), document.body);
