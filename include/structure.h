#ifndef STRUCTURE_H
#define STRUCTURE_H

typedef enum {
    AVAILABLE,
    UNAVAILABLE
} MachineStatus;

typedef struct {
    int id;
    char name[50];
    int cpu_cores;
    double Ram;
    MachineStatus status; 
}machine ;

typedef struct {
    int id;
    char name[50];
    int arrival_time;
    int duration; 
    int priority;
    double required_ram;
    int machine_id;
}processus ;










#endif

