<h3>Symbols Library</h3>

<p>A symbol library is a directory structure containing separate files
defining symbols that can be included into the document.  Each symbol
requires three files:  An SVG file containing the SVG code to be
inserted, a thumbnail image for use in previewing the symbol, and a
metadata file providing a description of the symbol as well as its
drawing units definitions, etc.</p>

<h4>Investigate Thumbnails Standards</h4>

<p>This is a pre-req for the symbol library/browser.  See if there is a
standard for how thumbnails are done, that is suitable and applicable to
Sodipodi's needs.  If so, then it will be used for things such as the
symbol library/browser.</p>

<h4>Loading / Saving Symbol Libraries</h4>

<p>Add a routine to scan a given directory tree and load up information
about valid symbols they contain.</p>

<p>Add a routine to serialize the currently loaded symbol library back
to disk.</p>

<p>Implement support for a global symbol library, to be installed with
Sodipodi (and including, as a minimum, the rect, ellipse, star, and
spiral symbols), and a user-area symbol library (initially empty) in the
current user's ~/.sodipodi directory.</p>

<h4>Symbol Library Preferences</h4>

<p>Add to the user's preferences file a note as to any symbol libraries
in addition to the standard ones, to be loaded when Sodipodi is
initialized.</p>

<h4>Symbol Library Browsing</h4>

<p>Add a mechanism to permit a user to review collections of
symbols.</p> 

<p>Create a new Symbol Library Dialog and add an entry to the dialog
submenu.</p>

<p>The user will need to be able to view multiple symbol library dialogs
at the same time.  They should also be able to attach these to the
browser window (so that they could, for example, place one symbol
library along the bottom of the drawing, and another along the right.</p>

<h4>Symbol Insert</h4>

<p>Create a symbol insert button and add it to the Draw area of the
toolbox.  It pops up the Symbol Library Browser and puts the user into
insert symbol mode, allowing them to begin inserting instances of the
selected symbol.</p>

<h4>Symbol Export</h4>

<p>The user can choose to save objects from the current drawing to
the symbol library.  The user selects one or more entities from the
current drawing and selects this function (which can be made available
via a button in the Selection area of the toolbox, perhaps).  A new
symbol is generated and placed into the currently loaded symbol
library.</p>

<h4>Auto-thumbnailer</h4>

<p>Implement a capability for Sodipodi to automatically attempt to
render thumbnails when storing SVG to a symbol library.</p>

