#include "../include/process_manager.h"
#include "../include/machine_manager.h"

#include <stdio.h>
#include <stdlib.h>

#define FICHIER_TEMP_PROCESSUS "data/processes.tmp"

static int processus_est_valide(const processus *processus_a_verifier)
{
    return processus_a_verifier->id > 0 &&
           processus_a_verifier->arrival_time >= 0 &&
           processus_a_verifier->duration > 0 &&
           processus_a_verifier->priority >= 0 &&
           processus_a_verifier->required_ram > 0;
}

static int lire_processus_ligne(const char *ligne, processus *processus_lu)
{
    return sscanf(ligne, "%d %49s %d %d %d %lf %d", &processus_lu->id,
                  processus_lu->name, &processus_lu->arrival_time,
                  &processus_lu->duration, &processus_lu->priority,
                  &processus_lu->required_ram, &processus_lu->machine_id) == 7;
}

static void ecrire_processus(FILE *fichier, const processus *processus_a_ecrire)
{
    fprintf(fichier, "%d %s %d %d %d %.2f %d\n", processus_a_ecrire->id,
            processus_a_ecrire->name, processus_a_ecrire->arrival_time,
            processus_a_ecrire->duration, processus_a_ecrire->priority,
            processus_a_ecrire->required_ram, processus_a_ecrire->machine_id);
}

int processus_existe(int id)
{
    FILE *fichier = fopen(CHEMIN_PROCESS_FILE, "r");
    char ligne[256];
    processus processus_lu;

    if (fichier == NULL) return 0;

    while (fgets(ligne, sizeof(ligne), fichier) != NULL) {
        if (lire_processus_ligne(ligne, &processus_lu) && processus_lu.id == id) {
            fclose(fichier);
            return 1;
        }
    }

    fclose(fichier);
    return 0;
}

int machine_a_des_processus(int machine_id)
{
    FILE *fichier = fopen(CHEMIN_PROCESS_FILE, "r");
    char ligne[256];
    processus processus_lu;

    if (fichier == NULL) return 0;

    while (fgets(ligne, sizeof(ligne), fichier) != NULL) {
        if (lire_processus_ligne(ligne, &processus_lu) &&
            processus_lu.machine_id == machine_id) {
            fclose(fichier);
            return 1;
        }
    }

    fclose(fichier);
    return 0;
}

void afficher_process(int id)
{
    FILE *fichier = fopen(CHEMIN_PROCESS_FILE, "r");
    char ligne[256];
    processus processus_lu;

    if (fichier == NULL) {
        perror("Ouverture des processus");
        return;
    }

    while (fgets(ligne, sizeof(ligne), fichier) != NULL) {
        if (lire_processus_ligne(ligne, &processus_lu) && processus_lu.id == id) {
            printf("\nProcessus %d : %s | Arrivee: %d | Duree: %d | ",
                   processus_lu.id, processus_lu.name, processus_lu.arrival_time,
                   processus_lu.duration);
            printf("Priorite: %d | RAM: %.2f Go | Machine: %d\n",
                   processus_lu.priority, processus_lu.required_ram,
                   processus_lu.machine_id);
            fclose(fichier);
            return;
        }
    }

    fclose(fichier);
    printf("Processus %d introuvable.\n", id);
}

void ajouter_process(processus processus_a_ajouter)
{
    FILE *fichier;

    if (!processus_est_valide(&processus_a_ajouter)) {
        printf("Donnees de processus invalides.\n");
        return;
    }
    if (processus_existe(processus_a_ajouter.id)) {
        printf("Ce processus existe deja.\n");
        return;
    }
    if (!machine_existe(processus_a_ajouter.machine_id)) {
        printf("Machine associee introuvable.\n");
        return;
    }

    fichier = fopen(CHEMIN_PROCESS_FILE, "a");
    if (fichier == NULL) {
        perror("Ouverture des processus");
        return;
    }

    ecrire_processus(fichier, &processus_a_ajouter);
    fclose(fichier);
    printf("Processus ajoute.\n");
}

