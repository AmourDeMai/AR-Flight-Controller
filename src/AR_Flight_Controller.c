// AR Drone Flight Controller

// Équipe Dédale
// Télécom SudParis
// Upsilonaudio.com

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

#include <SDL.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include "Gestion_Manette.h"

#ifdef HAVE_CONFIG_H
#include "../AR_Flight_Controller_config.h"
#endif

#if defined(HAVE_NFC_ENABLE) && HAVE_NFC_ENABLE == 1
#include "nfc_supprot.h"
#endif

// DEFINES

// ADRESSE IP AR.DRONE
#define ADRESSEIP "192.168.1.1"
#define PORT_AT 5556
#define BUFLEN 256 // Taille des paquets UDP
#define TAILLE_MESSAGE 65 // Taille maximale commande AT

// CONFIGURATION DU DRONE
#define DELAI_MICROSECONDES 45000 // Délai entre deux commandes AT (microsecondes)
#define ANIMATION_LOOPING_AVANT "\"control:flight_anim\",\"16,15\""
#define ANIMATION_LOOPING_GAUCHE "\"control:flight_anim\",\"18,15\""
#define ANIMATION_LOOPING_DROITE "\"control:flight_anim\",\"19,15\""
#define ANIMATION_LOOPING_ARRIERE "\"control:flight_anim\",\"17,15\""
#define MODE_DE_VOL_NORMAL "\"control:flying_mode\",\"0\""
#define MODE_DE_VOL_AUTONOME "\"control:flying_mode\",\"1\""
#define TYPE_DE_CIBLE "\"detect:detect_type\",\"12\""

// MODE "NERVOSITÉ"
#define NERVOSITE_ALTITUDE_MAXIMALE "\"control:altitude_max\",\"15000\"" // Altitude maximale (millimètres)
#define NERVOSITE_ALTITUDE_MINIMALE "\"control:altitude_min\",\"500\"" // Altitude minimale (millimètres)
#define NERVOSITE_VITESSE_VERTICALE_MAXIMALE "\"control:control_vz_max\",\"3500\"" // Vitesse verticale maximale (millimètres par seconde)
#define NERVOSITE_VITESSE_LACET "\"control:control_yaw\",\"8\"" // Vitesse de lacet (radians par seconde)
#define NERVOSITE_ANGLE_MAXIMAL "\"control:euler_angle_max\",\"0.55\"" // Angle de flexion maximal (radians)

// MODE "SÉCURITÉ"
#define SECURITE_ALTITUDE_MAXIMALE "\"control:altitude_max\",\"5000\"" // Altitude maximale (millimètres)
#define SECURITE_ALTITUDE_MINIMALE "\"control:altitude_min\",\"50\"" // Altitude minimale (millimètres)
#define SECURITE_VITESSE_VERTICALE_MAXIMALE "\"control:control_vz_max\",\"800\"" // Vitesse verticale maximale (millimètres par seconde)
#define SECURITE_VITESSE_LACET "\"control:control_yaw\",\"3.0\"" // Vitesse de lacet (radians par seconde)
#define SECURITE_ANGLE_MAXIMAL "\"control:euler_angle_max\",\"0.25\"" // Angle de flexion maximal (radians)

// PROTOTYPES DES FONCTIONS

int envoyer_AT(char* commande,
               unsigned int *compteur,
               char* argument);

int envoyer_AT_PCMD(unsigned int *compteur,
                    float my_floating_point_variable_4,
                    float my_floating_point_variable_3,
                    float my_floating_point_variable_1,
                    float my_floating_point_variable_2);

// VARIABLES GLOBALES

struct sockaddr_in serv_addr;
int sockfd, slen;

// FONCTION PRINCIPALE

