// vehicle.h
#ifndef VEHICLE_H
#define VEHICLE_H

#include "common.h"

void generate_ticket(char *ticket_id, size_t size);
int find_vehicle_by_plate(const VehicleRecord *vehicles, size_t count, const char *plate);
bool create_vehicle_entry(VehicleRecord *vehicle, const char *plate, int slot_id);
bool mark_vehicle_exit(VehicleRecord *vehicle, double fee);

#endif // VEHICLE_H
