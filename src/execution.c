#include "../include/execution.h"
#include "../include/machine_manager.h"
#include "../include/process_manager.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define FORMAT_CHEMIN_HISTORIQUE "data/historique_machine_%d.txt"

static int lire_machine_ligne(const char *ligne, machine *machine_lue)
{
    int statut;

    if (sscanf(ligne, "%d %49s %d %lf %d", &machine_lue->id,
               machine_lue->name, &machine_lue->cpu_cores, &machine_lue->Ram,
               &statut) != 5) {
        return 0;
    }

    machine_lue->status = (MachineStatus)statut;
    return statut == AVAILABLE || statut == UNAVAILABLE;
}

static int lire_processus_ligne(const char *ligne, processus *processus_lu)
{
    return sscanf(ligne, "%d %49s %d %d %d %lf %d", &processus_lu->id,
                  processus_lu->name, &processus_lu->arrival_time,
                  &processus_lu->duration, &processus_lu->priority,
                  &processus_lu->required_ram, &processus_lu->machine_id) == 7;
}

static int comparer_processus(const void *premier, const void *second)
{
    const processus *processus_a = premier;
    const processus *processus_b = second;

    if (processus_a->arrival_time != processus_b->arrival_time) {
        return processus_a->arrival_time - processus_b->arrival_time;
    }
    if (processus_a->priority != processus_b->priority) {
        return processus_a->priority - processus_b->priority;
    }

    return processus_a->id - processus_b->id;
}

static int ajouter_processus_a_liste(processus **liste, size_t *nombre,
                                      size_t *capacite, processus processus_a_ajouter)
{
    if (*nombre == *capacite) {
        size_t nouvelle_capacite = *capacite == 0 ? 8 : *capacite * 2;
        processus *nouvelle_liste = realloc(*liste,
                                            nouvelle_capacite * sizeof(**liste));

        if (nouvelle_liste == NULL) {
            return 0;
        }

        *liste = nouvelle_liste;
        *capacite = nouvelle_capacite;
    }

    (*liste)[(*nombre)++] = processus_a_ajouter;
    return 1;
}

static void afficher_entete_table(FILE *sortie)
{
    fprintf(sortie, "+----+--------------------+---------+-------+-----------+------------+------------+\n");
    fprintf(sortie, "| ID | Processus          | Arrivee | Duree | Debut     | Fin        | Etat       |\n");
    fprintf(sortie, "+----+--------------------+---------+-------+-----------+------------+------------+\n");
}

static void afficher_resultat_processus(FILE *sortie, const processus *processus_lu,
                                        const Execution *execution)
{
    if (execution->status == COMPLETED) {
        fprintf(sortie, "| %-2d | %-18s | %-7d | %-5d | %-9d | %-10d | TERMINE    |\n",
                processus_lu->id, processus_lu->name, processus_lu->arrival_time,
                processus_lu->duration, execution->start_time, execution->end_time);
    } else {
        fprintf(sortie, "| %-2d | %-18s | %-7d | %-5d | %-9s | %-10s | ECHEC      |\n",
                processus_lu->id, processus_lu->name, processus_lu->arrival_time,
                processus_lu->duration, "-", "-");
    }
}

static void afficher_statistiques(FILE *sortie, size_t total, size_t termines,
                                  int duree_min, int duree_max, int somme_durees)
{
    if (total == 0) {
        fprintf(sortie, "Statistiques : aucun processus.\n");
        return;
    }

    fprintf(sortie,
            "Statistiques : total=%zu | termines=%zu | echecs=%zu | "
            "duree min=%d | max=%d | moyenne=%.2f\n",
            total, termines, total - termines, duree_min, duree_max,
            (double)somme_durees / total);
}

static void ecrire_entete_historique(FILE *historique, const machine *machine_lue)
{
    time_t maintenant = time(NULL);
    struct tm *date_locale = localtime(&maintenant);

    fprintf(historique, "\n===== EXECUTION");
    if (date_locale != NULL) {
        fprintf(historique, " DU %04d-%02d-%02d %02d:%02d:%02d",
                date_locale->tm_year + 1900, date_locale->tm_mon + 1,
                date_locale->tm_mday, date_locale->tm_hour,
                date_locale->tm_min, date_locale->tm_sec);
    }
    fprintf(historique, " =====\n");
    fprintf(historique, "Machine %d - %s (%s, RAM %.2f Go)\n", machine_lue->id,
            machine_lue->name,
            machine_lue->status == AVAILABLE ? "DISPONIBLE" : "INDISPONIBLE",
            machine_lue->Ram);
}