int main(int argc, char *argv[]) {
    
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK) < 0) {
        
#ifdef DEBUG_PRINTF
        fprintf(stderr, "Erreur d'initialisation de la SDL : %s\n", SDL_GetError());
#endif
        exit(EXIT_FAILURE);
    }
    
    Manette manette; // Structure qui permet de gérer les boutons et les axes de la manette
    
    if (initialiser_Manette(&manette,0) == 0) // La manette est elle connectée ?
    {
        exit(EXIT_FAILURE);
    }
    
    slen=sizeof(serv_addr);
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
        perror("Socket ");
    }
    
    bzero(&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT_AT);
    if (inet_aton(ADRESSEIP, &serv_addr.sin_addr) == 0) {
        fprintf(stderr, "inet_aton() : ERREUR\n");
        exit(1);
    }
    printf("\nAppuyez sur \"R1\" pour décoller !\n");
    
    int boucle = 1;
    int status = 0; // status = 0 : Le drone est au sol - status = 0 : Le drone est en vol
    int urgence = 0; // urgence = 0 : Mode urgence désactivé - urgence = 1 : Mode urgence activé
    int mode = 1; // mode = 0 : Nervosité - mode = 1 : Sécurité
    int flying_mode = 0; // flying_mode = 0 : Mode normal - flying_mode = 1 "hover over oriented roundel mode"
    unsigned int compteur = 0; // Initialisation du compteur
    
#if defined(HAVE_NFC_ENABLE) && HAVE_NFC_ENABLE == 1
    cli_params params = { argc, argv };
    GThread *nfc_thread = g_thread_new("NFC Thread", &nfc_thread_func, &params);
