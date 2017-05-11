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

#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <gtk/gtk.h>
#include <gdk/gdkx.h>
#include <gio/gio.h>

#include "maximus-app.h"
#include "maximus-bind.h"
#include "xutils.h"

G_DEFINE_TYPE (MaximusApp, maximus_app, G_TYPE_OBJECT);

#define MAXIMUS_APP_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj),\
  MAXIMUS_TYPE_APP, \
  MaximusAppPrivate))

/* GSettings schemas and keys */
#define APP_SCHEMA        "org.mate.maximus"
#define APP_EXCLUDE_CLASS "exclude-class"
#define APP_UNDECORATE    "undecorate"
#define APP_NO_MAXIMIZE   "no-maximize"

/* A set of default exceptions */
static gchar *default_exclude_classes[] = 
{
  "Apport-gtk",
  "Bluetooth-properties",
  "Bluetooth-wizard",
  "Download", /* Firefox Download Window */
  "Ekiga",
  "Extension", /* Firefox Add-Ons/Extension Window */
  "Gcalctool",
  "Gimp",
  "Global", /* Firefox Error Console Window */
  "Mate-dictionary",
  "Mate-language-selector",
  "Mate-nettool",
  "Mate-volume-control",
  "Kiten",
  "Kmplot",
  "Nm-editor",
  "Pidgin",
  "Polkit-mate-authorization",
  "Update-manager",
  "Skype",
  "Toplevel", /* Firefox "Clear Private Data" Window */
  "Transmission"
};

struct _MaximusAppPrivate
{
  MaximusBind *bind;
  WnckScreen *screen;
  GSettings *settings;

  gchar **exclude_class_list;
  gboolean undecorate;
  gboolean no_maximize;
};

static GQuark was_decorated = 0;

/* <TAKEN FROM GDK> */
typedef struct {
    unsigned long flags;
    unsigned long functions;
    unsigned long decorations;
    long input_mode;
    unsigned long status;
} MotifWmHints, MwmHints;

#define MWM_HINTS_FUNCTIONS     (1L << 0)
#define MWM_HINTS_DECORATIONS   (1L << 1)
#define _XA_MOTIF_WM_HINTS		"_MOTIF_WM_HINTS"

static gboolean
wnck_window_is_decorated (WnckWindow *window)
{
  GdkDisplay *display = gdk_display_get_default();
  Atom hints_atom = None;
  guchar *data = NULL;
  MotifWmHints *hints = NULL;
  Atom type = None;
  gint format;
  gulong nitems;
  gulong bytes_after;
  gboolean retval;

  g_return_val_if_fail (WNCK_IS_WINDOW (window), FALSE);
  
  hints_atom = gdk_x11_get_xatom_by_name_for_display (display, 
                                                      _XA_MOTIF_WM_HINTS);

  XGetWindowProperty (GDK_DISPLAY_XDISPLAY (display), 
                      wnck_window_get_xid (window),
		                  hints_atom, 0, sizeof (MotifWmHints)/sizeof (long),
		                  False, AnyPropertyType, &type, &format, &nitems,
		                  &bytes_after, &data);
  
  if (type == None || !data) return TRUE;
  
  hints = (MotifWmHints *)data; 
  
  retval = hints->decorations;
  
  if (data)
    XFree (data);

  return retval;
}

