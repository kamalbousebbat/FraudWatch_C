#include "../include/execution.h"
#include "../include/machine_manager.h"
#include "../include/process_manager.h"

#include <stdio.h>
#include <stdlib.h>

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

static void afficher_entete_table(void)
{
    printf("+----+--------------------+---------+-------+-----------+------------+------------+\n");
    printf("| ID | Processus          | Arrivee | Duree | Debut     | Fin        | Etat       |\n");
    printf("+----+--------------------+---------+-------+-----------+------------+------------+\n");
}

static void afficher_resultat_processus(const processus *processus_lu,
                                        const Execution *execution)
{
    if (execution->status == COMPLETED) {
        printf("| %-2d | %-18s | %-7d | %-5d | %-9d | %-10d | TERMINE    |\n",
               processus_lu->id, processus_lu->name, processus_lu->arrival_time,
               processus_lu->duration, execution->start_time, execution->end_time);
    } else {
        printf("| %-2d | %-18s | %-7d | %-5d | %-9s | %-10s | ECHEC      |\n",
               processus_lu->id, processus_lu->name, processus_lu->arrival_time,
               processus_lu->duration, "-", "-");
    }
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
        size_t nombre_processus = 0;
        size_t capacite = 0;
        size_t index;
        int temps_courant = 0;

        if (!lire_machine_ligne(ligne, &machine_lue)) {
            continue;
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
        afficher_entete_table();

        if (nombre_processus == 0) {
            printf("|                         Aucun processus associe                          |\n");
        }

        for (index = 0; index < nombre_processus; ++index) {
            Execution execution = {liste_processus[index].id, machine_lue.id,
                                   -1, -1, FAILED};

            if (machine_lue.status == AVAILABLE &&
                liste_processus[index].required_ram <= machine_lue.Ram) {
                if (temps_courant < liste_processus[index].arrival_time) {
                    temps_courant = liste_processus[index].arrival_time;
                }

                execution.start_time = temps_courant;
                execution.end_time = temps_courant + liste_processus[index].duration;
                execution.status = COMPLETED;
                temps_courant = execution.end_time;
            }

            afficher_resultat_processus(&liste_processus[index], &execution);
        }

        printf("+----+--------------------+---------+-------+-----------+------------+------------+\n");
        free(liste_processus);
    }

    fclose(fichier_machines);
    fclose(fichier_processus);
}
