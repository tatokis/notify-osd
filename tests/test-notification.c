////////////////////////////////////////////////////////////////////////////////
//3456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789
//      10        20        30        40        50        60        70        80
//
// notify-osd
//
// test-notification.c - implements unit-tests for exercising API of abstract
//                       notification object
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

#define _XOPEN_SOURCE 500 // needed for usleep() from unistd.h
#include <unistd.h>

#include "notification.h"

static void
test_notification_new (gpointer fixture, gconstpointer user_data)
{
	Notification* n = NULL;

	// create new object
	n = notification_new ();

	// test validity of main notification object
	g_assert (n);

	// test validity of initialized notification object
	gint notification_id, notification_value, notification_onscreen_time, notification_sender_pid, notification_urgency;
	gchar *notification_title, *notification_body, *notification_icon_themename, *notification_icon_filename, *notification_sender_name;
	GdkPixbuf* notification_icon_pixbuf = NULL;
	GTimeVal* notification_timestamp = NULL;
	g_object_get (n,
	             "id", &notification_id,
	             "title", &notification_title,
	             "body", &notification_body,
	             "value", &notification_value,
	             "icon-themename", &notification_icon_themename,
	             "icon-filename", &notification_icon_filename,
	             "icon-pixbuf", &notification_icon_pixbuf,
	             "onscreen-time", &notification_onscreen_time,
	             "sender-name", &notification_sender_name,
	             "sender-pid", &notification_sender_pid,
	             "timestamp", &notification_timestamp,
	             "urgency", &notification_urgency,
	             NULL);
	g_assert_cmpint (notification_id, ==, -1);
	g_assert (!notification_title);
	g_assert (!notification_body);
	g_assert_cmpint (notification_value, ==, -2);
	g_assert (!notification_icon_themename);
	g_assert (!notification_icon_filename);
	g_assert (!notification_icon_pixbuf);
	g_assert_cmpint (notification_onscreen_time, ==, 0);
	g_assert (!notification_sender_name);
	g_assert_cmpint (notification_sender_pid, ==, 0);
	g_assert_cmpint (notification_timestamp->tv_sec, ==, 0);
	g_assert_cmpint (notification_timestamp->tv_usec, ==, 0);
	g_assert_cmpint (notification_urgency, ==, URGENCY_NONE);

	// clean up
	g_free (notification_title);
	g_free (notification_body);
	g_free (notification_icon_themename);
	g_free (notification_icon_filename);
	g_free (notification_sender_name);
	g_object_unref (n);
	n = NULL;
}

static void
test_notification_destroy (gpointer fixture, gconstpointer user_data)
{
	Notification* n = NULL;

	// create new object
	n = notification_new ();

	// test validity of main notification object
	g_assert (n != NULL);

	// clean up
	g_object_unref (n);
	n = NULL;
}

static void
test_notification_setget_id (gpointer fixture, gconstpointer user_data)
{
	Notification* n = NULL;
	gint notification_id = -3;

	// create new object
	n = notification_new ();

	// if nothing has been set yet it should return -1
	g_object_get (n, "id", &notification_id, NULL);
	g_assert_cmpint (notification_id, ==, -1);

	// a negative id should not be stored
	g_object_set (n, "id", -3, NULL);
	g_object_get (n, "id", &notification_id, NULL);
	g_assert_cmpint (notification_id, >=, -1);

	// smallest possible id is 0
	g_object_set (n, "id", 0, NULL);
	g_object_get (n, "id", &notification_id, NULL);
	g_assert_cmpint (notification_id, ==, 0);

	// largest possible id is G_MAXINT
	g_object_set (n, "id", G_MAXINT, NULL);
	g_object_get (n, "id", &notification_id, NULL);
	g_assert_cmpint (notification_id, ==, G_MAXINT);

	// clean up
	g_object_unref (n);
	n = NULL;
}

