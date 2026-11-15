#ifndef MACHINE_MANAGER_H
#define MACHINE_MANAGER_H

#include "structure.h"

#define CHEMIN_MACHINE_FILE "data/machines.txt"

void afficher_machine(int id);
void ajouter_machine(machine machine_a_ajouter);
void modifier_machine(machine machine_a_modifier);
void supprimer_machine(int id);
void lister_machines(void);
int machine_existe(int id);

#endif
