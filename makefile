all:
	gcc `pkg-config --cflags --libs gtk+-3.0` -o RhythmboxAlarm RhythmboxAlarm.c
launcher:
	cp RhythmboxAlarm.desktop $(HOME)/.local/share/applications/
	echo "Exec="$(CURDIR)"/RhythmboxAlarm" >> $(HOME)/.local/share/applications/RhythmboxAlarm.desktop
