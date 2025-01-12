////////////////////////////////////////////////////////////////////////////////
//3456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789
//      10        20        30        40        50        60        70        80
//
// notify-osd
//
// notification.c - notification object storing attributes like title- and body-
//                  text, value, icon, id, sender-pid etc.
//
// Copyright 2009 Canonical Ltd.
//
// Authors:
//    Mirco "MacSlow" Mueller <mirco.mueller@canonical.com>
//
// This program is free software: you can redistribute it and/or modify it
// under the terms of the GNU General Public License version 3, as published
// by the Free Software Foundation.
//
// This program is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranties of
// MERCHANTABILITY, SATISFACTORY QUALITY, or FITNESS FOR A PARTICULAR
// PURPOSE.  See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with this program.  If not, see <http://www.gnu.org/licenses/>.
//
////////////////////////////////////////////////////////////////////////////////

#include "notification.h"

enum
{
	PROP_DUMMY = 0,
	PROP_ID,
	PROP_TITLE,
	PROP_BODY,
	PROP_VALUE,
	PROP_ICON_THEMENAME,
	PROP_ICON_FILENAME,
	PROP_ICON_PIXBUF,
	PROP_ONSCREEN_TIME,
	PROP_SENDER_NAME,
	PROP_SENDER_PID,
	PROP_TIMESTAMP,
	PROP_URGENCY
};

struct _NotificationPrivate {
	gint       id;                  // unique notification-id
	GString*   title;               // summary-text strdup'ed from setter
	GString*   body;                // body-text strdup'ed from setter
	gint       value;               // range: 0..100, special: -1, 101
	GString*   icon_themename;      // e.g. "notification-message-email"
	GString*   icon_filename;       // e.g. "/usr/share/icons/icon.png"
	GdkPixbuf* icon_pixbuf;         // from setter memcpy'ed pixbuf 
	gint       onscreen_time;       // time on-screen in ms
	GString*   sender_name;         // app-name, strdup'ed from setter
	gint       sender_pid;          // pid of sending application
	GTimeVal   timestamp;           // timestamp of  reception
	Urgency    urgency;             // urgency-level: low, normal, high
};

G_DEFINE_TYPE_WITH_PRIVATE (Notification, notification, G_TYPE_OBJECT);

//-- private functions ---------------------------------------------------------

//-- internal functions --------------------------------------------------------

// this is what gets called if one does g_object_unref(bla)
static void
notification_dispose (GObject* gobject)
{
	Notification*        n;
	NotificationPrivate* priv;

	// sanity checks
	g_assert (gobject);
	n = NOTIFICATION (gobject);
	g_assert (n);
	g_assert (IS_NOTIFICATION (n));
	priv = notification_get_instance_private (n);
	g_assert (priv);

	// free any allocated resources
	if (priv->title)
	{
		g_string_free (priv->title, TRUE);
		priv->title = NULL;
	}

	if (priv->body)
	{
		g_string_free (priv->body, TRUE);
		priv->body = NULL;
	}

	if (priv->icon_themename)
	{
		g_string_free (priv->icon_themename, TRUE);
		priv->icon_themename = NULL;
	}

	if (priv->icon_filename)
	{
		g_string_free (priv->icon_filename, TRUE);
		priv->icon_filename = NULL;
	}

	if (priv->icon_pixbuf)
	{
		g_object_unref (priv->icon_pixbuf);
	}

	if (priv->sender_name)
	{
		g_string_free (priv->sender_name, TRUE);
		priv->sender_name = NULL;
	}

	// chain up to the parent class
	G_OBJECT_CLASS (notification_parent_class)->dispose (gobject);
}

static void
notification_finalize (GObject* gobject)
{
	// chain up to the parent class
	G_OBJECT_CLASS (notification_parent_class)->finalize (gobject);
}

static void
notification_init (Notification* n)
{
	// nothing to be done here for now
}

static void
notification_get_property (GObject*    gobject,
			   guint       prop,
			   GValue*     value,
			   GParamSpec* spec)
{
	Notification* n = NOTIFICATION (gobject);
	NotificationPrivate* priv = notification_get_instance_private (n);

	// sanity checks are done in public getters

	switch (prop)
	{
		case PROP_ID:
		    g_value_set_int (value, priv->id);
		break;

		case PROP_TITLE:
		    g_value_set_string (value, priv->title->str);
		break;

		case PROP_BODY:
		    g_value_set_string (value, priv->body->str);
		break;

		case PROP_VALUE:
		    g_value_set_int (value, priv->value);
		break;

		case PROP_ICON_THEMENAME:
		    g_value_set_string (value, priv->icon_themename->str);
		break;

		case PROP_ICON_FILENAME:
		    g_value_set_string (value, priv->icon_filename->str);
		break;

		case PROP_ICON_PIXBUF:
		    g_value_set_pointer (value, priv->icon_pixbuf);
		break;

		case PROP_ONSCREEN_TIME:
		    g_value_set_int (value, priv->onscreen_time);
		break;

		case PROP_SENDER_NAME:
		    g_value_set_string (value, priv->sender_name->str);
		break;

		case PROP_SENDER_PID:
		    g_value_set_int (value, priv->sender_pid);
		break;

		case PROP_TIMESTAMP:
		    g_value_set_pointer (value, &priv->timestamp);
		break;

		case PROP_URGENCY:
		    g_value_set_int (value, priv->urgency);
		break;

		default :
			G_OBJECT_WARN_INVALID_PROPERTY_ID (gobject, prop, spec);
		break;
	}
}

