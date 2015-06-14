//
//  nfc_supprot.h
//  UpsilonAudio
//
//  Created by Pierre Pelé on 14/06/2015.
//
//

#ifndef UpsilonAudio_nfc_supprot_h
#define UpsilonAudio_nfc_supprot_h

// HEADERS

#include <glib.h>
#include <glib-object.h>
#include <glib/gprintf.h>
#include <string.h>

// DÉCLARATION DES STRUCTURES

typedef struct cli_params {
    int agrc;
    char **argv;
    
} cli_params;

// PROTOTYPES DES FONCTIONS

void *nfc_thread_func(void *data);

#endif
