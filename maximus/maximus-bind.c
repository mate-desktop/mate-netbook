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

#include <gtk/gtk.h>
#include <gdk/gdkx.h>

#include <gdk/gdkkeysyms.h>

#include <gio/gio.h>

#define WNCK_I_KNOW_THIS_IS_UNSTABLE
#include <libwnck/libwnck.h>

#include <X11/Xlib.h>
#include <X11/Xresource.h>
#include <X11/Xutil.h>
#include <X11/extensions/XTest.h>
#include <X11/keysymdef.h>
#include <X11/keysym.h>

#include <fakekey/fakekey.h>

#include "maximus-bind.h"

#include "tomboykeybinder.h"
#include "eggaccelerators.h"

#define KEY_RELEASE_TIMEOUT 300
#define STATE_CHANGED_SLEEP 0.5

/* GSettings schemas and keys */
#define BIND_SCHEMA        "org.mate.maximus"
#define BIND_EXCLUDE_CLASS "binding"

#define SYSRULESDIR SYSCONFDIR"/maximus"

struct _MaximusBindPrivate
{
  FakeKey *fk;
  WnckScreen *screen;
  GSettings *settings;

  gchar *binding;

  GList *rules;
};

typedef struct
{
  gchar *wm_class;
  gchar *fullscreen;
  gchar *unfullscreen;
} MaximusRule;

G_DEFINE_TYPE_WITH_PRIVATE (MaximusBind, maximus_bind, G_TYPE_OBJECT);

static const gchar *
get_fullscreen_keystroke (GList *rules, WnckWindow *window)
{
  WnckClassGroup *group;
  const gchar *class_name;
  GList *r;

  group = wnck_window_get_class_group (window);
  class_name = wnck_class_group_get_name (group);

  g_debug ("Searching rules for %s:\n", wnck_window_get_name (window));

  for (r = rules; r; r = r->next)
  {
    MaximusRule *rule = r->data;

    g_debug ("\t%s ?= %s", class_name, rule->wm_class);

    if (class_name && rule->wm_class && strstr (class_name, rule->wm_class))
    {
      g_debug ("\tYES!\n");
      return rule->fullscreen;
    }
    g_debug ("\tNO!\n");
  }

  return NULL;
}

static const gchar *
get_unfullscreen_keystroke (GList *rules, WnckWindow *window)
{
  WnckClassGroup *group;
  const gchar *class_name;
  GList *r;

  group = wnck_window_get_class_group (window);
  class_name = wnck_class_group_get_name (group);

  for (r = rules; r; r = r->next)
  {
    MaximusRule *rule = r->data;

    if (class_name && rule->wm_class && strstr (class_name, rule->wm_class))
    {
      return rule->unfullscreen;
    }
  }

  return NULL;
}
static gboolean
real_fullscreen (MaximusBind *bind)
{
  MaximusBindPrivate *priv;
  WnckWindow *active;
  const gchar *keystroke;

  priv = bind->priv;

  active = wnck_screen_get_active_window (priv->screen);

  if (!WNCK_IS_WINDOW (active)
        || wnck_window_get_window_type (active) != WNCK_WINDOW_NORMAL)
    return FALSE;

  keystroke = get_fullscreen_keystroke (priv->rules, active);

  if (keystroke)
  {
    guint keysym = 0;
    EggVirtualModifierType modifiers = 0;

    if (egg_accelerator_parse_virtual (keystroke, &keysym, &modifiers))
    {
      guint mods = 0;

      if (modifiers & EGG_VIRTUAL_SHIFT_MASK)
        mods |= FAKEKEYMOD_SHIFT;
      if (modifiers & EGG_VIRTUAL_CONTROL_MASK)
        mods |= FAKEKEYMOD_CONTROL;
      if (modifiers & EGG_VIRTUAL_ALT_MASK)
        mods |= FAKEKEYMOD_ALT;
      if (modifiers & EGG_VIRTUAL_META_MASK)
        mods |= FAKEKEYMOD_META;

      g_debug ("Sending fullscreen special event: %s = %d %d",
               keystroke, keysym, mods);
      fakekey_press_keysym (priv->fk, keysym, mods);
      fakekey_release (priv->fk);

      return FALSE;
     }
  }

  if (!wnck_window_is_fullscreen (active))
  {
    g_debug ("Sending fullscreen F11 event");
    fakekey_press_keysym (priv->fk, XK_F11, 0);
    fakekey_release (priv->fk);
  }

  sleep (STATE_CHANGED_SLEEP);

  if (!wnck_window_is_fullscreen (active))
  {
    g_debug ("Forcing fullscreen wnck event");
    wnck_window_set_fullscreen (active, TRUE);
  }

  return FALSE;
}

