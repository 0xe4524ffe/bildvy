DEBUG = -g3 -O0
CFLAGS = -Wall -Werror -Wextra -Wpedantic -Wshadow -std=gnu11 $(shell pkg-config --cflags gtk+-3.0)
LFLAGS = -lm -lmagic -lfm -lfm-gtk3 $(shell pkg-config --libs gtk+-3.0)

release: main.c
	cc $(CFLAGS) -march=native -Os -ftree-vectorize $(LFLAGS) -o bildvy main.c
	strip -s bildvy
	objcopy --strip-all -R .comment -R .note.gnu.build-id -R .note.ABI-tag bildvy bildvy

debug: main.c
	cc $(CFLAGS) $(DEBUG) $(LFLAGS) -o bildvy main.c

clean:
	rm ./bildvy
