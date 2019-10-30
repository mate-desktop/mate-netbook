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

#include "task-title.h"

#define WNCK_I_KNOW_THIS_IS_UNSTABLE
#include <libwnck/libwnck.h>
#include <mate-panel-applet.h>

#include <math.h>

#define LOGOUT "mate-session-save --logout-dialog"

struct _TaskTitlePrivate
{
  WnckScreen *screen;
  WnckWindow *window;
  GtkWidget *align;
  GtkWidget *box;
  GtkWidget *label;
  GtkWidget *button;
  GtkWidget *button_image;
  GdkPixbuf *quit_icon;

  gboolean show_home_title;
  gboolean bold_window_title;
  gboolean mouse_in_close_button;
};

enum
{
  PROP_0,
  PROP_SHOW_HOME_TITLE,
  PROP_BOLD_WINDOW_TITLE
};

static void disconnect_window (TaskTitle *title);

G_DEFINE_TYPE_WITH_PRIVATE (TaskTitle, task_title, GTK_TYPE_EVENT_BOX);

static gboolean
start_logout_dialog (TaskTitle *title)
{
  GError *error = NULL;
  GAppInfo *app_info;
  GdkScreen *gdkscreen;

  g_return_val_if_fail (TASK_IS_TITLE (title), FALSE);

  gdkscreen = gtk_widget_get_screen (GTK_WIDGET (title));
  app_info = g_app_info_create_from_commandline (
    LOGOUT, _("Log off, switch user, lock screen or power down the computer"),
    G_APP_INFO_CREATE_NONE, &error);

  if (!error) {
    GdkDisplay *display;
    GdkAppLaunchContext *launch_context;
    display = gdk_screen_get_display (gdkscreen);
    launch_context = gdk_display_get_app_launch_context (display);
    gdk_app_launch_context_set_screen (launch_context, gdkscreen);
    g_app_info_launch (app_info, NULL, G_APP_LAUNCH_CONTEXT (launch_context), &error);
    g_object_unref (launch_context);
    return TRUE;
  }

  GtkWidget *dialog;
  dialog = gtk_message_dialog_new (NULL, GTK_DIALOG_DESTROY_WITH_PARENT,
                                   GTK_MESSAGE_ERROR, GTK_BUTTONS_OK,
                                   _("There was an error executing '%s': %s"),
                                   LOGOUT, error->message);

  g_signal_connect (dialog, "response", G_CALLBACK (gtk_widget_destroy), NULL);

  gtk_window_set_resizable (GTK_WINDOW (dialog), FALSE);
  gtk_window_set_screen (GTK_WINDOW (dialog), gdkscreen);

  gtk_widget_show (dialog);
  g_error_free (error);
  return FALSE;
}

static gboolean
on_close_clicked (GtkButton *button,
                  GdkEventButton *event,
                  TaskTitle *title)
{
  TaskTitlePrivate *priv;
  WnckWindow *window;
  gboolean retval;

  g_return_val_if_fail (TASK_IS_TITLE (title), FALSE);
  priv = title->priv;
  retval = FALSE;

  if (event->button != 1 || !priv->mouse_in_close_button)
    return retval;

  window = wnck_screen_get_active_window (priv->screen);

  if (WNCK_IS_WINDOW (window)
        && wnck_window_get_window_type (window) != WNCK_WINDOW_DESKTOP)
  {
    if (priv->window == window)
      disconnect_window (title);
    // event->time is buggy, thus we have to workaroud this
    GdkScreen *gdkscreen;
    GdkDisplay *display;
    gdkscreen = gtk_widget_get_screen (GTK_WIDGET (title));
    display = gdk_screen_get_display (gdkscreen);
    wnck_window_close (window, gdk_x11_display_get_user_time (display));
    retval = TRUE;
  }
  else if (priv->show_home_title)
  {
    // This is a logout click
    retval = start_logout_dialog (title);
  }
  gtk_widget_queue_draw (GTK_WIDGET (title));

  return retval;
}

static gboolean
on_enter_notify (GtkWidget *widget,
                 GdkEventCrossing *event,
                 TaskTitle *title)
{
  g_return_val_if_fail (TASK_IS_TITLE (title), FALSE);

  title->priv->mouse_in_close_button = TRUE;
  gtk_widget_queue_draw (widget);

  return FALSE;
}