static void
fullscreen (MaximusBind *bind, WnckWindow *window)
{
  g_timeout_add (KEY_RELEASE_TIMEOUT, (GSourceFunc)real_fullscreen, bind);
}

static gboolean
real_unfullscreen (MaximusBind *bind)
{
  MaximusBindPrivate *priv;
  WnckWindow *active;
  const gchar *keystroke;

  priv = bind->priv;

  active = wnck_screen_get_active_window (priv->screen);

  if (!WNCK_IS_WINDOW (active)
        || wnck_window_get_window_type (active) != WNCK_WINDOW_NORMAL)
    return FALSE;

  keystroke = get_unfullscreen_keystroke (priv->rules, active);

  if (keystroke)
  {
    guint keysym = 0;
    EggVirtualModifierType modifiers = 0;

    if (egg_accelerator_parse_virtual (keystroke, &keysym, &modifiers))
    {
      guint mods = 0;

      if (modifiers & EGG_VIRTUAL_SHIFT_MASK)
        mods |= FAKEKEYMOD_SHIFT;
      if (modifiers & EGG_VIRTUAL_CONTROL_MASK)
        mods |= FAKEKEYMOD_CONTROL;
      if (modifiers & EGG_VIRTUAL_ALT_MASK)
        mods |= FAKEKEYMOD_ALT;
      if (modifiers & EGG_VIRTUAL_META_MASK)
        mods |= FAKEKEYMOD_META;

      g_debug ("Sending fullscreen special event: %s = %d %d",
               keystroke, keysym, mods);
      fakekey_press_keysym (priv->fk, keysym, mods);
      fakekey_release (priv->fk);

      return FALSE;
     }
  }
  if (wnck_window_is_fullscreen (active))
  {
    g_debug ("Sending un-fullscreen F11 event");
    fakekey_press_keysym (priv->fk, XK_F11, 0);
    fakekey_release (priv->fk);
  }

  sleep (STATE_CHANGED_SLEEP);

  if (wnck_window_is_fullscreen (active))
  {
    g_debug ("Forcing un-fullscreen wnck event");
    wnck_window_set_fullscreen (active, FALSE);
  }

  return FALSE;
}

static void
unfullscreen (MaximusBind *bind, WnckWindow *window)
{
  g_timeout_add (KEY_RELEASE_TIMEOUT, (GSourceFunc)real_unfullscreen, bind);
}


static void
on_binding_activated (gchar *keystring, MaximusBind *bind)
{
  MaximusBindPrivate *priv;
  WnckWindow *active;

  g_return_if_fail (MAXIMUS_IS_BIND (bind));
  priv = bind->priv;

  active = wnck_screen_get_active_window (priv->screen);

  if (wnck_window_get_window_type (active) != WNCK_WINDOW_NORMAL)
    return;

  if (wnck_window_is_fullscreen (active))
  {
    unfullscreen (bind, active);
  }
  else
  {
    fullscreen (bind, active);
  }
}

/* Callbacks */
static gboolean
binding_is_valid (const gchar *binding)
{
  gboolean retval = TRUE;

  if (!binding || strlen (binding) < 1 || strcmp (binding, "disabled") == 0)
    retval = FALSE;

  return retval;
}

static void
on_binding_changed (GSettings      *settings,
                    gchar          *key,
                    MaximusBind    *bind)
{
  MaximusBindPrivate *priv;

  g_return_if_fail (MAXIMUS_IS_BIND (bind));
  priv = bind->priv;

  if (binding_is_valid (priv->binding))
    tomboy_keybinder_unbind (priv->binding,
                             (TomboyBindkeyHandler)on_binding_changed);
  g_free (priv->binding);

  priv->binding = g_settings_get_string (settings, BIND_EXCLUDE_CLASS);

  if (binding_is_valid (priv->binding))
    tomboy_keybinder_bind (priv->binding,
                           (TomboyBindkeyHandler)on_binding_activated,
                           bind);

  g_print ("Binding changed: %s\n", priv->binding);
}


