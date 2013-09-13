all:
	gcc -o rboxalarm src/daemon.c
	gcc -o RhythmboxAlarm src/RhythmboxAlarm.c
	gcc `pkg-config --cflags --libs gtk+-3.0` -o GRhythmboxAlarm src/GRhythmboxAlarm.c
	cp data/alarms $(HOME)/.alarms
launcher:
	cp data/RhythmboxAlarm.desktop $(HOME)/.local/share/applications/
	echo "Exec="$(CURDIR)"/GRhythmboxAlarm" >> $(HOME)/.local/share/applications/RhythmboxAlarm.desktop
