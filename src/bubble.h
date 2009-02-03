/*******************************************************************************
**3456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789
**      10        20        30        40        50        60        70        80
**
** project:
**    alsdorf
**
** file:
**    bubble.h
**
** author(s):
**    Mirco "MacSlow" Mueller <mirco.mueller@canonical.com>
**    David Barth <david.barth@canonical.com>
**
** copyright (C) Canonical, oct. 2008
**
*******************************************************************************/
#ifndef __BUBBLE_H
#define __BUBBLE_H

#include <glib-object.h>
#include <gdk-pixbuf/gdk-pixbuf.h>

#include "defaults.h"

G_BEGIN_DECLS

#define BUBBLE_TYPE             (bubble_get_type ())
#define BUBBLE(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), BUBBLE_TYPE, Bubble))
#define BUBBLE_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), BUBBLE_TYPE, BubbleClass))
#define IS_BUBBLE(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), BUBBLE_TYPE))
#define IS_BUBBLE_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), BUBBLE_TYPE))
#define BUBBLE_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), BUBBLE_TYPE, BubbleClass))

typedef struct _Bubble        Bubble;
typedef struct _BubbleClass   BubbleClass;
typedef struct _BubblePrivate BubblePrivate;

/* instance structure */
struct _Bubble
{
	GObject   parent;
	Defaults* defaults;

	/*< private >*/
	BubblePrivate *priv;
};

/* class structure */
struct _BubbleClass
{
	GObjectClass parent;

	/*< signals >*/
	void (*timed_out) (Bubble* bubble);
};

GType bubble_get_type (void);

Bubble*
bubble_new (Defaults* defaults);

gchar*
bubble_get_synchronous (Bubble *self);

void
bubble_del (Bubble* self);

void
bubble_set_title (Bubble*      self,
		  const gchar* title);

void
bubble_set_message_body (Bubble*      self,
			 const gchar* body);

void
bubble_set_icon (Bubble*      self,
		 const gchar* filename);

void
bubble_set_icon_from_pixbuf (Bubble*      self,
			     GdkPixbuf*   pixbuf);

void
bubble_set_value (Bubble* self,
		  gint    value);

void
bubble_set_size (Bubble* self,
		 gint    width,
		 gint    height);

void
bubble_set_timeout (Bubble* self,
		    guint   timeout);

guint
bubble_get_timeout (Bubble* self);

void
bubble_set_timer_id (Bubble* self,
		     guint   timer_id);

guint
bubble_get_timer_id (Bubble* self);

void
bubble_set_mouse_over (Bubble*  self,
		       gboolean flag);

gboolean
bubble_is_mouse_over (Bubble* self);

void
bubble_move (Bubble* self,
	     gint x,
	     gint y);

gboolean
bubble_timed_out (Bubble* self);

void
bubble_show (Bubble* self);

void
bubble_refresh (Bubble* self);

gboolean
bubble_hide (Bubble* self);

void
bubble_set_id (Bubble* self,
	       guint   id);

guint
bubble_get_id (Bubble* self);

gboolean
bubble_is_visible (Bubble* self);

void
bubble_start_timer (Bubble* self);

void
bubble_slide_to (Bubble* self,
		 gint    x,
		 gint    y);

void
bubble_get_position (Bubble* self,
		     gint*   x,
		     gint*   y);

gint
bubble_get_height (Bubble *self);

void
bubble_recalc_size (Bubble *self);

gboolean
bubble_is_synchronous (Bubble *self);

void
bubble_set_synchronous (Bubble *self,
			const gchar *sync);

void
bubble_fade_out (Bubble *self,
		 guint   msecs);

void
bubble_fade_in (Bubble *self,
		guint   msecs);

G_END_DECLS

#endif /* __BUBBLE_H */
