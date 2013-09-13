#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <pwd.h>
#include <string.h>

#define MAX_ALARMS 32

int read_file(int alarms[MAX_ALARMS][4]) {
	// Read the alarms file to gather the info.
	FILE *fn;
	int num_alarms, i;
	struct passwd *pw = getpwuid(getuid());
	char alarmdir[256];

	// Get alarm directory
	strcpy(alarmdir, pw->pw_dir);
	strcat(alarmdir, "/.alarms");

	// Open our default alarms file.
	fn = fopen(alarmdir, "r");
	if (fn == NULL)
		return 0;
	// Count alarms and read the data into the array
	fscanf(fn, "%d", &num_alarms);
	if (num_alarms > MAX_ALARMS) num_alarms = MAX_ALARMS;

	for (i = 0; i < num_alarms; i++)
		fscanf(fn, "%d %d:%d", &alarms[i][0], &alarms[i][1], &alarms[i][2]);

	// Close the file and return the number of alarms
	fclose(fn);
	return num_alarms;
}

int main(int argc, char *argv[]) {
	int i, num_alarms;
	time_t rawtime;
	struct tm *timeinfo;
	int alarms[MAX_ALARMS][4];

	// Run as a daemon
	pid_t pid = 0, sid = 0;
	pid = fork();
	if (pid < 0) {
		printf("Fork failed!\n");
		exit(1);
	}
	else if (pid > 0) {
		printf("pid of child process %d \n", pid);
        	exit(0);
    	}

	umask(0);
	sid = setsid();
	if(sid < 0)
		exit(1);
	close(STDIN_FILENO);
	close(STDOUT_FILENO);
	close(STDERR_FILENO);

	while (1) {
		// Calculate the raw time (seconds since 01/01/70) and convert it to a struct
		time(&rawtime);
		timeinfo = localtime(&rawtime);

		// Update alarm array
		num_alarms = read_file(alarms);

		// Test if each alarm is on and matches the current time
		for (i = 0; i < num_alarms; i++) {
			if (alarms[i][0]) { // Is alarm on?
				if (timeinfo->tm_hour == alarms[i][1] && timeinfo->tm_min == alarms[i][2]) { // Does time match?
					if (!alarms[i][3]) { // Is alarm not already going?
						system("amixer set Master 0");		// Turn off volume to start
						system("rhythmbox-client --play");	// Play rhythmbox if it's running.
						alarms[i][3] = 1;
					}
					else system("amixer set Master 5+"); // Increase volume slowly if alarm is going
				}
				else alarms[i][3] = 0;
			}
		}
		usleep(3000000);
	}
}
