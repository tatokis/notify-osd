/*******************************************************************************
**3456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789
**      10        20        30        40        50        60        70        80
**
** notify-osd
**
** defaults.c - a singelton providing all default values for sizes, colors etc. 
**
** Copyright 2009 Canonical Ltd.
**
** Authors:
**    Mirco "MacSlow" Mueller <mirco.mueller@canonical.com>
**    David Barth <david.barth@canonical.com>
**
** Contributor(s):
**    Chow Loong Jin <hyperair@gmail.com> (fix for LP: #401809, rev. 349)
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

#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <gdk/gdk.h>
#include <gdk/gdkx.h>
#include <gtk/gtk.h>
#include <libwnck/libwnck.h>

#include "defaults.h"
#include "util.h"
#include "enum-types.h"

G_DEFINE_TYPE (Defaults, defaults, G_TYPE_OBJECT);

enum
{
	PROP_DUMMY = 0,
	PROP_DESKTOP_BOTTOM_GAP,
	PROP_STACK_HEIGHT,
	PROP_BUBBLE_VERT_GAP,
	PROP_BUBBLE_HORZ_GAP,
	PROP_BUBBLE_WIDTH,
	PROP_BUBBLE_MIN_HEIGHT,
	PROP_BUBBLE_MAX_HEIGHT,
	PROP_BUBBLE_SHADOW_SIZE,
	PROP_BUBBLE_SHADOW_COLOR,
	PROP_BUBBLE_BG_COLOR,
	PROP_BUBBLE_BG_OPACITY,
	PROP_BUBBLE_HOVER_OPACITY,
	PROP_BUBBLE_CORNER_RADIUS,
	PROP_CONTENT_SHADOW_SIZE,
	PROP_CONTENT_SHADOW_COLOR,
	PROP_MARGIN_SIZE,
	PROP_ICON_SIZE,
	PROP_GAUGE_SIZE,
	PROP_GAUGE_OUTLINE_WIDTH,
	PROP_FADE_IN_TIMEOUT,
	PROP_FADE_OUT_TIMEOUT,
	PROP_ON_SCREEN_TIMEOUT,
	PROP_TEXT_FONT_FACE,
	PROP_TEXT_TITLE_COLOR,
	PROP_TEXT_TITLE_WEIGHT,
	PROP_TEXT_TITLE_SIZE,
	PROP_TEXT_BODY_COLOR,
	PROP_TEXT_BODY_WEIGHT,
	PROP_TEXT_BODY_SIZE,
	PROP_PIXELS_PER_EM,
	PROP_SYSTEM_FONT_SIZE,
	PROP_SCREEN_DPI,
	PROP_GRAVITY,
	PROP_CLOSE_ON_CLICK,
	PROP_FADE_ON_HOVER,
	PROP_IGNORE_SESSION_IDLE_INHIBITED,
	PROP_SLOT_ALLOCATION,
};

enum
{
	VALUE_CHANGED,
	LAST_SIGNAL
};

enum
{
	R = 0,
	G,
	B
};

/* taking hints from Pango here, Qt looks a bit different*/
enum
{
	TEXT_WEIGHT_LIGHT  = 300, /* QFont::Light  25 */
	TEXT_WEIGHT_NORMAL = 400, /* QFont::Normal 50 */
	TEXT_WEIGHT_BOLD   = 700  /* QFont::Bold   75 */
};

/* these values are interpreted as em-measurements and do comply to the 
 * visual guide for jaunty-notifications */
#define DEFAULT_DESKTOP_BOTTOM_GAP    6.0f
#define DEFAULT_BUBBLE_WIDTH         24.0f
#define DEFAULT_BUBBLE_MIN_HEIGHT     5.0f
#define DEFAULT_BUBBLE_MAX_HEIGHT    12.2f
#define DEFAULT_BUBBLE_VERT_GAP       0.5f
#define DEFAULT_BUBBLE_HORZ_GAP       0.5f
#define DEFAULT_BUBBLE_SHADOW_SIZE    0.7f
#define DEFAULT_BUBBLE_SHADOW_COLOR  "#000000"
#define DEFAULT_BUBBLE_BG_COLOR      "#131313"
#define DEFAULT_BUBBLE_BG_OPACITY    "#cc"
#define DEFAULT_BUBBLE_HOVER_OPACITY "#66"
#define DEFAULT_BUBBLE_CORNER_RADIUS 0.375f
#define DEFAULT_CONTENT_SHADOW_SIZE  0.125f
#define DEFAULT_CONTENT_SHADOW_COLOR "#000000"
#define DEFAULT_MARGIN_SIZE          1.0f
#define DEFAULT_ICON_SIZE            3.0f
#define DEFAULT_GAUGE_SIZE           0.625f
#define DEFAULT_GAUGE_OUTLINE_WIDTH  0.125f
#define DEFAULT_TEXT_FONT_FACE       "Sans"
#define DEFAULT_TEXT_TITLE_COLOR     "#ffffff"
#define DEFAULT_TEXT_TITLE_WEIGHT    TEXT_WEIGHT_BOLD
#define DEFAULT_TEXT_TITLE_SIZE      1.0f
#define DEFAULT_TEXT_BODY_COLOR      "#eaeaea"
#define DEFAULT_TEXT_BODY_WEIGHT     TEXT_WEIGHT_NORMAL
#define DEFAULT_TEXT_BODY_SIZE       0.9f
#define DEFAULT_PIXELS_PER_EM        10.0f
#define DEFAULT_SYSTEM_FONT_SIZE     10.0f
#define DEFAULT_SCREEN_DPI           96.0f
#define DEFAULT_GRAVITY              GRAVITY_NORTH_EAST
#define DEFAULT_CLOSE_ON_CLICK       TRUE
#define DEFAULT_FADE_ON_HOVER        TRUE
#define DEFAULT_IGNORE_SESSION_IDLE_INHIBITED TRUE
#define DEFAULT_SLOT_ALLOCATION      SLOT_ALLOCATION_DYNAMIC

/* these values are interpreted as milliseconds-measurements and do comply to
 * the visual guide for jaunty-notifications */
#define DEFAULT_FADE_IN_TIMEOUT      250
#define DEFAULT_FADE_OUT_TIMEOUT     1000
#define DEFAULT_ON_SCREEN_TIMEOUT    10000

/* notify-osd settings */
#define NOTIFY_OSD_SCHEMA            "com.canonical.notify-osd"
#define GSETTINGS_GRAVITY_KEY        "gravity"
#define GSETTINGS_MULTIHEAD_MODE_KEY "multihead-mode"
#define GSETTINGS_CLOSE_ON_CLICK_KEY "close-on-click"
#define GSETTINGS_FADE_ON_HOVER_KEY  "fade-on-hover"
#define GSETTINGS_IGNORE_LIST_KEY    "action-ignore"
#define GSETTINGS_IGNORE_SESSION_IDLE_INHIBITED_KEY "ignore-session-idle-inhibited"
#define GSETTINGS_SLOT_ALLOCATION_KEY "slot-allocation"

/* gnome settings */
#define GNOME_DESKTOP_SCHEMA         "org.gnome.desktop.interface"
#define GSETTINGS_FONT_KEY           "font-name"

/* unity settings */
#define UNITY_SCHEMA                 "com.canonical.Unity"
#define GSETTINGS_AVG_BG_COL_KEY     "average-bg-color"

static guint g_defaults_signals[LAST_SIGNAL] = { 0 };

/*-- internal API ------------------------------------------------------------*/

