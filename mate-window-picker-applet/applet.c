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

#include <libmatewnck/libmatewnck.h>

#include <gio/gio.h>
#include <mate-panel-applet.h>
#include <mate-panel-applet-gsettings.h>

#include "task-list.h"
#include "task-title.h"

#define APPLET_SCHEMA "org.mate.panel.applet.mate-window-picker-applet"
#define SHOW_WIN_KEY "show-all-windows"

#define MAXIMUS_SCHEMA "org.mate.maximus"
#define UNDECORATE_KEY "undecorate"
#define NO_MAXIMIZE_KEY "no-maximize"

typedef struct 
{
  GtkWidget    *tasks;
  GtkWidget    *applet;
  GtkWidget    *title;
  GSettings    *settings;
  
} WinPickerApp;

static WinPickerApp *mainapp;
static gpointer parent_class;

static void cw_panel_background_changed (MatePanelApplet               *applet,
                                         MatePanelAppletBackgroundType  type,
				         GdkColor                  *colour,
				         GdkPixmap                 *pixmap,
                                         gpointer                   user_data);
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
  /* disable Maximus */
  GSettings *maximus_settings;
  maximus_settings = g_settings_new (MAXIMUS_SCHEMA);
  g_settings_set_boolean (maximus_settings, UNDECORATE_KEY, FALSE);
  g_settings_set_boolean (maximus_settings, NO_MAXIMIZE_KEY, TRUE);
  g_object_unref (maximus_settings);

  if (G_OBJECT_CLASS (parent_class)->finalize)
    (* G_OBJECT_CLASS (parent_class)->finalize) (object);
}

static gboolean
cw_applet_fill (MatePanelApplet *applet, 
                const gchar *iid, 
                gpointer     data)
{
  MatewnckScreen *screen;
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

  matewnck_set_client_type (MATEWNCK_CLIENT_TYPE_PAGER);
  
  app = g_slice_new0 (WinPickerApp);
  mainapp = app;
  screen = matewnck_screen_get_default ();

  /* prepare to disable Maximus */
  object_class = G_OBJECT_GET_CLASS (G_OBJECT(applet));
  object_class->finalize = cw_applet_finalize;
  parent_class = g_type_class_peek_parent (object_class);

  /* enable Maximus */
  GSettings *maximus_settings;
  maximus_settings = g_settings_new (MAXIMUS_SCHEMA);
  g_settings_set_boolean (maximus_settings, UNDECORATE_KEY, TRUE);
  g_settings_set_boolean (maximus_settings, NO_MAXIMIZE_KEY, FALSE);
  g_object_unref (maximus_settings);

  /* gsettings prefs */
  app->settings = mate_panel_applet_settings_new (applet, APPLET_SCHEMA);
  g_signal_connect (app->settings, "changed::" SHOW_WIN_KEY,
                    G_CALLBACK (on_show_all_windows_changed), app);

  app->applet = GTK_WIDGET (applet);
  force_no_focus_padding (GTK_WIDGET (applet));
  gtk_container_set_border_width (GTK_CONTAINER (applet), 0);

  eb = gtk_hbox_new (FALSE, 6);
	gtk_container_add (GTK_CONTAINER (applet), eb);
  gtk_container_set_border_width (GTK_CONTAINER (eb), 0);

  tasks = app->tasks = task_list_get_default ();
  gtk_box_pack_start (GTK_BOX (eb), tasks, FALSE, FALSE, 0);

  title = app->title = task_title_new ();
  gtk_box_pack_start (GTK_BOX (eb), title, TRUE, TRUE, 0);

  gtk_widget_show_all (GTK_WIDGET (applet));
	
  on_show_all_windows_changed (app->settings, SHOW_WIN_KEY, app);
		
  /* Signals */
	g_signal_connect (applet, "change-background",
			             G_CALLBACK (cw_panel_background_changed), NULL);
	

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
cw_panel_background_changed (MatePanelApplet               *applet,
                             MatePanelAppletBackgroundType  type,
                             GdkColor                  *colour,
                             GdkPixmap                 *pixmap,
                             gpointer                   user_data)
{
  GtkRcStyle *rc_style;
  GtkStyle *style;

  /* reset style */
  gtk_widget_set_style (GTK_WIDGET (applet), NULL);
  rc_style = gtk_rc_style_new ();
  gtk_widget_modify_style (GTK_WIDGET (applet), rc_style);
  gtk_rc_style_unref (rc_style);

  gtk_widget_set_style (mainapp->title, NULL);
  rc_style = gtk_rc_style_new ();
  gtk_widget_modify_style (mainapp->title, rc_style);
  gtk_rc_style_unref (rc_style);

  switch (type) 
  {
    case PANEL_NO_BACKGROUND:
      break;
    case PANEL_COLOR_BACKGROUND:
      gtk_widget_modify_bg (GTK_WIDGET (applet), GTK_STATE_NORMAL, colour);
      gtk_widget_modify_bg (mainapp->title, GTK_STATE_NORMAL, colour); 
      break;
    
    case PANEL_PIXMAP_BACKGROUND:
      style = gtk_style_copy (GTK_WIDGET (applet)->style);
      if (style->bg_pixmap[GTK_STATE_NORMAL])
        g_object_unref (style->bg_pixmap[GTK_STATE_NORMAL]);
      style->bg_pixmap[GTK_STATE_NORMAL] = g_object_ref (pixmap);
      gtk_widget_set_style (GTK_WIDGET (applet), style);
      g_object_unref (style);

      /*style = gtk_style_copy (mainapp->title->style);
      if (style->bg_pixmap[GTK_STATE_NORMAL])
        g_object_unref (style->bg_pixmap[GTK_STATE_NORMAL]);
      style->bg_pixmap[GTK_STATE_NORMAL] = g_object_ref (pixmap);
      gtk_widget_set_style (mainapp->title, style);
      g_object_unref (style);*/
 
      break;
  }
}

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
                "logo-icon-name", "preferences-system-windows",
                "copyright", "Copyright \xc2\xa9 2008 Canonical Ltd\n"
                             "Copyright \xc2\xa9 2013 Stefano Karapetsas",
                NULL);

  gtk_widget_show (panel_about_dialog);

  g_signal_connect (panel_about_dialog, "response",
                    G_CALLBACK (gtk_widget_destroy), NULL);
	

  gtk_window_present (GTK_WINDOW (panel_about_dialog));
}

