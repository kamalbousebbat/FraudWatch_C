#include "../include/process_manager.h"
#include <stdio.h>
#include <stdlib.h>



#define chemin "data/processes.txt"
#define temp_chemin "data/temp.txt"

void afficher_process(int id_process){
    FILE *file = fopen(chemin ,"r" );
    if ( file == NULL){
        printf("Error opening file\n");
        return;
    }
    char line[256];
    while(fgets(line , sizeof(line),file) != NULL){
        int id;
        sscanf(line , "%d" , &id);
        if(id == id_process){
            int arrival_time , duration , priority , machine_id;
            double required_ram;
            char name[50];
            sscanf(line , "%d %49s %d %d %d %lf %d ", &id, name, &arrival_time, &duration, &priority, &required_ram, &machine_id);
            printf("\n");
            printf("+------+------------+---------+----------+----------+------------+-----------+\n");

            printf("| %-4s | %-10s | %-7s | %-8s | %-8s | %-10s | %-9s |\n",
                "ID", "Name", "Arrival", "Duration", "Priority", "RAM (GB)", "Machine");

            printf("+------+------------+---------+----------+----------+------------+-----------+\n");

            printf("| %-4d | %-10s | %-7d | %-8d | %-8d | %-10.2f | %-9d |\n",
                id,
                name,
                arrival_time,
                duration,
                priority,
                required_ram,
                machine_id);

            printf("+------+------------+---------+----------+----------+------------+-----------+\n");
            

        }
    }
    fclose(file);

}

void ajouter_process(processus p){
    FILE *file = fopen(chemin , "a");
    if ( file == NULL){
        printf("Error opening file\n");
        return;
    }
    fprintf(file , "\n%d %s %d %d %d %.2f %d" , p.id , p.name , p.arrival_time , p.duration , p.priority , p.required_ram , p.machine_id);
    fclose(file);
}

void modifier_process(processus p){
    FILE *file = fopen(chemin , "r");
    FILE *temp_file = fopen(temp_chemin , "w"); 
    if( file == NULL || temp_file == NULL){
        
        printf("error opening file modefiy process");
        if(temp_file) fclose(temp_file);
        if(file) fclose(file);

        remove(temp_chemin);

        return ;
    } 

    char line[256] ;
    while(fgets(line , sizeof(line) ,file) != NULL){
        int id ;
        sscanf(line , "%d" , &id);
        if(id == p.id){

            fprintf(temp_file, "%d %s %d %d %d %.2f %d\n" , p.id , p.name , p.arrival_time , p.duration , p.priority , p.required_ram , p.machine_id);

        }else {
            fputs(line, temp_file);
        }

    }

    fclose(file);
    fclose(temp_file);

    
    if (remove(chemin) != 0) {
        perror("remove");
        remove(temp_chemin);
        return;
    }

    if (rename(temp_chemin, chemin) != 0) {
        perror("rename");
    }

}

void supprimer_process(int id_supprimer){
    
    FILE *file = fopen(chemin , "r");
    FILE *temp_file = fopen(temp_chemin , "w");
    if(file == NULL || temp_file == NULL){
        perror("opening file error");
        if(file) fclose(file);
        if(temp_file) fclose(temp_file);
        remove(temp_chemin);
        return;
    }

    char line[256];
    while(fgets(line , sizeof(line), file)){
        int id ;
        sscanf(line , "%d", &id);
        if(id != id_supprimer){
            fputs(line, temp_file);
        }
    
    }

    fclose(file);
    fclose(temp_file);

    if (remove(chemin) != 0) {
        perror("remove");
        remove(temp_chemin);
        return;
    }

    if (rename(temp_chemin, chemin) != 0) {
        perror("rename");
    }
}

void lister_process(){
    FILE *file = fopen(chemin , "r");
    if(file== NULL){
        perror("opening file");
        return ;
    }

    char line[256];
    printf("\n");
    printf("+------+------------+---------+----------+----------+------------+-----------+\n");

    printf("| %-4s | %-10s | %-7s | %-8s | %-8s | %-10s | %-9s |\n",
        "ID", "Name", "Arrival", "Duration", "Priority", "RAM (GB)", "Machine");

    printf("+------+------------+---------+----------+----------+------------+-----------+\n");
    while(fgets(line , sizeof(line), file) != NULL ){

        int id , arrival_time , duration , priority , machine_id;
        double required_ram;
        char name[50];
        sscanf(line , "%d %49s %d %d %d %lf %d ", &id, name, &arrival_time, &duration, &priority, &required_ram, &machine_id);

        printf("| %-4d | %-10s | %-7d | %-8d | %-8d | %-10.2f | %-9d |\n",
            id,
            name,
            arrival_time,
            duration,
            priority,
            required_ram,
            machine_id);

        printf("+------+------------+---------+----------+----------+------------+-----------+\n");

    }
    fclose(file);
}