static gboolean
on_leave_notify (GtkWidget *widget,
                 GdkEventCrossing *event,
                 TaskTitle *title)
{
  g_return_val_if_fail (TASK_IS_TITLE (title), FALSE);

  title->priv->mouse_in_close_button = FALSE;
  gtk_widget_queue_draw (widget);

  return FALSE;
}

static gboolean
on_button_draw (GtkWidget *widget,
                cairo_t *cr,
                TaskTitle *title)
{
  g_return_val_if_fail (TASK_IS_TITLE (title), FALSE);

  TaskTitlePrivate *priv;
  priv = title->priv;

  if (priv->mouse_in_close_button)
  {
    GtkStyle *style = gtk_widget_get_style (widget);
    GdkRectangle area;
    gdouble x1, y1, x2, y2;
    cairo_clip_extents (cr, &x1, &y1, &x2, &y2);
    area.x = floor (x1);
    area.y = floor (y1);
    area.width = ceil (x2) - area.x;
    area.height = ceil (y2) - area.y;

    gtk_paint_box (style,
                   cr,
                   GTK_STATE_PRELIGHT,
                   GTK_SHADOW_NONE,
                   NULL,
                   NULL,
                   area.x,
                   area.y + 2,
                   area.width,
                   area.height - 4);
  }
  return FALSE;
}

static void
on_name_changed (WnckWindow *window, TaskTitle *title)
{
  TaskTitlePrivate *priv;

  g_return_if_fail (TASK_IS_TITLE (title));
  g_return_if_fail (WNCK_IS_WINDOW (window));

  priv = title->priv;
  if (priv->window != window)
    return;

  gtk_label_set_text (GTK_LABEL (title->priv->label),
                      wnck_window_get_name (window));
  gtk_widget_set_tooltip_text (GTK_WIDGET (title),
                               wnck_window_get_name (window));
  gtk_widget_queue_draw (GTK_WIDGET (title));
}


static void
on_icon_changed (WnckWindow *window, TaskTitle *title)
{
  TaskTitlePrivate *priv;

  g_return_if_fail (TASK_IS_TITLE (title));
  g_return_if_fail (WNCK_IS_WINDOW (window));

  priv = title->priv;
  if (priv->window != window)
    return;

  gtk_widget_queue_draw (GTK_WIDGET (title));
}

static void
on_state_changed (WnckWindow *window,
                  WnckWindowState changed_mask,
                  WnckWindowState new_state,
                  TaskTitle *title)
{
  TaskTitlePrivate *priv;

  g_return_if_fail (TASK_IS_TITLE (title));
  g_return_if_fail (WNCK_IS_WINDOW (window));

  priv = title->priv;
  if (priv->window != window)
    return;

  if (wnck_window_is_maximized (window))
  {
    gtk_widget_set_state_flags (GTK_WIDGET (title), GTK_STATE_FLAG_ACTIVE,
                                TRUE);
    gtk_widget_show (priv->box);
  }
  else
  {
    gtk_widget_set_state_flags (GTK_WIDGET (title), GTK_STATE_FLAG_NORMAL,
                               TRUE);
    gtk_widget_hide (priv->box);
  }
}

static void
disconnect_window (TaskTitle *title)
{
  TaskTitlePrivate *priv = title->priv;
  if (!priv->window)
    return;
  g_signal_handlers_disconnect_by_func (priv->window, on_name_changed, title);
  g_signal_handlers_disconnect_by_func (priv->window, on_icon_changed, title);
  g_signal_handlers_disconnect_by_func (priv->window, on_state_changed, title);
  priv->window = NULL;
}