static void
gdk_window_set_mwm_hints (WnckWindow *window,
                          MotifWmHints *new_hints)
{
  GdkDisplay *display = gdk_display_get_default();
  Atom hints_atom = None;
  guchar *data = NULL;
  MotifWmHints *hints = NULL;
  Atom type = None;
  gint format;
  gulong nitems;
  gulong bytes_after;

  g_return_if_fail (WNCK_IS_WINDOW (window));
  g_return_if_fail (GDK_IS_DISPLAY (display));
  
  hints_atom = gdk_x11_get_xatom_by_name_for_display (display, 
                                                      _XA_MOTIF_WM_HINTS);

  gdk_error_trap_push ();
  XGetWindowProperty (GDK_DISPLAY_XDISPLAY (display), 
                      wnck_window_get_xid (window),
		                  hints_atom, 0, sizeof (MotifWmHints)/sizeof (long),
		                  False, AnyPropertyType, &type, &format, &nitems,
		                  &bytes_after, &data);
  if (gdk_error_trap_pop ())
    return;
  
  if (type != hints_atom || !data)
    hints = new_hints;
  else
  {
    hints = (MotifWmHints *)data;
	
    if (new_hints->flags & MWM_HINTS_FUNCTIONS)
    {
      hints->flags |= MWM_HINTS_FUNCTIONS;
      hints->functions = new_hints->functions;  
    }
    if (new_hints->flags & MWM_HINTS_DECORATIONS)
    {
      hints->flags |= MWM_HINTS_DECORATIONS;
      hints->decorations = new_hints->decorations;
    }
  }
  
  _wnck_error_trap_push ();
  XChangeProperty (GDK_DISPLAY_XDISPLAY (display), 
                   wnck_window_get_xid (window),
                   hints_atom, hints_atom, 32, PropModeReplace,
                   (guchar *)hints, sizeof (MotifWmHints)/sizeof (long));
  gdk_flush ();
  _wnck_error_trap_pop ();
  
  if (data)
    XFree (data);
}

static void
_window_set_decorations (WnckWindow      *window,
			                   GdkWMDecoration decorations)
{
  MotifWmHints *hints;
  
  g_return_if_fail (WNCK_IS_WINDOW (window));
  
  /* initialize to zero to avoid writing uninitialized data to socket */
  hints = g_slice_new0 (MotifWmHints);
  hints->flags = MWM_HINTS_DECORATIONS;
  hints->decorations = decorations;
 
  gdk_window_set_mwm_hints (window, hints);

  g_slice_free (MotifWmHints, hints);
}

/* </TAKEN FROM GDK> */

gboolean
window_is_too_large_for_screen (WnckWindow *window)
{
  static GdkScreen *screen = NULL;
  gint x=0, y=0, w=0, h=0;

  g_return_val_if_fail (WNCK_IS_WINDOW (window), FALSE);

  if (screen == NULL)
    screen = gdk_screen_get_default ();
    
  wnck_window_get_geometry (window, &x, &y, &w, &h);
  
  /* some wiggle room */
  return (screen && 
          (w > (gdk_screen_get_width (screen) + 20) ||
           h > (gdk_screen_get_height (screen)+20)));
}

static gboolean
on_window_maximised_changed (WnckWindow *window)
{
  g_return_val_if_fail (WNCK_IS_WINDOW (window), FALSE);

  if (window_is_too_large_for_screen (window))
    {
      _window_set_decorations (window, 1);
      wnck_window_unmaximize (window);
    }
  else
    {
      _window_set_decorations (window, 0);
    }
  return FALSE;
}

static gboolean
enable_window_decorations (WnckWindow *window)
{
  g_return_val_if_fail (WNCK_IS_WINDOW (window), FALSE);

  _window_set_decorations (window, 1);
  return FALSE;
}

static void
on_window_state_changed (WnckWindow      *window,
                         WnckWindowState  change_mask,
                         WnckWindowState  new_state,
                         MaximusApp     *app)
{
  g_return_if_fail (WNCK_IS_WINDOW (window));

  if (GPOINTER_TO_INT (g_object_get_data (G_OBJECT (window), "exclude"))==1)
    return;
  
  if (change_mask & WNCK_WINDOW_STATE_MAXIMIZED_HORIZONTALLY
      || change_mask & WNCK_WINDOW_STATE_MAXIMIZED_VERTICALLY)
  {
    if (wnck_window_is_maximized (window) && app->priv->undecorate)
    {
      g_idle_add ((GSourceFunc)on_window_maximised_changed, window);
    }
    else
    {
      g_idle_add ((GSourceFunc)enable_window_decorations, window);
    }
  }
}

