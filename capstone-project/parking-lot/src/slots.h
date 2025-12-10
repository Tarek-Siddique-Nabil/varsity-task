// slots.h
#ifndef SLOTS_H
#define SLOTS_H

#include "common.h"

int find_free_slot(const Slot *slots, size_t count);
bool assign_slot(Slot *slots, size_t count, int slot_id, const char *plate);
bool release_slot(Slot *slots, size_t count, int slot_id);
int count_free_slots(const Slot *slots, size_t count);

#endif // SLOTS_H
