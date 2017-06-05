/* Nom: reflecteur.h
 * Description: lecture et test de l'entité reflecteur
 * Date: 21.05.2016
 * version : 3.0
 * Auteur: David Sanchez Del Rio, Frank Dupont
 */
 
#include <stdio.h>
#include <stdbool.h>
#include "utilitaire.h"
 
#ifndef REFLECTEUR_H
#define REFLECTEUR_H

typedef struct reflecteur REFLECTEUR;

bool reflecteur_lecture_nb(char* ligne, int* nb_refl);

bool reflecteur_lecture(char* ligne, int *i);

int reflecteur_info(void);

void reflecteur_save(FILE *fichier);

void reflecteur_dessine_tout(void);

void reflecteur_dessine(REFLECTEUR* p);

void reflecteur_destruction(void);

void reflecteur_def_limites(float *x_min, float *y_min, float *x_max, 
												float *y_max, bool *def);


int verification_reflecteur_reflecteur();

VECTEUR reflecteur_get_vecteur(int a);

REFLECTEUR* get_pointeur_refl(REFLECTEUR* courant);

int get_id_refl(int a);

void reflecteur_creation(POINT souris_p, POINT souris);

void reflecteur_selection(POINT souris);

void reflecteur_suppression_selec(void);

void enlever_tete_refl(void);

#endif
