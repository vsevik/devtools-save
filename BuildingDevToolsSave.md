## Getting your feet wet ##

  * Install [Depot Tools](http://dev.chromium.org/developers/how-tos/install-depot-tools)
  * obtain `devtools-save.pem` (or use your own key if you're just hacking)
  * create `~/.gyp/include.gypi`, configure the following variables there:
```
 'variables': {
   'devtools-save_staging_area': '/path/to/staging/directory',
   'devtools-save_pem_file': '/path/to/devtools-save.pem'
 },
```
  * `mkdir devtools-save && cd devtools-save`
  * `git clone https://code.google.com/p/devtools-save src`
  * `gclient config http://devtools-save.googlecode.com/svn/trunk` --name src _(yes, it's a bit weird; we have to fool gclient cause the main project uses git and some of the dependencies are in svn; we'll fix that!)_
  * `gclient sync`
  * `cd src`
  * build according to your plaform (e.g. `make` or `xcodebuild -project all.xcodeproj` or `devenv all.sln /build`)

**Note** The extension includes a NPAPI plug-in that needs to be built for all supported platforms (linux, Windows and Mac OS X at the moment). The plug-ins built for different platforms are copied to the staging area, which is supposed to be visible from different hosts by `publish-plugin` target and pulled from there when building the extension. If you can't build all platforms, you're on your own for the time being (hack `extension.gyp` and `manifest.json`). We'll make a simplified build mode with a single platform support for local hacking later.

