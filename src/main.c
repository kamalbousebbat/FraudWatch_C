#include "../include/machine_manager.h"
#include <stdio.h>
#include <string.h>


int main(void){
    afficher_machine(2);
    printf("listes des machines \n");
    lister_machines();
    machine m;
    m.id =10 ;
    strcpy(m.name , "machine_10");
    m.cpu_cores = 3;
    m.Ram = 8 ;
    m.status = AVAILABLE ;

    ajouter_machine(m);
    printf("listes des machines \n");
    lister_machines();
    
    strcpy(m.name,"machine_modifier");
    
    modifier_machine(m);
    printf("listes des machines \n");
    lister_machines();

    supprimer_machine(10);
    printf("listes des machines \n");
    lister_machines();


    return 0;


}