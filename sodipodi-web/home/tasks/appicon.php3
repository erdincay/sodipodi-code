<h3>AppIcons</h3>

<p>Different window managers and desktop environments provide the
ability to install icons into the window framework (start menus, task
bars, etc.)</p>


<h4>Create AppIcon for GNOME/KDE Tasklist</h4>

<PRE>
    file = gnome_program_locate_file (NULL,
               GNOME_FILE_DOMAIN_APP_PIXMAP,
              "sodipodi.png", TRUE, NULL);
    if (file != NULL)
    {
        gnome_window_icon_set_default_from_file (file);
        g_free (file);
    }
</PRE>

<p>This can be easily done with Gtk+ as well.</p>