static void
test_notification_setget_title (gpointer fixture, gconstpointer user_data)
{
	Notification* n = NULL;
	gchar* notification_title = "";

	// create new object
	n = notification_new ();

	// if no title has been set yet it should return NULL
	g_object_get (n, "title", &notification_title, NULL);
	g_assert (notification_title == NULL);
	g_free (notification_title);

	// set an initial title-text and verify it
	g_object_set (n, "title", "Some title text", NULL);
	g_object_get (n, "title", &notification_title, NULL);
	g_assert_cmpstr (notification_title, ==, "Some title text");
	g_free (notification_title);

	// set a new title-text and verify it
	g_object_set (n, "title", "The new summary", NULL);
	g_object_get (n, "title", &notification_title, NULL);
	g_assert_cmpstr (notification_title, !=, "Some title text");
	g_assert_cmpstr (notification_title, ==, "The new summary");
	g_free (notification_title);

	// clean up
	g_object_unref (n);
	n = NULL;
}

static void
test_notification_setget_body (gpointer fixture, gconstpointer user_data)
{
	Notification* n = NULL;
	gchar* notification_body = "";

	// create new object
	n = notification_new ();

	// if no body has been set yet it should return NULL
	g_object_get (n, "body", &notification_body, NULL);
	g_assert (notification_body == NULL);
	g_free (notification_body);

	// set an initial body-text and verify it
	g_object_set (n, "body", "Example body text", NULL);
	g_object_get (n, "body", &notification_body, NULL);
	g_assert_cmpstr (notification_body, ==, "Example body text");
	g_free (notification_body);

	// set a new body-text and verify it
	g_object_set (n, "body", "Some new body text", NULL);
	g_object_get (n, "body", &notification_body, NULL);
	g_assert_cmpstr (notification_body, !=, "Example body text");
	g_assert_cmpstr (notification_body, ==, "Some new body text");
	g_free (notification_body);

	// clean up
	g_object_unref (n);
	n = NULL;
}

static void
test_notification_setget_value (gpointer fixture, gconstpointer user_data)
{
	Notification* n = NULL;
	gint notification_value = -10;

	// create new object
	n = notification_new ();

	// if no value has been set yet it should return 0
	g_object_get (n, "value", &notification_value, NULL);
	g_assert_cmpint (notification_value, ==, -2);

	// set an initial value and verify it
	g_object_set (n, "value", 25, NULL);
	g_object_get (n, "value", &notification_value, NULL);
	g_assert_cmpint (notification_value, ==, 25);

	// set a new value and verify it
	g_object_set (n, "value", 45, NULL);
	g_object_get (n, "value", &notification_value, NULL);
	g_assert_cmpint (notification_value, !=, 25);
	g_assert_cmpint (notification_value, ==, 45);

	// test allowed range
	g_object_set (n, "value", NOTIFICATION_VALUE_MAX_ALLOWED + 1, NULL);
	g_object_get (n, "value", &notification_value, NULL);
	g_assert_cmpint (notification_value,
			 ==,
			 NOTIFICATION_VALUE_MAX_ALLOWED);
	g_object_set (n, "value", NOTIFICATION_VALUE_MIN_ALLOWED - 2, NULL);
	g_object_get (n, "value", &notification_value, NULL);
	g_assert_cmpint (notification_value,
			 ==,
			 NOTIFICATION_VALUE_MIN_ALLOWED);

	// clean up
	g_object_unref (n);
	n = NULL;
}

static void
test_notification_setget_icon_themename (gpointer fixture, gconstpointer user_data)
{
	Notification* n = NULL;
	gchar* notification_icon_themename = "";

	// create new object
	n = notification_new ();

	// if no icon-themename has been set yet it should return NULL
	g_object_get (n, "icon-themename", &notification_icon_themename, NULL);
	g_assert (notification_icon_themename == NULL);
	g_free (notification_icon_themename);

	// set an initial icon-themename and verify it
	g_object_set (n, "icon-themename", "notification-message-im", NULL);
	g_object_get (n, "icon-themename", &notification_icon_themename, NULL);
	g_assert_cmpstr (notification_icon_themename,
			 ==,
			 "notification-message-im");
	g_free (notification_icon_themename);

	// set a new icon-themename and verify it
	g_object_set (n, "icon-themename", "notification-device-usb", NULL);
	g_object_get (n, "icon-themename", &notification_icon_themename, NULL);
	g_assert_cmpstr (notification_icon_themename,
			 !=,
			 "notification-message-im");
	g_assert_cmpstr (notification_icon_themename,
			 ==,
			 "notification-device-usb");
	g_free (notification_icon_themename);

	// clean up
	g_object_unref (n);
	n = NULL;
}

