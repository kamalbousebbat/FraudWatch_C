#include "../include/machine_manager.h"
#include "../include/process_manager.h"

#include <stdio.h>
#include <stdlib.h>

#define FICHIER_TEMP_MACHINES "data/machines.tmp"

static const char *obtenir_statut_machine(MachineStatus status)
{
    return status == AVAILABLE ? "DISPONIBLE" : "INDISPONIBLE";
}

static int machine_est_valide(const machine *machine_a_verifier)
{
    return machine_a_verifier->id > 0 &&
           machine_a_verifier->cpu_cores > 0 &&
           machine_a_verifier->Ram > 0 &&
           (machine_a_verifier->status == AVAILABLE ||
            machine_a_verifier->status == UNAVAILABLE);
}

static int lire_machine_ligne(const char *ligne, machine *machine_lue)
{
    int statut;
    int champs_lus;

    champs_lus = sscanf(ligne, "%d %49s %d %lf %d", &machine_lue->id,
                         machine_lue->name, &machine_lue->cpu_cores,
                         &machine_lue->Ram, &statut);

    if (champs_lus != 5 || (statut != AVAILABLE && statut != UNAVAILABLE)) {
        return 0;
    }

    machine_lue->status = (MachineStatus)statut;
    return 1;
}

static void ecrire_machine(FILE *fichier, const machine *machine_a_ecrire)
{
    fprintf(fichier, "%d %s %d %.2f %d\n", machine_a_ecrire->id,
            machine_a_ecrire->name, machine_a_ecrire->cpu_cores,
            machine_a_ecrire->Ram, machine_a_ecrire->status);
}

int machine_existe(int id)
{
    FILE *fichier = fopen(CHEMIN_MACHINE_FILE, "r");
    char ligne[256];
    machine machine_lue;

    if (fichier == NULL) {
        return 0;
    }

    while (fgets(ligne, sizeof(ligne), fichier) != NULL) {
        if (lire_machine_ligne(ligne, &machine_lue) && machine_lue.id == id) {
            fclose(fichier);
            return 1;
        }
    }

    fclose(fichier);
    return 0;
}

void afficher_machine(int id)
{
    FILE *fichier = fopen(CHEMIN_MACHINE_FILE, "r");
    char ligne[256];
    machine machine_lue;

    if (fichier == NULL) {
        perror("Ouverture des machines");
        return;
    }

    while (fgets(ligne, sizeof(ligne), fichier) != NULL) {
        if (lire_machine_ligne(ligne, &machine_lue) && machine_lue.id == id) {
            printf("\nMachine %d : %s | CPU: %d | RAM: %.2f Go | %s\n",
                   machine_lue.id, machine_lue.name, machine_lue.cpu_cores,
                   machine_lue.Ram, obtenir_statut_machine(machine_lue.status));
            fclose(fichier);
            return;
        }
    }

    fclose(fichier);
    printf("Machine %d introuvable.\n", id);
}

void ajouter_machine(machine machine_a_ajouter)
{
    FILE *fichier;

    if (!machine_est_valide(&machine_a_ajouter)) {
        printf("Donnees de machine invalides.\n");
        return;
    }

    if (machine_existe(machine_a_ajouter.id)) {
        printf("Cette machine existe deja.\n");
        return;
    }

    fichier = fopen(CHEMIN_MACHINE_FILE, "a");
    if (fichier == NULL) {
        perror("Ouverture des machines");
        return;
    }

    ecrire_machine(fichier, &machine_a_ajouter);
    fclose(fichier);
    printf("Machine ajoutee.\n");
}

void modifier_machine(machine machine_a_modifier)
{
    FILE *fichier;
    FILE *fichier_temporaire;
    char ligne[256];
    machine machine_lue;
    int trouve = 0;

    if (!machine_est_valide(&machine_a_modifier)) {
        printf("Donnees de machine invalides.\n");
        return;
    }

    fichier = fopen(CHEMIN_MACHINE_FILE, "r");
    fichier_temporaire = fopen(FICHIER_TEMP_MACHINES, "w");

    if (fichier == NULL || fichier_temporaire == NULL) {
        perror("Modification de machine");
        if (fichier != NULL) fclose(fichier);
        if (fichier_temporaire != NULL) fclose(fichier_temporaire);
        return;
    }

    while (fgets(ligne, sizeof(ligne), fichier) != NULL) {
        if (lire_machine_ligne(ligne, &machine_lue) &&
            machine_lue.id == machine_a_modifier.id) {
            ecrire_machine(fichier_temporaire, &machine_a_modifier);
            trouve = 1;
        } else {
            fputs(ligne, fichier_temporaire);
        }
    }

    fclose(fichier);
    fclose(fichier_temporaire);

    if (!trouve) {
        remove(FICHIER_TEMP_MACHINES);
        printf("Machine %d introuvable.\n", machine_a_modifier.id);
        return;
    }

    if (remove(CHEMIN_MACHINE_FILE) != 0 ||
        rename(FICHIER_TEMP_MACHINES, CHEMIN_MACHINE_FILE) != 0) {
        perror("Sauvegarde de machine");
        return;
    }

    printf("Machine modifiee.\n");
}

void supprimer_machine(int id)
{
    FILE *fichier;
    FILE *fichier_temporaire;
    char ligne[256];
    machine machine_lue;
    int trouve = 0;

    if (machine_a_des_processus(id)) {
        printf("Suppression impossible : des processus sont associes a cette machine.\n");
        return;
    }

    fichier = fopen(CHEMIN_MACHINE_FILE, "r");
    fichier_temporaire = fopen(FICHIER_TEMP_MACHINES, "w");

    if (fichier == NULL || fichier_temporaire == NULL) {
        perror("Suppression de machine");
        if (fichier != NULL) fclose(fichier);
        if (fichier_temporaire != NULL) fclose(fichier_temporaire);
        return;
    }

    while (fgets(ligne, sizeof(ligne), fichier) != NULL) {
        if (lire_machine_ligne(ligne, &machine_lue) && machine_lue.id == id) {
            trouve = 1;
        } else {
            fputs(ligne, fichier_temporaire);
        }
    }

    fclose(fichier);
    fclose(fichier_temporaire);

    if (!trouve) {
        remove(FICHIER_TEMP_MACHINES);
        printf("Machine %d introuvable.\n", id);
        return;
    }

    if (remove(CHEMIN_MACHINE_FILE) != 0 ||
        rename(FICHIER_TEMP_MACHINES, CHEMIN_MACHINE_FILE) != 0) {
        perror("Sauvegarde de machine");
        return;
    }

    printf("Machine supprimee.\n");
}

void lister_machines(void)
{
    FILE *fichier = fopen(CHEMIN_MACHINE_FILE, "r");
    char ligne[256];
    machine machine_lue;

    if (fichier == NULL) {
        perror("Ouverture des machines");
        return;
    }

    printf("\n+----+--------------------+------+-----------+----------------+\n");
    printf("| ID | Nom                | CPU  | RAM (Go)  | Etat           |\n");
    printf("+----+--------------------+------+-----------+----------------+\n");

    while (fgets(ligne, sizeof(ligne), fichier) != NULL) {
        if (lire_machine_ligne(ligne, &machine_lue)) {
            printf("| %-2d | %-18s | %-4d | %-9.2f | %-14s |\n",
                   machine_lue.id, machine_lue.name, machine_lue.cpu_cores,
                   machine_lue.Ram, obtenir_statut_machine(machine_lue.status));
        }
    }

    printf("+----+--------------------+------+-----------+----------------+\n");
    fclose(fichier);
}
