#include "../include/machine_manager.h"
#include "../include/process_manager.h"
#include <stdio.h>
#include <string.h>


int main(void){
    // afficher_machine(2);
    // printf("listes des machines \n");
    // lister_machines();
    // machine m;
    // m.id =10 ;
    // strcpy(m.name , "machine_10");
    // m.cpu_cores = 3;
    // m.Ram = 8 ;
    // m.status = AVAILABLE ;

    // ajouter_machine(m);
    // printf("listes des machines \n");
    // lister_machines();
    
    // strcpy(m.name,"machine_modifier");
    
    // modifier_machine(m);
    // printf("listes des machines \n");
    // lister_machines();

    // supprimer_machine(10);
    // printf("listes des machines \n");
    // lister_machines();

    afficher_process(2);
    printf("listes des processs \n");
    lister_process();
    processus m;
    m.id =10 ;
    strcpy(m.name , "process_10");
    m.arrival_time = 3;
    m.duration = 5;
    m.priority = 2;
    m.required_ram = 8.0;
    m.machine_id = 1;

    ajouter_process(m);
    printf("listes des processs \n");
    lister_process();
    
    strcpy(m.name,"process_modifier");
    
    modifier_process(m);
    printf("listes des processs \n");
    lister_process();

    supprimer_process(10);
    printf("listes des processs \n");
    lister_process();


    return 0;


}