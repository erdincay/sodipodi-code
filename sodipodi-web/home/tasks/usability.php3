
<h3>Usability</h3>

<h4>Forwarding unhandled key events from dialogs to canvas</h4>

<p><i>Status:</i> Proposed</p>

<p>Dialogs do not use F1..F10 keys, but they block them - when you have
focus on e.g. object style dialog, you cannot use F1 to switch to
selector. Those keys which are not handled by a dialog should be
forwarded to the canvas window for handling.</p>

<h4>Configuring shortcuts</h4>

<p><i>Status:</i> Proposed</p>

As a first step that can be implemented
quickly, a GUI-less option would be really great, with a text (XML?)
config file that lists keys and actions. Is this doable?

<h4>Always on Top Dialog Windows</h4>

<p><i>Status:</i> Proposed</p>

<p>When using the app in maximized mode, dialogs "disappear" behind the
canvas.  The main toolbox can be docked which removes this problem for
it, but not the others.  It can be possible to manually set the windows
to be always on top (like in KDE), but window status is not remembered
from call to call.</p>

<p>Make it possible to configure Sodipodi to allow appropriate dialog
windows to be displayed alwayus on top.</p>

<h4>Focus Behavior</h4>

<p><i>Status:</i> Proposed</p>

<p>All dialog windows should return focus to the (last used) canvas
window <u>whenever possible</u>. I.e. only if the focus is on a specific
widget, such as text field or a list, the dialog window has focus; as
soon as you leave all widgets, canvas gets focus again. Which means, for
example, that the toolbox window is never in focus because it has no
widgets that use keyboard. Of course this will work only if the dialogs
are "always on top" (see above): the unfocused toolbox will just float
above your canvas and you can use it as usual by clicking buttons on it,
but it never steals your keyboard.</p>

<p>Note that clickable buttons that are accessible by cycling with Tab
key are also widgets that keep the window in focus. E.g. in the "Text
properties" dialog, if you start with any widget such as font list, you
can use Tab to move to any other widget in this dialog, and the dialog
will stay in focus. In this case, you'll need a key to leave the dialog.
Standardise Esc as a shortcut to leave the dialog (by moving focus to
the canvas) without making changes, and Enter as a shortcut to apply the
changes and, too, leave the dialog by moving focus to the canvas (but
without closing the dialog - this should be another shortcut). These Esc
and Enter actions must be standard across ALL dialogs. (Currently Esc
does nothing and Enter activates the current widget - this must be done
only by space.) We will also need new shortcuts to jump focus to the
common dialogs (and open them first if they're not yet open).</p>

<p>On the other hand things which are not controllable by keyboard
should never steal focus. This includes e.g. the buttons on the toolbox
and the color sliders in any color editor. That is, you can move the
slider with a mouse but as soon as you release it the focus is returned
to the canvas.</p>

<h4>Right mouse menu</h4>

<p>In reviewing the Gnome HIT and the Sodipodi menu, Tobias
Jakobs [tobias.jakobs/web_de] makes the following recommendations: 

<ul>
<li> "View" should be above "Selection"
<li> "Print Preview..." should be above "Print"
<li> Revert function would be useful. (Reverts the document to the
last saved state.)
<li> In the File menu is the "About Sodipodi" the last item in the
"Sodipodi" menu not View.
<li> "New Docked Toolbox" and "Remove Docked Toolbox" should be a Checkbox
Item
</ul>


<h4>References</h4>

<p><a href="http://sourceforge.net/mailarchive/forum.php?thread_id=1799679&forum_id=3970">http://sourceforge.net/mailarchive/forum.php?thread_id=1799679&forum_id=3970</a>

