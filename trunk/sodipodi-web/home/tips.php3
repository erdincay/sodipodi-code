<h2>Generic tricks and tips</h2>

<h3>Aligning to center of page</h3>
<p>
To align something to the center or side of a page, select the object or
group and then choose [page] from the align dialog.  Click on the middle
button (for instance) in both rows.  The object should align to the page
now (in the dead center).
</p>

<h3>Manually Placing Text Along a Curve</h3>

<ol>
<li> Use 'Text Tool' to place text in font and size you want, you will
  not be able to change the string later.</li>

<li>Select text and 'Convert Object to Curve'. This creates a path for
   each of the characters, but they are still grouped together.</li>

<li>Use 'Break Selected Path to Subpaths' to seperate characters. This
   has the side effect of losing the cutout centers of characters like
   'e' and 'd'.</li>

<li>Select the exterior and interior paths for each of the 'damaged'
   characters in turn and 'Combine Mutiple Paths'.</li>
</ol>

<p. You now have a series of characters which can be individually moved,
 rotated or scaled as desired. I sugest using a path as a template for
 the curve and then removing (or making invisible by setting no stroke)
 later.
</p>

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
from GUI. For example, display of patterned fills was implemented before
it was ever possible to create and modify them.  The XML editor is one
way to get around the lack of GUI tool support for new features.
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

<h3>Using grid for icon drawing</h3>

<p> 
Say, I want to create 24x24 pixel icon. Then I'll make image
24x24mm, and set grid at 0.5 mm (48x48 gridlines).  Now, if I align
filled objects to even gridlines, and stroked objects to odd gridlines,
with stroke width even number of mm, and export it 1mm->1pixel, I get
crisp image.
</p>

<h3>Object rotation</h3>

<p>When in Select mode, click on an object to see the scaling arrows,
then click again on the object to see the rotation and shift arrows.  If
the arrows at the corners are clicked and dragged, the object will
rotate about the opposite corner.  If you hold down the shift key while
doing this, the rotation will occur about the Rotation Point (nominally
the center of the object).</p>

<p>The Rotation Point can be moved by clicking on the very center of the
object and dragging the center point to where you want to pivot around.
Then if you shift-drag on a corner point, it will rotate about that
point.</p> 

<h3>Landscape Printing</h3>

<p>When printing documents in A4 landscape format to a postscript device,
Sodipodi 0.28 generates code for A4 portrait format but doesn't rotate
the graphics accordingly. To fix this problem, you need to change:</p>

<PRE>
    %%PageResources: (atend)
    0 0 m
    595.276 0 l
    595.276 841.89 l
    0 841.89 l
</PRE>
<p>into</p>

<PRE>
    %%PageResources: (atend)
    596 0 translate
    90 rotate
    0 0 m
    0 595.276 l
    841.89 595.276 l
    841.89 0 l
</PRE>

<p>in the postscript code.</p>



