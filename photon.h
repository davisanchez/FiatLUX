/* Nom: photon.h
 * Description: lecture et test de l'entité photon
 * Date: 21.05.2016
 * version : 3.0
 * Auteur: David Sanchez Del Rio, Frank Dupont
 */
 
#include <stdio.h>
#include <stdbool.h>
#include "utilitaire.h"
 
#ifndef PHOTON_H
#define PHOTON_H


typedef struct photon PHOTON;

bool photon_lecture_nb(char* ligne, int* nb_phot);

bool photon_lecture(char* ligne);

int photon_info(void);

void photon_save(FILE *fichier);

void photon_destruction(void);

void photon_dessine(void);

void photon_def_limites(float *x_min, float *y_min, float *x_max, 
												float *y_max, bool *def);
				
VECTEUR photon_vecteur(PHOTON* phot);

void photon_sim(void);

void photon_inter(PHOTON* temp, int prec, float l);

void destruction_photon(PHOTON* p);

void projection_photon(double projx, double projy, double angle);

PHOTON* get_tete_phot(void);

PHOTON* creation_photon(PHOTON * tetedeliste, POINT position, 
														double alpha);

void photon_transfert(POINT pos, double alpha);

void photon_beyond(float x_min, float x_max, float y_min, float y_max);

#endif
