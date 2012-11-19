/* Xlib utils */
/* vim: set sw=2 et: */

/*
 * Copyright (C) 2001 Havoc Pennington
 * Copyright (C) 2005-2007 Vincent Untz
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#include <config.h>
#include "xutils.h"
#include <string.h>
#include <stdio.h>
#include <libmatewnck/libmatewnck.h>

#ifdef __GNUC__
#define UNUSED_VARIABLE __attribute__ ((unused))
#else
#define UNUSED_VARIABLE
#endif

void
_matewnck_error_trap_push (void)
{
  gdk_error_trap_push ();
}

int
_matewnck_error_trap_pop (void)
{
  XSync (gdk_display, False);
  return gdk_error_trap_pop ();
}

static char*
latin1_to_utf8 (const char *latin1)
{
  GString *str;
  const char *p;
  
  str = g_string_new (NULL);

  p = latin1;
  while (*p)
    {
      g_string_append_unichar (str, (gunichar) *p);
      ++p;
    }

  return g_string_free (str, FALSE);
}

void
_matewnck_get_wmclass (Window xwindow,
                   char **res_class,
                   char **res_name)
{
  XClassHint ch;
  char UNUSED_VARIABLE *retval;
  
  _matewnck_error_trap_push ();

  ch.res_name = NULL;
  ch.res_class = NULL;

  XGetClassHint (gdk_display, xwindow,
                 &ch);

  _matewnck_error_trap_pop ();
  
  retval = NULL;

  if (res_class)
    *res_class = NULL;

  if (res_name)
    *res_name = NULL;
  
  if (ch.res_name)
    {
      if (res_name)
        *res_name = latin1_to_utf8 (ch.res_name);
      
      XFree (ch.res_name);
    }

  if (ch.res_class)
    {
      if (res_class)
        *res_class = latin1_to_utf8 (ch.res_class);
      
      XFree (ch.res_class);
    }
}
