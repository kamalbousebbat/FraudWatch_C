#include "../include/execution.h"
#include "../include/machine_manager.h"
#include "../include/process_manager.h"

#include <stdio.h>

static int lire_entier(const char *message, int *valeur)
{
    char ligne[128];
    char caractere_supplementaire;

    printf("%s", message);

    if (fgets(ligne, sizeof(ligne), stdin) == NULL) {
        return 0;
    }

    return sscanf(ligne, "%d %c", valeur, &caractere_supplementaire) == 1;
}

static int lire_reel(const char *message, double *valeur)
{
    char ligne[128];
    char caractere_supplementaire;

    printf("%s", message);

    if (fgets(ligne, sizeof(ligne), stdin) == NULL) {
        return 0;
    }

    return sscanf(ligne, "%lf %c", valeur, &caractere_supplementaire) == 1;
}

static int lire_nom(const char *message, char *nom)
{
    char ligne[128];

    printf("%s", message);

    if (fgets(ligne, sizeof(ligne), stdin) == NULL) {
        return 0;
    }

    return sscanf(ligne, "%49s", nom) == 1;
}

static int saisir_machine(machine *machine_saisie, int conserver_id)
{
    int statut;

    if (!conserver_id && !lire_entier("ID : ", &machine_saisie->id)) {
        return 0;
    }

    if (!lire_nom("Nom (sans espace) : ", machine_saisie->name) ||
        !lire_entier("Nombre de coeurs CPU : ", &machine_saisie->cpu_cores) ||
        !lire_reel("RAM en Go : ", &machine_saisie->Ram) ||
        !lire_entier("Etat (0=disponible, 1=indisponible) : ", &statut)) {
        return 0;
    }

    if (statut != AVAILABLE && statut != UNAVAILABLE) {
        return 0;
    }

    machine_saisie->status = (MachineStatus)statut;
    return 1;
}

static int saisir_processus(processus *processus_saisi, int conserver_id)
{
    if (!conserver_id && !lire_entier("ID : ", &processus_saisi->id)) {
        return 0;
    }

    return lire_nom("Nom (sans espace) : ", processus_saisi->name) &&
           lire_entier("Temps d'arrivee : ", &processus_saisi->arrival_time) &&
           lire_entier("Duree : ", &processus_saisi->duration) &&
           lire_entier("Priorite : ", &processus_saisi->priority) &&
           lire_reel("RAM requise en Go : ", &processus_saisi->required_ram) &&
           lire_entier("ID de la machine : ", &processus_saisi->machine_id);
}

static void afficher_menu_machines(void)
{
    printf("\n--- GESTION DES MACHINES ---\n");
    printf("1. Lister\n2. Afficher\n3. Ajouter\n4. Modifier\n5. Supprimer\n0. Retour\n");
}

static void menu_machines(void)
{
    int choix = -1;

    while (choix != 0) {
        afficher_menu_machines();

        if (!lire_entier("Choix : ", &choix)) {
            printf("Choix invalide.\n");
            continue;
        }

        switch (choix) {
            case 1:
                lister_machines();
                break;
            case 2: {
                int id;
                if (lire_entier("ID : ", &id)) afficher_machine(id);
                break;
            }
            case 3: {
                machine nouvelle_machine;
                if (saisir_machine(&nouvelle_machine, 0)) ajouter_machine(nouvelle_machine);
                else printf("Saisie invalide.\n");
                break;
            }
            case 4: {
                machine machine_modifiee;
                if (lire_entier("ID de la machine a modifier : ", &machine_modifiee.id) &&
                    saisir_machine(&machine_modifiee, 1)) modifier_machine(machine_modifiee);
                else printf("Saisie invalide.\n");
                break;
            }
            case 5: {
                int id;
                if (lire_entier("ID : ", &id)) supprimer_machine(id);
                break;
            }
            case 0:
                break;
            default:
                printf("Choix invalide.\n");
        }
    }
}

static void afficher_menu_processus(void)
{
    printf("\n--- GESTION DES PROCESSUS ---\n");
    printf("1. Lister\n2. Afficher\n3. Ajouter\n4. Modifier\n5. Supprimer\n0. Retour\n");
}

static void menu_processus(void)
{
    int choix = -1;

    while (choix != 0) {
        afficher_menu_processus();

        if (!lire_entier("Choix : ", &choix)) {
            printf("Choix invalide.\n");
            continue;
        }

        switch (choix) {
            case 1:
                lister_process();
                break;
            case 2: {
                int id;
                if (lire_entier("ID : ", &id)) afficher_process(id);
                break;
            }
            case 3: {
                processus nouveau_processus;
                if (saisir_processus(&nouveau_processus, 0)) ajouter_process(nouveau_processus);
                else printf("Saisie invalide.\n");
                break;
            }
            case 4: {
                processus processus_modifie;
                if (lire_entier("ID du processus a modifier : ", &processus_modifie.id) &&
                    saisir_processus(&processus_modifie, 1)) modifier_process(processus_modifie);
                else printf("Saisie invalide.\n");
                break;
            }
            case 5: {
                int id;
                if (lire_entier("ID : ", &id)) supprimer_process(id);
                break;
            }
            case 0:
                break;
            default:
                printf("Choix invalide.\n");
        }
    }
}

int main(void)
{
    int choix = -1;

    while (choix != 0) {
        printf("\n========== FRAUDWATCH ==========\n");
        printf("1. Gestion des machines\n2. Gestion des processus\n3. Execution\n0. Quitter\n");

        if (!lire_entier("Choix : ", &choix)) {
            printf("Choix invalide.\n");
            continue;
        }

        switch (choix) {
            case 1:
                menu_machines();
                break;
            case 2:
                menu_processus();
                break;
            case 3:
                execute();
                break;
            case 0:
                break;
            default:
                printf("Choix invalide.\n");
        }
    }

    printf("Au revoir.\n");
    return 0;
}
