// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

function DataGrid() {
  this.element = document.createElement("table");
  this.element.className = "data-grid";
  this.element.addEventListener("click", this._onClick.bind(this), false);
  this.element.addEventListener("keydown", this._onKeyDown.bind(this), false);
  this.element.tabIndex = 0;

  this._elementBeingEdited = null;
  this._boundOnBlur = this._onBlur.bind(this);
}

DataGrid.prototype.findPosition = function(element) {
  var td = enclosingOrSelfWithName(element, "td");
  if (!td)
    return;
  var tr = enclosingOrSelfWithName(element, "tr");
  return { 
    column: td.cellIndex,
    row: tr.sectionRowIndex
  };
}

DataGrid.prototype.getData = function(row, column) {
  return this.element.rows[row].cells[column].firstChild;
}

DataGrid.prototype.setRowFactory = function(rowFactory) {
  this._rowFactory = rowFactory;
  this.element.classList.add("appendable");
  this._appendNewRow();
}

DataGrid.prototype.setCommitCallback = function(commitCallback) {
  this._commitCallback = commitCallback;
}

DataGrid.prototype.setColumns = function(headers) {
  var colgroup = document.createElement("colgroup");
  for (var i = 0; i < headers.length; ++i) {
    var col = document.createElement("col");
    colgroup.appendChild(col);
  }
  this.element.insertBefore(colgroup, this._newRow);
  var row = this._createRow("th", headers);
  this.element.insertBefore(row, this._newRow);
}

DataGrid.prototype.appendRow = function(row) {
  var tr = this._createRow("td", row);
  this.element.insertBefore(tr, this._newRow);
}

DataGrid.prototype.deleteRow = function(rowIndex) {
  this.element.removeChild(this.element.rows[rowIndex])
}                                                  

DataGrid.prototype._appendNewRow = function() {
  this._newRow = this._createRow("td", this._rowFactory());
  this.element.appendChild(this._newRow);
}

DataGrid.prototype._cancelEditing = function() {
  this._elementBeingEdited.textContent = this._savedContent;
  delete this._savedContent;
  this._completeEditing();
}

DataGrid.prototype._createRow = function(celltype, data) {
  var tr = document.createElement("tr");
  for (var i = 0; i < data.length; ++i) {
    var cell = document.createElement(celltype);
    if (data[i]) {
      var node = data[i] instanceof Node ? data[i]
          : document.createTextNode(data[i]);
      cell.appendChild(node);
    }
    tr.appendChild(cell);
  }
  return tr;
}

DataGrid.prototype._commitEditing = function() {
  var tr = enclosingOrSelfWithName(this._elementBeingEdited, "tr");
  if (this._commitCallback)
    this._commitCallback(this._elementBeingEdited);
  if (tr === this.element.rows[this.element.rows.length - 1] && this._elementBeingEdited.textContent !== "")
    this._appendNewRow();
  this._completeEditing();
}

DataGrid.prototype._completeEditing = function() {
  var td = enclosingOrSelfWithName(this._elementBeingEdited, "td");
  td.classList.remove("being-edited");
  td.scrollLeft = 0;
  delete this._savedContent;
  this._elementBeingEdited.removeEventListener("blur", this._boundOnBlur, false);
  this._elementBeingEdited.contentEditable = "inherit";
  this._elementBeingEdited.scrollLeft = 0;
  this._elementBeingEdited = null;
}

DataGrid.prototype._editElement = function(node) {
  var td = enclosingOrSelfWithName(node, "td");
  if (td)
    td.classList.add("being-edited");
  this._savedContent = node.textContent;
  this._elementBeingEdited = node;
  this._elementBeingEdited.tabIndex = 1;
  this._elementBeingEdited.addEventListener("blur", this._boundOnBlur, false);
  this._elementBeingEdited.contentEditable = "plaintext-only";
}

DataGrid.prototype._onClick = function(event) {
  var editable = enclosingOrSelfWithClass(event.target, "editable");
  if (this._elementBeingEdited) {
    if (this._elementBeingEdited === editable)
      return;
    this._commitEditing();
  } 
  if (editable)
    this._editElement(editable);
}

DataGrid.prototype._onKeyDown = function(event) {
  // console.log(event.keyIdentifier + " / " + event.keyLocation);
  switch (event.keyIdentifier) {
  case "Enter":
    if (this._elementBeingEdited)
      this._commitEditing();
    break;
  case "U+001B":
    if (this._elementBeingEdited)
      this._cancelEditing();
    break;
  }
}

DataGrid.prototype._onBlur = function(event) {
  this._commitEditing(event.target);
}

function enclosingOrSelfWithName(node, name) {
  name = name.toLowerCase();
  while (node && node.nodeName.toLowerCase() !== name)
    node = node.parentElement;
  return node;
}

function enclosingOrSelfWithClass(node, className) {
  while (node && !node.classList.contains(className))
    node = node.parentElement;
  return node;
}