static void
test_notification_setget_icon_filename (gpointer fixture, gconstpointer user_data)
{
	Notification* n = NULL;
	gchar* notification_icon_filename = "";

	// create new object
	n = notification_new ();

	// if no icon-filename has been set yet it should return NULL
	g_object_get (n, "icon-filename", &notification_icon_filename, NULL);
	g_assert (notification_icon_filename == NULL);
	g_free (notification_icon_filename);

	// set an initial icon-filename and verify it
	g_object_set (n, "icon-filename", "/usr/share/icon/photo.png", NULL);
	g_object_get (n, "icon-filename", &notification_icon_filename, NULL);
	g_assert_cmpstr (notification_icon_filename,
			 ==,
			 "/usr/share/icon/photo.png");
	g_free (notification_icon_filename);

	// set a new icon-filename and verify it
	g_object_set (n, "icon-filename", "/tmp/drawing.svg", NULL);
	g_object_get (n, "icon-filename", &notification_icon_filename, NULL);
	g_assert_cmpstr (notification_icon_filename,
			 !=,
			 "/usr/share/icon/photo.png");
	g_assert_cmpstr (notification_icon_filename,
			 ==,
			 "/tmp/drawing.svg");
	g_free (notification_icon_filename);

	// passing an invalid/NULL pointer should not change the stored
	// icon-filename
	g_object_set (n, "icon-filename", NULL, NULL);
	g_object_get (n, "icon-filename", &notification_icon_filename, NULL);
	g_assert_cmpstr (notification_icon_filename,
			 ==,
			 "/tmp/drawing.svg");
	g_free (notification_icon_filename);

	// pass empty (but not NULL) strings
	g_object_set (n, "icon-filename", "", NULL);
	g_object_get (n, "icon-filename", &notification_icon_filename, NULL);
	g_assert_cmpstr (notification_icon_filename,
			 ==,
			 "");
	g_free (notification_icon_filename);
	g_object_set (n, "icon-filename", "\0", NULL);
	g_object_get (n, "icon-filename", &notification_icon_filename, NULL);
	g_assert_cmpstr (notification_icon_filename,
			 ==,
			 "\0");
	g_free (notification_icon_filename);

	// clean up
	g_object_unref (n);
	n = NULL;
}

static void
test_notification_setget_icon_pixbuf (gpointer fixture, gconstpointer user_data)
{
	Notification* n      = NULL;
	GdkPixbuf*    pixbuf = NULL;
	GdkPixbuf*    notification_icon_pixbuf = NULL;

	// create new object
	n = notification_new ();

	// create pixbuf for testing
	notification_icon_pixbuf = pixbuf =
	        gdk_pixbuf_new (GDK_COLORSPACE_RGB, TRUE, 8, 100, 100);

	g_assert (pixbuf);

	// if no icon-pixbuf has been set yet it should return NULL
	g_object_get (n, "icon-pixbuf", &notification_icon_pixbuf, NULL);
	g_assert (notification_icon_pixbuf == NULL);

	// set an initial icon-pixbuf and verify it
	g_object_set (n, "icon-pixbuf", pixbuf, NULL);
	g_object_get (n, "icon-pixbuf", &notification_icon_pixbuf, NULL);
	g_assert (notification_icon_pixbuf != NULL);

	// passing a NULL pointer should not change the stored
	// icon-pixbuf
	g_object_set (n, "icon-pixbuf", NULL, NULL);
	g_object_get (n, "icon-pixbuf", &notification_icon_pixbuf, NULL);
	g_assert (notification_icon_pixbuf != NULL);

	// clean up
	g_object_unref (n);
	n = NULL;

	// more clean up
	g_object_unref (pixbuf);
	pixbuf = NULL;
}