static void
_get_font_size_dpi (Defaults* self)
{
	GString*              string        = NULL;
	gdouble               points        = 0.0f;
	GString*              font_face     = NULL;
	gdouble               dpi           = 0.0f;
	gdouble               pixels_per_em = 0;
	gchar*                font_name     = NULL;
	PangoFontDescription* desc          = NULL;
	GtkSettings*          gtk_settings  = NULL;
	gint                  value         = 0;

	if (!IS_DEFAULTS (self))
		return;

	/* determine current system font-name/size */
	font_name = g_settings_get_string (self->gnome_settings, GSETTINGS_FONT_KEY);
	string = g_string_new (font_name);

	// extract text point-size
	desc = pango_font_description_from_string (font_name);
	if (pango_font_description_get_size_is_absolute (desc))
		points = (gdouble) pango_font_description_get_size (desc); 
	else
		points = (gdouble) pango_font_description_get_size (desc) /
			 (gdouble) PANGO_SCALE;
	pango_font_description_free (desc);
	g_free ((gpointer) font_name);

	// extract font-face-name/style
	font_face = extract_font_face (string->str);

	if (string != NULL)
		g_string_free (string, TRUE);

	/* update stored font-face name and clean up */
	if (font_face != NULL)
	{
		g_object_set (self, "text-font-face", font_face->str, NULL);
		g_string_free (font_face, TRUE);
	}

	/* update stored system-font size (in pt!) */
	g_object_set (self, "system-font-size", (gdouble) points, NULL);

	/* determine current system DPI-setting */
	gtk_settings = gtk_settings_get_default (); // not ref'ed
	g_object_get (gtk_settings, "gtk-xft-dpi", &value, NULL);
	dpi = (float) value / (float) 1024;

	/* update stored DPI-value */
	pixels_per_em = points * dpi / 72.0f;
	g_object_set (self, "pixels-per-em", pixels_per_em, NULL);
	g_object_set (self, "screen-dpi", dpi, NULL);

	if (g_getenv ("DEBUG"))
		g_print ("font-size: %fpt\ndpi: %3.1f\npixels/EM: %2.2f\nwidth: %d px\ntitle-height: %2.2f pt\nbody-height: %2.2f pt\n\n",
			 points,
			 defaults_get_screen_dpi (self),
			 pixels_per_em,
			 (gint) (pixels_per_em * DEFAULT_BUBBLE_WIDTH),
			 defaults_get_system_font_size (self) *
			 defaults_get_text_title_size (self),
			 defaults_get_system_font_size (self) *
			 defaults_get_text_body_size (self));
}

static void
_font_changed (GSettings* settings,
			   gchar*     key,
			   gpointer   data)
{
	Defaults* defaults;

	if (!data)
		return;

	defaults = (Defaults*) data;
	if (!IS_DEFAULTS (defaults))
		return;

	/* grab system-wide font-face/size and DPI */
	_get_font_size_dpi (defaults);

	g_signal_emit (defaults, g_defaults_signals[VALUE_CHANGED], 0);
}

void
defaults_refresh_bg_color_property (Defaults *self)
{
	Atom         real_type;
	gint         result;
	gint         real_format;
	gulong       items_read;
	gulong       items_left;
	gchar*       colors;
	Atom         representative_colors_atom;
	Display*     display;

	g_return_if_fail ((self != NULL) && IS_DEFAULTS (self));

	representative_colors_atom = gdk_x11_get_xatom_by_name ("_GNOME_BACKGROUND_REPRESENTATIVE_COLORS");
	display = gdk_x11_display_get_xdisplay (gdk_display_get_default ());

	gdk_error_trap_push ();
	result = XGetWindowProperty (display,
				     GDK_ROOT_WINDOW (),
				     representative_colors_atom,
				     0L,
				     G_MAXLONG,
				     False,
				     XA_STRING,
				     &real_type,
				     &real_format,
				     &items_read,
				     &items_left,
				     (guchar **) &colors);
	gdk_flush ();
	gdk_error_trap_pop_ignored ();

	if (result == Success && items_read)
	{
		/* by treating the result as a nul-terminated string, we
		 * select the first colour in the list.
		 */
		g_object_set (self,
			      "bubble-bg-color",
			      colors,
			      NULL);
		XFree (colors);
	}
}

static void
defaults_constructed (GObject* gobject)
{
	Defaults*    self;
	gdouble      margin_size;
	gdouble      icon_size;
	gdouble      bubble_height;
	gdouble      new_bubble_height;
	GdkScreen*   screen;
	gint         x;
	gint         y;

	self = DEFAULTS (gobject);

	defaults_get_top_corner (self, &screen, &x, &y);
	defaults_refresh_bg_color_property (self);

	/* grab system-wide font-face/size and DPI */
	_get_font_size_dpi (self);

	/* correct the default min. bubble-height, according to the icon-size */
	g_object_get (self,
		      "margin-size",
		      &margin_size,
		      NULL);
	g_object_get (self,
		      "icon-size",
		      &icon_size,
		      NULL);
	g_object_get (self,
		      "bubble-min-height",
		      &bubble_height,
		      NULL);

#if 0
	/* try to register the non-standard size for the gtk_icon_theme_lookup
	   calls to work */
	gtk_icon_size_register ("52x52",
				pixels_per_em * icon_size,
				pixels_per_em * icon_size);
#endif

	new_bubble_height = 2.0f * margin_size + icon_size;

	if (new_bubble_height > bubble_height)
	{
		g_object_set (self,
			      "bubble-min-height",
			      new_bubble_height,
			      NULL);
	}

	g_settings_bind (self->nosd_settings,
	                 GSETTINGS_GRAVITY_KEY,
	                 self,
	                 "gravity",
	                 G_SETTINGS_BIND_DEFAULT);

	g_settings_bind (self->nosd_settings,
	                 GSETTINGS_CLOSE_ON_CLICK_KEY,
	                 self,
	                 "close-on-click",
	                 G_SETTINGS_BIND_DEFAULT);

	g_settings_bind (self->nosd_settings,
	                 GSETTINGS_FADE_ON_HOVER_KEY,
	                 self,
	                 "fade-on-hover",
	                 G_SETTINGS_BIND_DEFAULT);

	g_settings_bind (self->nosd_settings,
	                 GSETTINGS_IGNORE_SESSION_IDLE_INHIBITED_KEY,
	                 self,
	                 "ignore-session-idle-inhibited",
	                 G_SETTINGS_BIND_DEFAULT);

	g_settings_bind (self->nosd_settings,
	                 GSETTINGS_SLOT_ALLOCATION_KEY,
	                 self,
	                 "slot-allocation",
	                 G_SETTINGS_BIND_DEFAULT);

	/* FIXME: calling this here causes a segfault */
	/* chain up to the parent class */
	/*G_OBJECT_CLASS (defaults_parent_class)->constructed (gobject);*/
}

static void
defaults_dispose (GObject* gobject)
{
	Defaults* defaults;

	defaults = DEFAULTS (gobject);

	g_object_unref (defaults->nosd_settings);
	g_object_unref (defaults->gnome_settings);

	if (defaults->bubble_shadow_color)
	{
		g_string_free (defaults->bubble_shadow_color, TRUE);
		defaults->bubble_shadow_color = NULL;
	}

	if (defaults->bubble_bg_color)
	{
		g_string_free (defaults->bubble_bg_color, TRUE);
		defaults->bubble_bg_color = NULL;
	}

	if (defaults->bubble_bg_opacity)
	{
		g_string_free (defaults->bubble_bg_opacity, TRUE);
		defaults->bubble_bg_opacity = NULL;
	}

	if (defaults->bubble_hover_opacity)
	{
		g_string_free (defaults->bubble_hover_opacity, TRUE);
		defaults->bubble_hover_opacity = NULL;
	}

	if (defaults->content_shadow_color)
	{
		g_string_free (defaults->content_shadow_color, TRUE);
		defaults->content_shadow_color = NULL;
	}

	if (defaults->text_font_face)
	{
		g_string_free (defaults->text_font_face, TRUE);
		defaults->text_font_face = NULL;
	}

	if (defaults->text_title_color)
	{
		g_string_free (defaults->text_title_color, TRUE);
		defaults->text_title_color = NULL;
	}

	if (defaults->text_body_color)
	{
		g_string_free (defaults->text_body_color, TRUE);
		defaults->text_body_color = NULL;
	}

	// chain up to the parent class
	G_OBJECT_CLASS (defaults_parent_class)->dispose (gobject);
}

static void
defaults_finalize (GObject* gobject)
{
	// chain up to the parent class
	G_OBJECT_CLASS (defaults_parent_class)->finalize (gobject);
}

static void
defaults_init (Defaults* self)
{
	/* "connect" to the required GSettings schemas */
	self->nosd_settings  = g_settings_new (NOTIFY_OSD_SCHEMA);
	self->gnome_settings = g_settings_new (GNOME_DESKTOP_SCHEMA);

	g_signal_connect (self->gnome_settings,
					  "changed",
					  G_CALLBACK (_font_changed),
					  self);

	g_signal_connect (gtk_settings_get_default (),
			  "notify::gtk-xft-dpi",
			  G_CALLBACK (_font_changed),
			  self);
}

