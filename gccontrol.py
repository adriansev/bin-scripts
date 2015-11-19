#!/usr/bin/python
# Simple GTK+ frontend to ccontrol

import gtk
import egg.trayicon
import os
import stat

LIBDIR = "/usr/local/lib/ccontrol"


def unexpand_user(path):
    home = os.getenv('HOME')
    common = os.path.commonprefix([path, home])
    if len(common) > 0:
        return '~%s' % path[len(common):]
    else:
        return path


class ConfigNotSymlinkException(Exception):
    def __init__(self, file, dir):
        self.file = file
        self.dir = dir


class ConfigManager(object):
    CC_DIR  = os.path.join(os.getenv('HOME'), '.ccontrol')
    CONFIG = os.path.join(CC_DIR, 'config')

    def config_files(self):
        try:
            for file in os.listdir(self.CC_DIR):
                path = os.path.join(self.CC_DIR, file)
                if not os.path.isfile(path) or os.path.islink(path) or \
                        file.startswith('.') or file.endswith('~'):
                    continue
                yield file
        except OSError, e:
            if getattr(e, 'errno', None) == 2:
                return

    def get_config(self):
        if os.path.islink(self.CONFIG):
            return os.readlink(self.CONFIG)
        elif os.path.exists(self.CONFIG):
            raise ConfigNotSymlinkException(self.CONFIG, self.CC_DIR)
        else:
            return None

    def set_config(self, file):
        # Remove file if it already existed
        if os.access(self.CONFIG, os.F_OK):
            os.unlink(self.CONFIG)
        os.symlink(file, self.CONFIG)

class ConfigEditor(gtk.Window):
    class ConfigTab(gtk.Frame):
        def __init__(self, file):
            gtk.Frame.__init__(self)
            self.file = file

        def save(self):
            pass

    def button_response(self, resp):
        if resp == "cancel":
            self.hide_all()
        elif resp == "ok":
            for p in range(0,self.notebook.get_n_pages()):
                self.notebook.get_nth_page(p).save()
            self.hide_all()

    def __init__(self):
        gtk.Window.__init__(self)

        self.set_title("Manage ccontrol profiles")
        self.set_default_size(400, 400)

        vbox = gtk.VBox()

        self.notebook = gtk.Notebook()

        cm = ConfigManager()
        for f in cm.config_files():
            self.notebook.append_page(ConfigEditor.ConfigTab(f), gtk.Label(f))

        vbox.add(self.notebook)

        buttons = gtk.HButtonBox()

        ok_button = gtk.Button(stock='gtk-ok')
        ok_button.connect_object("clicked", self.button_response, "ok")
        buttons.add(ok_button)

        cancel_button = gtk.Button(stock='gtk-cancel')
        cancel_button.connect_object("clicked", self.button_response, "cancel")
        buttons.add(cancel_button)

        vbox.add(buttons)

        self.add(vbox)

class ProfilePopup(gtk.Menu):
    def menuitem_response(self, file):
        try:
            self._cm.set_config(file)
        except Exception, e:
            dialog = gtk.MessageDialog(None, flags=gtk.DIALOG_MODAL,
                    type=gtk.MESSAGE_ERROR, buttons=gtk.BUTTONS_CLOSE)
            dialog.set_markup('<b>An error occurred switching ' \
                    'configuration files!</b>')
            dialog.format_secondary_markup('The error was: <tt>%s</tt>' % e)
            dialog.connect("response", lambda dialog, resp: dialog.destroy())
            dialog.show()
            raise e

    def __init__(self, event):
        gtk.Menu.__init__(self)
        self._cm = ConfigManager()

        try:
            current = self._cm.get_config()
        except ConfigNotSymlinkException, e:
            dialog = gtk.MessageDialog(None, flags=gtk.DIALOG_MODAL,
                    type=gtk.MESSAGE_ERROR,
                    buttons=gtk.BUTTONS_CLOSE)
            dialog.set_markup('<b><tt>%s</tt> is not a symbolic link.</b>' % \
                        unexpand_user(e.file))
            dialog.format_secondary_markup('To use the ccontrol GUI you ' \
                        'must symlink <tt>%s</tt> to a configuration file ' \
                        'in <tt>%s</tt>' % (unexpand_user(e.file),
                            unexpand_user(e.dir)))
            dialog.connect("response", lambda dialog, resp: dialog.destroy())
            dialog.show()
            return

        found = False
        for f in self._cm.config_files():
            found = True
            item = gtk.CheckMenuItem(f)
            item.set_draw_as_radio(True)
            if f == current:
                item.set_active(True)
            self.append(item)
            item.connect_object("activate", self.menuitem_response, f)

        if not found:
            item = gtk.MenuItem('No config files found')
            item.set_property('sensitive', False)
            self.append(item)

        self.show_all()
        self.popup(None, None, None, event.button, event.time)

class TrayIcon(egg.trayicon.TrayIcon):
    def __init__(self):
        egg.trayicon.TrayIcon.__init__(self, 'ccontrol')

        self._gui = ConfigEditor()

        img = gtk.Image()
        img.set_from_file("%s/ccontrol-key.png" % LIBDIR)
        eventbox = gtk.EventBox()
        eventbox.add(img)
        eventbox.set_events(gtk.gdk.BUTTON_PRESS_MASK)
        eventbox.connect_object("button_press_event", self.tray_clicked, self)
        self.add(eventbox)

    def tray_clicked(self, widget, event):
        if event.button == 1:
            ProfilePopup(event)
        elif event.button == 3:
            self._gui.show_all()

def main():
    tray_icon = TrayIcon()
    tray_icon.show_all()
    gtk.main()

main()