static void
on_active_window_changed (WnckScreen *screen,
                          WnckWindow *old_window,
                          TaskTitle   *title)
{
  TaskTitlePrivate *priv;
  WnckWindow *act_window;
  WnckWindowType type = WNCK_WINDOW_NORMAL;

  g_return_if_fail (TASK_IS_TITLE (title));
  priv = title->priv;

  act_window = wnck_screen_get_active_window (screen);
  if (act_window)
    type = wnck_window_get_window_type (act_window);

  if (WNCK_IS_WINDOW (act_window)
      && wnck_window_is_skip_tasklist (act_window)
      && type != WNCK_WINDOW_DESKTOP)
    return;

  if (type == WNCK_WINDOW_DOCK
      || type == WNCK_WINDOW_SPLASHSCREEN
      || type == WNCK_WINDOW_MENU)
    return;

  disconnect_window (title);

  if (!WNCK_IS_WINDOW (act_window)
        || wnck_window_get_window_type (act_window) == WNCK_WINDOW_DESKTOP)
  {
    if (priv->show_home_title)
    {
      gtk_label_set_text (GTK_LABEL (priv->label), _("Desktop"));
      gtk_image_set_from_pixbuf (GTK_IMAGE (priv->button_image),
                                 priv->quit_icon);

      gtk_widget_set_tooltip_text (GTK_WIDGET (title), NULL);
      gtk_widget_set_tooltip_text (priv->button,
                                   _("Log off, switch user, lock screen or "
                                     "power down the computer"));

      gtk_widget_set_state_flags (GTK_WIDGET (title), GTK_STATE_FLAG_ACTIVE,
                                  TRUE);
      gtk_widget_show (priv->box);
    }
    else
    {
      gtk_widget_set_tooltip_text (priv->button, NULL);
      gtk_widget_set_tooltip_text (GTK_WIDGET (title), NULL);
      gtk_widget_set_state_flags (GTK_WIDGET (title), GTK_STATE_FLAG_NORMAL,
                                  TRUE);
      gtk_widget_hide (priv->box);
    }
  }
  else
  {
    gtk_label_set_text (GTK_LABEL (priv->label),
                        wnck_window_get_name (act_window));
    gtk_image_set_from_icon_name (GTK_IMAGE (priv->button_image),
                                  "window-close", GTK_ICON_SIZE_MENU);

    gtk_widget_set_tooltip_text (GTK_WIDGET (title),
                                 wnck_window_get_name (act_window));
    gtk_widget_set_tooltip_text (priv->button, _("Close window"));

    g_signal_connect (act_window, "name-changed",
                      G_CALLBACK (on_name_changed), title);
    g_signal_connect (act_window, "icon-changed",
                      G_CALLBACK (on_icon_changed), title);
    g_signal_connect_after (act_window, "state-changed",
                            G_CALLBACK (on_state_changed), title);
    priv->window = act_window;
    if (wnck_window_is_maximized (act_window))
    {
      gtk_widget_set_state_flags (GTK_WIDGET (title), GTK_STATE_FLAG_ACTIVE,
                                  TRUE);
      gtk_widget_show (priv->box);
    }
    else
    {
      gtk_widget_set_state_flags (GTK_WIDGET (title), GTK_STATE_FLAG_NORMAL,
                                  TRUE);
      gtk_widget_hide (priv->box);
    }
  }

  gtk_widget_queue_draw (GTK_WIDGET (title));
}

static gboolean
on_button_release (GtkWidget *title, GdkEventButton *event)
{
  TaskTitlePrivate *priv;
  WnckWindow *window;

  g_return_val_if_fail (TASK_IS_TITLE (title), FALSE);
  priv = TASK_TITLE (title)->priv;

  window = wnck_screen_get_active_window (priv->screen);

  g_return_val_if_fail (WNCK_IS_WINDOW (window), FALSE);

  if (event->button == 3)
  {
    if (wnck_window_get_window_type (window) != WNCK_WINDOW_DESKTOP)
    {
      GtkWidget *menu;
      menu = wnck_action_menu_new (window);
      gtk_menu_popup_at_pointer (GTK_MENU (menu), (GdkEvent*)event);
      return TRUE;
    }
  }
  else if (event->button == 1)
  {
    if (event->type == GDK_2BUTTON_PRESS && wnck_window_is_maximized (window))
    {
      wnck_window_unmaximize (window);
    }
  }

  return FALSE;
}

