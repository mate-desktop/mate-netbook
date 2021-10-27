/*
 * Copyright (C) 2008 Canonical Ltd
 * Copyright (C) 2012-2021 MATE Developers
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

#ifndef _MAXIMUS_APP_H_
#define _MAXIMUS_APP_H_

#include <glib-object.h>
#include <gtk/gtk.h>
#define WNCK_I_KNOW_THIS_IS_UNSTABLE
#include <libwnck/libwnck.h>

#define MAXIMUS_TYPE_APP (maximus_app_get_type ())

#define MAXIMUS_APP(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj),\
	MAXIMUS_TYPE_APP, MaximusApp))

#define MAXIMUS_APP_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass),\
	MAXIMUS_TYPE_APP, MaximusAppClass))

#define MAXIMUS_IS_APP(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj),\
	MAXIMUS_TYPE_APP))

#define MAXIMUS_IS_APP_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass),\
	MAXIMUS_TYPE_APP))

#define MAXIMUS_APP_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj),\
	MAXIMUS_TYPE_APP, MaximusAppClass))

typedef struct _MaximusApp        MaximusApp;
typedef struct _MaximusAppClass   MaximusAppClass;
typedef struct _MaximusAppPrivate MaximusAppPrivate;
 
struct _MaximusApp
{
  GObject        parent;	

  MaximusAppPrivate *priv;
};

struct _MaximusAppClass
{
  GObjectClass   parent_class;
};

GType maximus_app_get_type (void) G_GNUC_CONST;

MaximusApp * maximus_app_get_default (void);

#endif /* _MAXIMUS_APP_H_ */

