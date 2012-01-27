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
  this._firstRow = 0;
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
  ++this._firstRow;
}

DataGrid.prototype.appendRow = function(row) {
  var tr = this._createRow("td", row);
  this.element.insertBefore(tr, this._newRow);
}

DataGrid.prototype.deleteRow = function(rowIndex) {
  this.element.removeChild(this.element.rows[rowIndex])
}                                                  

DataGrid.prototype.updateFocus = function() {
  this._moveFocusTo(this.element.rows.length - 1, 0, 1);
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
      var editableChildren = cell.querySelectorAll(".editable");
      for (var j = 0; j < editableChildren.length; ++j)
        editableChildren[j].tabIndex = 0;
    }
    tr.appendChild(cell);
  }
  return tr;
}

DataGrid.prototype._commitEditing = function() {
  var tr = enclosingOrSelfWithName(this._elementBeingEdited, "tr");
  if (this._commitCallback)
    this._commitCallback(this._elementBeingEdited);
  if (tr === this.element.rows[this.element.rows.length - 1] &&
      this._elementBeingEdited.textContent !== "")
    this._appendNewRow();
  this._completeEditing();
}

DataGrid.prototype._completeEditing = function() {
  var td = enclosingOrSelfWithName(this._elementBeingEdited, "td");
  td.classList.remove("being-edited");
  td.scrollLeft = 0;
  delete this._savedContent;
  this._elementBeingEdited.removeEventListener("blur",
                                               this._boundOnBlur, false);
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
  console.log(event.keyIdentifier + " / " + event.keyCode + " / " + event.charCode);
  switch (event.keyIdentifier) {
  case "Enter":
    if (!this._elementBeingEdited) {
      this._editActiveCell();
      event.preventDefault();
    } else {
      this._commitEditing();
      this._moveFocusBy(0, 1);
    }
    break;
  case "U+001B":
    if (this._elementBeingEdited)
      this._cancelEditing();
    break;
  case "Up":
    if (!this._elementBeingEdited)
      this._moveFocusBy(-1, 0);
    break;
  case "Down":
    this._moveFocusBy(1, 0);
    break;
  case "Left":
    if (!this._elementBeingEdited)
      this._moveFocusBy(0, -1);
    break;
  case "Right":
    if (!this._elementBeingEdited)
      this._moveFocusBy(0, 1);
    break;
  case "Home":
    if (!this._elementBeingEdited) {
      if (event.ctrlKey)
        this._moveFocusTo(this._firstRow, null);
      else
        this._moveFocusTo(null, 0, 1);
    }
    break;
  case "End":
    if (!this._elementBeingEdited) {
      if (event.ctrlKey)
        this._moveFocusTo(this.element.rows.length - 1, null);
      else
        this._moveFocusTo(null, this.element.rows[0].cells.length - 1, -1);
    }
    break;
  default:
    if (!event.ctrlKey && !event.altKey &&
        !this._elementBeingEdited &&
        /^U\+00[2-9]/.test(event.keyIdentifier))
      this._editActiveCell();
  }
}

DataGrid.prototype._editActiveCell = function() {
  var editable = enclosingOrSelfWithClass(document.activeElement, "editable");
  if (editable) {
    this._editElement(editable);
    window.getSelection().collapse(editable, 1);
  }
}

DataGrid.prototype._getActiveCell = function() {
  return document.activeElement && this.findPosition(document.activeElement);
}

DataGrid.prototype._trySetActiveCell = function(row, column) {
  var rowElement = this.element.rows[row];
  if (!rowElement)
    return;
  var cell = rowElement.cells[column];
  if (!cell)
    return;
  var editable = cell.querySelector(".editable");
  if (!editable)
    return;
  editable.focus();
  return editable;
}

DataGrid.prototype._moveFocusTo = function(row, column, searchDirection) {
  if (row == null || column == null) {
    var cellPosition = this._getActiveCell();
    if (!cellPosition)
      return;
    if (row == null)
      row = cellPosition.row;
    if (column == null)
      column = cellPosition.column;
  }
  var cell = this._trySetActiveCell(row, column);
  if (cell || !searchDirection)
    return;
  column += searchDirection;
  var rowElement = this.element.rows[row];
  if (!rowElement)
    return;
  if (column < 0) {
    rowElement = this.element.rows[--row];
    if (rowElement)
      column = rowElement.cells.length - 1;
  } else if (column >= rowElement.cells.length) {
    column = 0;
    rowElement = this.element.rows[++row];
  }
  if (!rowElement)
    return;
  for (; rowElement.cells[column]; column += searchDirection) {
    if (this._trySetActiveCell(row, column))
      return true;
  }
  return false;
}

DataGrid.prototype._moveFocusBy = function(rowOffset, columnOffset) {
  var cellPosition = this._getActiveCell();
  if (!cellPosition)
    return;
  this._moveFocusTo(cellPosition.row + rowOffset,
                    cellPosition.column + columnOffset,
                    columnOffset);
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