static gboolean
is_excluded (MaximusApp *app, WnckWindow *window)
{
  MaximusAppPrivate *priv;
  WnckWindowType type;
  WnckWindowActions actions;
  gchar *res_name;
  gchar *class_name;
  gint i;

  g_return_val_if_fail (MAXIMUS_IS_APP (app), TRUE);
  g_return_val_if_fail (WNCK_IS_WINDOW (window), TRUE);
  priv = app->priv;

  type = wnck_window_get_window_type (window);
  if (type != WNCK_WINDOW_NORMAL)
    return TRUE;

  if (GPOINTER_TO_INT (g_object_get_data (G_OBJECT (window), "exclude"))==1)
    return TRUE;

  /* Ignore if the window is already fullscreen */
  if (wnck_window_is_fullscreen (window))
  {
    g_debug ("Excluding (is fullscreen): %s\n",wnck_window_get_name (window));
    return TRUE;
  }
  
  /* Make sure the window supports maximising */
  actions = wnck_window_get_actions (window);
  if (actions & WNCK_WINDOW_ACTION_RESIZE
      && actions & WNCK_WINDOW_ACTION_MAXIMIZE_HORIZONTALLY
      && actions & WNCK_WINDOW_ACTION_MAXIMIZE_VERTICALLY
      && actions & WNCK_WINDOW_ACTION_MAXIMIZE)
    ; /* Is good to maximise */
  else
    return TRUE;

  _wnck_get_wmclass (wnck_window_get_xid (window), &res_name, &class_name);

  g_debug ("Window opened: res_name=%s -- class_name=%s", res_name, class_name);
 
  /* Check internal list of class_ids */
  for (i = 0; i < G_N_ELEMENTS (default_exclude_classes); i++)
  {
    if ((class_name && default_exclude_classes[i] 
        && strstr (class_name, default_exclude_classes[i]))
        || (res_name && default_exclude_classes[i] && strstr (res_name, 
                                            default_exclude_classes[i])))
    {
      g_debug ("Excluding: %s\n", wnck_window_get_name (window));
      return TRUE;
    } 
  }

  /* Check user list */
  for (i = 0; priv->exclude_class_list[i] != NULL; i++)
  {
    if ((class_name && strstr (class_name, priv->exclude_class_list[i]))
        || (res_name && strstr (res_name, priv->exclude_class_list[i]) ))
    {
      g_debug ("Excluding: %s\n", wnck_window_get_name (window));
      return TRUE;
    }
  }

  g_free (res_name);
  g_free (class_name);
  return FALSE;
}

extern gboolean no_maximize;

static void
on_window_opened (WnckScreen  *screen,
                  WnckWindow  *window,
                  MaximusApp *app)
{ 
  MaximusAppPrivate *priv;
  WnckWindowType type;
  gint exclude = 0;
  
  g_return_if_fail (MAXIMUS_IS_APP (app));
  g_return_if_fail (WNCK_IS_WINDOW (window));
  priv = app->priv;

  type = wnck_window_get_window_type (window);
  if (type != WNCK_WINDOW_NORMAL)
    return;

  /* Ignore undecorated windows */
  gdk_error_trap_push ();
  exclude = wnck_window_is_decorated (window) ? 0 : 1;
  if (gdk_error_trap_pop ())
    return;

  if (wnck_window_is_maximized (window))
    exclude = 0;
  g_object_set_data (G_OBJECT (window), "exclude", GINT_TO_POINTER (exclude));

  if (is_excluded (app, window))
  {
    g_signal_connect (window, "state-changed",
                      G_CALLBACK (on_window_state_changed), app);
    return;
  }

  if (no_maximize || priv->no_maximize)
  {
    if (wnck_window_is_maximized(window) && priv->undecorate)
    {
      _window_set_decorations (window, 0);
      gdk_flush ();
    }
    g_signal_connect (window, "state-changed",
                      G_CALLBACK (on_window_state_changed), app);
    return;
  }

  if (priv->undecorate)
  {
    /* Only undecorate right now if the window is smaller than the screen */
    if (!window_is_too_large_for_screen (window))
    {
      _window_set_decorations (window, 0);
      gdk_flush ();
    }
  }

  wnck_window_maximize (window);

  g_signal_connect (window, "state-changed",
                    G_CALLBACK (on_window_state_changed), app);
}

/* GSettings Callbacks */
static void
on_app_no_maximize_changed (GSettings *settings,
                            gchar *key,
                            MaximusApp *app)
{
  MaximusAppPrivate *priv;

  g_return_if_fail (MAXIMUS_IS_APP (app));
  priv = app->priv;
  priv->no_maximize = g_settings_get_boolean (settings, key);
}