static void
test_notification_setget_onscreen_time (gpointer fixture, gconstpointer user_data)
{
	Notification* n = NULL;
	gint notification_onscreen_time	= -10;

	// create new object
	n = notification_new ();

	// if no onscreen-time has been set yet it should return 0
	g_object_get (n, "onscreen-time", &notification_onscreen_time, NULL);
	g_assert_cmpint (notification_onscreen_time, ==, 0);

	// setting a negative onscreen-time should fail
	g_object_set (n, "onscreen-time", -1, NULL);
	g_object_get (n, "onscreen-time", &notification_onscreen_time, NULL);
	g_assert_cmpint (notification_onscreen_time, ==, 0);

	// set an positive onscreen-time and verify it
	g_object_set (n, "onscreen-time", 1000, NULL);
	g_object_get (n, "onscreen-time", &notification_onscreen_time, NULL);
	g_assert_cmpint (notification_onscreen_time, ==, 1000);

	// set a new onscreen-time and verify it
	g_object_set (n, "onscreen-time", 5000, NULL);
	g_object_get (n, "onscreen-time", &notification_onscreen_time, NULL);
	g_assert_cmpint (notification_onscreen_time, !=, 1000);
	g_assert_cmpint (notification_onscreen_time, ==, 5000);

	// setting a new onscreen-time smaller than the currently stored one
	// should fail
	g_object_set (n, "onscreen-time", 4000, NULL);
	g_object_get (n, "onscreen-time", &notification_onscreen_time, NULL);
	g_assert_cmpint (notification_onscreen_time, ==, 5000);

	// clean up
	g_object_unref (n);
	n = NULL;
}

static void
test_notification_setget_sender_name (gpointer fixture, gconstpointer user_data)
{
	Notification* n = NULL;
	gchar* notification_sender_name = "";

	// create new object
	n = notification_new ();

	// if no sender-name has been set yet it should return NULL
	g_object_get (n, "sender-name", &notification_sender_name, NULL);
	g_assert (notification_sender_name == NULL);
	g_free (notification_sender_name);

	// set an initial sender-name and verify it
	g_object_set (n, "sender-name", "evolution", NULL);
	g_object_get (n, "sender-name", &notification_sender_name, NULL);
	g_assert_cmpstr (notification_sender_name, ==, "evolution");
	g_free (notification_sender_name);

	// set a new sender-name and verify it
	g_object_set (n, "sender-name", "pidgin", NULL);
	g_object_get (n, "sender-name", &notification_sender_name, NULL);
	g_assert_cmpstr (notification_sender_name, !=, "evolution");
	g_assert_cmpstr (notification_sender_name, ==, "pidgin");
	g_free (notification_sender_name);

	// passing an invalid/NULL pointer should not change the stored
	// sender-name 
	g_object_set (n, "sender-name", NULL, NULL);
	g_object_get (n, "sender-name", &notification_sender_name, NULL);
	g_assert_cmpstr (notification_sender_name, ==, "pidgin");
	g_free (notification_sender_name);

	// pass an empty (but not NULL) strings
	g_object_set (n, "sender-name", "", NULL);
	g_object_get (n, "sender-name", &notification_sender_name, NULL);
	g_assert_cmpstr (notification_sender_name, ==, "");
	g_free (notification_sender_name);
	g_object_set (n, "sender-name", "\0", NULL);
	g_object_get (n, "sender-name", &notification_sender_name, NULL);
	g_assert_cmpstr (notification_sender_name, ==, "\0");
	g_free (notification_sender_name);

	// clean up
	g_object_unref (n);
	n = NULL;
}

