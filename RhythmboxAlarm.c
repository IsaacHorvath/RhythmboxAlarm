#include <gtk/gtk.h>
#include <time.h>

typedef struct {
	gboolean on;
	int hour;
	int minute;
	GtkWidget *switcher;
} Alarm;

const int NUM_ALARMS = 8; // Number of alarms to display and use. This can be anything.

static void switch_on_off(GtkSwitch *switcher, GParamSpec *pspec, gpointer user_data) {
	// Turn the alarm off when the respective GtkSwitch is pressed
	Alarm *alarm = user_data;
	alarm->on = gtk_switch_get_active(GTK_SWITCH(switcher));
}

static void change_hour(GtkSpinButton *spinbutton, GtkScrollType arg1, gpointer user_data) {
	// Update the alarm hour when the respective spinbutton changes
	Alarm *alarm = user_data;
	alarm->hour = gtk_spin_button_get_value_as_int(spinbutton);
}

static void change_minute(GtkSpinButton *spinbutton, GtkScrollType arg1, gpointer user_data) {
	// Update the alarm minute when the respective spinbutton changes
	Alarm *alarm = user_data;
	alarm->minute = gtk_spin_button_get_value_as_int(spinbutton);
}

static gboolean timer(Alarm alarms[]) {
	int i;
	time_t rawtime;
	struct tm *timeinfo;

	// Calculate the raw time (seconds since 01/01/70) and convert it to a struct
	time(&rawtime);
	timeinfo = localtime(&rawtime);

	for (i = 0; i < NUM_ALARMS; i++) {
		if (alarms[i].on) {
			// Alarm is on!
			if (timeinfo->tm_hour == alarms[i].hour && timeinfo->tm_min == alarms[i].minute) {
				system("rhythmbox-client --play");	// Play rhythmbox if it's running.
				alarms[i].on == FALSE;			// Turn alarm off
				gtk_switch_set_active(GTK_SWITCH(alarms[i].switcher), FALSE);
			}
		}
	}
	return TRUE; // Ensure this function keeps getting called
}

int main(int argc, char *argv[]) {
	GtkWidget *window;
	GtkWidget *grid;
	GtkWidget *switcher[NUM_ALARMS];
	GtkWidget *spinbutton;
	Alarm alarms[NUM_ALARMS];
	int i;

	gtk_init (&argc, &argv);

	// Create the window
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW (window), "Rhythmbox Alarm");
	g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
	gtk_container_set_border_width(GTK_CONTAINER(window), 10);

	// Create the grid to arrange the objects inside
	grid = gtk_grid_new();
	gtk_container_add(GTK_CONTAINER(window), grid);

	for (i = 0; i < NUM_ALARMS; i++) {
		// Create the switches. These have to be in an array so they can be referenced later.
		switcher[i] = gtk_switch_new();
		g_signal_connect(switcher[i], "notify::active", G_CALLBACK(switch_on_off), &alarms[i]);
		gtk_grid_attach(GTK_GRID(grid), switcher[i], 0, i, 1, 1);

		// Create the hour spinbuttons
		spinbutton = gtk_spin_button_new_with_range(0.0, 23.0, 1.0);
		gtk_spin_button_set_wrap(GTK_SPIN_BUTTON(spinbutton), TRUE);
		g_signal_connect(spinbutton, "value-changed", G_CALLBACK(change_hour), &alarms[i]);
		gtk_grid_attach(GTK_GRID(grid), spinbutton, 1, i, 1, 1);

		// Create the minute spinbuttons
		spinbutton = gtk_spin_button_new_with_range(0.0, 59.0, 1.0);
		gtk_spin_button_set_wrap(GTK_SPIN_BUTTON(spinbutton), TRUE);
		g_signal_connect(spinbutton, "value-changed", G_CALLBACK(change_minute), &alarms[i]);
		gtk_grid_attach(GTK_GRID(grid), spinbutton, 2, i, 1, 1);

		// Initialize each alarm to be turned off and set to 00:00
		alarms[i].on = FALSE;
		alarms[i].hour = 0;
		alarms[i].minute = 0;
		alarms[i].switcher = switcher[i];
	}

	// Add a timout, so our timer function can be called every 3 seconds (seems like an okay default)
	g_timeout_add_seconds(3, (GSourceFunc)timer, alarms);

	// Show everything and enter the main loop
	gtk_widget_show_all(window);
	gtk_main();

  	return 0;
}
