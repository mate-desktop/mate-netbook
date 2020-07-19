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

#include <glib.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <gtk/gtk.h>
#include <gdk/gdkx.h>

#include <gio/gio.h>

#include "maximus-app.h"

static gboolean version    = FALSE;
gboolean no_maximize = FALSE;

GOptionEntry entries[] =
{
 {
   "version", 'v',
   0, G_OPTION_ARG_NONE,
   &version,
   "Prints the version number", NULL
 },
 {
   "no-maximize", 'm',
   0, G_OPTION_ARG_NONE,
   &no_maximize,
   "Do not automatically maximize every window", NULL
 },
 {
   NULL
 }
};

gint main (gint argc, gchar *argv[])
{
  GApplication *application;
  GOptionContext  *context;
  GError *error = NULL;
  GdkDisplay *gdk_display;

  g_set_application_name ("Maximus");

  gtk_init (&argc, &argv);

  application = g_application_new ("com.canonical.Maximus", G_APPLICATION_FLAGS_NONE);

  if (!g_application_register (application, NULL, &error))
  {
    g_warning ("%s", error->message);
    g_error_free (error);
    return 1;
  }

  if (g_application_get_is_remote(application))
  {
    return 0;
  }

  context = g_option_context_new ("- Maximus");
  g_option_context_add_main_entries (context, entries, "maximus");
  g_option_context_add_group (context, gtk_get_option_group (TRUE));
  g_option_context_parse (context, &argc, &argv, NULL);
  g_option_context_free(context);

  gdk_display = gdk_display_get_default ();
  gdk_x11_display_error_trap_push (gdk_display);
  // Initialize app, but don't reference return value
  maximus_app_get_default ();
  gdk_x11_display_error_trap_pop_ignored (gdk_display);

  gtk_main ();

  return EXIT_SUCCESS;
}
