all: musicplayer.hex

musicplayer.hex: musicplayer.ihx
	packihx musicplayer.ihx > musicplayer.hex

musicplayer.ihx: musicplayer.c mylib.lib
	sdcc musicplayer.c mylib.lib
 
mylib.lib: Keypad4x4.rel
	sdar -rs mylib.lib Keypad4x4.rel
	sdranlib mylib.lib

Keypad4x4.rel: Keypad4x4.c Keypad4x4.h
	sdcc -c Keypad4x4.c