static gboolean
on_draw (GtkWidget *w, cairo_t *cr)
{
  if (gtk_widget_get_state_flags (w) == GTK_STATE_FLAG_ACTIVE) {
    GtkStyleContext *context = gtk_widget_get_style_context (w);
    gtk_render_frame (context,
                      cr,
                      0, 0,
                      gtk_widget_get_allocated_width (w),
                      gtk_widget_get_allocated_height (w));
  }

  gtk_container_propagate_draw (GTK_CONTAINER (w),
                                gtk_bin_get_child (GTK_BIN (w)),
                                cr);
  return TRUE;
}

/* GObject stuff */
static void
task_title_set_show_home_title (TaskTitle *title, gboolean show_home_title)
{
  TaskTitlePrivate *priv = title->priv;

  priv->show_home_title = show_home_title;

  g_debug ("Show home title: %s", show_home_title ? "true" : "false");

  on_active_window_changed(priv->screen, priv->window, title);
}

static void
task_title_set_bold_window_title (TaskTitle *title, gboolean bold_window_title)
{
  TaskTitlePrivate *priv = title->priv;

  priv->bold_window_title = bold_window_title;

  g_debug ("Bold window title: %s", bold_window_title ? "true" : "false");

  PangoAttribute *attr;
  PangoAttrList *attr_list = pango_attr_list_new ();
  if (priv->bold_window_title)
  {
    attr = pango_attr_weight_new (PANGO_WEIGHT_BOLD);
  }
  else
  {
    attr = pango_attr_weight_new (PANGO_WEIGHT_NORMAL);
  }
  pango_attr_list_insert (attr_list, attr);
  gtk_label_set_attributes (GTK_LABEL (priv->label), attr_list);
}

static void
task_title_get_property (GObject    *object,
                         guint       prop_id,
                         GValue     *value,
                         GParamSpec *pspec)
{
  TaskTitle *title = TASK_TITLE (object);
  TaskTitlePrivate *priv;

  g_return_if_fail (TASK_IS_TITLE (title));
  priv = title->priv;

  switch (prop_id)
  {
    case PROP_SHOW_HOME_TITLE:
      g_value_set_boolean (value, priv->show_home_title);
      break;
    case PROP_BOLD_WINDOW_TITLE:
      g_value_set_boolean (value, priv->bold_window_title);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
  }
}

static void
task_title_set_property (GObject      *object,
                         guint         prop_id,
                         const GValue *value,
                         GParamSpec   *pspec)
{
  TaskTitle *title = TASK_TITLE (object);

  g_return_if_fail (TASK_IS_TITLE (title));

  switch (prop_id)
  {
    case PROP_SHOW_HOME_TITLE:
      task_title_set_show_home_title (title, g_value_get_boolean (value));
      break;
    case PROP_BOLD_WINDOW_TITLE:
      task_title_set_bold_window_title (title, g_value_get_boolean (value));
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
  }
}

static void
task_title_finalize (GObject *object)
{
  TaskTitlePrivate *priv;

  priv = TASK_TITLE (object)->priv;
  disconnect_window (TASK_TITLE (object));

  g_object_unref (G_OBJECT (priv->quit_icon));

  G_OBJECT_CLASS (task_title_parent_class)->finalize (object);
}

static void
task_title_class_init (TaskTitleClass *klass)
{
  GObjectClass        *obj_class = G_OBJECT_CLASS (klass);
  GtkWidgetClass      *wid_class = GTK_WIDGET_CLASS (klass);

  obj_class->finalize = task_title_finalize;
  obj_class->set_property = task_title_set_property;
  obj_class->get_property = task_title_get_property;

  g_object_class_install_property (obj_class,
    PROP_SHOW_HOME_TITLE,
    g_param_spec_boolean ("show_home_title",
                          "Show Home Title",
                          "Show Home Title",
                          FALSE,
                          G_PARAM_READWRITE | G_PARAM_CONSTRUCT));

  g_object_class_install_property (obj_class,
    PROP_BOLD_WINDOW_TITLE,
    g_param_spec_boolean ("bold_window_title",
                          "Bold Window Title",
                          "Show windows title with a bold face",
                          TRUE,
                          G_PARAM_READWRITE | G_PARAM_CONSTRUCT));

  wid_class->draw = on_draw;
}

