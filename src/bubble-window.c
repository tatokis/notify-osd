/*******************************************************************************
**3456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789
**      10        20        30        40        50        60        70        80
**
** notify-osd
**
** bubble-window.c - implements bubble window
**
** Copyright 2009 Canonical Ltd.
**
** Authors:
**    Eitan Isaacson <eitan@ascender.com>
**
** This program is free software: you can redistribute it and/or modify it
** under the terms of the GNU General Public License version 3, as published
** by the Free Software Foundation.
**
** This program is distributed in the hope that it will be useful, but
** WITHOUT ANY WARRANTY; without even the implied warranties of
** MERCHANTABILITY, SATISFACTORY QUALITY, or FITNESS FOR A PARTICULAR
** PURPOSE.  See the GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License along
** with this program.  If not, see <http://www.gnu.org/licenses/>.
**
*******************************************************************************/

#include "bubble-window.h"
#include "bubble.h"
#include "bubble-window-accessible-factory.h"

G_DEFINE_TYPE (BubbleWindow, bubble_window, GTK_TYPE_WINDOW);

static AtkObject* bubble_window_get_accessible (GtkWidget *widget);

static void
bubble_window_init (BubbleWindow *object)
{
	/* TODO: Add initialization code here */
}

static void
bubble_window_finalize (GObject *object)
{
	/* TODO: Add deinitalization code here */

	G_OBJECT_CLASS (bubble_window_parent_class)->finalize (object);
}

static void
bubble_window_class_init (BubbleWindowClass *klass)
{
	GObjectClass* object_class = G_OBJECT_CLASS (klass);
	GtkWidgetClass* widget_class = GTK_WIDGET_CLASS (klass);
	
	widget_class->get_accessible = bubble_window_get_accessible;

	object_class->finalize = bubble_window_finalize;
}

GtkWidget *
bubble_window_new (void)
{
	GtkWidget *bubble_window;

	bubble_window = g_object_new (BUBBLE_TYPE_WINDOW, 
						 "type", GTK_WINDOW_POPUP, NULL);
	return bubble_window;
}

static AtkObject *
bubble_window_get_accessible (GtkWidget *widget)
{
  static gboolean first_time = TRUE;
  static GQuark quark_accessible_object;

  if (first_time) 
    {
      AtkObjectFactory *factory = NULL;
      AtkRegistry *registry = NULL;
      GType derived_type = G_TYPE_NONE;
      GType derived_atk_type = G_TYPE_NONE;

      /*
       * Figure out whether accessibility is enabled by looking at the
       * type of the accessible object which would be created for
       * the parent type WnckPager.
       */
      derived_type = g_type_parent (BUBBLE_TYPE_WINDOW);

      registry = atk_get_default_registry ();
      factory = atk_registry_get_factory (registry,
                                          derived_type);
      derived_atk_type = atk_object_factory_get_accessible_type (factory);
      atk_registry_set_factory_type (registry,
                                     BUBBLE_TYPE_WINDOW,
                                     BUBBLE_WINDOW_TYPE_ACCESSIBLE_FACTORY);
      quark_accessible_object = g_quark_from_static_string ("gtk-accessible-object");
      first_time = FALSE;
    }

    AtkRegistry *default_registry = atk_get_default_registry ();
    AtkObjectFactory *factory = NULL;
    AtkObject *accessible = g_object_get_qdata (G_OBJECT (widget),
                                                quark_accessible_object);
    if (accessible)
        return accessible;
    factory = atk_registry_get_factory (default_registry,
                                        G_TYPE_FROM_INSTANCE (widget));
    accessible = atk_object_factory_create_accessible (factory,
                                                       G_OBJECT (widget));
    g_object_set_qdata (G_OBJECT (widget), quark_accessible_object,
                        accessible);
    return accessible;
}
