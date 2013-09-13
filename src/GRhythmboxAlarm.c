#include <gtk/gtk.h>
#include <stdio.h>
#include <unistd.h>
#include <pwd.h>
#include <string.h>

typedef struct {
	gboolean on;
	int hour;
	int minute;
} Alarm;

#define NUM_ALARMS 8 // Number of alarms to display and use. This can be anything.

static Alarm alarms[NUM_ALARMS];

void read_file() {
	// Read the alarms file to gather the info.
	FILE *fn;
	int i, temp;
	struct passwd *pw = getpwuid(getuid());
	char alarmdir[256];

	// Get alarm directory
	strcpy(alarmdir, pw->pw_dir);
	strcat(alarmdir, "/.alarms");

	// Open our default alarms file.
	fn = fopen(alarmdir, "r");
	if (fn == NULL)
		return;
	// Count alarms and discard
	fscanf(fn, "%d", &temp);

	for (i = 0; i < NUM_ALARMS; i++) {
		fscanf(fn, "%d %d:%d", &temp, &alarms[i].hour, &alarms[i].minute);
		alarms[i].on = (temp)?TRUE:FALSE;
	}

	// Close the file and return the number of alarms
	fclose(fn);
}

void write_file() {
	// Write the alarm data to the file.
	FILE *fn;
	int i;
	struct passwd *pw = getpwuid(getuid());
	char alarmdir[256];

	// Get alarm directory
	strcpy(alarmdir, pw->pw_dir);
	strcat(alarmdir, "/.alarms");

	// Open our default alarms file.
	fn = fopen(alarmdir, "w");
	if (fn == NULL)
		return;
	// Print number of alarms to file
	fprintf(fn, "%d\n", NUM_ALARMS);

	for (i = 0; i < NUM_ALARMS; i++)
		fprintf(fn, "%d %02d:%02d\n", (alarms[i].on)?1:0, alarms[i].hour, alarms[i].minute);

	// Close the file and return the number of alarms
	fclose(fn);
}

static void switch_on_off(GtkSwitch *switcher, GParamSpec *pspec, gpointer user_data) {
	// Turn the alarm off when the respective GtkSwitch is pressed
	Alarm *alarm = user_data;
	alarm->on = gtk_switch_get_active(GTK_SWITCH(switcher));
	write_file();
}

static void change_hour(GtkSpinButton *spinbutton, GtkScrollType arg1, gpointer user_data) {
	// Update the alarm hour when the respective spinbutton changes
	Alarm *alarm = user_data;
	alarm->hour = gtk_spin_button_get_value_as_int(spinbutton);
	write_file();
}

static void change_minute(GtkSpinButton *spinbutton, GtkScrollType arg1, gpointer user_data) {
	// Update the alarm minute when the respective spinbutton changes
	Alarm *alarm = user_data;
	alarm->minute = gtk_spin_button_get_value_as_int(spinbutton);
	write_file();
}

int main(int argc, char *argv[]) {
	GtkWidget *window;
	GtkWidget *grid;
	GtkWidget *switcher[NUM_ALARMS];
	GtkWidget *spinbutton;
	int i;

	gtk_init (&argc, &argv);

	// Read in alarm structure
	read_file();

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
		gtk_switch_set_active(GTK_SWITCH(switcher[i]), alarms[i].on);
		g_signal_connect(switcher[i], "notify::active", G_CALLBACK(switch_on_off), &alarms[i]);
		gtk_grid_attach(GTK_GRID(grid), switcher[i], 0, i, 1, 1);

		// Create the hour spinbuttons
		spinbutton = gtk_spin_button_new_with_range(0.0, 23.0, 1.0);
		gtk_spin_button_set_wrap(GTK_SPIN_BUTTON(spinbutton), TRUE);
		gtk_spin_button_set_value(GTK_SPIN_BUTTON(spinbutton), alarms[i].hour);
		g_signal_connect(spinbutton, "value-changed", G_CALLBACK(change_hour), &alarms[i]);
		gtk_grid_attach(GTK_GRID(grid), spinbutton, 1, i, 1, 1);

		// Create the minute spinbuttons
		spinbutton = gtk_spin_button_new_with_range(0.0, 59.0, 1.0);
		gtk_spin_button_set_wrap(GTK_SPIN_BUTTON(spinbutton), TRUE);
		gtk_spin_button_set_value(GTK_SPIN_BUTTON(spinbutton), alarms[i].minute);
		g_signal_connect(spinbutton, "value-changed", G_CALLBACK(change_minute), &alarms[i]);
		gtk_grid_attach(GTK_GRID(grid), spinbutton, 2, i, 1, 1);
	}

	// Show everything and enter the main loop
	gtk_widget_show_all(window);
	gtk_main();

  	return 0;
}
