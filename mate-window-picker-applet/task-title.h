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

#ifndef _TASK_TITLE_H_
#define _TASK_TITLE_H_

#include <glib.h>
#include <glib/gi18n.h>
#include <gtk/gtk.h>

#define TASK_TYPE_TITLE (task_title_get_type ())

#define TASK_TITLE(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj),\
	TASK_TYPE_TITLE, TaskTitle))

#define TASK_TITLE_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass),\
	TASK_TYPE_TITLE, TaskTitleClass))

#define TASK_IS_TITLE(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj),\
	TASK_TYPE_TITLE))

#define TASK_IS_TITLE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass),\
	TASK_TYPE_TITLE))

#define TASK_TITLE_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj),\
	TASK_TYPE_TITLE, TaskTitleClass))

typedef struct _TaskTitle        TaskTitle;
typedef struct _TaskTitleClass   TaskTitleClass;
typedef struct _TaskTitlePrivate TaskTitlePrivate;
 
struct _TaskTitle
{
  GtkEventBox        parent;	

  TaskTitlePrivate *priv;
};

struct _TaskTitleClass
{
  GtkEventBoxClass   parent_class;
};

GType task_title_get_type (void) G_GNUC_CONST;

GtkWidget * task_title_new (void);


#endif /* _TASK_TITLE_H_ */

