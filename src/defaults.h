/*******************************************************************************
**3456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789
**      10        20        30        40        50        60        70        80
**
** notify-osd
**
** defaults.h - a singelton providing all default values for sizes, colors etc. 
**
** Copyright 2009 Canonical Ltd.
**
** Authors:
**    Mirco "MacSlow" Mueller <mirco.mueller@canonical.com>
**    David Barth <david.barth@canonical.com>
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

#ifndef __DEFAULTS_H
#define __DEFAULTS_H

#include <glib-object.h>
#include <gconf/gconf-client.h>

G_BEGIN_DECLS

#define DEFAULTS_TYPE             (defaults_get_type ())
#define DEFAULTS(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), DEFAULTS_TYPE, Defaults))
#define DEFAULTS_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), DEFAULTS_TYPE, DefaultsClass))
#define IS_DEFAULTS(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), DEFAULTS_TYPE))
#define IS_DEFAULTS_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), DEFAULTS_TYPE))
#define DEFAULTS_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), DEFAULTS_TYPE, DefaultsClass))

/* FIXME: quick hack to get every measurement to use em instead of pixels, to
 * correctly use these two macros you'll need to pass it a valid Defaults class
 * object
 *
 * example use:
 *   PIXEL2EM(42, defaults) - that will returns a gdouble
 *   EM2PIXEL(0.375, defaults) - that will return a gint */
#define PIXELS2EM(pixel_value, d) ((gdouble) ((gdouble) pixel_value / defaults_get_pixel_per_em(d)))
#define EM2PIXELS(em_value, d) ((gint) (em_value * defaults_get_pixel_per_em(d)))

typedef struct _Defaults      Defaults;
typedef struct _DefaultsClass DefaultsClass;

/* instance structure */
struct _Defaults
{
	GObject parent;

	/* private */
	GConfClient* context;
	guint        notifier[5];
	gint         desktop_width;
	gint         desktop_height;
	gint         desktop_top;
	gint         desktop_bottom;
	gint         desktop_left;
	gint         desktop_right;
	gdouble      desktop_bottom_gap;
	gdouble      stack_height;
	gdouble      bubble_vert_gap;
	gdouble      bubble_horz_gap;
	gdouble      bubble_width;
	gdouble      bubble_min_height;
	gdouble      bubble_max_height;
	gdouble      bubble_shadow_size;
	GString*     bubble_shadow_color;
	GString*     bubble_bg_color;
	GString*     bubble_bg_opacity;
	GString*     bubble_hover_opacity;
	gdouble      bubble_corner_radius;
	gdouble      content_shadow_size;
	GString*     content_shadow_color;
	gdouble      margin_size;
	gdouble      icon_size;
	gint         fade_in_timeout;
	gint         fade_out_timeout;
	gint         on_screen_timeout;
	GString*     text_font_face;
	GString*     text_title_color;
	gint         text_title_weight;
	gdouble      text_title_size;
	GString*     text_body_color;
	gint         text_body_weight;
	gdouble      text_body_size;
	gdouble      pixels_per_em;
	gdouble      system_font_size;
	gdouble      screen_dpi;
};

/* class structure */
struct _DefaultsClass
{
	GObjectClass parent;

	/*< signals >*/
	void (*value_changed) (Defaults* defaults); /* used to "inform" bubble
						    ** about any changes in
						    ** rendering and position */
};

GType defaults_get_type (void);

Defaults*
defaults_new (void);

void
defaults_del (Defaults* self);

gint
defaults_get_desktop_width (Defaults* self);

gint
defaults_get_desktop_height (Defaults* self);

gint
defaults_get_desktop_top (Defaults* self);

gint
defaults_get_desktop_bottom (Defaults* self);

gint
defaults_get_desktop_left (Defaults* self);

gint
defaults_get_desktop_right (Defaults* self);

gdouble
defaults_get_desktop_bottom_gap (Defaults* self);

gdouble
defaults_get_stack_height (Defaults* self);

gdouble
defaults_get_bubble_width (Defaults* self);

gdouble
defaults_get_bubble_min_height (Defaults* self);

gdouble
defaults_get_bubble_max_height (Defaults* self);

gdouble
defaults_get_bubble_vert_gap (Defaults* self);

gdouble
defaults_get_bubble_horz_gap (Defaults* self);

gdouble
defaults_get_bubble_shadow_size (Defaults* self);

gchar*
defaults_get_bubble_shadow_color (Defaults* self);

gchar*
defaults_get_bubble_bg_color (Defaults* self);

gchar*
defaults_get_bubble_bg_opacity (Defaults* self);

gchar*
defaults_get_bubble_hover_opacity (Defaults* self);

gdouble
defaults_get_bubble_corner_radius (Defaults* self);

gdouble
defaults_get_content_shadow_size (Defaults* self);

gchar*
defaults_get_content_shadow_color (Defaults* self);

gdouble
defaults_get_margin_size (Defaults* self);

gdouble
defaults_get_icon_size (Defaults* self);

gint
defaults_get_fade_in_timeout (Defaults* self);

gint
defaults_get_fade_out_timeout (Defaults* self);

gint
defaults_get_on_screen_timeout (Defaults* self);

gchar*
defaults_get_text_font_face (Defaults* self);

gchar*
defaults_get_text_title_color (Defaults* self);

gint
defaults_get_text_title_weight (Defaults* self);

gdouble
defaults_get_text_title_size (Defaults* self);

gchar*
defaults_get_text_body_color (Defaults* self);

gint
defaults_get_text_body_weight (Defaults* self);

gdouble
defaults_get_text_body_size (Defaults* self);

gdouble
defaults_get_pixel_per_em (Defaults* self);

gdouble
defaults_get_system_font_size (Defaults* self);

gdouble
defaults_get_screen_dpi (Defaults* self);

void
defaults_refresh_screen_dimension_properties (Defaults *self);

void
defaults_get_top_corner (Defaults *self, gint *x, gint *y);

G_END_DECLS

#endif /* __DEFAULTS_H */
