// vehicle.c
#include "vehicle.h"

#include <stdio.h>
#include <string.h>
#include <time.h>

static int ticket_counter = 1000;

void generate_ticket(char *ticket_id, size_t size) {
    if (!ticket_id || size < 2) return;
    snprintf(ticket_id, size, "T%d", ++ticket_counter);
}

int find_vehicle_by_plate(const VehicleRecord *vehicles, size_t count, const char *plate) {
    if (!vehicles || !plate) return -1;
    for (size_t i = 0; i < count; ++i) {
        if (strcmp(vehicles[i].plate, plate) == 0 && vehicles[i].active) {
            return (int)i;
        }
    }
    return -1;
}

bool create_vehicle_entry(VehicleRecord *vehicle, const char *plate, int slot_id) {
    if (!vehicle || !plate) return false;

    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    
    generate_ticket(vehicle->ticket_id, MAX_TICKET_LEN);
    strncpy(vehicle->plate, plate, MAX_PLATE_LEN - 1);
    vehicle->plate[MAX_PLATE_LEN - 1] = '\0';
    
    strftime(vehicle->entry_ts, MAX_TIMESTAMP_LEN, "%Y-%m-%dT%H:%M:%S", t);
    vehicle->exit_ts[0] = '\0';
    vehicle->slot_id = slot_id;
    vehicle->active = true;
    vehicle->fee = 0.0;
    
    return true;
}

bool mark_vehicle_exit(VehicleRecord *vehicle, double fee) {
    if (!vehicle || !vehicle->active) return false;

    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    
    strftime(vehicle->exit_ts, MAX_TIMESTAMP_LEN, "%Y-%m-%dT%H:%M:%S", t);
    vehicle->active = false;
    vehicle->fee = fee;
    
    return true;
}
