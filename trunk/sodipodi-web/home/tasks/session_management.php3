<h3>Session Management</h3>

<p>In X, it is traditional for users to be able to keep their Sodipodi
windows across sessions, and we wish for the same with Sodipodi.  The
first (and biggest) part of work is to save/load window positions/sizes
from configuration.  Do not use GConf for this, though, since Havoc was
strong against using it for things like this.</p>

