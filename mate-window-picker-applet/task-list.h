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

#ifndef _TASK_LIST_H_
#define _TASK_LIST_H_

#include <glib.h>
#include <gtk/gtk.h>

#define TASK_TYPE_LIST (task_list_get_type ())

#define TASK_LIST(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj),\
	TASK_TYPE_LIST, TaskList))

#define TASK_LIST_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass),\
	TASK_TYPE_LIST, TaskListClass))

#define TASK_IS_LIST(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj),\
	TASK_TYPE_LIST))

#define TASK_IS_LIST_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass),\
	TASK_TYPE_LIST))

#define TASK_LIST_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj),\
	TASK_TYPE_LIST, TaskListClass))

typedef struct _TaskList        TaskList;
typedef struct _TaskListClass   TaskListClass;
typedef struct _TaskListPrivate TaskListPrivate;
 
struct _TaskList
{
  GtkHBox        parent;	

  TaskListPrivate *priv;
};

struct _TaskListClass
{
  GtkHBoxClass   parent_class;
};

GType task_list_get_type (void) G_GNUC_CONST;

GtkWidget * task_list_new (void);

GtkWidget * task_list_get_default (void);

gboolean    task_list_get_desktop_visible (TaskList *list);

gboolean    task_list_get_show_all_windows (TaskList *list);

#endif /* _TASK_LIST_H_ */