#endif
    
    while (boucle) {
        
        if (actualiser_Etat_Manette(&manette) == 0) // Actualisaton de l'état de la manette
        {
            boucle = 0; // On sort de la boucle
            if (status== 1)
            {
#ifdef DEBUG_PRINTF
                printf("\nAtterissage !\n");
#endif
                envoyer_AT("AT*REF",&compteur,"290717696"); // Atterissage du drone
                status = 0;
            }
        }
        
        if (manette.boutons[PS3_BOUTON_START]) {
            
            boucle = 0; // On sort de la boucle
            if (status== 1) {
                
#ifdef DEBUG_PRINTF
                printf("\nAtterissage !\n");
#endif
                envoyer_AT("AT*REF",&compteur,"290717696"); // Atterissage du drone
                status = 0;
            }
            
        } else if (status==0 && manette.boutons[PS3_BOUTON_R1]) {
            
#ifdef DEBUG_PRINTF
            printf("\nDécollage !\n");
#endif
            envoyer_AT("AT*REF",&compteur,"290718208"); // Décollage du drone
#ifdef DEBUG_PRINTF
            printf("\nMode \"SÉCURITÉ\" !\n"); // Activation du mode "sécurité"
#endif
            envoyer_AT("AT*CONFIG",&compteur,SECURITE_ALTITUDE_MAXIMALE);
            envoyer_AT("AT*CONFIG",&compteur,SECURITE_ALTITUDE_MINIMALE);
            envoyer_AT("AT*CONFIG",&compteur,SECURITE_VITESSE_VERTICALE_MAXIMALE);
            envoyer_AT("AT*CONFIG",&compteur,SECURITE_VITESSE_LACET);
            envoyer_AT("AT*CONFIG",&compteur,SECURITE_ANGLE_MAXIMAL);
            status = 1;
            mode = 1;
            
        } else if (status== 1 && manette.boutons[PS3_BOUTON_L1]) {
            
#ifdef DEBUG_PRINTF
            printf("\nAtterissage !\n");
#endif
            envoyer_AT("AT*REF",&compteur,"290717696"); // Atterissage du drone
            status = 0;
            
        } else if (manette.boutons[PS3_BOUTON_L3] && mode == 0) {
            
            // Activation du mode "sécurité"
            
#ifdef DEBUG_PRINTF
            printf("\nMode \"SÉCURITÉ\" !\n");
#endif
            envoyer_AT("AT*CONFIG",&compteur,SECURITE_ALTITUDE_MAXIMALE);
            envoyer_AT("AT*CONFIG",&compteur,SECURITE_ALTITUDE_MINIMALE);
            envoyer_AT("AT*CONFIG",&compteur,SECURITE_VITESSE_VERTICALE_MAXIMALE);
            envoyer_AT("AT*CONFIG",&compteur,SECURITE_VITESSE_LACET);
            envoyer_AT("AT*CONFIG",&compteur,SECURITE_ANGLE_MAXIMAL);
            mode = 1;
            manette.boutons[PS3_BOUTON_L3] = 0;
            
        } else if (manette.boutons[PS3_BOUTON_R3] && mode == 1) {
            
            // Activation du mode "nervosité"
            
#ifdef DEBUG_PRINTF
            printf("\nMode \"NERVOSITÉ\" !\n");
#endif
            envoyer_AT("AT*CONFIG",&compteur,NERVOSITE_ALTITUDE_MAXIMALE);
            envoyer_AT("AT*CONFIG",&compteur,NERVOSITE_ALTITUDE_MINIMALE);
            envoyer_AT("AT*CONFIG",&compteur,NERVOSITE_VITESSE_VERTICALE_MAXIMALE);
            envoyer_AT("AT*CONFIG",&compteur,NERVOSITE_VITESSE_LACET);
            envoyer_AT("AT*CONFIG",&compteur,NERVOSITE_ANGLE_MAXIMAL);
            mode = 0;
            manette.boutons[PS3_BOUTON_R3] = 0;
            
        } else if (urgence==0 && manette.boutons[PS3_BOUTON_TRIANGLE] && manette.boutons[PS3_BOUTON_CROIX]) {
            
#ifdef DEBUG_PRINTF
            printf("ARRET_URGENCE\n");
#endif
            envoyer_AT("AT*REF",&compteur,"290717952");
            status = 0;
            urgence = 1;
            
        } else if (urgence== 1 && manette.boutons[PS3_BOUTON_CARRE] && manette.boutons[PS3_BOUTON_ROND]) {
            
#ifdef DEBUG_PRINTF
            printf("ANTI_ARRET_URGENCE\n");
#endif
            envoyer_AT("AT*REF",&compteur,"290717696");
            urgence = 0;
            
        } else if (status== 1 && manette.boutons[PS3_BOUTON_HAUT] == 1 && manette.boutons[PS3_BOUTON_PLAYSTATION] == 1) {
            
#ifdef DEBUG_PRINTF
            printf("\nLooping vers l'avant !\n");
#endif
            envoyer_AT("AT*CONFIG",&compteur,ANIMATION_LOOPING_AVANT);
            manette.boutons[PS3_BOUTON_HAUT] = 0;
            
        } else if (status== 1 && manette.boutons[PS3_BOUTON_BAS] == 1 && manette.boutons[PS3_BOUTON_PLAYSTATION] == 1) {
            
#ifdef DEBUG_PRINTF
            printf("\nLooping vers l'arrière !\n");
#endif
            envoyer_AT("AT*CONFIG",&compteur,ANIMATION_LOOPING_ARRIERE);
            manette.boutons[PS3_BOUTON_BAS] = 0;
            
        } else if (status== 1 && manette.boutons[PS3_BOUTON_GAUCHE] == 1 && manette.boutons[PS3_BOUTON_PLAYSTATION] == 1) {
            
#ifdef DEBUG_PRINTF
            printf("\nLooping vers la gauche !\n");
#endif
            envoyer_AT("AT*CONFIG",&compteur,ANIMATION_LOOPING_GAUCHE);
            manette.boutons[PS3_BOUTON_GAUCHE] = 0;
            
        } else if (status== 1 && manette.boutons[PS3_BOUTON_DROITE] == 1 && manette.boutons[PS3_BOUTON_PLAYSTATION] == 1) {
            
#ifdef DEBUG_PRINTF
            printf("\nLooping vers la droite !\n");
#endif
            envoyer_AT("AT*CONFIG",&compteur,ANIMATION_LOOPING_DROITE);
            manette.boutons[PS3_BOUTON_DROITE] = 0;
            
        } else if (manette.boutons[PS3_BOUTON_SELECT]) {
            
            if ( flying_mode == 1) {
                
                // Activation du mode de vol "normal"
                
#ifdef DEBUG_PRINTF
                printf("\nMode de vol : NORMAL\n");
#endif
                envoyer_AT("AT*CONFIG",&compteur,MODE_DE_VOL_NORMAL);
                manette.boutons[PS3_BOUTON_SELECT] = 0;
                flying_mode = 0;
                
            } else {
                
                // Activation du mode de vol autonome
                
#ifdef DEBUG_PRINTF
                printf("\nMode de vol : AUTONOME\n");
#endif
                envoyer_AT("AT*CONFIG",&compteur,MODE_DE_VOL_AUTONOME);
                envoyer_AT("AT*CONFIG",&compteur,TYPE_DE_CIBLE);
                manette.boutons[PS3_BOUTON_SELECT] = 0;
                flying_mode = 1;
                
            }
            
            manette.boutons[PS3_BOUTON_SELECT] = 0;
            
        } else if (status== 1) {
            
            if (envoyer_AT_PCMD(&compteur,manette.axes[PS3_AXE_GAUCHE_HORIZONTAL],manette.axes[PS3_AXE_GAUCHE_VERTICAL],manette.axes[PS3_AXE_DROITE_HORIZONTAL],manette.axes[PS3_AXE_DROITE_VERTICAL]) == 0 ) {
                boucle = 0;
            }
        }
    }
    
    // Libération de la mémoire allouée à la manette et au socket.
    close(sockfd);
    detruire_Manette(&manette);
    SDL_Quit();
    
    return EXIT_SUCCESS;
}

