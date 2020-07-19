/*
 * Copyright (C) 2008 Canonical Ltd
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Authored by Neil Jagdish Patel <neil.patel@canonical.com>
 *
 */

#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <string.h>
#include <stdlib.h>

#include <glib.h>
#include <glib/gi18n.h>
#include <gtk/gtk.h>

#define WNCK_I_KNOW_THIS_IS_UNSTABLE
#include <libwnck/libwnck.h>

#include <gio/gio.h>
#include <mate-panel-applet.h>
#include <mate-panel-applet-gsettings.h>

#include "task-list.h"
#include "task-title.h"

#define APPLET_SCHEMA "org.mate.panel.applet.mate-window-picker-applet"
#define SHOW_WIN_KEY "show-all-windows"
#define SHOW_HOME_TITLE_KEY "show-home-title"
#define BOLD_WINDOW_TITLE_KEY "bold-window-title"

typedef struct
{
  GtkWidget    *tasks;
  GtkWidget    *applet;
  GtkWidget    *title;
  GSettings    *settings;

} WinPickerApp;

static WinPickerApp *mainapp;
static gpointer parent_class;

static void display_about_dialog (GtkAction    *action,
                                  WinPickerApp *applet);

static void display_prefs_dialog (GtkAction    *action,
                                  WinPickerApp *applet);

static const GtkActionEntry window_picker_menu_actions [] = {
  { "MenuPrefs", GTK_STOCK_PREFERENCES, N_("_Preferences"),
      NULL, NULL,
      G_CALLBACK (display_prefs_dialog) },
  { "MenuAbout", GTK_STOCK_ABOUT, N_("_About"),
      NULL, NULL,
      G_CALLBACK (display_about_dialog) }
};

static const gchar *close_window_authors [] = {
  "Neil J. Patel <neil.patel@canonical.com>",
  "Stefano Karapetsas <stefano@karapetsas.com>",
  NULL
};

static void
on_show_all_windows_changed (GSettings   *settings,
                             gchar       *key,
                             gpointer     data)
{
  WinPickerApp *app;
  gboolean show_windows = TRUE;

  app = (WinPickerApp*)data;

  show_windows = g_settings_get_boolean (settings, SHOW_WIN_KEY);

  g_object_set (app->tasks, "show_all_windows", show_windows, NULL);
}

static void
on_show_home_title_changed (GSettings   *settings,
                            gchar       *key,
                            gpointer     data)
{
  WinPickerApp *app;
  gboolean show_home = FALSE;

  app = (WinPickerApp*)data;

  show_home = g_settings_get_boolean (settings, SHOW_HOME_TITLE_KEY);
  g_object_set (app->title, "show_home_title", show_home, NULL);
}

static void
on_bold_window_title_changed (GSettings   *settings,
                              gchar       *key,
                              gpointer     data)
{
  WinPickerApp *app;
  gboolean bold_win = TRUE;

  app = (WinPickerApp*)data;

  bold_win = g_settings_get_boolean (settings, BOLD_WINDOW_TITLE_KEY);
  g_object_set (app->title, "bold_window_title", bold_win, NULL);
}

static inline void
force_no_focus_padding (GtkWidget *widget)
{
  static gboolean first_time = TRUE;

  if (first_time)
  {
    gtk_rc_parse_string ("\n"
    " style \"na-tray-style\"\n"
    " {\n"
    " GtkWidget::focus-line-width=0\n"
    " GtkWidget::focus-padding=0\n"
    " }\n"
    "\n"
    " widget \"*.na-tray\" style \"na-tray-style\"\n"
    "\n");
    first_time = FALSE;
  }

  gtk_widget_set_name (widget, "na-tray");
}

static void
cw_applet_finalize (GObject *object)
{
  if (G_OBJECT_CLASS (parent_class)->finalize)
    (* G_OBJECT_CLASS (parent_class)->finalize) (object);
}

