/* Nom: aborbeur.h
 * Description: lecture et test de l'entité absorbeur
 * Date: 21.05.2016
 * version : 3.0
 * Auteur: David Sanchez Del Rio, Frank Dupont
 */

#include <stdio.h>
#include <stdbool.h>
#include "utilitaire.h"
#include "constantes.h"
 
#ifndef ABSORBEUR_H
#define ABSORBEUR_H

typedef struct absorbeur ABSORBEUR;

bool absorbeur_lecture_nb(char* ligne, int* nb_abso);

bool absorbeur_lecture(char* ligne);

int absorbeur_info(void);

void absorbeur_destruction(void);

void absorbeur_dessine_tout(void);

void absorbeur_dessine(ABSORBEUR* p);

void absorbeur_def_limites(float *x_min, float *y_min, float *x_max, 
												float *y_max, bool *def);

void absorbeur_save(FILE *fichier);

POINT get_point_abs(int i, int j);

VECTEUR get_abs(int i, int j);

ABSORBEUR* get_pointeur_abs(ABSORBEUR* courant);

int get_nbp(int a);

ABSORBEUR* get_tete_abs(void);

void absorbeur_creation(POINT souris_p, POINT souris, int* compteur);

void absorbeur_selection(POINT souris);

void absorbeur_suppression_selec(void);

void enlever_tete_abs(void);

#endif
