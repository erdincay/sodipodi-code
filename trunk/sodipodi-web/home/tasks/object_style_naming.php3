<h3>Object Style Naming</h3>

<p>Often a user will wish to have several style settings to apply to
various objects, as a 'class'.  Colors, fonts, line styles, etc. should
be able to be specified, and a name assigned. 

<h4>Style Class Structure Definition</h4>

<p>A "style" will consist of one or more of the following properties:</p>

<ul>
 <li>Name
 <li>Stroke width
 <li>Stroke color
 <li>Fill color
 <li>Gradient
 <li>Pattern
 <li>Font type
 <li>Font size
</ul>

<p>It is allowed to leave some/all of the properties undefined, in which
case no changes will be made.</p>

<h4>Style Class Application</h4>

<p>Each entity can be assigned one or more classes.  If multiple classes
are assigned, then the traits are applied in order.

<p>If a style class definition is ever changed, then all entities which
are set to that style class will be updated accordingly.</p>

<h4>Style Class Storage</h4>

<p>By default, a drawing's style class definitions are stored in the
document itself.  However, the user may choose to maintain these
definitions in a separate file, or set of files.  These are stored in
the 'Cascading Style Sheets' (CSS) format, and are called
'stylesheets'.</p> 

<p>If external stylesheets are used, the appropriate include statements
are placed at the top of the document's SVG file.  If multiple
stylesheets with the same class definition are included, the order of
inclusion determines which definition takes precidence.  If the user
specifies styles within the document, these definitions overload any
defined in separate files.

<p>The File->Import dialog is modified such that if a file containing
css is loaded, these definitions will be added to the document's style
definitions, overriding any existing style classes of the same name.

<p>A new command 'Export styles' is added to the File menu.  This
permits storing all of the document's current styles to a .css file (by
default, the same name as the document's filename, except s/svg/css/.)
When styles are exported, they're removed from the document, and a css
include statement used instead.

<h4>Style Class Editor</h4>

<p>An XML editor dialog is provided for managing the document's styles.
The classes can be sorted and browsed by storage location (current file
vs. separate included *.css files), class name, styles affected, etc.

<p>Within the editor, the user can add new styles, and edit or remove
existing ones to the current document (i.e., not to included css
files).</p>

<p>The user can also choose to edit style class definitions in external
included css files.  This is done by loading the stylesheet as a
separate document.  When the user saves this document, any currently
loaded documents that include this stylesheet will be updated, as
appropriate.</p>

<h4>Apply Style Button / Chooser</h4>

<p>A button 'Apply style' is to be added to the Toolbox in the Object
section.  This button should pop up a dialog listing the style classes
available.  This should be displayed as a table, with the class name in
the left column, a human-readable definition of what the style class
does in the middle, and an indicator on the right to show if it is a
document-specific class, or is loaded from an external file.



<h4>References</h4>

<p>[<A HREF="http://sourceforge.net/tracker/index.php?func=detail&aid=696926&group_id=4054&atid=354054">696926</A>]
- "Object style chooser in toolbox"
</p>

<p>
[<A HREF="http://sourceforge.net/tracker/index.php?func=detail&aid=616228&group_id=4054&atid=354054">354054</A>
- "Giving styles a name"
</p>