void modifier_process(processus processus_a_modifier)
{
    FILE *fichier;
    FILE *fichier_temporaire;
    char ligne[256];
    processus processus_lu;
    int trouve = 0;

    if (!processus_est_valide(&processus_a_modifier)) {
        printf("Donnees de processus invalides.\n");
        return;
    }
    if (!machine_existe(processus_a_modifier.machine_id)) {
        printf("Machine associee introuvable.\n");
        return;
    }

    fichier = fopen(CHEMIN_PROCESS_FILE, "r");
    fichier_temporaire = fopen(FICHIER_TEMP_PROCESSUS, "w");
    if (fichier == NULL || fichier_temporaire == NULL) {
        perror("Modification de processus");
        if (fichier != NULL) fclose(fichier);
        if (fichier_temporaire != NULL) fclose(fichier_temporaire);
        return;
    }

    while (fgets(ligne, sizeof(ligne), fichier) != NULL) {
        if (lire_processus_ligne(ligne, &processus_lu) &&
            processus_lu.id == processus_a_modifier.id) {
            ecrire_processus(fichier_temporaire, &processus_a_modifier);
            trouve = 1;
        } else {
            fputs(ligne, fichier_temporaire);
        }
    }

    fclose(fichier);
    fclose(fichier_temporaire);

    if (!trouve) {
        remove(FICHIER_TEMP_PROCESSUS);
        printf("Processus %d introuvable.\n", processus_a_modifier.id);
        return;
    }
    if (remove(CHEMIN_PROCESS_FILE) != 0 ||
        rename(FICHIER_TEMP_PROCESSUS, CHEMIN_PROCESS_FILE) != 0) {
        perror("Sauvegarde de processus");
        return;
    }

    printf("Processus modifie.\n");
}

void supprimer_process(int id)
{
    FILE *fichier = fopen(CHEMIN_PROCESS_FILE, "r");
    FILE *fichier_temporaire = fopen(FICHIER_TEMP_PROCESSUS, "w");
    char ligne[256];
    processus processus_lu;
    int trouve = 0;

    if (fichier == NULL || fichier_temporaire == NULL) {
        perror("Suppression de processus");
        if (fichier != NULL) fclose(fichier);
        if (fichier_temporaire != NULL) fclose(fichier_temporaire);
        return;
    }

    while (fgets(ligne, sizeof(ligne), fichier) != NULL) {
        if (lire_processus_ligne(ligne, &processus_lu) && processus_lu.id == id) {
            trouve = 1;
        } else {
            fputs(ligne, fichier_temporaire);
        }
    }

    fclose(fichier);
    fclose(fichier_temporaire);

    if (!trouve) {
        remove(FICHIER_TEMP_PROCESSUS);
        printf("Processus %d introuvable.\n", id);
        return;
    }
    if (remove(CHEMIN_PROCESS_FILE) != 0 ||
        rename(FICHIER_TEMP_PROCESSUS, CHEMIN_PROCESS_FILE) != 0) {
        perror("Sauvegarde de processus");
        return;
    }

    printf("Processus supprime.\n");
}

void lister_process(void)
{
    FILE *fichier = fopen(CHEMIN_PROCESS_FILE, "r");
    char ligne[256];
    processus processus_lu;

    if (fichier == NULL) {
        perror("Ouverture des processus");
        return;
    }

    printf("\n+----+--------------------+---------+-------+----------+-----------+---------+\n");
    printf("| ID | Nom                | Arrivee | Duree | Priorite | RAM (Go)  | Machine |\n");
    printf("+----+--------------------+---------+-------+----------+-----------+---------+\n");

    while (fgets(ligne, sizeof(ligne), fichier) != NULL) {
        if (lire_processus_ligne(ligne, &processus_lu)) {
            printf("| %-2d | %-18s | %-7d | %-5d | %-8d | %-9.2f | %-7d |\n",
                   processus_lu.id, processus_lu.name, processus_lu.arrival_time,
                   processus_lu.duration, processus_lu.priority,
                   processus_lu.required_ram, processus_lu.machine_id);
        }
    }

    printf("+----+--------------------+---------+-------+----------+-----------+---------+\n");
    fclose(fichier);
}