/* GObject stuff */
static void
create_rule (MaximusBind *bind, const gchar *filename)
{
#define RULE_GROUP "Fullscreening"
#define RULE_WMCLASS "WMClass"
#define RULE_FULLSCREEN "Fullscreen"
#define RULE_UNFULLSCREEN "Unfullscreen"
  MaximusBindPrivate *priv;
  GKeyFile *file;
  GError *error = NULL;
  MaximusRule *rule;

  priv = bind->priv;

  file = g_key_file_new ();
  g_key_file_load_from_file (file, filename, 0, &error);
  if (error)
  {
    g_warning ("Unable to load %s: %s\n", filename, error->message);
    g_error_free (error);
    g_key_file_free (file);
    return;
  }

  rule = g_slice_new0 (MaximusRule);

  rule->wm_class = g_key_file_get_string (file,
                                          RULE_GROUP, RULE_WMCLASS,
                                          NULL);
  rule->fullscreen = g_key_file_get_string (file,
                                            RULE_GROUP, RULE_FULLSCREEN,
                                            NULL);
  rule->unfullscreen = g_key_file_get_string (file,
                                              RULE_GROUP, RULE_UNFULLSCREEN,
                                              NULL);
  if (!rule->wm_class || !rule->fullscreen || !rule->unfullscreen)
  {
    g_free (rule->wm_class);
    g_free (rule->fullscreen);
    g_free (rule->unfullscreen);
    g_slice_free (MaximusRule, rule);

    g_warning ("Unable to load %s, missing strings", filename);
  }
  else
    priv->rules = g_list_append (priv->rules, rule);

  g_key_file_free (file);
}

static void
load_rules (MaximusBind *bind, const gchar *path)
{
  GDir *dir;
  const gchar *name;

  dir = g_dir_open (path, 0, NULL);

  if (!dir)
    return;

  while ((name = g_dir_read_name (dir)))
  {
    gchar *filename;

    filename= g_build_filename (path, name, NULL);

    create_rule (bind, filename);

    g_free (filename);
  }


  g_dir_close (dir);
}

static void
maximus_bind_finalize (GObject *obj)
{
  MaximusBind *bind = MAXIMUS_BIND (obj);
  MaximusBindPrivate *priv;
  GList *r;

  g_return_if_fail (MAXIMUS_IS_BIND (bind));
  priv = bind->priv;

  for (r = priv->rules; r; r = r->next)
  {
    MaximusRule *rule = r->data;

    g_free (rule->wm_class);
    g_free (rule->fullscreen);
    g_free (rule->unfullscreen);

    g_slice_free (MaximusRule, rule);
  }
  g_free (priv->binding);

  G_OBJECT_CLASS (maximus_bind_parent_class)->finalize (obj);
}

static void
maximus_bind_class_init (MaximusBindClass *klass)
{
  GObjectClass        *obj_class = G_OBJECT_CLASS (klass);

  obj_class->finalize = maximus_bind_finalize;
}

static void
maximus_bind_init (MaximusBind *bind)
{
  MaximusBindPrivate *priv;
  GdkDisplay *display = gdk_display_get_default ();
  WnckScreen *screen;

  priv = bind->priv = maximus_bind_get_instance_private (bind);

  priv->fk = fakekey_init (GDK_DISPLAY_XDISPLAY (display));
  priv->screen = screen = wnck_screen_get_default ();
  priv->rules = NULL;
  priv->settings = g_settings_new (BIND_SCHEMA);

  tomboy_keybinder_init ();

  g_signal_connect (priv->settings, "changed::" BIND_EXCLUDE_CLASS,
                    G_CALLBACK (on_binding_changed), bind);

  priv->binding = g_settings_get_string (priv->settings, BIND_EXCLUDE_CLASS);

  if (binding_is_valid (priv->binding))
    tomboy_keybinder_bind (priv->binding,
                           (TomboyBindkeyHandler)on_binding_activated,
                           bind);

  load_rules (bind, SYSRULESDIR);
}

MaximusBind *
maximus_bind_get_default (void)

{
  static MaximusBind *bind = NULL;

  if (!bind)
    bind = g_object_new (MAXIMUS_TYPE_BIND,
                       NULL);

  return bind;
}
