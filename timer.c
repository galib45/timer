#include <stdio.h>
#include <gtk/gtk.h>
#include <libnotify/notify.h>

enum State {
	RUNNING,
	PAUSED,
	RESET
};

int config_session_minutes = 30; 								// session duration in minutes
const char* config_file_path = "timer.config";		// config file path

enum State timer_state;
int seconds_left;
int text_opacity;
const gchar* format;

GtkWidget *label;
GString* label_text;

void notify(const char* message) {
	notify_init ("Timer");
	NotifyNotification* notification;
	notification = notify_notification_new("Timer", message, "dialog-information");
	notify_notification_show (notification, NULL);
	g_object_unref(notification);
	notify_uninit();
}

void update_label() {
	int seconds = seconds_left % 60;
	int minutes = (seconds_left / 60) % 60;
	int hours = seconds_left / 3600;

	format = "<span size='40pt' weight='bold' alpha='%d%'>%02d:%02d:%02d</span>";
	g_string_printf(label_text, format, text_opacity, hours, minutes, seconds);
	gtk_label_set_markup(GTK_LABEL(label), label_text->str);
}

void reset_timer() {
	timer_state = RESET;
	seconds_left = config_session_minutes * 60;
	text_opacity = 70;
	update_label();
}

gboolean timer_tick(gpointer user_data) {
	if (timer_state == RUNNING) {
		if (seconds_left == 0) {
			reset_timer();
			notify("Session Completed");
		} else {
			seconds_left--;
			update_label();
		}
	} 	
	return TRUE;
}

gboolean key_release(GtkWidget* self, GdkEventKey* event, gpointer user_data) {
	switch(event->keyval) {
		case GDK_KEY_space:
		case GDK_KEY_KP_Space:
			if (timer_state != RUNNING) {
				timer_state = RUNNING;
				text_opacity = 100;
			} else {
				timer_state = PAUSED;
				text_opacity = 70;
			}
			update_label();
			break;
		case GDK_KEY_R:
		case GDK_KEY_r:
			reset_timer();
			break;
		case GDK_KEY_Q:
		case GDK_KEY_q:
			FILE* file = fopen(config_file_path, "w");
			if (file) {
				fprintf(file, "%d", config_session_minutes);
				fclose(file);
			}
			gtk_main_quit();
			break;
		case GDK_KEY_uparrow:
		case GDK_KEY_Up:
			if (timer_state == RESET) {
				config_session_minutes += 5;
				seconds_left = config_session_minutes * 60;
				update_label();
			}
			break;
		case GDK_KEY_downarrow:
		case GDK_KEY_Down:
			if (timer_state == RESET) {
				config_session_minutes -= 5;
				if(config_session_minutes < 0) config_session_minutes = 0;
				seconds_left = config_session_minutes * 60;
				update_label();
			}
			break;
		case GDK_KEY_rightarrow:
		case GDK_KEY_Right:
			if (timer_state == RESET) {
				config_session_minutes += 1;
				seconds_left = config_session_minutes * 60;
				update_label();
			}
			break;
		case GDK_KEY_leftarrow:
		case GDK_KEY_Left:
			if (timer_state == RESET) {
				config_session_minutes -= 1;
				if(config_session_minutes < 0) config_session_minutes = 0;
				seconds_left = config_session_minutes * 60;
				update_label();
			}
			break;
		
	}
	return TRUE;
}

gboolean delete(GtkWidget* self, GdkEvent* event, gpointer user_data) {
	FILE* file = fopen(config_file_path, "w");
	if (file) {
		fprintf(file, "%d", config_session_minutes);
		fclose(file);
	}
	return FALSE;
}


gboolean label_clicked (GtkWidget* self, GdkEventButton* event, gpointer user_data) {
	if (event->button == 1) {
		switch(timer_state) {
			case RESET:
			case PAUSED:
				timer_state = RUNNING;
				text_opacity = 100;
				update_label();
				break;
			case RUNNING:
				timer_state = PAUSED;
				text_opacity = 70;
				update_label();
				break;
		}
	}
	return TRUE;
}


int main (int argc, char **argv) {
  GtkWidget* window;
  GtkWidget* event_box;

	gtk_init(&argc, &argv);
	
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title (GTK_WINDOW (window), "Timer");
  gtk_window_set_default_size (GTK_WINDOW (window), 400, 200);
	g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
	g_signal_connect(window, "key_release_event", G_CALLBACK(key_release), NULL);
	g_signal_connect(window, "delete-event", G_CALLBACK(delete), NULL);
	
	event_box = gtk_event_box_new();
	gtk_container_add(GTK_CONTAINER(window), event_box);

	// load config from file
	FILE* file = fopen(config_file_path, "r");
	if (file) {
		fscanf(file, "%d", &config_session_minutes);
		fclose(file);
	}
	
	// initialize global variables	
	label_text = g_string_new("");
	label = gtk_label_new(NULL);
	g_signal_connect_swapped(event_box, "button-release-event", G_CALLBACK(label_clicked), label);
	gtk_container_add(GTK_CONTAINER(event_box), label);			
	reset_timer();

	g_timeout_add_seconds(1, timer_tick, NULL);

	gtk_widget_show_all(window);
	gtk_main();
	return 0;
}