// DÉFINITION DES FONCTIONS

int envoyer_AT_PCMD(unsigned int *compteur,
                    float my_floating_point_variable_4,
                    float my_floating_point_variable_3,
                    float my_floating_point_variable_1,
                    float my_floating_point_variable_2)
{
    char message[TAILLE_MESSAGE]; // Commande AT à envoyer
    *compteur=*compteur+1; // Incrémentation du compteur
    
#ifdef DEBUG_PRINTF
#ifdef INFOS_AXES
    printf("INT - GAUCHE - AXE HORIZONTAL : %f\n",my_floating_point_variable_4);
    printf("INT - GAUCHE - AXE VERTICAL : %f\n",my_floating_point_variable_3);
    printf("INT - DROITE - AXE HORIZONTAL : %f\n",my_floating_point_variable_1);
    printf("INT - DROITE - AXE VERTICAL : %f\n",my_floating_point_variable_2);
#endif
#endif
    
    if ((my_floating_point_variable_4 == -32768.000000) &&
        (my_floating_point_variable_3 == -32768.000000) &&
        (my_floating_point_variable_2 == -32768.000000) &&
        (my_floating_point_variable_1 == -32768.000000)) {
        
#ifdef DEBUG_PRINTF
        printf("Atterissage d'urgence !\n");
#endif
        sprintf(message,"AT*REF=%d,290717696\r",*compteur); // Atterissage d'urgence car la manette est déconnectée
#ifdef DEBUG_PRINTF
        printf("%s\n",message);
#endif
        
        if (sendto(sockfd, message, BUFLEN, 0, (struct sockaddr*)&serv_addr, slen) == -1) {
            perror("sendto() ");
        }
        
        return 0; // On sort de la fonction
        
    } else if((abs(my_floating_point_variable_4) < PS3_AXES_SEUIL_DETECTION) &&
              (abs(my_floating_point_variable_3) < PS3_AXES_SEUIL_DETECTION) &&
              (abs(my_floating_point_variable_2) < PS3_AXES_SEUIL_DETECTION) &&
              (abs(my_floating_point_variable_1) < PS3_AXES_SEUIL_DETECTION)) {
        
#ifdef DEBUG_PRINTF
        printf("Vol stationnaire !\n");
#endif
        sprintf(message,"AT*PCMD=%d,0,0,0,0,0\r",*compteur); // Vol stationnaire
        
    } else {
        
        if(my_floating_point_variable_1>PS3_AXES_SEUIL_DETECTION) {
            
            my_floating_point_variable_1=my_floating_point_variable_1/32767.;
            my_floating_point_variable_1=adaptation_axe(my_floating_point_variable_1); // Correction
            
        } else if (my_floating_point_variable_1<-PS3_AXES_SEUIL_DETECTION) {
            
            my_floating_point_variable_1=my_floating_point_variable_1/32768.;
            my_floating_point_variable_1=adaptation_axe(my_floating_point_variable_1); // Correction
            
        } else {
            
            my_floating_point_variable_1 = 0;
        }
        
        if(my_floating_point_variable_2>PS3_AXES_SEUIL_DETECTION) {
            
            my_floating_point_variable_2=my_floating_point_variable_2/32767.;
            my_floating_point_variable_2=adaptation_axe(my_floating_point_variable_2); // Correction
            
        } else if (my_floating_point_variable_2<-PS3_AXES_SEUIL_DETECTION) {
            
            my_floating_point_variable_2=my_floating_point_variable_2/32768.;
            my_floating_point_variable_2=adaptation_axe(my_floating_point_variable_2); // Correction
            
        } else {
            
            my_floating_point_variable_2 = 0;
        }
        
        if(my_floating_point_variable_3>PS3_AXES_SEUIL_DETECTION) {
            
            my_floating_point_variable_3=-my_floating_point_variable_3/32767.;
            my_floating_point_variable_3=adaptation_axe(my_floating_point_variable_3); // Correction
            
        } else if (my_floating_point_variable_3<-PS3_AXES_SEUIL_DETECTION) {
            
            my_floating_point_variable_3=-my_floating_point_variable_3/32768.;
            my_floating_point_variable_3=adaptation_axe(my_floating_point_variable_3); // Correction
            
        } else {
            my_floating_point_variable_3 = 0;
        }
        
        if(my_floating_point_variable_4>PS3_AXES_SEUIL_DETECTION) {
            
            my_floating_point_variable_4=my_floating_point_variable_4/32767.;
            my_floating_point_variable_4=adaptation_axe(my_floating_point_variable_4); // Correction
            
        } else if (my_floating_point_variable_4<-PS3_AXES_SEUIL_DETECTION) {
            
            my_floating_point_variable_4=my_floating_point_variable_4/32768.;
            my_floating_point_variable_4=adaptation_axe(my_floating_point_variable_4); // Correction
            
        } else {
            
            my_floating_point_variable_4 = 0;
        }
        
#ifdef DEBUG_PRINTF
#ifdef INFOS_AXES
        printf("FLOAT - GAUCHE - AXE HORIZONTAL : %f\n", my_floating_point_variable_4);
        printf("FLOAT - GAUCHE - AXE VERTICAL : %f\n", my_floating_point_variable_3);
        printf("FLOAT - DROITE - AXE HORIZONTAL : %f\n", my_floating_point_variable_1);
        printf("FLOAT - DROITE - AXE VERTICAL : %f\n", my_floating_point_variable_2);
#endif
#endif
        sprintf(message,"AT*PCMD=%d,1,%d,%d,%d,%d\r",
                *compteur,
                *(int*)(&my_floating_point_variable_1),
                *(int*)(&my_floating_point_variable_2),
                *(int*)(&my_floating_point_variable_3),
                *(int*)(&my_floating_point_variable_4));
    }
    
#ifdef DEBUG_PRINTF
    printf("%s\n",message);
#endif
    
    if (sendto(sockfd, message, BUFLEN, 0, (struct sockaddr*)&serv_addr, slen) == -1) {
        perror("sendto() ");
    }
    usleep(DELAI_MICROSECONDES);
    
    return 1;
}

int envoyer_AT(char* commande,
               unsigned int *compteur,
               char *argument)
{
    char message[TAILLE_MESSAGE]; // Commande AT à envoyer
    *compteur=*compteur+1; // Incrémentation du compteur
    sprintf(message,"%s=%d,%s\r",commande,*compteur,argument);
    
#ifdef DEBUG_PRINTF
    printf("%s\n",message);
#endif
    
    if (sendto(sockfd, message, BUFLEN, 0, (struct sockaddr*)&serv_addr, slen)== -1) {
        
        perror("sendto() ");
        
        return 0; // ÉCHEC
    }
    
    usleep(DELAI_MICROSECONDES);
    return 1; // SUCCÈS
}