static gboolean
cw_applet_fill (MatePanelApplet *applet,
                const gchar *iid,
                gpointer     data)
{
  WinPickerApp *app;
  GtkWidget *eb, *tasks, *title;
  gchar *ui_path;
  GtkActionGroup *action_group;
  GObjectClass *object_class;

  if (strcmp (iid, "MateWindowPicker") != 0)
    return FALSE;

  bindtextdomain (GETTEXT_PACKAGE, MATELOCALEDIR);
  bind_textdomain_codeset (GETTEXT_PACKAGE, "UTF-8");
  textdomain (GETTEXT_PACKAGE);

  /* Have our background automatically painted. */
  mate_panel_applet_set_background_widget (MATE_PANEL_APPLET (applet), GTK_WIDGET (applet));

  wnck_set_client_type (WNCK_CLIENT_TYPE_PAGER);

  app = g_slice_new0 (WinPickerApp);
  mainapp = app;

  object_class = G_OBJECT_GET_CLASS (G_OBJECT(applet));
  object_class->finalize = cw_applet_finalize;
  parent_class = g_type_class_peek_parent (object_class);

  /* gsettings prefs */
  app->settings = mate_panel_applet_settings_new (applet, APPLET_SCHEMA);
  g_signal_connect (app->settings, "changed::" SHOW_WIN_KEY,
                    G_CALLBACK (on_show_all_windows_changed), app);
  g_signal_connect (app->settings, "changed::" SHOW_HOME_TITLE_KEY,
                    G_CALLBACK (on_show_home_title_changed), app);
  g_signal_connect (app->settings, "changed::" BOLD_WINDOW_TITLE_KEY,
                    G_CALLBACK (on_bold_window_title_changed), app);

  app->applet = GTK_WIDGET (applet);
  force_no_focus_padding (GTK_WIDGET (applet));
  gtk_container_set_border_width (GTK_CONTAINER (applet), 0);

  eb = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 6);
  gtk_container_add (GTK_CONTAINER (applet), eb);
  gtk_container_set_border_width (GTK_CONTAINER (eb), 0);

  tasks = app->tasks = task_list_get_default ();
  gtk_box_pack_start (GTK_BOX (eb), tasks, FALSE, FALSE, 0);

  title = app->title = task_title_new ();
  gtk_box_pack_start (GTK_BOX (eb), title, TRUE, TRUE, 0);

  gtk_widget_show_all (GTK_WIDGET (applet));

  on_show_all_windows_changed (app->settings, SHOW_WIN_KEY, app);
  on_show_home_title_changed (app->settings, SHOW_HOME_TITLE_KEY, app);
  on_bold_window_title_changed (app->settings, BOLD_WINDOW_TITLE_KEY, app);

  action_group = gtk_action_group_new ("MateWindowPicker Applet Actions");
  gtk_action_group_set_translation_domain (action_group, GETTEXT_PACKAGE);
  gtk_action_group_add_actions (action_group,
                                window_picker_menu_actions,
                                G_N_ELEMENTS (window_picker_menu_actions),
                                app);
  ui_path = g_build_filename (MATEWINDOWPICKER_MENU_UI_DIR, "mate-window-picker-applet-menu.xml", NULL);
  mate_panel_applet_setup_menu_from_file (MATE_PANEL_APPLET (app->applet),
                                          ui_path, action_group);
  g_free (ui_path);
  g_object_unref (action_group);

  mate_panel_applet_set_flags (MATE_PANEL_APPLET (applet),
                               MATE_PANEL_APPLET_EXPAND_MAJOR
                               | MATE_PANEL_APPLET_EXPAND_MINOR
                               | MATE_PANEL_APPLET_HAS_HANDLE);

  return TRUE;
}

MATE_PANEL_APPLET_OUT_PROCESS_FACTORY ("MateWindowPickerFactory",
                                       PANEL_TYPE_APPLET,
                                       "MateWindowPicker",
                                       cw_applet_fill,
                                       NULL);

static void
display_about_dialog (GtkAction       *action,
                      WinPickerApp *applet)
{
  GtkWidget *panel_about_dialog;

  panel_about_dialog = gtk_about_dialog_new ();

  g_object_set (panel_about_dialog,
                "name", _("Window Picker"),
                "comments", _("Window Picker"),
                "version", PACKAGE_VERSION,
                "authors", close_window_authors,
                "icon-name", "preferences-system-windows",
                "logo-icon-name", "preferences-system-windows",
                "copyright", "Copyright \xc2\xa9 2008 Canonical Ltd\n"
                             "Copyright \xc2\xa9 2013-2014 Stefano Karapetsas\n"
                             "Copyright \xc2\xa9 2015-2020 MATE developers",
                NULL);

  gtk_widget_show (panel_about_dialog);

  g_signal_connect (panel_about_dialog, "response",
                    G_CALLBACK (gtk_widget_destroy), NULL);

  gtk_window_present (GTK_WINDOW (panel_about_dialog));
}

static void
on_show_win_key_checkbox_toggled (GtkToggleButton *check, gpointer null)
{
  gboolean is_active;

  is_active = gtk_toggle_button_get_active (check);

  g_settings_set_boolean (mainapp->settings, SHOW_WIN_KEY, is_active);
}