static void
test_notification_setget_sender_pid (gpointer fixture, gconstpointer user_data)
{
	Notification* n = NULL;
	gint notification_sender_pid = -10;

	// create new object
	n = notification_new ();

	// if nothing has been set yet it should return 0
	g_object_get (n, "sender-pid", &notification_sender_pid, NULL);
	g_assert_cmpint (notification_sender_pid, ==, 0);

	// a negative pid makes no sense and should therefore not be stored
	g_object_set (n, "sender-pid", -1, NULL);
	g_object_get (n, "sender-pid", &notification_sender_pid, NULL);
	g_assert_cmpint (notification_sender_pid, ==, 0);

	// smallest possible pid is 1
	g_object_set (n, "sender-pid", 1, NULL);
	g_object_get (n, "sender-pid", &notification_sender_pid, NULL);
	g_assert_cmpint (notification_sender_pid, ==, 1);

	// largest possible id is G_MAXINT
	g_object_set (n, "sender-pid", G_MAXINT, NULL);
	g_object_get (n, "sender-pid", &notification_sender_pid, NULL);
	g_assert_cmpint (notification_sender_pid, ==, G_MAXINT);

	// a pid of 0 would mean something before the init process is sending us
	// a notification, leave the stored pid untouched
	g_object_set (n, "sender-pid", 0, NULL);
	g_object_get (n, "sender-pid", &notification_sender_pid, NULL);
	g_assert_cmpint (notification_sender_pid, ==, G_MAXINT);

	// clean up
	g_object_unref (n);
	n = NULL;
}

static void
test_notification_setget_timestamp (gpointer fixture, gconstpointer user_data)
{
	Notification* n     = NULL;
	GTimeVal*       tvptr = NULL;
	GTimeVal        tv_old;
	GTimeVal        tv_new;

	// create new object
	n = notification_new ();

	// if no reception-time has been set yet it should return 0/0
	g_object_get (n, "timestamp", &tvptr, NULL);
	g_assert_cmpint (tvptr->tv_sec, ==, 0);
	g_assert_cmpint (tvptr->tv_usec, ==, 0);

	// ehm... well, get current time
	g_get_current_time (&tv_old);

	// store current time as reception-time and verify it
	g_object_set (n, "timestamp", &tv_old, NULL);
	g_object_get (n, "timestamp", &tvptr, NULL);
	g_assert_cmpint (tvptr->tv_sec, ==, tv_old.tv_sec);
	g_assert_cmpint (tvptr->tv_usec, ==, tv_old.tv_usec);

	// wait at least two seconds
	sleep (2);

	// get current time
	g_get_current_time (&tv_new);

	// trying to store an older timestamp over a newer one should fail
	// second-granularity
	g_object_set (n, "timestamp", &tv_new, NULL);
	g_object_set (n, "timestamp", &tv_old, NULL);
	g_object_get (n, "timestamp", &tvptr, NULL);
	g_assert_cmpint (tvptr->tv_sec, !=, tv_old.tv_sec);
	g_assert_cmpint (tvptr->tv_sec, ==, tv_new.tv_sec);

	// get current time
	g_get_current_time (&tv_old);
	g_object_set (n, "timestamp", &tv_old, NULL);

	// wait some micro-seconds
	usleep (10000);

	// get current time
	g_get_current_time (&tv_new);

	// trying to store an older timestamp over a newer one should fail
	// microsecond-granularity
	g_object_set (n, "timestamp", &tv_new, NULL);
	g_object_set (n, "timestamp", &tv_old, NULL);
	g_object_get (n, "timestamp", &tvptr, NULL);
	g_assert_cmpint (tvptr->tv_usec, !=, tv_old.tv_usec);
	g_assert_cmpint (tvptr->tv_usec, ==, tv_new.tv_usec);

	// clean up
	g_object_unref (n);
	n = NULL;
}