static void
defaults_get_property (GObject*    gobject,
		       guint       prop,
		       GValue*     value,
		       GParamSpec* spec)
{
	Defaults* defaults;

	defaults = DEFAULTS (gobject);

	switch (prop)
	{
		case PROP_DESKTOP_BOTTOM_GAP:
			g_value_set_double (value, defaults->desktop_bottom_gap);
		break;

		case PROP_STACK_HEIGHT:
			g_value_set_double (value, defaults->stack_height);
		break;

		case PROP_BUBBLE_VERT_GAP:
			g_value_set_double (value, defaults->bubble_vert_gap);
		break;

		case PROP_BUBBLE_HORZ_GAP:
			g_value_set_double (value, defaults->bubble_horz_gap);
		break;

		case PROP_BUBBLE_WIDTH:
			g_value_set_double (value, defaults->bubble_width);
		break;

		case PROP_BUBBLE_MIN_HEIGHT:
			g_value_set_double (value, defaults->bubble_min_height);
		break;

		case PROP_BUBBLE_MAX_HEIGHT:
			g_value_set_double (value, defaults->bubble_max_height);
		break;

		case PROP_BUBBLE_SHADOW_SIZE:
			g_value_set_double (value, defaults->bubble_shadow_size);
		break;

		case PROP_BUBBLE_SHADOW_COLOR:
			g_value_set_string (value,
					    defaults->bubble_shadow_color->str);
		break;

		case PROP_BUBBLE_BG_COLOR:
			g_value_set_string (value,
					    defaults->bubble_bg_color->str);
		break;

		case PROP_BUBBLE_BG_OPACITY:
			g_value_set_string (value,
					    defaults->bubble_bg_opacity->str);
		break;

		case PROP_BUBBLE_HOVER_OPACITY:
			g_value_set_string (value,
					    defaults->bubble_hover_opacity->str);
		break;

		case PROP_BUBBLE_CORNER_RADIUS:
			g_value_set_double (value, defaults->bubble_corner_radius);
		break;

		case PROP_CONTENT_SHADOW_SIZE:
			g_value_set_double (value, defaults->content_shadow_size);
		break;

		case PROP_CONTENT_SHADOW_COLOR:
			g_value_set_string (value,
					    defaults->content_shadow_color->str);
		break;

		case PROP_MARGIN_SIZE:
			g_value_set_double (value, defaults->margin_size);
		break;

		case PROP_ICON_SIZE:
			g_value_set_double (value, defaults->icon_size);
		break;

		case PROP_GAUGE_SIZE:
			g_value_set_double (value, defaults->gauge_size);
		break;

		case PROP_GAUGE_OUTLINE_WIDTH:
			g_value_set_double (value, defaults->gauge_outline_width);
		break;

		case PROP_FADE_IN_TIMEOUT:
			g_value_set_int (value, defaults->fade_in_timeout);
		break;

		case PROP_FADE_OUT_TIMEOUT:
			g_value_set_int (value, defaults->fade_out_timeout);
		break;

		case PROP_ON_SCREEN_TIMEOUT:
			g_value_set_int (value, defaults->on_screen_timeout);
		break;

		case PROP_TEXT_FONT_FACE:
			g_value_set_string (value,
					    defaults->text_font_face->str);
		break;

		case PROP_TEXT_TITLE_COLOR:
			g_value_set_string (value,
					    defaults->text_title_color->str);
		break;

		case PROP_TEXT_TITLE_WEIGHT:
			g_value_set_int (value, defaults->text_title_weight);
		break;

		case PROP_TEXT_TITLE_SIZE:
			g_value_set_double (value, defaults->text_title_size);
		break;

		case PROP_TEXT_BODY_COLOR:
			g_value_set_string (value,
					    defaults->text_body_color->str);
		break;

		case PROP_TEXT_BODY_WEIGHT:
			g_value_set_int (value, defaults->text_body_weight);
		break;

		case PROP_TEXT_BODY_SIZE:
			g_value_set_double (value, defaults->text_body_size);
		break;

		case PROP_PIXELS_PER_EM:
			g_value_set_double (value, defaults->pixels_per_em);
		break;

		case PROP_SYSTEM_FONT_SIZE:
			g_value_set_double (value, defaults->system_font_size);
		break;

		case PROP_SCREEN_DPI:
			g_value_set_double (value, defaults->screen_dpi);
		break;

		case PROP_GRAVITY:
			g_value_set_enum (value, defaults->gravity);
		break;

	    case PROP_CLOSE_ON_CLICK:
		    g_value_set_boolean (value, defaults->close_on_click);
		break;

	    case PROP_FADE_ON_HOVER:
		    g_value_set_boolean (value, defaults->fade_on_hover);
		break;

	    case PROP_IGNORE_SESSION_IDLE_INHIBITED:
		    g_value_set_boolean (value, defaults->ignore_session_idle_inhibited);
		break;

	    case PROP_SLOT_ALLOCATION:
		    g_value_set_enum(value, defaults->slot_allocation);
		break;

		default :
			G_OBJECT_WARN_INVALID_PROPERTY_ID (gobject, prop, spec);
		break;
	}
}

