var devtools = chrome.experimental.devtools || chrome.devtools;

function error(message) {
  devtools.inspectedWindow.eval("console.error(unescape('" + escape(message) + "'));");
}

function callback(response) {
  var message;
  if (!response.saved && response.actualTarget)
    error("Failed to save " + response.actualTarget + ": " + response.error);
}

function onContentCommitted(resource, content) {
  // skip document updates for the time being -- these are only generated by
  // inline stylesheets currently, and content is nvalid (set to stylesheet,
  // not the entire document)
  if (resource.type === "document")
    return;
  chrome.extension.sendRequest({ url: resource.url, content: content }, callback);
}

devtools.inspectedWindow.onResourceContentCommitted.addListener(onContentCommitted);