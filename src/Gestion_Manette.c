// AR Drone Flight Controller

/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

// HEADERS
#include <stdlib.h>
#include <SDL.h>
#include "Gestion_Manette.h"

int initialiser_Manette(Manette *manette, int numero_Manette)
{
    // Le numéro de la manette est-il correct ?
    
    if(numero_Manette < SDL_NumJoysticks()) {
        
        manette->joystick = SDL_JoystickOpen(numero_Manette);
        manette->numero = numero_Manette;
        manette->boutons = malloc(SDL_JoystickNumButtons(manette->joystick) * sizeof(char));
        manette->axes = malloc(SDL_JoystickNumAxes(manette->joystick) * sizeof(int));
        manette->chapeaux = malloc(SDL_JoystickNumHats(manette->joystick) * sizeof(int));
        int i;
        for(i=0;i<SDL_JoystickNumButtons(manette->joystick);i++) // Tant qu'on a pas atteint le nombre maximal de boutons ...
        {
            manette->boutons[i] = 0; // Initialisation des valeurs
        }
        
        for(i=0;i<SDL_JoystickNumAxes(manette->joystick);i++) // Tant qu'on a pas atteint le nombre maximal d'axes ...
        {
            manette->axes[i] = 0; // Initialisation des valeurs
        }
        
        for(i=0;i<SDL_JoystickNumHats(manette->joystick);i++) // Tant qu'on a pas atteint le nombre maximal de chapeaux ...
        {
            manette->chapeaux[i] = SDL_HAT_CENTERED; // Initialisation des valeurs
        }
        #ifdef DEBUG_PRINTF
        #ifdef INFOS_MANETTE
        printf("\nManette N°%d : %s\n",numero_Manette,SDL_JoystickName(numero_Manette));
        printf("Nombre de boutons : %d\n",SDL_JoystickNumButtons(manette->joystick));
        printf("Nombre d'axes : %d\n",SDL_JoystickNumAxes(manette->joystick));
        printf("Nombre de chapeaux : %d\n\n",SDL_JoystickNumHats(manette->joystick));
        #endif
        #endif
        
        return 1 ;
        
    } else {
        
        // Le numéro de la manette n'est pas correct !
        
        #ifdef DEBUG_PRINTF
        #ifdef INFOS_MANETTE
        printf("\nERREUR : Aucune manette détectée !\n\n");
        #endif
        #endif
        manette->joystick = NULL;
        manette->boutons = NULL;
        manette->axes = NULL;
        manette->chapeaux = NULL;
        
        return 0 ;
    }   
}

void detruire_Manette(Manette *manette)
{
    if(manette->joystick != NULL) // La manette est-elle connectée ?
    {
        manette->numero = 0; // Initialisation des valeurs
        // Libération de la mémoire
        free(manette->boutons);
        free(manette->axes);
        free(manette->chapeaux);
        SDL_JoystickClose(manette->joystick);
    }
}

int actualiser_Etat_Manette(Manette *manette)
{
    int status = 1 ;
    static SDL_Event evenements; // en statique car appelle plusieurs fois par seconde
    while(SDL_PollEvent(&evenements)) // Tant qu'il y a des évènements à traiter ...
    {
        if(manette->joystick != NULL && (evenements.jbutton.which == manette->numero || evenements.jaxis.which == manette->numero || evenements.jhat.which == manette->numero ))
        {
            switch(evenements.type)
            {
                case SDL_JOYBUTTONDOWN:
                    manette->boutons[evenements.jbutton.button] = 1; // Bouton appuyé - Valeur du "bouton" : 1
                    break;

                case SDL_JOYBUTTONUP:
                    manette->boutons[evenements.jbutton.button] = 0; // Bouton relâché - Valeur du "bouton" : 0
                    break;

                case SDL_JOYAXISMOTION:
                    manette->axes[evenements.jaxis.axis] = evenements.jaxis.value;
                    break;

                case SDL_JOYHATMOTION:
                    manette->chapeaux[evenements.jhat.hat] = evenements.jhat.value;
                    break;
                    
                case SDL_QUIT: // On quitte le programme
                    status = 0;
                    break;

                default:
                    break;
            }
        }
    }
    
    return status;
}

float adaptation_axe (float my_floating_point_variable) {
    if (my_floating_point_variable < 0) {
        return -(my_floating_point_variable * my_floating_point_variable);
    } else {
        return (my_floating_point_variable * my_floating_point_variable);
    }
}