static void
defaults_set_property (GObject*      gobject,
		       guint         prop,
		       const GValue* value,
		       GParamSpec*   spec)
{
	Defaults* defaults;

	defaults = DEFAULTS (gobject);

	switch (prop)
	{
		case PROP_DESKTOP_BOTTOM_GAP:
			defaults->desktop_bottom_gap = g_value_get_double (value);
		break;

		case PROP_STACK_HEIGHT:
			defaults->stack_height = g_value_get_double (value);
		break;

		case PROP_BUBBLE_WIDTH:
			defaults->bubble_width = g_value_get_double (value);
		break;

		case PROP_BUBBLE_VERT_GAP:
			defaults->bubble_vert_gap = g_value_get_double (value);
		break;

		case PROP_BUBBLE_HORZ_GAP:
			defaults->bubble_horz_gap = g_value_get_double (value);
		break;

		case PROP_BUBBLE_MIN_HEIGHT:
			defaults->bubble_min_height = g_value_get_double (value);
		break;

		case PROP_BUBBLE_MAX_HEIGHT:
			defaults->bubble_max_height = g_value_get_double (value);
		break;

		case PROP_BUBBLE_SHADOW_SIZE:
			defaults->bubble_shadow_size = g_value_get_double (value);
		break;

		case PROP_BUBBLE_SHADOW_COLOR:
			if (defaults->bubble_shadow_color != NULL)
			{
				g_string_free (defaults->bubble_shadow_color,
					       TRUE);
			}
			defaults->bubble_shadow_color = g_string_new (
				g_value_get_string (value));
		break;

		case PROP_BUBBLE_BG_COLOR:
			if (defaults->bubble_bg_color != NULL)
			{
				g_string_free (defaults->bubble_bg_color, TRUE);
			}
			defaults->bubble_bg_color = g_string_new (
				g_value_get_string (value));
		break;

		case PROP_BUBBLE_BG_OPACITY:
			if (defaults->bubble_bg_opacity != NULL)
			{
				g_string_free (defaults->bubble_bg_opacity,
					       TRUE);
			}
			defaults->bubble_bg_opacity = g_string_new (
				g_value_get_string (value));
		break;

		case PROP_BUBBLE_HOVER_OPACITY:
			if (defaults->bubble_hover_opacity != NULL)
			{
				g_string_free (defaults->bubble_hover_opacity,
					       TRUE);
			}
			defaults->bubble_hover_opacity = g_string_new (
				g_value_get_string (value));
		break;

		case PROP_BUBBLE_CORNER_RADIUS:
			defaults->bubble_corner_radius = g_value_get_double (value);
		break;

		case PROP_CONTENT_SHADOW_SIZE:
			defaults->content_shadow_size = g_value_get_double (value);
		break;

		case PROP_CONTENT_SHADOW_COLOR:
			if (defaults->content_shadow_color != NULL)
			{
				g_string_free (defaults->content_shadow_color,
					       TRUE);
			}
			defaults->content_shadow_color = g_string_new (
				g_value_get_string (value));
		break;

		case PROP_MARGIN_SIZE:
			defaults->margin_size = g_value_get_double (value);
		break;

		case PROP_ICON_SIZE:
			defaults->icon_size = g_value_get_double (value);
		break;

		case PROP_GAUGE_SIZE:
			defaults->gauge_size = g_value_get_double (value);
		break;

		case PROP_GAUGE_OUTLINE_WIDTH:
			defaults->gauge_outline_width = g_value_get_double (value);
		break;

		case PROP_FADE_IN_TIMEOUT:
			defaults->fade_in_timeout = g_value_get_int (value);
		break;

		case PROP_FADE_OUT_TIMEOUT:
			defaults->fade_out_timeout = g_value_get_int (value);
		break;

		case PROP_ON_SCREEN_TIMEOUT:
			defaults->on_screen_timeout = g_value_get_int (value);
		break;

		case PROP_TEXT_FONT_FACE:
			if (defaults->text_font_face != NULL)
			{
				g_string_free (defaults->text_font_face,
					       TRUE);
			}
			defaults->text_font_face = g_string_new (
				g_value_get_string (value));
		break;

		case PROP_TEXT_TITLE_COLOR:
			if (defaults->text_title_color != NULL)
			{
				g_string_free (defaults->text_title_color,
					       TRUE);
			}
			defaults->text_title_color = g_string_new (
				g_value_get_string (value));
		break;

		case PROP_TEXT_TITLE_WEIGHT:
			defaults->text_title_weight = g_value_get_int (value);
		break;

		case PROP_TEXT_TITLE_SIZE:
			defaults->text_title_size = g_value_get_double (value);
		break;

		case PROP_TEXT_BODY_COLOR:
			if (defaults->text_body_color != NULL)
			{
				g_string_free (defaults->text_body_color, TRUE);
			}
			defaults->text_body_color = g_string_new (
				g_value_get_string (value));
		break;

		case PROP_TEXT_BODY_WEIGHT:
			defaults->text_body_weight = g_value_get_int (value);
		break;

		case PROP_TEXT_BODY_SIZE:
			defaults->text_body_size = g_value_get_double (value);
		break;

		case PROP_PIXELS_PER_EM:
			defaults->pixels_per_em = g_value_get_double (value);
		break;

		case PROP_SYSTEM_FONT_SIZE:
			defaults->system_font_size = g_value_get_double (value);
		break;

		case PROP_SCREEN_DPI:
			defaults->screen_dpi = g_value_get_double (value);
		break;

		case PROP_GRAVITY:
			defaults->gravity = g_value_get_enum (value);
		break;

	    case PROP_CLOSE_ON_CLICK:
		    defaults->close_on_click = g_value_get_boolean (value);
		break;

	    case PROP_FADE_ON_HOVER:
		    defaults->fade_on_hover = g_value_get_boolean (value);
		break;

	    case PROP_IGNORE_SESSION_IDLE_INHIBITED:
		    defaults->ignore_session_idle_inhibited = g_value_get_boolean (value);
		break;

	    case PROP_SLOT_ALLOCATION:
		    defaults->slot_allocation = g_value_get_enum (value);
		break;

		default :
			G_OBJECT_WARN_INVALID_PROPERTY_ID (gobject, prop, spec);
		break;
	}
}