static void
on_checkbox_toggled (GtkToggleButton *check, gpointer null)
{
  gboolean is_active;
    
  is_active = gtk_toggle_button_get_active (check);

  g_settings_set_boolean (mainapp->settings, SHOW_WIN_KEY, is_active);
}

static void
display_prefs_dialog (GtkAction       *action,
                      WinPickerApp *applet)
{
  GtkWidget *window, *box, *vbox, *nb, *hbox, *label, *check, *button;

  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title (GTK_WINDOW (window), _("Preferences"));
  gtk_window_set_type_hint (GTK_WINDOW (window),
                            GDK_WINDOW_TYPE_HINT_DIALOG);
  gtk_container_set_border_width (GTK_CONTAINER (window), 12);

  box = gtk_vbox_new (FALSE, 8);
  gtk_container_add (GTK_CONTAINER (window), box);

  nb = gtk_notebook_new ();
  g_object_set (nb, "show-tabs", FALSE, "show-border", TRUE, NULL);
  gtk_box_pack_start (GTK_BOX (box), nb, TRUE, TRUE, 0);

  vbox = gtk_vbox_new (FALSE, 8);
  gtk_container_set_border_width (GTK_CONTAINER (vbox), 8);
  gtk_notebook_append_page (GTK_NOTEBOOK (nb), vbox, NULL);

  check = gtk_check_button_new_with_label (_("Show windows from all workspaces"));
  gtk_box_pack_start (GTK_BOX (vbox), check, FALSE, TRUE, 0);
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (check),
                                g_settings_get_boolean (mainapp->settings, SHOW_WIN_KEY));
  g_signal_connect (check, "toggled",
                    G_CALLBACK (on_checkbox_toggled), NULL);

  check = gtk_label_new (" ");
  gtk_box_pack_start (GTK_BOX (vbox), check, TRUE, TRUE, 0);

  gtk_widget_set_size_request (nb, -1, 100);
  
  hbox = gtk_hbox_new (FALSE, 0);
  gtk_box_pack_start (GTK_BOX (box), hbox, TRUE, TRUE, 0);
  
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