static void
task_title_init (TaskTitle *title)
{
  TaskTitlePrivate *priv;
  GdkScreen *gdkscreen;
  GtkIconTheme *theme;
  AtkObject *atk;
  int width, height;

  priv = title->priv = task_title_get_instance_private (title);

  priv->screen = wnck_screen_get_default ();
  priv->window = NULL;

  // Default values
  priv->show_home_title = FALSE;
  priv->bold_window_title = TRUE;

  gtk_widget_add_events (GTK_WIDGET (title), GDK_ALL_EVENTS_MASK);

  priv->align = gtk_alignment_new (0.0, 0.5, 1.0, 1.0);
  gtk_alignment_set_padding (GTK_ALIGNMENT (priv->align),
                             0, 0, 6, 6);
  gtk_container_add (GTK_CONTAINER (title), priv->align);

  priv->box = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 2);
  gtk_container_add (GTK_CONTAINER (priv->align), priv->box);
  gtk_widget_set_no_show_all (priv->box, TRUE);
  gtk_widget_show (priv->box);

  priv->label = gtk_label_new (_("Desktop"));
  gtk_label_set_ellipsize (GTK_LABEL (priv->label), PANGO_ELLIPSIZE_END);
  gtk_widget_set_halign (priv->label, GTK_ALIGN_START);
  gtk_widget_set_valign (priv->label, GTK_ALIGN_CENTER);

  if (priv->bold_window_title)
  {
    PangoAttrList *attr_list = pango_attr_list_new ();
    PangoAttribute *attr = pango_attr_weight_new (PANGO_WEIGHT_BOLD);
    pango_attr_list_insert (attr_list, attr);
    gtk_label_set_attributes (GTK_LABEL (priv->label), attr_list);
  }

  gtk_box_pack_start (GTK_BOX (priv->box), priv->label, TRUE, TRUE, 0);
  gtk_widget_show (priv->label);

  priv->button = g_object_new (GTK_TYPE_EVENT_BOX,
                               "visible-window", FALSE,
                               "above-child", TRUE,
                               NULL);
  gtk_box_pack_start (GTK_BOX (priv->box), priv->button, FALSE, FALSE, 0);
  gtk_widget_show (priv->button);

  atk = gtk_widget_get_accessible (priv->button);
  atk_object_set_name (atk, _("Close"));
  atk_object_set_description (atk, _("Close current window."));
  atk_object_set_role (atk, ATK_ROLE_PUSH_BUTTON);

  g_signal_connect (priv->button, "button-release-event",
                    G_CALLBACK (on_close_clicked), title);
  g_signal_connect (priv->button, "enter-notify-event",
                    G_CALLBACK (on_enter_notify), title);
  g_signal_connect (priv->button, "leave-notify-event",
                    G_CALLBACK (on_leave_notify), title);
  g_signal_connect (priv->button, "draw",
                    G_CALLBACK (on_button_draw), title);

  // Initialize button_image to allow easy setting by icon name after that.
  gdkscreen = gtk_widget_get_screen (GTK_WIDGET (title));
  theme = gtk_icon_theme_get_for_screen (gdkscreen);
  gtk_icon_size_lookup (GTK_ICON_SIZE_MENU, &width, &height);
  priv->quit_icon = gtk_icon_theme_load_icon (theme, "system-log-out", width, 0, NULL);
  priv->button_image = gtk_image_new_from_pixbuf (priv->quit_icon);

  gtk_container_add (GTK_CONTAINER (priv->button), priv->button_image);
  gtk_widget_show (priv->button_image);

  gtk_widget_hide (priv->box);

  gtk_widget_add_events (GTK_WIDGET (title), GDK_ALL_EVENTS_MASK);

  g_signal_connect (priv->screen, "active-window-changed",
                    G_CALLBACK (on_active_window_changed), title);
  g_signal_connect (title, "button-press-event",
                    G_CALLBACK (on_button_release), NULL);
}

GtkWidget *
task_title_new (void)

{
  GtkWidget *title = NULL;

  title = g_object_new (TASK_TYPE_TITLE,
                        "border-width", 0,
                        "name", "tasklist-button",
                        "visible-window", FALSE,
                        NULL);

  return title;
}
