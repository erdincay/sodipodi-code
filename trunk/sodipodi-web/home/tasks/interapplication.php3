<h3>Interapplication Communication</h3>

<p>Sodipodi works well as an independent application, however there is
of course desire to see it interact easily with other office type
applications.  These tasks describe some features that would enable
this.</p>

<h4>Cut and paste with other applications</h4>

<p>Support cut and paste of SVG images between Sodipodi, Abiword,
Gnumeric, Gimp, and other applications.  Sodipodi needs to post
image/svg+xml data to the clipboard and also be able to recieve the
same.  Support of GNOME, KDE, Windows, Mac, and other cut and paste
systems is desired, in that order of importance.</p>

<h4>Bonobo Editor Control</h4>

<p>This would permit other applications to do inline editing of SVG
files.  This has to be done as a compile-time option and will require a
dedicated maintainer.</p>

<p>Jody Goldberg had looked into this, but preferred shared library
build of Sodipodi, due to complexities encountered doing Gnumeric
charting with Guppi/bonobo.</p>

<p>There was an implementation of this in an earlier version of Sodipodi
but was disabled when it got out of sync.</p>

<h4>Bonobo Viewer Control</h4>

<p>Sodipodi should not start new instance when selecting SVG images in
Nautilus, instead new images should be loaded into existing instance.
This is done by basically needs to make it a "factory",
gedit-application-server.c is a good place to look at an example.</p>


<h4>GNOME panel recent-files</h4>

<p>See libegg (gedit etc. already use it).  See src/gedit-file.c for
example implementation.  Also investigate if there is a freedesktop
standard for this.</p>