static void
defaults_class_init (DefaultsClass* klass)
{
	GObjectClass* gobject_class = G_OBJECT_CLASS (klass);

	GParamSpec*   property_desktop_bottom_gap;
	GParamSpec*   property_stack_height;
	GParamSpec*   property_bubble_vert_gap;
	GParamSpec*   property_bubble_horz_gap;
	GParamSpec*   property_bubble_width;
	GParamSpec*   property_bubble_min_height;
	GParamSpec*   property_bubble_max_height;
	GParamSpec*   property_bubble_shadow_size;
	GParamSpec*   property_bubble_shadow_color;
	GParamSpec*   property_bubble_bg_color;
	GParamSpec*   property_bubble_bg_opacity;
	GParamSpec*   property_bubble_hover_opacity;
	GParamSpec*   property_bubble_corner_radius;
	GParamSpec*   property_content_shadow_size;
	GParamSpec*   property_content_shadow_color;
	GParamSpec*   property_margin_size;
	GParamSpec*   property_icon_size;
	GParamSpec*   property_gauge_size;
	GParamSpec*   property_gauge_outline_width;
	GParamSpec*   property_fade_in_timeout;
	GParamSpec*   property_fade_out_timeout;
	GParamSpec*   property_on_screen_timeout;
	GParamSpec*   property_text_font_face;
	GParamSpec*   property_text_title_color;
	GParamSpec*   property_text_title_weight;
	GParamSpec*   property_text_title_size;
	GParamSpec*   property_text_body_color;
	GParamSpec*   property_text_body_weight;
	GParamSpec*   property_text_body_size;
	GParamSpec*   property_pixels_per_em;
	GParamSpec*   property_system_font_size;
	GParamSpec*   property_screen_dpi;
	GParamSpec*   property_gravity;
	GParamSpec*   property_close_on_click;
	GParamSpec*   property_fade_on_hover;
	GParamSpec*   property_ignore_session_idle_inhibited;
	GParamSpec*   property_slot_allocation;

	gobject_class->constructed  = defaults_constructed;
	gobject_class->dispose      = defaults_dispose;
	gobject_class->finalize     = defaults_finalize;
	gobject_class->get_property = defaults_get_property;
	gobject_class->set_property = defaults_set_property;

	g_defaults_signals[VALUE_CHANGED] = g_signal_new (
		"value-changed",
		G_OBJECT_CLASS_TYPE (gobject_class),
		G_SIGNAL_RUN_LAST,
		G_STRUCT_OFFSET (DefaultsClass, value_changed),
		NULL,
		NULL,
		g_cclosure_marshal_VOID__VOID,
		G_TYPE_NONE,
		0);

	property_desktop_bottom_gap = g_param_spec_double (
				"desktop-bottom-gap",
				"desktop-bottom-gap",
				"Bottom gap on the desktop in em",
				0.0f,
				16.0f,
				DEFAULT_DESKTOP_BOTTOM_GAP,
				G_PARAM_CONSTRUCT |
				G_PARAM_READWRITE |
				G_PARAM_STATIC_STRINGS);
	g_object_class_install_property (gobject_class,
					 PROP_DESKTOP_BOTTOM_GAP,
					 property_desktop_bottom_gap);

	property_stack_height = g_param_spec_double (
				"stack-height",
				"stack-height",
				"Maximum allowed height of stack (in em)",
				0.0f,
				256.0f,
				50.0f,
				G_PARAM_CONSTRUCT |
				G_PARAM_READWRITE |
				G_PARAM_STATIC_STRINGS);
	g_object_class_install_property (gobject_class,
					 PROP_STACK_HEIGHT,
					 property_stack_height);

    	property_bubble_vert_gap = g_param_spec_double (
				"bubble-vert-gap",
				"bubble-vert-gap",
				"Vert. gap between bubble and workarea edge (in em)",
				0.0f,
				10.0f,
				DEFAULT_BUBBLE_VERT_GAP,
				G_PARAM_CONSTRUCT |
				G_PARAM_READWRITE |
				G_PARAM_STATIC_STRINGS);
	g_object_class_install_property (gobject_class,
					 PROP_BUBBLE_VERT_GAP,
					 property_bubble_vert_gap);

    	property_bubble_horz_gap = g_param_spec_double (
				"bubble-horz-gap",
				"bubble-horz-gap",
				"Horz. gap between bubble and workarea edge (in em)",
				0.0f,
				10.0f,
				DEFAULT_BUBBLE_HORZ_GAP,
				G_PARAM_CONSTRUCT |
				G_PARAM_READWRITE |
				G_PARAM_STATIC_STRINGS);
	g_object_class_install_property (gobject_class,
					 PROP_BUBBLE_HORZ_GAP,
					 property_bubble_horz_gap);

	property_bubble_width = g_param_spec_double (
				"bubble-width",
				"bubble-width",
				"Width of bubble (in em)",
				0.0f,
				256.0f,
				DEFAULT_BUBBLE_WIDTH,
				G_PARAM_CONSTRUCT |
				G_PARAM_READWRITE |
				G_PARAM_STATIC_STRINGS);
	g_object_class_install_property (gobject_class,
					 PROP_BUBBLE_WIDTH,
					 property_bubble_width);

	property_bubble_min_height = g_param_spec_double (
				"bubble-min-height",
				"bubble-min-height",
				"Min. height of bubble (in em)",
				0.0f,
				256.0f,
				DEFAULT_BUBBLE_MIN_HEIGHT,
				G_PARAM_CONSTRUCT |
				G_PARAM_READWRITE |
				G_PARAM_STATIC_STRINGS);
	g_object_class_install_property (gobject_class,
					 PROP_BUBBLE_MIN_HEIGHT,
					 property_bubble_min_height);

	property_bubble_max_height = g_param_spec_double (
				"bubble-max-height",
				"bubble-max-height",
				"Max. height of bubble (in em)",
				0.0f,
				256.0f,
				DEFAULT_BUBBLE_MAX_HEIGHT,
				G_PARAM_CONSTRUCT |
				G_PARAM_READWRITE |
				G_PARAM_STATIC_STRINGS);
	g_object_class_install_property (gobject_class,
					 PROP_BUBBLE_MAX_HEIGHT,
					 property_bubble_max_height);

	property_bubble_shadow_size = g_param_spec_double (
				"bubble-shadow-size",
				"bubble-shadow-size",
				"Size (in em) of bubble drop-shadow",
				0.0f,
				32.0f,
				DEFAULT_BUBBLE_SHADOW_SIZE,
				G_PARAM_CONSTRUCT |
				G_PARAM_READWRITE |
				G_PARAM_STATIC_STRINGS);
	g_object_class_install_property (gobject_class,
					 PROP_BUBBLE_SHADOW_SIZE,
					 property_bubble_shadow_size);

	property_bubble_shadow_color = g_param_spec_string (
				"bubble-shadow-color",
				"bubble-shadow-color",
				"Color of bubble drop-shadow",
				DEFAULT_BUBBLE_SHADOW_COLOR,
				G_PARAM_CONSTRUCT |
				G_PARAM_READWRITE |
				G_PARAM_STATIC_STRINGS);
	g_object_class_install_property (gobject_class,
					 PROP_BUBBLE_SHADOW_COLOR,
					 property_bubble_shadow_color);

	property_bubble_bg_color = g_param_spec_string (
				"bubble-bg-color",
				"bubble-bg-color",
				"Color of bubble-background",
				DEFAULT_BUBBLE_BG_COLOR,
				G_PARAM_CONSTRUCT |
				G_PARAM_READWRITE |
				G_PARAM_STATIC_STRINGS);
	g_object_class_install_property (gobject_class,
					 PROP_BUBBLE_BG_COLOR,
					 property_bubble_bg_color);

	property_bubble_bg_opacity = g_param_spec_string (
				"bubble-bg-opacity",
				"bubble-bg-opacity",
				"Opacity of bubble-background",
				DEFAULT_BUBBLE_BG_OPACITY,
				G_PARAM_CONSTRUCT |
				G_PARAM_READWRITE |
				G_PARAM_STATIC_STRINGS);
	g_object_class_install_property (gobject_class,
					 PROP_BUBBLE_BG_OPACITY,
					 property_bubble_bg_opacity);

	property_bubble_hover_opacity = g_param_spec_string (
				"bubble-hover-opacity",
				"bubble-hover-opacity",
				"Opacity of bubble in mouse-over case",
				DEFAULT_BUBBLE_HOVER_OPACITY,
				G_PARAM_CONSTRUCT |
				G_PARAM_READWRITE |
				G_PARAM_STATIC_STRINGS);
	g_object_class_install_property (gobject_class,
					 PROP_BUBBLE_HOVER_OPACITY,
					 property_bubble_hover_opacity);

	property_bubble_corner_radius = g_param_spec_double (
				"bubble-corner-radius",
				"bubble-corner-radius",
				"Corner-radius of bubble (in em)",
				0.0f,
				16.0f,
				DEFAULT_BUBBLE_CORNER_RADIUS,
				G_PARAM_CONSTRUCT |
				G_PARAM_READWRITE |
				G_PARAM_STATIC_STRINGS);
	g_object_class_install_property (gobject_class,
					 PROP_BUBBLE_CORNER_RADIUS,
					 property_bubble_corner_radius);

	property_content_shadow_size = g_param_spec_double (
				"content-shadow-size",
				"content-shadow-size",
				"Size (in em) of icon/text drop-shadow",
				0.0f,
				8.0f,
				DEFAULT_CONTENT_SHADOW_SIZE,
				G_PARAM_CONSTRUCT |
				G_PARAM_READWRITE |
				G_PARAM_STATIC_STRINGS);
	g_object_class_install_property (gobject_class,
					 PROP_CONTENT_SHADOW_SIZE,
					 property_content_shadow_size);

	property_content_shadow_color = g_param_spec_string (
				"content-shadow-color",
				"content-shadow-color",
				"Color of icon/text drop-shadow",
				DEFAULT_CONTENT_SHADOW_COLOR,
				G_PARAM_CONSTRUCT |
				G_PARAM_READWRITE |
				G_PARAM_STATIC_STRINGS);
	g_object_class_install_property (gobject_class,
					 PROP_CONTENT_SHADOW_COLOR,
					 property_content_shadow_color);

	property_margin_size = g_param_spec_double (
				"margin-size",
				"margin-size",
				"Size (in em) of margin",
				0.0f,
				32.0f,
				DEFAULT_MARGIN_SIZE,
				G_PARAM_CONSTRUCT |
				G_PARAM_READWRITE |
				G_PARAM_STATIC_STRINGS);
	g_object_class_install_property (gobject_class,
					 PROP_MARGIN_SIZE,
					 property_margin_size);

	property_icon_size = g_param_spec_double (
				"icon-size",
				"icon-size",
				"Size (in em) of icon/avatar",
				0.0f,
				64.0f,
				DEFAULT_ICON_SIZE,
				G_PARAM_CONSTRUCT |
				G_PARAM_READWRITE |
				G_PARAM_STATIC_STRINGS);
	g_object_class_install_property (gobject_class,
					 PROP_ICON_SIZE,
					 property_icon_size);

	property_gauge_size = g_param_spec_double (
				"gauge-size",
				"gauge-size",
				"Size/height (in em) of gauge/indicator",
				0.5f,
				1.0f,
				DEFAULT_GAUGE_SIZE,
				G_PARAM_CONSTRUCT |
				G_PARAM_READWRITE |
				G_PARAM_STATIC_STRINGS);
	g_object_class_install_property (gobject_class,
					 PROP_GAUGE_SIZE,
					 property_gauge_size);

	property_gauge_outline_width = g_param_spec_double (
				"gauge-outline-width",
				"gauge-outline-width",
				"Width/thickness (in em) of gauge-outline",
				0.1f,
				0.2f,
				DEFAULT_GAUGE_OUTLINE_WIDTH,
				G_PARAM_CONSTRUCT |
				G_PARAM_READWRITE |
				G_PARAM_STATIC_STRINGS);
	g_object_class_install_property (gobject_class,
					 PROP_GAUGE_OUTLINE_WIDTH,
					 property_gauge_outline_width);

	property_fade_in_timeout = g_param_spec_int (
				"fade-in-timeout",
				"fade-in-timeout",
				"Timeout for bubble fade-in in milliseconds",
				0,
				10000,
				DEFAULT_FADE_IN_TIMEOUT,
				G_PARAM_CONSTRUCT |
				G_PARAM_READWRITE |
				G_PARAM_STATIC_STRINGS);
	g_object_class_install_property (gobject_class,
					 PROP_FADE_IN_TIMEOUT,
					 property_fade_in_timeout);

	property_fade_out_timeout = g_param_spec_int (
				"fade-out-timeout",
				"fade-out-timeout",
				"Timeout for bubble fade-out in milliseconds",
				0,
				10000,
				DEFAULT_FADE_OUT_TIMEOUT,
				G_PARAM_CONSTRUCT |
				G_PARAM_READWRITE |
				G_PARAM_STATIC_STRINGS);
	g_object_class_install_property (gobject_class,
					 PROP_FADE_OUT_TIMEOUT,
					 property_fade_out_timeout);

	property_on_screen_timeout = g_param_spec_int (
				"on-screen-timeout",
				"on-screen-timeout",
				"Timeout for bubble on screen in milliseconds",
				0,
				10000,
				DEFAULT_ON_SCREEN_TIMEOUT,
				G_PARAM_CONSTRUCT |
				G_PARAM_READWRITE |
				G_PARAM_STATIC_STRINGS);
	g_object_class_install_property (gobject_class,
					 PROP_ON_SCREEN_TIMEOUT,
					 property_on_screen_timeout);

	property_text_font_face = g_param_spec_string (
				"text-font-face",
				"text-font-face",
				"Font-face to use of any rendered text",
				DEFAULT_TEXT_FONT_FACE,
				G_PARAM_CONSTRUCT |
				G_PARAM_READWRITE |
				G_PARAM_STATIC_STRINGS);
	g_object_class_install_property (gobject_class,
					 PROP_TEXT_FONT_FACE,
					 property_text_font_face);

	property_text_title_color = g_param_spec_string (
				"text-title-color",
				"text-title-color",
				"Color to use for content title-text",
				DEFAULT_TEXT_TITLE_COLOR,
				G_PARAM_CONSTRUCT |
				G_PARAM_READWRITE |
				G_PARAM_STATIC_STRINGS);
	g_object_class_install_property (gobject_class,
					 PROP_TEXT_TITLE_COLOR,
					 property_text_title_color);

	property_text_title_weight = g_param_spec_int (
				"text-title-weight",
				"text-title-weight",
				"Weight to use for content title-text",
				0,
				1000,
				DEFAULT_TEXT_TITLE_WEIGHT,
				G_PARAM_CONSTRUCT |
				G_PARAM_READWRITE |
				G_PARAM_STATIC_STRINGS);
	g_object_class_install_property (gobject_class,
					 PROP_TEXT_TITLE_WEIGHT,
					 property_text_title_weight);

	property_text_title_size = g_param_spec_double (
				"text-title-size",
				"text-title-size",
				"Size (in em) of font to use for content title-text",
				0.0f,
				32.0f,
				DEFAULT_TEXT_TITLE_SIZE,
				G_PARAM_CONSTRUCT |
				G_PARAM_READWRITE |
				G_PARAM_STATIC_STRINGS);
	g_object_class_install_property (gobject_class,
					 PROP_TEXT_TITLE_SIZE,
					 property_text_title_size);

	property_text_body_color = g_param_spec_string (
				"text-body-color",
				"text-body-color",
				"Color to use for content body-text",
				DEFAULT_TEXT_BODY_COLOR,
				G_PARAM_CONSTRUCT |
				G_PARAM_READWRITE |
				G_PARAM_STATIC_STRINGS);
	g_object_class_install_property (gobject_class,
					 PROP_TEXT_BODY_COLOR,
					 property_text_body_color);

	property_text_body_weight = g_param_spec_int (
				"text-body-weight",
				"text-body-weight",
				"Weight to use for content body-text",
				0,
				1000,
				DEFAULT_TEXT_BODY_WEIGHT,
				G_PARAM_CONSTRUCT |
				G_PARAM_READWRITE |
				G_PARAM_STATIC_STRINGS);
	g_object_class_install_property (gobject_class,
					 PROP_TEXT_BODY_WEIGHT,
					 property_text_body_weight);

	property_text_body_size = g_param_spec_double (
				"text-body-size",
				"text-body-size",
				"Size (in em) of font to use for content body-text",
				0.0f,
				32.0f,
				DEFAULT_TEXT_BODY_SIZE,
				G_PARAM_CONSTRUCT |
				G_PARAM_READWRITE |
				G_PARAM_STATIC_STRINGS);
	g_object_class_install_property (gobject_class,
					 PROP_TEXT_BODY_SIZE,
					 property_text_body_size);

	property_pixels_per_em = g_param_spec_double (
				"pixels-per-em",
				"pixels-per-em",
				"Number of pixels for one em-unit",
				1.0f,
				100.0f,
				DEFAULT_PIXELS_PER_EM,
				G_PARAM_CONSTRUCT |
				G_PARAM_READWRITE |
				G_PARAM_STATIC_STRINGS);
	g_object_class_install_property (gobject_class,
					 PROP_PIXELS_PER_EM,
					 property_pixels_per_em);

	property_system_font_size = g_param_spec_double (
				"system-font-size",
				"system-font-size",
				"System font-size in pt",
				1.0f,
				100.0f,
				DEFAULT_SYSTEM_FONT_SIZE,
				G_PARAM_CONSTRUCT |
				G_PARAM_READWRITE |
				G_PARAM_STATIC_STRINGS);
	g_object_class_install_property (gobject_class,
					 PROP_SYSTEM_FONT_SIZE,
					 property_system_font_size);

	property_screen_dpi = g_param_spec_double (
				"screen-dpi",
				"screen-dpi",
				"Screen DPI value",
				10.0f,
				600.0f,
				DEFAULT_SCREEN_DPI,
				G_PARAM_CONSTRUCT |
				G_PARAM_READWRITE |
				G_PARAM_STATIC_STRINGS);
	g_object_class_install_property (gobject_class,
					 PROP_SCREEN_DPI,
					 property_screen_dpi);

	property_gravity = g_param_spec_enum (
				"gravity",
				"gravity",
				"Positional hint for placing bubbles",
				GRAVITY_TYPE_,
				DEFAULT_GRAVITY,
				G_PARAM_CONSTRUCT |
				G_PARAM_READWRITE |
				G_PARAM_STATIC_STRINGS);
	g_object_class_install_property (gobject_class,
					 PROP_GRAVITY,
					 property_gravity);

	property_close_on_click = g_param_spec_boolean (
	            "close-on-click",
	            "close-on-click",
	            "Close notification when clicking on it",
	            DEFAULT_CLOSE_ON_CLICK,
	            G_PARAM_CONSTRUCT |
	            G_PARAM_READWRITE |
	            G_PARAM_STATIC_STRINGS);
	g_object_class_install_property (gobject_class,
	                 PROP_CLOSE_ON_CLICK,
	                 property_close_on_click);

	property_fade_on_hover = g_param_spec_boolean (
	            "fade-on-hover",
	            "fade-on-hover",
	            "Fade notification bubble when cursor is hovering over it",
	            DEFAULT_FADE_ON_HOVER,
	            G_PARAM_CONSTRUCT |
	            G_PARAM_READWRITE |
	            G_PARAM_STATIC_STRINGS);
	g_object_class_install_property (gobject_class,
	                 PROP_FADE_ON_HOVER,
	                 property_fade_on_hover);

	property_ignore_session_idle_inhibited = g_param_spec_boolean (
	            "ignore-session-idle-inhibited",
	            "ignore-session-idle-inhibited",
	            "Show notifications even if an application is inhibiting system idle.",
	            DEFAULT_IGNORE_SESSION_IDLE_INHIBITED,
	            G_PARAM_CONSTRUCT |
	            G_PARAM_READWRITE |
	            G_PARAM_STATIC_STRINGS);
	g_object_class_install_property (gobject_class,
	                 PROP_IGNORE_SESSION_IDLE_INHIBITED,
	                 property_ignore_session_idle_inhibited);

	property_slot_allocation = g_param_spec_enum (
	            "slot-allocation",
	            "slot-allocation",
	            "Show notifications even if an application is inhibiting system idle.",
	            SLOT_TYPE_ALLOCATION,
	            DEFAULT_SLOT_ALLOCATION,
	            G_PARAM_CONSTRUCT |
	            G_PARAM_READWRITE |
	            G_PARAM_STATIC_STRINGS);
	g_object_class_install_property (gobject_class,
	                 PROP_SLOT_ALLOCATION,
	                 property_slot_allocation);
}