static void
notification_set_property (GObject*      gobject,
			   guint         prop,
			   const GValue* value,
			   GParamSpec*   spec)
{
	Notification* n = NOTIFICATION (gobject);
	NotificationPrivate* priv = notification_get_instance_private (n);

	// sanity checks are done in the public setters

	switch (prop)
	{
		case PROP_ID:
		    priv->id = g_value_get_int (value);
			if (priv->id < -1)
				priv->id = -1;
		break;

		case PROP_TITLE:
		    if(priv->title)
				g_string_free(priv->title, TRUE);
			priv->title = g_string_new(g_value_get_string (value));
		break;

		case PROP_BODY:
		    if(priv->body)
				g_string_free(priv->body, TRUE);
			priv->body = g_string_new(g_value_get_string (value));
		break;

		case PROP_VALUE:
		    // the allowed range for stored values is -1..101, thus a return-value of -2
		    // indicates an error on behalf of the caller
		    // valid range is -1..101, -2 is used to indicate it's "unset"
		    priv->value = g_value_get_int (value);

			G_STATIC_ASSERT(NOTIFICATION_VALUE_MIN_ALLOWED - 1 == -2);

			if (priv->value < NOTIFICATION_VALUE_MIN_ALLOWED - 1)
				priv->value = NOTIFICATION_VALUE_MIN_ALLOWED;
			else if (priv->value > NOTIFICATION_VALUE_MAX_ALLOWED)
				priv->value = NOTIFICATION_VALUE_MAX_ALLOWED;
		break;

		case PROP_ICON_THEMENAME:
		    if(priv->icon_themename)
				g_string_free(priv->icon_themename, TRUE);
			priv->icon_themename = g_string_new(g_value_get_string (value));
		break;

		case PROP_ICON_FILENAME:
		    if(priv->icon_filename)
				g_string_free(priv->icon_filename, TRUE);
			priv->icon_filename = g_string_new(g_value_get_string (value));
		break;

		case PROP_ICON_PIXBUF:
		    if(priv->icon_pixbuf)
				g_object_unref (priv->icon_pixbuf);
			// create a new/private copy of the supplied pixbuf
			priv->icon_pixbuf = gdk_pixbuf_copy (g_value_get_pointer (value));
		break;

		case PROP_ONSCREEN_TIME:
	    {
		    gint onscreen_time = g_value_get_int (value);
			// FIXME: -1?????
			// a return-value of -1 indicates an error on behalf of the caller, a
			// return-value of 0 would indicate that a notification has not been displayed
			// yet

			// onscreen-time is non negative and can only increase, not decrease
			if (onscreen_time >= 0 && onscreen_time > priv->onscreen_time)
				priv->onscreen_time = onscreen_time;
	    }
		break;

		case PROP_SENDER_NAME:
		    if(priv->sender_name)
				g_string_free(priv->sender_name, TRUE);
			priv->sender_name = g_string_new(g_value_get_string (value));
		break;

		case PROP_SENDER_PID:
	    {
		    gint sender_pid = g_value_get_int (value);
			// it's hardly possible we'll get a notification from init, but anyway
			if (sender_pid > 0)
				priv->sender_pid = sender_pid;
	    }
		break;

		case PROP_TIMESTAMP:
	    {
		    GTimeVal* timestamp = g_value_get_pointer (value);
			// don't store older timestamp over newer one
			if (timestamp->tv_sec > priv->timestamp.tv_sec
			        || (timestamp->tv_sec == priv->timestamp.tv_sec
			            && timestamp->tv_usec > priv->timestamp.tv_usec))
			{
				priv->timestamp.tv_sec  = timestamp->tv_sec;
				priv->timestamp.tv_usec = timestamp->tv_usec;
			}
	    }
		break;

		case PROP_URGENCY:
	    {
		    gint urgency = g_value_get_int (value);

			// URGENCY_NONE is meant to indicate the "unset" state
			// also, don't store any values outside of allowed range LOW..HIGH
			if (urgency == URGENCY_LOW    ||
			    urgency == URGENCY_NORMAL ||
			    urgency == URGENCY_HIGH   ||
			    urgency == URGENCY_NONE)
				priv->urgency = urgency;
	    }
		break;

		default :
			G_OBJECT_WARN_INVALID_PROPERTY_ID (gobject, prop, spec);
		break;
	}
}

