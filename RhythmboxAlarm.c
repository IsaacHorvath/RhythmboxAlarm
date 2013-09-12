#include <stdio.h>
#include <time.h>
#include <unistd.h>

int main(int argc, char **argv) {
	time_t rawtime;
	struct tm *timeinfo;
	int hour, minute, currenth, currentm;
	char execute_string[] = "rhythmbox-client --play";
	if (argc < 2) {
		printf("Usage: RhythmboxAlarm <time>\n");
		return;
	}
	sscanf(argv[1], "%d:%d", &hour, &minute);
	printf("Setting alarm for %02d:%02d\n", hour, minute);
	do {
		time(&rawtime);
		timeinfo = localtime(&rawtime);
		currenth = timeinfo->tm_hour;
		currentm = timeinfo->tm_min;
		printf("\rThe time is now %02d:%02d.", currenth, currentm);
		usleep(50000);
	} while (hour != currenth || minute != currentm);
	printf("\n");
	system(execute_string);
}
