<h3>Keyboard Shortcuts</h3>

<p>Documentation of 
<a href="http://sodipodi.sourceforge.net/index.php3?section=documentation/usage/keybindings">Currently
implemented keyboard shortcuts</a> is available from the website.</p>

<p>The following is a detailed proposal from bulia byak [bulia/dr_com]
regarding keyboard shortcuts.  This has been discussed in the sodipodi
mailing list (in late March 2003).  Feedback from that series of
discussions has been folded into this proposal.</p>

<p>This only covers general contexts and two of the tools,
selector and node editor; other tools will be explored later.
The new keys are not only more usable but also more familiar for
users of other vector editors, mostly Windows ones.  Some of
the shortcuts require new functions, but mostly they remap what
is already available in the menu.</p>

<p>It is very important that all unhandled events are forwarded
from dialogs to the canvas, as described on the
<a
href="http://sodipodi.sourceforge.net/index.php3?section=development/tasks/usability">usability
task page</a> so that if a shortcut such as open a document (ctrl-o) is
struck when the toolbox is in focus, that's very bad usability indeed.</p>

<h4>"Actions"</h4>

<p>All shortcuts must be displayed next to the corresponding commands
the menu.  Gtk+ can do this automatically for static menus, but
unfortunately we want (at least some) menus to be dynamic. So we have to
invent our own solution.  </p>

<p>We have to invent 'actions' that serve as proxies for toolbox
buttons, menus, drawing commands, dialog actions and so on. Thus they
will be responsible for things like graying out unusable menu items,
implementing 'repeat action', allowing customization of keys, etc.</p>

<h4>Selector</h4>

<table border=0 width=100%>
 <tr>
  <td valign="top" width="100">  ctrl a   </td>
  <td valign="top">    select all objects.  Currently assigned to
          'append/add' in draw context mode.  </td>
 </tr>

 <tr>
  <td valign="top">  esc      </td>
  <td valign="top">    cancel if dragging, or deselect otherwise (badly needed!)   </td> 
 </tr>

 <tr>
  <td valign="top">  shift-esc      </td>
  <td valign="top">    toggle back deselected selection </td> 
 </tr>

 <tr>
  <td valign="top">  ctrl u   </td>
  <td valign="top">    ungroup, in addition to ctrl-G; ctrl-u is common in other
          vector apps  </td>
 </td>

 <tr>
  <td valign="top">  ctrl Z    </td>
  <td valign="top">    redo; ctrl-r is consistent with GIMP but not familiar to
          windows users (one would rather expect ctrl-r to be "redraw")
  </td>
 </tr>

 <tr>
  <td valign="top">  ctrl y    </td>
  <td valign="top">    another common Windows shortcut for redo </td>
 </tr>

 <tr>
  <td valign="top">  keypad +/-   </td>
  <td valign="top">    zoom, same as +/- on the main keyboard (also with ctrl)
          (currently these don't work, at least for me under
          Linux; it was reported that they work under Windows)
  </td>
 </tr>

 <tr>
  <td valign="top">  space     </td>
  <td valign="top">    stamp   </td>
 </tr>

 <tr>
  <td valign="top">  arrows:  </td>

  <td valign="top">    <p>Now both arrows and shift-arrows move selection in pixel
          units. This means that the actual movement depends on the zoom
          level. This is sometimes useful and sometimes not. Xara has
          movements by one visible pixel (alt-arrows) and two modes of
          movement by a fixed pixel-independed amount (arrows and
          ctrl-arrows), which is more convenient. So I propose a similar
          scheme:</p>

          <p><i>arrows</i> -- move by some fixed amount X (settable in preferences,
             e.g. 5mm by default), independent of zoom</p>
          <p><i>shift-arrows</i> -- move by amount 10*X</p>
          <p><i>alt-arrows</i> --  move by one visible pixel (same as shift-arrows now);
             this is consistent with "alt + transform = slow movement"

          <p>This should include keypad arrows which currently don't work.</p>
  </td>
 </tr>
</table>

<h4>Drawing</h4>

<table border=0 width=100%>
 <tr>
  <td valign="top">  Shift-drawing:  </td>
  <td valign="top">   Use shift if you want the new line to be appended
          to the old one, just as you shift-drag to extend a
          selection.  this append/new toggle should be added to the tool
          options for the drawing tools.</td> 
 </tr>
</table>

<h4>Node editor</h4>