static void
notification_class_init (NotificationClass* klass)
{
	GObjectClass* gobject_class = G_OBJECT_CLASS (klass);
	GParamSpec*   property_id;
	GParamSpec*   property_title;
	GParamSpec*   property_body;
	GParamSpec*   property_value;
	GParamSpec*   property_icon_themename;
	GParamSpec*   property_icon_filename;
	GParamSpec*   property_icon_pixbuf;
	GParamSpec*   property_onscreen_time;
	GParamSpec*   property_sender_name;
	GParamSpec*   property_sender_pid;
	GParamSpec*   property_timestamp;
	GParamSpec*   property_urgency;

	gobject_class->dispose      = notification_dispose;
	gobject_class->finalize     = notification_finalize;
	gobject_class->get_property = notification_get_property;
	gobject_class->set_property = notification_set_property;

	property_id = g_param_spec_int ("id",
					"id",
					"unique notification id",
					-1,
					G_MAXINT,
					-1,
					G_PARAM_CONSTRUCT |
					G_PARAM_READWRITE);
	g_object_class_install_property (gobject_class,
					 PROP_ID,
					 property_id);

	property_title = g_param_spec_string ("title",
					      "title",
					      "title-text of a notification",
					      NULL,
					      G_PARAM_CONSTRUCT |
					      G_PARAM_READWRITE);
	g_object_class_install_property (gobject_class,
					 PROP_TITLE,
					 property_title);

	property_body = g_param_spec_string ("body",
					     "body",
					     "body-text of a notification",
					     NULL,
					     G_PARAM_CONSTRUCT |
					     G_PARAM_READWRITE);
	g_object_class_install_property (gobject_class,
					 PROP_BODY,
					 property_body);

	property_value = g_param_spec_int ("value",
					   "value",
					   "value between -1..101 to render",
					   -2,
					   101,
					   -2,
					   G_PARAM_CONSTRUCT |
					   G_PARAM_READWRITE);
	g_object_class_install_property (gobject_class,
					 PROP_VALUE,
					 property_value);

	property_icon_themename = g_param_spec_string (
					"icon-themename",
					"icon-themename",
					"theme-name of icon to use",
					NULL,
					G_PARAM_CONSTRUCT |
					G_PARAM_READWRITE);
	g_object_class_install_property (gobject_class,
					 PROP_ICON_THEMENAME,
					 property_icon_themename);

	property_icon_filename = g_param_spec_string (
					"icon-filename",
					"icon-filename",
					"file-name of icon to use",
					NULL,
					G_PARAM_CONSTRUCT |
					G_PARAM_READWRITE);
	g_object_class_install_property (gobject_class,
					 PROP_ICON_FILENAME,
					 property_icon_filename);

	property_icon_pixbuf = g_param_spec_pointer ("icon-pixbuf",
						     "icon-pixbuf",
						     "pixbuf of icon to use",
						     G_PARAM_CONSTRUCT |
						     G_PARAM_READWRITE);
	g_object_class_install_property (gobject_class,
					 PROP_ICON_PIXBUF,
					 property_icon_pixbuf);

	property_onscreen_time = g_param_spec_int ("onscreen-time",
						   "onscreen-time",
						   "time on screen sofar",
						   0,
						   G_MAXINT,
						   0,
						   G_PARAM_CONSTRUCT |
						   G_PARAM_READWRITE);
	g_object_class_install_property (gobject_class,
					 PROP_ONSCREEN_TIME,
					 property_onscreen_time);

	property_sender_name = g_param_spec_string (
					"sender-name",
					"sender-name",
					"name of sending application",
					NULL,
					G_PARAM_CONSTRUCT |
					G_PARAM_READWRITE);
	g_object_class_install_property (gobject_class,
					 PROP_SENDER_NAME,
					 property_sender_name);

	property_sender_pid = g_param_spec_int (
					"sender-pid",
					"sender-pid",
					"process ID of sending application",
					0,
					G_MAXSHORT,
					0,
					G_PARAM_CONSTRUCT |
					G_PARAM_READWRITE);
	g_object_class_install_property (gobject_class,
					 PROP_SENDER_PID,
					 property_sender_pid);

	property_timestamp = g_param_spec_pointer ("timestamp",
						   "timestamp",
						   "timestamp of reception",
					 	   G_PARAM_CONSTRUCT |
						   G_PARAM_READWRITE);
	g_object_class_install_property (gobject_class,
					 PROP_TIMESTAMP,
					 property_timestamp);

	property_urgency = g_param_spec_int ("urgency",
					     "urgency",
					     "urgency-level of notification",
					     URGENCY_LOW,
					     URGENCY_NONE,
					     URGENCY_NONE,
					     G_PARAM_CONSTRUCT |
					     G_PARAM_READWRITE);
	g_object_class_install_property (gobject_class,
					 PROP_URGENCY,
					 property_urgency);
}

//-- public functions ----------------------------------------------------------

Notification*
notification_new ()
{
	const GTimeVal timestamp = {0};

#warning "notification_new will fail"
	// FIXME: Create a GdkPixbuf here (or somewhere)
	// otherwise the constructor fails.
	return g_object_new (NOTIFICATION_TYPE,
	                     "timestamp", &timestamp,
	                     NULL);
}
