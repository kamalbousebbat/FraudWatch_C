#ifndef PROCESS_MANAGER_H
#define PROCESS_MANAGER_H

#include "structure.h"

#define CHEMIN_PROCESS_FILE "data/processes.txt"

void afficher_process(int id);
void ajouter_process(processus processus_a_ajouter);
void modifier_process(processus processus_a_modifier);
void supprimer_process(int id);
void lister_process(void);
int processus_existe(int id);
int machine_a_des_processus(int machine_id);

#endif