<table border=0 width=100%>
 <tr>
  <td valign="top" width="100">  arrows, shift-arrows, alt-arrows:</td>
  <td valign="top">    The same as in selector, but applying to the currently
          selected node(s)
  </td>
 </tr>

 <tr>
  <td valign="top">  ctrl-alt-arrows:</td>

  <td valign="top">    Move both control points of the currently selected node
          horizontally (right/left) or vertically (up/down) in opposite
          directions, rotating the control handles around the selected
          node 
  </td>
 </tr>

 <tr>
  <td valign="top">  del:  </td>

  <td valign="top"> Now it works only while you're actually dragging the
          points, and affects only one node even if more are selected. I
          think it should work on all selected nodes when they are not
          being dragged. The same applies to most other keys (c, s,
          etc.) 
  </td>
 </tr>
</table>

<br> 
<p>Control point drag modifiers:</p>

<p>Currently there is 'selected' vs. 'highlighted' split. Plain key
affects highlighted node, Shift+key selected ones.</p>

<table border=0 width=100%>
 <tr>
  <td valign="top" width="100">  ctrl   </td>
  <td valign="top">    angle constrained movement (0, 15, ... degrees etc),
          relative to its node
  </td>
 </tr>

 <tr>
  <td valign="top">  alt   </td>
  <td valign="top">    move slower</td>
 </tr>

 <tr>
  <td valign="top">  ctrl-shift </td>
  <td valign="top">    keep the direction unchanged, change only distance
          from node</td>
 </tr>

 <tr>
  <td valign="top">  alt-shift </td>
  <td valign="top">    keep the distance unchanged, change only direction
          (i.e. rotate control point around the node)</td>
 </tr>

 <tr>
  <td valign="top">  ctrl-alt  </td>
  <td valign="top">    preserve the angle at the node: the opposite control
          point moves so that the angle between the handles is
          constant</td>
 </tr>
</table>

<br>
<p>Node drag modifiers:</p>

<table border=0 width=100%>
 <tr>
  <td valign="top" width="100">  ctrl  </td>
  <td valign="top">    angle constraint movement (0, 15, ... degrees etc),
          relative to the original position</td>
 </tr>

 <tr>
  <td valign="top">  alt  </td>
  <td valign="top">    move slower</td>
 </tr>

 <tr>
  <td valign="top">  ctrl-shift</td>
  <td valign="top">    move the node but not its control points</td>
 </tr>
</table>

<p>Copying and pasting nodes:</p>

<p>When a node is selected and ctrl-c is pressed, what goes to cilpboard is
the node type (cusp/smooth) and the _relative_ positions of its control
points, but not the coordinates of the node itself. To paste it, you
must first select one or more nodes; then you press ctrl-v and the
stored parameters are applied to the selected nodes (only their control
points are moved but not the nodes themselves). If you selected and
copied e.g. 4 nodes, and then select 6 nodes and press ctrl-v, the first
4 nodes get the parameters of the 4 copied nodes, and the remaining 2
nodes become the same as the first two (i.e. if the number of copied
nodes is more than the number of nodes selected during paste, the
sequence is truncated, otherwise it is repeated). The cut operation
(ctrl-x) is equivalent to the copy plus delete node.</p>



<h4>XML Editor</h4>

<table border=0 width=100%>
 <tr>
  <td valign="top" width="100">  Del  </td>
  <td valign="top">    delete element or attribute </td>
 </tr>

 <tr>
  <td valign="top">  Ins  </td>
  <td valign="top">    new element or attribute (depending on which pane
          is in focus)</td>
 </tr>

 <tr>
  <td valign="top">  ctr z, ctrl r, etc </td>
  <td valign="top">    undo and redo must work in the editor, undoing
          the last XML editor action</td>
 </tr>

 <tr>
  <td valign="top">  shift-arrows </td>
  <td valign="top">    selecting more than one element or attribute</td>
 </tr>

 <tr>
  <td valign="top">  ctrl-c, ctrl-x, ctrl-v  </td>
  <td valign="top">    copying and pasting must work on selected
          lines in the lists, so you can rearrange the tree by cutting
          and pasting stuff</td>
 </tr>
</table>

<h4>Everywhere</h4>

<p>Calling dialogs (these shortcult must toggle corresponding dialogs
on or off):</p>

