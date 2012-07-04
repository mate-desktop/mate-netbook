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

#ifndef _MAXIMUS_BIND_H_
#define _MAXIMUS_BIND_H_

#include <glib-object.h>
#include <gtk/gtk.h>

#define MAXIMUS_TYPE_BIND (maximus_bind_get_type ())

#define MAXIMUS_BIND(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj),\
	MAXIMUS_TYPE_BIND, MaximusBind))

#define MAXIMUS_BIND_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass),\
	MAXIMUS_TYPE_BIND, MaximusBindClass))

#define MAXIMUS_IS_BIND(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj),\
	MAXIMUS_TYPE_BIND))

#define MAXIMUS_IS_BIND_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass),\
	MAXIMUS_TYPE_BIND))

#define MAXIMUS_BIND_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj),\
	MAXIMUS_TYPE_BIND, MaximusBindClass))

typedef struct _MaximusBind        MaximusBind;
typedef struct _MaximusBindClass   MaximusBindClass;
typedef struct _MaximusBindPrivate MaximusBindPrivate;
 
struct _MaximusBind
{
  GObject        parent;	

  MaximusBindPrivate *priv;
};

struct _MaximusBindClass
{
  GObjectClass   parent_class;
};

GType maximus_bind_get_type (void) G_GNUC_CONST;

MaximusBind * maximus_bind_get_default (void);


#endif /* _MAXIMUS_BIND_H_ */