/*-- public API --------------------------------------------------------------*/

Defaults*
defaults_new (void)
{
	Defaults* this = g_object_new (DEFAULTS_TYPE, NULL);

	return this;
}

gint
defaults_get_desktop_width (Defaults* self)
{
	if (!self || !IS_DEFAULTS (self))
		return 0;

	return self->desktop_width;
}

gint
defaults_get_desktop_height (Defaults* self)
{
	if (!self || !IS_DEFAULTS (self))
		return 0;

	return self->desktop_height;
}

gint
defaults_get_desktop_left (Defaults* self)
{
	if (!self || !IS_DEFAULTS (self))
		return 0;

	return self->desktop_left;
}

gdouble
defaults_get_desktop_bottom_gap (Defaults* self)
{
	gdouble bottom_gap;

	if (!self || !IS_DEFAULTS (self))
		return 0.0f;

	g_object_get (self, "desktop-bottom-gap", &bottom_gap, NULL);

	return bottom_gap;
}

gdouble
defaults_get_stack_height (Defaults* self)
{
	gdouble stack_height;

	if (!self || !IS_DEFAULTS (self))
		return 0.0f;

	g_object_get (self, "stack-height", &stack_height, NULL);

	return stack_height;
}

gdouble
defaults_get_bubble_gap (Defaults* self)
{
	gdouble gap;

	if (!self || !IS_DEFAULTS (self))
		return 0.0f;

	g_object_get (self, "bubble-gap", &gap, NULL);

	return gap;
}