<table border=0 width=100%>
 <tr>
  <td valign="top" width="100">  Tab </td>
  <td valign="top">    Cycle selection among items.  </td>
 </tr>

 <tr>
  <td valign="top" width="100">  Shift-F10 </td>
  <td valign="top">    Open the "Right Mouse Menu".  See 
   <A HREF="http://developer.gnome.org/projects/gup/hig/1.0/menus.html#menu-type-popup">GNOME
    HIG for menu-type-popup</A>
  </td>
 </tr>

 <tr>
  <td valign="top" width="100">  F12 </td>
  <td valign="top">    make all dialogs temporarily disappear;
         another Tab restores them.  This will provide sort of a "full
         screen mode" - an unobstructed view of your drawing.
  </td>
 </tr>

 <tr>
  <td valign="top" width="100">  ctrl T </td>
  <td valign="top">    transform selection</td>
 </tr>

 <tr>
  <td valign="top">  ctrl F </td>
  <td valign="top">    fill and stroke (by the way the dialog is named
          differently than the command - needs to be unified)</td>
 </tr>

 <tr>
  <td valign="top">  ctrl O </td>
  <td valign="top">    tool options </td>
 </tr>

 <tr>
  <td valign="top">  ctrl E </td> 
  <td valign="top">    text properties (again name of the command is different)</td>
 </tr>

 <tr>
  <td valign="top">  ctrl A  </td>
  <td valign="top">    align objects</td>
 </tr>

 <tr>
  <td valign="top">  ctrl D </td>
  <td valign="top">    desktop settings (again name of the command is different)</td>
 </tr>

 <tr>
  <td valign="top" width="100">  ctrl X </td>
  <td valign="top">    XML editor</td>
 </tr>
</table>

<p>Changing tools:</p>

<p>Gimp uses letter keys to change tools (z for zoom, etc.) It's very
convenient and we should enable this too (in addition to function
keys). It's also useful since there are more tools than function keys,
so we can use letters to select arc, star, etc which are not accessible
via F keys.</p>

<p>One problem: in node editor c, s, b, and y are used for various
functions specific to this tool. Perhaps we'll need to remap them to
some ctrl- or alt- combinations. Also, the letter keys will be blocked
by the text tool and when you are editing values in dialog text fields,
but that's normal.</p>

<table border=0 width=100%>
 <tr>
  <td valign="top" width="100">s</td><td>
    Selector
  </td></tr><tr>
  <td valign="top" width="100">n</td><td>
   Node edit
  </td></tr><tr>
  <td valign="top" width="100">r</td><td>
    Rectangle
  </td></tr><tr>
  <td valign="top" width="100">c</td><td>
    Circle/arc
  </td></tr><tr>
  <td valign="top" width="100">p</td><td>
    Pencil
  </td></tr><tr>
  <td valign="top" width="100">b</td><td>
    pen (because it makes Beziers)
  </td></tr><tr>
  <td valign="top" width="100">l</td><td>
    caLLiraphic
  </td></tr><tr>
  <td valign="top" width="100">g</td><td>
    Gradient (to be implemented)
  </td></tr><tr>
  <td valign="top" width="100">m</td><td>
    Mask (to be implemented)
  </td></tr><tr>
  <td valign="top" width="100">t</td><td>
    Text
  </td></tr><tr>
  <td valign="top" width="100">d</td><td>
    Dropper
  </td></tr><tr>
  <td valign="top" width="100">z</td><td>
    Zoom
  </td></tr><tr>
  <td valign="top" width="100">f</td><td>
    flip horizontally
  </td></tr><tr>
  <td valign="top" width="100">F</td><td>
    flip vertically
  </td>
 </tr>
</table>

<p>As for single-letter keys in node editor, maybe move them to
alt-letter?  Or ssign unconditional (working in text context and text
entry fields) Alt variants to tool keys instead?  </p>


<h4>Missing pieces</h4>

<p>These shortcuts are listed on the
<a
href="http://sodipodi.sourceforge.net/index.php3?section=documentation/usage/keybindings">keybindings
page</a> on the website but don't appear to work (0.31.1 on linux), or
are implemented but undocumented, or need to be remapped:

<table border=0 width=100%>
 <tr>
  <td valign="top" width="100">  Ctrl e </td>
  <td valign="top">    export file </td>
 </tr>

 <tr>
  <td valign="top" width="100">  Ctrl P  </td>
  <td valign="top">    print preview</td>
 </tr>

 <tr>
  <td valign="top" width="100">  F7  </td>
  <td valign="top">    dropper.  Currently available as F8</td>
 </tr>

 <tr>
  <td valign="top" width="100">  F8  </td>
  <td valign="top">    Text tool.  Currently available as F7, but more
                       consistent with Xara if it is available as F8.</td>
 </tr>

 <tr>
  <td valign="top" width="100">  F10  </td>
  <td valign="top">    "Sodipodi" menu on toolbox</td>
 </tr>
</table>






