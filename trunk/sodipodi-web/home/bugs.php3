<h2>Generic tricks</h2>
<h3>Cleaning up document</h3>
<p>
Time to time it is good idea to take a look to XML editor dialog.
Sodipodi does not yet do garbage collection, so if you make
object invisible, it still is in document. The same stands for once
used but later abandoned gradients (although the latter are reused,
if you request new gradient fill). XML editor allows you to change
almost all aspects of document, without using external text editor.
</p>
<p>
Also, sodipodi usually supports more SVG features, than are accessible
from GUI. For example, at the time of writing this, sodipodi can display
(and print) patterned fills, but the only way to create and modify these
is XML editor (or editing SVG file directly).
</p>
<h3>Changing ruler unit system</h3>
<p>
The default unit system for ruler are millimetres (this is also the
units used in displaying coordinates at lower-left corner). You can
change these from 'Grid' section of 'Editing Window" dialog.
</p>
<h3>Stamping</h3>
<p>
If you have to quickly copy objects, you can use stamping. Just drag
object (or scale or rotate it), and while holding mousebutton down,
press [Space]. This leaves 'stamp' of current object shape. You can
repeat it as many times as yo wish.
</p>
<h3>Finishing pen line</h3>
<p>
You have the following options to finish current line:
</p>
<ul>
<li>Press [Enter]
<li>Double click left button
<li>Select pen tool again
<li>Select another tool
</ul>
<h3>Pen tool tricks</h3>
<p>
Backspace key forgets last point (how surprising!).
</p>
<p>
Pressing '+' toggles 'add' and 'append' modes in pen and pencil tools.
In 'add' mode, whenever you start new line, new object is created. In
'append' mode new line is appended to existing objects (which has to be
shape, of course). There are as many anchors, as there are open ends of
subsegments of selected line, and starting fron arbitrary anchor
continues existing line.
</p>
<h3>Entering unicode values</h3>
<p>
While in text tool, pressing 'Ctrl u' toggles between unicode and
normal mode. In unicode mode, entered 4 hexadecimal digits are parsed
as single unicode value, thus allowing one to enter arbitrary symbols
(as long as you know its unicode codepoint and font supports it).
Unfortunately there is not visual indication unicode mode yet.
</p>

<h2>0.25 bugs and workarounds</h2>
<h3>Broken drawing and transformations</h3>
<p>
Just drawing anything with freehand works, until you
release mousebutton. Then line turns into crazy mishmesh.
</p>
<p>
Transformation works, until you release mousebutton. Then
object jump onto semi-random position.
</p>
<h3>Cause</h3>
<p>
Sodipodi incorrectly sets numeric locale to user default one.
If decimal separator is not point, resulting SVG will be
invalid.
</p>
<h3>Workaround</h3>
<p>
Set environment variable LC_NUMERIC to some locale
with point as decimal separator, such as POSIX or C.
</p>
<p>
If you have LC_ALL set, you have either unset it, or
set it to POSIX or C too.
</p>