static void
on_show_home_title_checkbox_toggled (GtkToggleButton *check, gpointer null)
{
  gboolean is_active;

  is_active = gtk_toggle_button_get_active (check);

  g_settings_set_boolean (mainapp->settings, SHOW_HOME_TITLE_KEY, is_active);
}

static void
on_bold_window_title_checkbox_toggled (GtkToggleButton *check, gpointer null)
{
  gboolean is_active;

  is_active = gtk_toggle_button_get_active (check);

  g_settings_set_boolean (mainapp->settings, BOLD_WINDOW_TITLE_KEY, is_active);
}

static void
display_prefs_dialog (GtkAction       *action,
                      WinPickerApp *applet)
{
  GtkWidget *window, *box, *vbox, *nb, *hbox, *label, *check, *button;

  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title (GTK_WINDOW (window), _("Preferences"));
  gtk_window_set_icon_name (GTK_WINDOW (window), "preferences-system-windows");
  gtk_window_set_type_hint (GTK_WINDOW (window),
                            GDK_WINDOW_TYPE_HINT_DIALOG);
  gtk_container_set_border_width (GTK_CONTAINER (window), 12);

  box = gtk_box_new (GTK_ORIENTATION_VERTICAL, 8);
  gtk_container_add (GTK_CONTAINER (window), box);

  nb = gtk_notebook_new ();
  g_object_set (nb, "show-tabs", FALSE, "show-border", TRUE, NULL);
  gtk_box_pack_start (GTK_BOX (box), nb, TRUE, TRUE, 0);

  vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 8);
  gtk_container_set_border_width (GTK_CONTAINER (vbox), 8);
  gtk_notebook_append_page (GTK_NOTEBOOK (nb), vbox, NULL);

  check = gtk_check_button_new_with_label (_("Show all windows"));
  gtk_widget_set_tooltip_text (GTK_WIDGET (check),
                               _("Show windows from all workspaces."));
  gtk_box_pack_start (GTK_BOX (vbox), check, FALSE, TRUE, 0);
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (check),
                                g_settings_get_boolean (mainapp->settings, SHOW_WIN_KEY));
  g_signal_connect (check, "toggled",
                    G_CALLBACK (on_show_win_key_checkbox_toggled), NULL);

  check = gtk_check_button_new_with_label (_("Show desktop title and logout button"));
  gtk_widget_set_tooltip_text (GTK_WIDGET (check),
                               _("Show a title for the desktop when no window is selected, and append a logout button."));
  gtk_box_pack_start (GTK_BOX (vbox), check, FALSE, TRUE, 0);
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (check),
                                g_settings_get_boolean (mainapp->settings, SHOW_HOME_TITLE_KEY));
  g_signal_connect (check, "toggled",
                    G_CALLBACK (on_show_home_title_checkbox_toggled), NULL);

  check = gtk_check_button_new_with_label (_("Bold windows title"));
  gtk_widget_set_tooltip_text (GTK_WIDGET (check),
                               _("Show windows title with a bold face."));
  gtk_box_pack_start (GTK_BOX (vbox), check, FALSE, TRUE, 0);
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (check),
                                g_settings_get_boolean (mainapp->settings, BOLD_WINDOW_TITLE_KEY));
  g_signal_connect (check, "toggled",
                    G_CALLBACK (on_bold_window_title_checkbox_toggled), NULL);

  check = gtk_label_new (" ");
  gtk_box_pack_start (GTK_BOX (vbox), check, TRUE, TRUE, 0);

  gtk_widget_set_size_request (nb, -1, 100);

  hbox = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
  gtk_box_pack_start (GTK_BOX (box), hbox, FALSE, FALSE, 0);

  label = gtk_label_new (" ");
  gtk_box_pack_start (GTK_BOX (hbox), label, TRUE, TRUE, 0);

  button = gtk_button_new_from_stock (GTK_STOCK_CLOSE);
  gtk_box_pack_start (GTK_BOX (hbox), button, FALSE, FALSE, 0);

  gtk_widget_show_all (window);

  g_signal_connect (window, "delete-event",
                    G_CALLBACK (gtk_widget_destroy), window);

  g_signal_connect (window, "destroy",
                    G_CALLBACK (gtk_widget_destroy), window);
  g_signal_connect_swapped (button, "clicked",
                            G_CALLBACK (gtk_widget_destroy), window);

  gtk_window_present (GTK_WINDOW (window));
}
