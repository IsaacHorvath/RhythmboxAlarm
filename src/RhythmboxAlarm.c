#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <pwd.h>

void usage() {
	// Print usage;
	printf("Usage:\n\tRhythmboxAlarm set <alarm> <on|off> <hour>:<minute>");
	printf("\n\tRhythmboxAlarm get <alarm>\n");
}

int main(int argc, char *argv[]) {
	FILE *fn;
	int alarm, hour, minute, on_off, jnk, i;
	struct passwd *pw = getpwuid(getuid());
	char alarmdir[256], jnk_str[16];

	// Get alarm directory
	strcpy(alarmdir, pw->pw_dir);
	strcat(alarmdir, "/.alarms");

	if (argc > 4 && !strcmp(argv[1], "set")) {
		// Setting an alarm
		alarm = atoi(argv[2]);
		on_off = strcmp(argv[3], "on")?0:1;
		sscanf(argv[4], "%d:%d", &hour, &minute);
		fn = fopen(alarmdir, "r+");
		for (i = 0; i < alarm; i++) fgets(jnk_str, 16, fn);
		fprintf(fn, "%d %02d:%02d", on_off, hour, minute);
		fclose(fn);
	}
	else if (argc > 2 && !strcmp(argv[1], "get")) {
		// Getting an alarm
		alarm = atoi(argv[2]);
		fn = fopen(alarmdir, "r");
		fscanf(fn, "%d", &jnk);
		for (i = 0; i < alarm; i++)
			fscanf(fn, "%d %d:%d", &jnk, &hour, &minute);
		printf("Alarm %d: %02d:%02d\n", alarm, hour, minute);
		fclose(fn);
	}
	else
		usage();
}
