vars = {
  "googlecode_url": "http://%s.googlecode.com/svn"
}

deps = {
  "src/tools/gyp": (Var("googlecode_url") % "gyp") + "/trunk@971",
  "src/third_party/npapi":
    (Var("googlecode_url") % "npapi-sdk") + "/trunk/headers@7",
  "src/third_party/glog":
    (Var("googlecode_url") % "google-glog") + "/trunk/@93"
}

hooks = [
  {
    # A change to a .gyp, .gypi, or to GYP itself should run the generator.
    "pattern": ".",
    "action": ["python", "src/tools/gyp/gyp", "src/all.gyp"],
  }
]