static void
test_notification_setget_urgency (gpointer fixture, gconstpointer user_data)
{
	Notification* n = NULL;
	gint notification_urgency = -10;

	// create new object
	n = notification_new ();

	// if no urgency has been set yet it should return urgency-none
	g_object_get (n, "urgency", &notification_urgency, NULL);
	g_assert_cmpint (notification_urgency, ==, URGENCY_NONE);

	// test all three urgency-levels
	g_object_set (n, "urgency", URGENCY_HIGH, NULL);
	g_object_get (n, "urgency", &notification_urgency, NULL);
	g_assert_cmpint (notification_urgency, ==, URGENCY_HIGH);
	g_object_set (n, "urgency", URGENCY_LOW, NULL);
	g_object_get (n, "urgency", &notification_urgency, NULL);
	g_assert_cmpint (notification_urgency, ==, URGENCY_LOW);
	g_object_set (n, "urgency", URGENCY_NORMAL, NULL);
	g_object_get (n, "urgency", &notification_urgency, NULL);
	g_assert_cmpint (notification_urgency, ==, URGENCY_NORMAL);

	// test non-urgency levels, last valid urgency should be returned
	g_object_set (n, "urgency", 5, NULL);
	g_object_get (n, "urgency", &notification_urgency, NULL);
	g_assert_cmpint (notification_urgency, ==, URGENCY_NORMAL);
	g_object_set (n, "urgency", 23, NULL);
	g_object_get (n, "urgency", &notification_urgency, NULL);
	g_assert_cmpint (notification_urgency, ==, URGENCY_NORMAL);
	g_object_set (n, "urgency", -2, NULL);
	g_object_get (n, "urgency", &notification_urgency, NULL);
	g_assert_cmpint (notification_urgency, ==, URGENCY_NORMAL);

	// clean up
	g_object_unref (n);
	n = NULL;
}

GTestSuite *
test_notification_create_test_suite (void)
{
	GTestSuite *ts = NULL;
	GTestCase  *tc = NULL;

	ts = g_test_create_suite ("notification");
	tc = g_test_create_case ("can create",
				 0,
				 NULL,
				 NULL,
				 test_notification_new,
				 NULL);
	g_test_suite_add (ts, tc);

	g_test_suite_add (
		ts,
		g_test_create_case (
			"can destroy",
			0,
			NULL,
			NULL,
			test_notification_destroy,
			NULL));

	g_test_suite_add (
		ts,
		g_test_create_case (
			"can set|get id",
			0,
			NULL,
			NULL,
			test_notification_setget_id,
			NULL));

	g_test_suite_add (
		ts,
		g_test_create_case (
			"can set|get title",
			0,
			NULL,
			NULL,
			test_notification_setget_title,
			NULL));

	g_test_suite_add (
		ts,
		g_test_create_case (
			"can set|get body",
			0,
			NULL,
			NULL,
			test_notification_setget_body,
			NULL));

	g_test_suite_add (
		ts,
		g_test_create_case (
			"can set|get value",
			0,
			NULL,
			NULL,
			test_notification_setget_value,
			NULL));

	g_test_suite_add (
		ts,
		g_test_create_case (
			"can set|get icon-themename",
			0,
			NULL,
			NULL,
			test_notification_setget_icon_themename,
			NULL));

	g_test_suite_add (
		ts,
		g_test_create_case (
			"can set|get icon-filename",
			0,
			NULL,
			NULL,
			test_notification_setget_icon_filename,
			NULL));

	g_test_suite_add (
		ts,
		g_test_create_case (
			"can set|get icon-pixbuf",
			0,
			NULL,
			NULL,
			test_notification_setget_icon_pixbuf,
			NULL));

	g_test_suite_add (
		ts,
		g_test_create_case (
			"can set|get onscreen-time",
			0,
			NULL,
			NULL,
			test_notification_setget_onscreen_time,
			NULL));

	g_test_suite_add (
		ts,
		g_test_create_case (
			"can set|get sender-name",
			0,
			NULL,
			NULL,
			test_notification_setget_sender_name,
			NULL));

	g_test_suite_add (
		ts,
		g_test_create_case (
			"can set|get sender-pid",
			0,
			NULL,
			NULL,
			test_notification_setget_sender_pid,
			NULL));

	g_test_suite_add (
		ts,
		g_test_create_case (
			"can set|get timestamp",
			0,
			NULL,
			NULL,
			test_notification_setget_timestamp,
			NULL));

	g_test_suite_add (
		ts,
		g_test_create_case (
			"can set|get urgency",
			0,
			NULL,
			NULL,
			test_notification_setget_urgency,
			NULL));

	return ts;
}