static void
on_exclude_class_changed (GSettings *settings,
                          gchar *key,
                          MaximusApp         *app)
{
  MaximusAppPrivate *priv;
  
  g_return_if_fail (MAXIMUS_IS_APP (app));
  priv = app->priv;

  if (priv->exclude_class_list)
    g_strfreev (priv->exclude_class_list);
  
  priv->exclude_class_list= g_settings_get_strv (settings, 
                                                 APP_EXCLUDE_CLASS);
}

static gboolean
show_desktop (WnckScreen *screen)
{
  g_return_val_if_fail (WNCK_IS_SCREEN (screen), FALSE);
  
  wnck_screen_toggle_showing_desktop (screen, TRUE);
  return FALSE;
}

static void
on_app_undecorate_changed (GSettings          *settings,
                           gchar              *key,
                           MaximusApp         *app)
{
  MaximusAppPrivate *priv;
  GList *windows, *w;
    
  g_return_if_fail (MAXIMUS_IS_APP (app));
  priv = app->priv;
  g_return_if_fail (WNCK_IS_SCREEN (priv->screen));

  priv->undecorate = g_settings_get_boolean (settings, APP_UNDECORATE);
  g_debug ("%s\n", priv->undecorate ? "Undecorating" : "Decorating");
  
  windows = wnck_screen_get_windows (priv->screen);
  for (w = windows; w; w = w->next)
  {
    WnckWindow *window = w->data;

    if (!WNCK_IS_WINDOW (window))
      continue;

    if (no_maximize || priv->no_maximize)
    {
      if (!wnck_window_is_maximized(window))
        continue;
    }

    if (!is_excluded (app, window))
    {
      gdk_error_trap_push ();
      _window_set_decorations (window, priv->undecorate ? 0 : 1);
      wnck_window_unmaximize (window);
      wnck_window_maximize (window);
      gdk_flush ();
      gdk_error_trap_pop_ignored ();

      sleep (1);
    }
  }
  /* We want the user to be left on the launcher/desktop after switching modes*/
  g_timeout_add_seconds (1, (GSourceFunc)show_desktop, priv->screen);
}


/* GObject stuff */
static void
maximus_app_class_init (MaximusAppClass *klass)
{
  GObjectClass        *obj_class = G_OBJECT_CLASS (klass);

  g_type_class_add_private (obj_class, sizeof (MaximusAppPrivate));
}

static void
maximus_app_init (MaximusApp *app)
{
  MaximusAppPrivate *priv;
  WnckScreen *screen;
	
  priv = app->priv = MAXIMUS_APP_GET_PRIVATE (app);

  priv->bind = maximus_bind_get_default ();

  was_decorated = g_quark_from_static_string ("was-decorated");

  priv->settings = g_settings_new (APP_SCHEMA);

  g_signal_connect (priv->settings, "changed::" APP_EXCLUDE_CLASS,
                    G_CALLBACK (on_exclude_class_changed), app);
  g_signal_connect (priv->settings, "changed::" APP_UNDECORATE,
                    G_CALLBACK (on_app_undecorate_changed), app);
  g_signal_connect (priv->settings, "changed::" APP_NO_MAXIMIZE,
                    G_CALLBACK (on_app_no_maximize_changed), app);

  priv->exclude_class_list = g_settings_get_strv (priv->settings, APP_EXCLUDE_CLASS); 
  priv->undecorate = g_settings_get_boolean (priv->settings, APP_UNDECORATE);
  priv->no_maximize = g_settings_get_boolean (priv->settings, APP_NO_MAXIMIZE);
  g_print ("no maximize: %s\n", priv->no_maximize ? "true" : "false");
 
  priv->screen = screen = wnck_screen_get_default ();
  g_signal_connect (screen, "window-opened",
                    G_CALLBACK (on_window_opened), app);
}

MaximusApp *
maximus_app_get_default (void)

{
  static MaximusApp *app = NULL;

  if (!app)
    app = g_object_new (MAXIMUS_TYPE_APP, 
                        NULL);

  return app;
}
