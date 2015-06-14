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

#ifndef Flight_Controller_Gestion_Manette_h
#define Flight_Controller_Gestion_Manette_h

// HEADERS

#include <SDL.h>

// MANNETTE PLAYSTATION 3
#define PS3_AXE_GAUCHE_HORIZONTAL 0
#define PS3_AXE_GAUCHE_VERTICAL 1
#define PS3_AXE_DROITE_HORIZONTAL 2
#define PS3_AXE_DROITE_VERTICAL 3
#define PS3_BOUTON_SELECT 0
#define PS3_BOUTON_START 3
#define PS3_BOUTON_HAUT 4
#define PS3_BOUTON_BAS 6
#define PS3_BOUTON_GAUCHE 7
#define PS3_BOUTON_DROITE 5
#define PS3_BOUTON_TRIANGLE 12
#define PS3_BOUTON_ROND 13
#define PS3_BOUTON_CROIX 14
#define PS3_BOUTON_CARRE 15
#define PS3_BOUTON_PLAYSTATION 16
#define PS3_BOUTON_R1 11
#define PS3_BOUTON_L1 10
#define PS3_BOUTON_R2 9
#define PS3_BOUTON_L2 8
#define PS3_BOUTON_L3 1
#define PS3_BOUTON_R3 2

// MANNETTE XBOX360 - À VÉRIFIER !
#define XBOX_360_AXE_GAUCHE_HORIZONTAL 0
#define XBOX_360_AXE_GAUCHE_VERTICAL 1
#define XBOX_360_AXE_DROITE_HORIZONTAL 2
#define XBOX_360_AXE_DROITE_VERTICAL 3
#define XBOX_360_BOUTON_BACK 0
#define XBOX_360_BOUTON_START 3
#define XBOX_360_BOUTON_HAUT 4
#define XBOX_360_BOUTON_BAS 6
#define XBOX_360_BOUTON_GAUCHE 7
#define XBOX_360_BOUTON_DROITE 5
#define XBOX_360_BOUTON_Y 12
#define XBOX_360_BOUTON_B 13
#define XBOX_360_BOUTON_A 14
#define XBOX_360_BOUTON_X 15
#define XBOX_360_BOUTON_XBOX 16
#define XBOX_360_BOUTON_RB 11
#define XBOX_360_BOUTON_LB 10
#define XBOX_360_BOUTON_RT 9
#define XBOX_360_BOUTON_LT 8

// SEUIL DE DÉTECTION
#define PS3_AXES_SEUIL_DETECTION 6000

#define DEBUG_PRINTF
#define INFOS_AXES // Afficher les valeurs des axes
#define INFOS_MANETTE // Afficher des informations sur la manette en début de programme

typedef struct Manette Manette;
struct Manette {
    
    SDL_Joystick *joystick; // La manette
    char *boutons; // Les boutons de la manette
    int *axes; // les axes de la manette
    int *chapeaux; // Les chapeaux de la manette
    int numero; // Numéro de la manette
    
};

int initialiser_Manette(Manette *manette,int numero_Manette); // Initailisaton de la manette
void detruire_Manette(Manette *manette); // Libération de la mémoire allouée
int actualiser_Etat_Manette(Manette *manette); // Actualisation de l'état de la manette
float adaptation_axe(float my_floating_point_variable);

#endif