gdouble
defaults_get_bubble_width (Defaults* self)
{
	gdouble width;

	if (!self || !IS_DEFAULTS (self))
		return 0.0f;

	g_object_get (self, "bubble-width", &width, NULL);

	return width;
}

gdouble
defaults_get_bubble_min_height (Defaults* self)
{
	gdouble bubble_min_height;

	if (!self || !IS_DEFAULTS (self))
		return 0.0f;

	g_object_get (self, "bubble-min-height", &bubble_min_height, NULL);

	return bubble_min_height;
}

gdouble
defaults_get_bubble_max_height (Defaults* self)
{
	gdouble bubble_max_height;

	if (!self || !IS_DEFAULTS (self))
		return 0.0f;

	g_object_get (self, "bubble-max-height", &bubble_max_height, NULL);

	return bubble_max_height;
}

gdouble
defaults_get_bubble_vert_gap (Defaults* self)
{
	gdouble bubble_vert_gap;

	if (!self || !IS_DEFAULTS (self))
		return 0.0f;

	g_object_get (self, "bubble-vert-gap", &bubble_vert_gap, NULL);

	return bubble_vert_gap;
}

gdouble
defaults_get_bubble_horz_gap (Defaults* self)
{
	gdouble bubble_horz_gap;

	if (!self || !IS_DEFAULTS (self))
		return 0.0f;

	g_object_get (self, "bubble-horz-gap", &bubble_horz_gap, NULL);

	return bubble_horz_gap;
}

gdouble
defaults_get_bubble_height (Defaults* self)
{
	gdouble height;

	if (!self || !IS_DEFAULTS (self))
		return 0.0f;

	g_object_get (self, "bubble-height", &height, NULL);

	return height;
}

gdouble
defaults_get_bubble_shadow_size (Defaults* self, gboolean is_composited)
{
	gdouble bubble_shadow_size;

	if (!self || !IS_DEFAULTS (self) || !is_composited)
		return 0.0f;

	g_object_get (self, "bubble-shadow-size", &bubble_shadow_size, NULL);

	return bubble_shadow_size;
}

gchar*
defaults_get_bubble_shadow_color (Defaults* self)
{
	gchar* bubble_shadow_color = NULL;

	if (!self || !IS_DEFAULTS (self))
		return NULL;

	g_object_get (self,
		      "bubble-shadow-color",
		      &bubble_shadow_color,
		      NULL);

	return bubble_shadow_color;
}

gchar*
defaults_get_bubble_bg_color (Defaults* self)
{
	gchar* bubble_bg_color = NULL;

	if (!self || !IS_DEFAULTS (self))
		return NULL;

	defaults_refresh_bg_color_property (self);

	g_object_get (self,
		      "bubble-bg-color",
		      &bubble_bg_color,
		      NULL);

	return bubble_bg_color;
}

gchar*
defaults_get_bubble_bg_opacity (Defaults* self)
{
	gchar* bubble_bg_opacity = NULL;

	if (!self || !IS_DEFAULTS (self))
		return NULL;

	g_object_get (self,
		      "bubble-bg-opacity",
		      &bubble_bg_opacity,
		      NULL);

	return bubble_bg_opacity;
}

gchar*
defaults_get_bubble_hover_opacity (Defaults* self)
{
	gchar* bubble_hover_opacity = NULL;

	if (!self || !IS_DEFAULTS (self))
		return NULL;

	g_object_get (self,
		      "bubble-hover-opacity",
		      &bubble_hover_opacity,
		      NULL);

	return bubble_hover_opacity;
}

gdouble
defaults_get_bubble_corner_radius (Defaults* self, gboolean is_composited)
{
	gdouble bubble_corner_radius;

	if (!self || !IS_DEFAULTS (self) || !is_composited)
		return 0.0f;

	g_object_get (self,
		      "bubble-corner-radius",
		      &bubble_corner_radius,
		      NULL);

	return bubble_corner_radius;
}

gdouble
defaults_get_content_shadow_size (Defaults* self)
{
	gdouble content_shadow_size;

	if (!self || !IS_DEFAULTS (self))
		return 0.0f;

	g_object_get (self, "content-shadow-size", &content_shadow_size, NULL);

	return content_shadow_size;
}

gchar*
defaults_get_content_shadow_color (Defaults* self)
{
	gchar* content_shadow_color = NULL;

	if (!self || !IS_DEFAULTS (self))
		return NULL;

	g_object_get (self,
		      "content-shadow-color",
		      &content_shadow_color,
		      NULL);

	return content_shadow_color;
}

gdouble
defaults_get_margin_size (Defaults* self)
{
	gdouble margin_size;

	if (!self || !IS_DEFAULTS (self))
		return 0.0f;

	g_object_get (self, "margin-size", &margin_size, NULL);

	return margin_size;
}

gdouble
defaults_get_icon_size (Defaults* self)
{
	gdouble icon_size;

	if (!self || !IS_DEFAULTS (self))
		return 0.0f;

	g_object_get (self, "icon-size", &icon_size, NULL);

	return icon_size;
}

gdouble
defaults_get_gauge_size (Defaults* self)
{
	gdouble gauge_size;

	if (!self || !IS_DEFAULTS (self))
		return 0.0f;

	g_object_get (self, "gauge-size", &gauge_size, NULL);

	return gauge_size;
}

gdouble
defaults_get_gauge_outline_width (Defaults* self)
{
	gdouble gauge_outline_width;

	if (!self || !IS_DEFAULTS (self))
		return 0.0f;

	g_object_get (self, "gauge-outline-width", &gauge_outline_width, NULL);

	return gauge_outline_width;
}

gint
defaults_get_fade_in_timeout (Defaults* self)
{
	gint fade_in_timeout;

	if (!self || !IS_DEFAULTS (self))
		return 0;

	g_object_get (self, "fade-in-timeout", &fade_in_timeout, NULL);

	return fade_in_timeout;
}

gint
defaults_get_fade_out_timeout (Defaults* self)
{
	gint fade_out_timeout;

	if (!self || !IS_DEFAULTS (self))
		return 0;

	g_object_get (self, "fade-out-timeout", &fade_out_timeout, NULL);

	return fade_out_timeout;
}

gint
defaults_get_on_screen_timeout (Defaults* self)
{
	gint on_screen_timeout;

	if (!self || !IS_DEFAULTS (self))
		return 0;

	g_object_get (self, "on-screen-timeout", &on_screen_timeout, NULL);

	return on_screen_timeout;
}

gchar*
defaults_get_text_font_face (Defaults* self)
{
	gchar* text_font_face = NULL;

	if (!self || !IS_DEFAULTS (self))
		return NULL;

	g_object_get (self, "text-font-face", &text_font_face, NULL);

	return text_font_face;
}

gchar*
defaults_get_text_title_color (Defaults* self)
{
	gchar* text_title_color = NULL;

	if (!self || !IS_DEFAULTS (self))
		return NULL;

	g_object_get (self, "text-title-color", &text_title_color, NULL);

	return text_title_color;
}

gint
defaults_get_text_title_weight (Defaults* self)
{
	gint text_title_weight;

	if (!self || !IS_DEFAULTS (self))
		return 0;

	g_object_get (self, "text-title-weight", &text_title_weight, NULL);

	return text_title_weight;
}


