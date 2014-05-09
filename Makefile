all: AR_Flight_Controller

AR_Flight_Controller: AR_Flight_Controller.o Gestion_Manette.o
	gcc -L/usr/local/opt/sdl/lib -lSDLmain -lSDL -framework Cocoa -o AR_Flight_Controller AR_Flight_Controller.o Gestion_Manette.o

AR_Flight_Controller.o: AR_Flight_Controller.c
	gcc -o AR_Flight_Controller.o -c AR_Flight_Controller.c

Gestion_Manette.o: Gestion_Manette.c Gestion_Manette.h
	gcc -o Gestion_Manette.o -c Gestion_Manette.c

clean:
	rm -rf *.o

mrproper: clean
	rm -rf AR_Flight_Controller