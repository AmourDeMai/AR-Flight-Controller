bin_PROGRAMS=AR_Flight_Controller

AR_Flight_Controller_SOURCES= \
	AR_Flight_Controller.c \
	Gestion_Manette.c Gestion_Manette.h

AM_CFLAGS	= -g $(shell pkg-config --cflags sdl)
AM_LDFLAGS	= -g
LIBS 	= $(shell pkg-config --libs sdl)