gdouble
defaults_get_text_title_size (Defaults* self)
{
	gdouble text_title_size;

	if (!self || !IS_DEFAULTS (self))
		return 0.0f;

	g_object_get (self, "text-title-size", &text_title_size, NULL);

	return text_title_size;
}


gchar*
defaults_get_text_body_color (Defaults* self)
{
	gchar* text_body_color = NULL;

	if (!self || !IS_DEFAULTS (self))
		return NULL;

	g_object_get (self, "text-body-color", &text_body_color, NULL);

	return text_body_color;
}

gint
defaults_get_text_body_weight (Defaults* self)
{
	gint text_body_weight;

	if (!self || !IS_DEFAULTS (self))
		return 0;

	g_object_get (self, "text-body-weight", &text_body_weight, NULL);

	return text_body_weight;
}

gdouble
defaults_get_text_body_size (Defaults* self)
{
	gdouble text_body_size;

	if (!self || !IS_DEFAULTS (self))
		return 0.0f;

	g_object_get (self, "text-body-size", &text_body_size, NULL);

	return text_body_size;
}

/* we use the normal font-height in pixels ("pixels-per-em") as the measurement
 * for 1 em, thus it should _not_ be called before defaults_constructed() */
gdouble
defaults_get_pixel_per_em (Defaults* self)
{
	if (!self || !IS_DEFAULTS (self))
		return 0.0f;

	gdouble pixels_per_em;

	g_object_get (self, "pixels-per-em", &pixels_per_em, NULL);

	return pixels_per_em;
}

gdouble
defaults_get_system_font_size (Defaults* self)
{
	if (!self || !IS_DEFAULTS (self))
		return 0.0f;

	gdouble system_font_size;

	g_object_get (self, "system-font-size", &system_font_size, NULL);

	return system_font_size;
}

gdouble
defaults_get_screen_dpi (Defaults* self)
{
	if (!self || !IS_DEFAULTS (self))
		return 0.0f;

	gdouble screen_dpi;

	g_object_get (self, "screen-dpi", &screen_dpi, NULL);

	return screen_dpi;
}

GStrv
defaults_get_fake_action_ignore_list (Defaults *self)
{
	return g_settings_get_strv (self->nosd_settings,
	                GSETTINGS_IGNORE_LIST_KEY);
}

static gboolean
defaults_multihead_does_focus_follow (Defaults *self)
{
	gboolean mode = FALSE;

	g_return_val_if_fail (self != NULL && IS_DEFAULTS (self), FALSE);

	gchar* mode_str = g_settings_get_string (self->nosd_settings,
											 GSETTINGS_MULTIHEAD_MODE_KEY);

	if (mode_str != NULL)
	{
		if (! g_strcmp0 (mode_str, "focus-follow"))
			mode = TRUE;

		g_free ((gpointer) mode_str);
	}
	
	return mode;
}

void
defaults_get_top_corner (Defaults *self, GdkScreen **screen, gint *x, gint *y)
{
	GdkRectangle rect;
	GdkWindow*   active_window    = NULL;
	GdkDeviceManager* device_manager;
	GdkDevice*   device;
	gint         mx;
	gint         my;
	gint         monitor          = 0;
	gint         aw_monitor;
	gboolean     follow_focus     = defaults_multihead_does_focus_follow (self);
	gboolean     is_composited    = FALSE;
	gint         primary_monitor;

	g_return_if_fail (self != NULL && IS_DEFAULTS (self));

	device_manager = gdk_display_get_device_manager (gdk_display_get_default ());
	device = gdk_device_manager_get_client_pointer (device_manager);
	gdk_device_get_position (device, screen, &mx, &my);

	is_composited = gdk_screen_is_composited (*screen);

	if (follow_focus)
	{
		g_debug ("multi_head_focus_follow mode");
		monitor = gdk_screen_get_monitor_at_point (*screen, mx, my);
		active_window = gdk_screen_get_active_window (*screen);
		if (active_window != NULL)
		{
			aw_monitor = gdk_screen_get_monitor_at_window (
					*screen,
			                active_window);

			if (monitor != aw_monitor)
			{
				g_debug ("choosing the monitor with the active"
				         " window, not the one with the mouse"
				         " cursor");
			}

			monitor = aw_monitor;

			g_object_unref (active_window);
		}
	}

	/* _NET_WORKAREA is always a rectangle spanning all monitors of
	 * a screen. As such, it can't properly deal with monitor setups
	 * that aren't aligned or have different resolutions.
	 * gdk_screen_get_monitor_workarea() works around this by only
	 * returning the workarea for the primary screen and the full
	 * geometry for all other monitors.
	 *
	 * This leads to the sync bubbles sometimes overlapping the
	 * panel on secondary monitors. To work around this, we get the
	 * panel's height on the primary monitor and use that for all
	 * other monitors as well.
	 */

	primary_monitor = gdk_screen_get_primary_monitor (*screen);
	if (monitor == primary_monitor)
	{
		gdk_screen_get_monitor_workarea (*screen, primary_monitor, &rect);
	}
	else
	{
		GdkRectangle workarea;
		GdkRectangle primary_geom;
		gint panel_height;

		gdk_screen_get_monitor_workarea (*screen, primary_monitor, &workarea);
		gdk_screen_get_monitor_geometry (*screen, primary_monitor, &primary_geom);
		panel_height = workarea.y - primary_geom.y;

		gdk_screen_get_monitor_geometry (*screen, monitor, &rect);
		rect.y += panel_height;
		rect.height -= panel_height;
	}

	g_debug ("selecting monitor %d at (%d,%d) - %dx%d",
		 monitor,
	         rect.x,
	         rect.y,
	         rect.width,
	         rect.height);

	self->desktop_width = rect.width;
	self->desktop_height = rect.height;
	self->desktop_left = rect.x;

	*y   = rect.y;
	*y  += EM2PIXELS (defaults_get_bubble_vert_gap (self), self)
	       - EM2PIXELS (defaults_get_bubble_shadow_size (self, is_composited), self);

	if (gtk_widget_get_default_direction () == GTK_TEXT_DIR_LTR)
	{
		*x = rect.x + rect.width;
		*x -= EM2PIXELS (defaults_get_bubble_shadow_size (self, is_composited), self)
			+ EM2PIXELS (defaults_get_bubble_horz_gap (self), self)
			+ EM2PIXELS (defaults_get_bubble_width (self), self);
	} else {
		*x = rect.x
			- EM2PIXELS (defaults_get_bubble_shadow_size (self, is_composited), self)
			+ EM2PIXELS (defaults_get_bubble_horz_gap (self), self);
	}

	g_debug ("top corner at: %d, %d", *x, *y);
}

Gravity
defaults_get_gravity (Defaults* self)
{
	if (!self || !IS_DEFAULTS (self))
		return GRAVITY_NONE;

	Gravity gravity;

	g_object_get (self, "gravity", &gravity, NULL);

	return gravity;
}

SlotAllocation
defaults_get_slot_allocation (Defaults *self)
{
	if (!self || !IS_DEFAULTS (self))
		return SLOT_ALLOCATION_NONE;

	SlotAllocation slot_allocation;

	g_object_get (self, "slot-allocation", &slot_allocation, NULL);

	return slot_allocation;
}

gboolean
defaults_get_close_on_click (Defaults* self)
{
	if (!self || !IS_DEFAULTS (self))
		return DEFAULT_CLOSE_ON_CLICK;

	gboolean close_on_click;

	g_object_get (self, "close-on-click", &close_on_click, NULL);

	return close_on_click;
}

gboolean
defaults_get_fade_on_hover (Defaults* self)
{
	if (!self || !IS_DEFAULTS (self))
		return DEFAULT_FADE_ON_HOVER;

	gboolean fade_on_hover;

	g_object_get (self, "fade-on-hover", &fade_on_hover, NULL);

	return fade_on_hover;
}

gboolean
defaults_get_ignore_session_idle_inhibited (Defaults* self)
{
	if (!self || !IS_DEFAULTS (self))
		return DEFAULT_IGNORE_SESSION_IDLE_INHIBITED;

	gboolean ignore_session_idle_inhibited;

	g_object_get (self, "ignore-session-idle-inhibited", &ignore_session_idle_inhibited, NULL);

	return ignore_session_idle_inhibited;
}