void afficher_historique_machine(int machine_id)
{
    char chemin_historique[128];
    char ligne[256];
    FILE *historique;

    if (!machine_existe(machine_id)) {
        printf("Machine %d introuvable.\n", machine_id);
        return;
    }

    snprintf(chemin_historique, sizeof(chemin_historique),
             FORMAT_CHEMIN_HISTORIQUE, machine_id);
    historique = fopen(chemin_historique, "r");
    if (historique == NULL) {
        printf("Aucun historique d'execution pour la machine %d.\n", machine_id);
        return;
    }

    printf("\n===== HISTORIQUE DE LA MACHINE %d =====\n", machine_id);
    while (fgets(ligne, sizeof(ligne), historique) != NULL) {
        fputs(ligne, stdout);
    }
    fclose(historique);
}

void execute(void)
{
    FILE *fichier_machines = fopen(CHEMIN_MACHINE_FILE, "r");
    FILE *fichier_processus = fopen(CHEMIN_PROCESS_FILE, "r");
    char ligne[256];
    machine machine_lue;

    if (fichier_machines == NULL || fichier_processus == NULL) {
        perror("Chargement pour execution");
        if (fichier_machines != NULL) fclose(fichier_machines);
        if (fichier_processus != NULL) fclose(fichier_processus);
        return;
    }

    printf("\n===== RESULTAT DE L'EXECUTION =====\n");

    while (fgets(ligne, sizeof(ligne), fichier_machines) != NULL) {
        processus *liste_processus = NULL;
        FILE *historique;
        char chemin_historique[128];
        size_t nombre_processus = 0;
        size_t capacite = 0;
        size_t index;
        size_t nombre_termines = 0;
        int temps_courant = 0;
        int duree_min = 0;
        int duree_max = 0;
        int somme_durees = 0;

        if (!lire_machine_ligne(ligne, &machine_lue)) {
            continue;
        }

        snprintf(chemin_historique, sizeof(chemin_historique),
                 FORMAT_CHEMIN_HISTORIQUE, machine_lue.id);
        historique = fopen(chemin_historique, "a");
        if (historique == NULL) {
            perror("Ouverture de l'historique");
        } else {
            ecrire_entete_historique(historique, &machine_lue);
        }

        rewind(fichier_processus);
        while (fgets(ligne, sizeof(ligne), fichier_processus) != NULL) {
            processus processus_lu;

            if (!lire_processus_ligne(ligne, &processus_lu) ||
                processus_lu.machine_id != machine_lue.id) {
                continue;
            }

            if (!ajouter_processus_a_liste(&liste_processus, &nombre_processus,
                                            &capacite, processus_lu)) {
                free(liste_processus);
                if (historique != NULL) fclose(historique);
                fclose(fichier_machines);
                fclose(fichier_processus);
                perror("Memoire");
                return;
            }
        }

        qsort(liste_processus, nombre_processus, sizeof(*liste_processus),
              comparer_processus);

        printf("\nMachine %d - %s (%s, RAM %.2f Go)\n", machine_lue.id,
               machine_lue.name,
               machine_lue.status == AVAILABLE ? "DISPONIBLE" : "INDISPONIBLE",
               machine_lue.Ram);
        afficher_entete_table(stdout);
        if (historique != NULL) {
            afficher_entete_table(historique);
        }

        if (nombre_processus == 0) {
            printf("|                         Aucun processus associe                          |\n");
            if (historique != NULL) {
                fprintf(historique, "|                         Aucun processus associe                          |\n");
            }
        }

        for (index = 0; index < nombre_processus; ++index) {
            Execution execution = {liste_processus[index].id, machine_lue.id,
                                   -1, -1, FAILED};

            if (index == 0 || liste_processus[index].duration < duree_min) {
                duree_min = liste_processus[index].duration;
            }
            if (liste_processus[index].duration > duree_max) {
                duree_max = liste_processus[index].duration;
            }
            somme_durees += liste_processus[index].duration;

            if (machine_lue.status == AVAILABLE &&
                liste_processus[index].required_ram <= machine_lue.Ram) {
                if (temps_courant < liste_processus[index].arrival_time) {
                    temps_courant = liste_processus[index].arrival_time;
                }

                execution.start_time = temps_courant;
                execution.end_time = temps_courant + liste_processus[index].duration;
                execution.status = COMPLETED;
                temps_courant = execution.end_time;
                ++nombre_termines;
            }

            afficher_resultat_processus(stdout, &liste_processus[index], &execution);
            if (historique != NULL) {
                afficher_resultat_processus(historique, &liste_processus[index],
                                            &execution);
            }
        }

        printf("+----+--------------------+---------+-------+-----------+------------+------------+\n");
        afficher_statistiques(stdout, nombre_processus, nombre_termines,
                              duree_min, duree_max, somme_durees);
        if (historique != NULL) {
            fprintf(historique, "+----+--------------------+---------+-------+-----------+------------+------------+\n");
            afficher_statistiques(historique, nombre_processus, nombre_termines,
                                  duree_min, duree_max, somme_durees);
            fclose(historique);
        }
        free(liste_processus);
    }

    fclose(fichier_machines);
    fclose(fichier_processus);
}
