#include "../include/machine_manager.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


void afficher_machine(int id) {
    FILE *file = fopen("data/machines.txt", "r");
    if (file == NULL) {
        printf("Error opening file\n");
        return;
    }
    char line[256];
    while(fgets(line,sizeof(line),file) != NULL){
        char *mot ;
        mot = strtok(line , " \n");
        if(mot!=NULL){
            int id_machine = atoi(mot);
            if (id_machine == id){
                printf("Machine ID: %d\n", id);
                mot = strtok(NULL, " \n");
                printf("Machine Name: %s\n", mot);
                mot = strtok(NULL, " \n");
                printf("CPU Cores: %d\n", atoi(mot));
                mot = strtok(NULL, " \n");
                printf("RAM: %.2f\n", atof(mot));
                mot = strtok(NULL, " \n");
                if(atoi(mot) == AVAILABLE)
                    mot = "AVAILABLE";
                else
                    mot = "UNAVAILABLE";
                printf("Status: %s\n", mot);
                break;
            }
        }
    }
    fclose(file);
}

void ajouter_machine(machine m){
    FILE *file = fopen("data/machines.txt", "a");
    if (file == NULL) {
        printf("Error opening file\n");
        return;
    }
    fprintf(file, "%d %s %d %.2f %d\n", m.id, m.name, m.cpu_cores, m.Ram, m.status);
    fclose(file);
}

void modifier_machine(machine m) {

    FILE *file = fopen("data/machines.txt", "r");
    FILE *temp_file = fopen("../data/temp.txt", "w");

    if (file == NULL || temp_file == NULL) {
        perror("fopen");

        if (file) fclose(file);
        if (temp_file) fclose(temp_file);

        remove("../data/temp.txt");
        return;
    }

    char line[256];
    int id;

    while (fgets(line, sizeof(line), file) != NULL) {

        if (sscanf(line, "%d", &id) != 1) {
            fputs(line, temp_file);
            continue;
        }

        if (id == m.id) {
            fprintf(temp_file, "%d %s %d %.2f %d\n",
                    m.id, m.name, m.cpu_cores, m.Ram, m.status);
        }
        else {
            fputs(line, temp_file);
        }
    }

    fclose(file);
    fclose(temp_file);

    if (remove("data/machines.txt") != 0) {
        perror("remove");
        remove("../data/temp.txt");
        return;
    }

    if (rename("../data/temp.txt", "data/machines.txt") != 0) {
        perror("rename");
    }
}
void supprimer_machine(int id_supprimer){
    FILE *file = fopen("data/machines.txt", "r");
    FILE *temp_file = fopen("../data/temp.txt", "w");
    if (file == NULL || temp_file == NULL) {
        perror("fopen");

        if (file) fclose(file);
        if (temp_file) fclose(temp_file);

        remove("../data/temp.txt");
        return;
    }
    char line[256];
    while(fgets(line,sizeof(line),file) != NULL){
        int id ;
        sscanf(line, "%d", &id);
        if (id != id_supprimer){
            fputs(line, temp_file);
        }
    }
    fclose(file);
    fclose(temp_file);
    if (remove("data/machines.txt") != 0) {
        perror("remove");
        remove("../data/temp.txt");
        return;
    }

    if (rename("../data/temp.txt", "data/machines.txt") != 0) {
        perror("rename");
    }
}

void lister_machines(){
    FILE *file = fopen("data/machines.txt","r");
    if(file == NULL){
        printf("Error overture file");
        return ;
    }
    char line[256];
    while(fgets(line , sizeof(line),file) != NULL){
        int id  , cpu_cores ;
        double Ram;
        char nom[50] ;
        int status_value ;
        sscanf(line ,"%d %49s %d %lf %d\t",&id , nom , &cpu_cores , &Ram ,&status_value);
        MachineStatus status = (MachineStatus)status_value;
        printf("Machine ID: %d\t", id);
        printf("Machine Name: %s\t", nom);
        printf("CPU Cores: %d\t", cpu_cores);
        printf("RAM: %.2f\t", Ram);
        printf("Status: %d\n", status);
    }
    fclose(file);

}