#include <stdio.h>
#include <gtk/gtk.h>

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

GtkApplication *app;
GtkWidget *label;
GString* label_text;

void update_label() {
	int seconds = seconds_left % 60;
	int minutes = (seconds_left / 60) % 60;
	int hours = seconds_left / 3600;

	format = "<span size='40pt' weight='bold' alpha='%d%'>%02d:%02d:%02d</span>";
	g_string_printf(label_text, format, text_opacity, hours, minutes, seconds);
	gtk_label_set_markup(GTK_LABEL(label), label_text->str);
}

gboolean timer_tick(gpointer user_data) {
	if (timer_state == RUNNING) {
		seconds_left--;
		update_label();
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
				update_label();
			}
			break;
		case GDK_KEY_R:
		case GDK_KEY_r:
			timer_state = RESET;
			text_opacity = 70;
			seconds_left = config_session_minutes * 60;
			update_label();
			break;
		case GDK_KEY_Q:
		case GDK_KEY_q:
			FILE* file = fopen(config_file_path, "w");
			if (file) {
				fprintf(file, "%d", config_session_minutes);
				fclose(file);
			}
			g_application_quit(G_APPLICATION(app));
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
		default:
			g_print("no bindings for this key\n");
			return FALSE;
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

void activate (GtkApplication *app, gpointer user_data) {
  GtkWidget *window;

  window = gtk_application_window_new (app);
  gtk_window_set_title (GTK_WINDOW (window), "Timer");
  gtk_window_set_default_size (GTK_WINDOW (window), 400, 200);
	g_signal_connect (window, "key_release_event", G_CALLBACK(key_release), NULL);
	g_signal_connect (window, "delete-event", G_CALLBACK(delete), NULL);

	// load config from file
	FILE* file = fopen(config_file_path, "r");
	if (file) {
		fscanf(file, "%d", &config_session_minutes);
		fclose(file);
	}
	
	// initialize global variables
	timer_state = RESET;
	seconds_left = config_session_minutes * 60;
	text_opacity = 70;
	label_text = g_string_new("");
	label = gtk_label_new(NULL);
	gtk_container_add(GTK_CONTAINER(window), label);			
	update_label();
	
	g_timeout_add_seconds(1, timer_tick, NULL);
  gtk_widget_show_all(window);
}

int main (int argc, char **argv) {
  int status;

  app = gtk_application_new ("org.galib.timer", G_APPLICATION_DEFAULT_FLAGS);
  g_signal_connect (app, "activate", G_CALLBACK (activate), NULL);
  status = g_application_run (G_APPLICATION (app), argc, argv);
  g_object_unref (app);

  return status;
}
