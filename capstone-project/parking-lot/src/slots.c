// slots.c
#include "slots.h"

#include <string.h>

int find_free_slot(const Slot *slots, size_t count) {
    if (!slots) return -1;
    for (size_t i = 0; i < count; ++i) {
        if (slots[i].status == SLOT_FREE) {
            return slots[i].slot_id;
        }
    }
    return -1;
}

bool assign_slot(Slot *slots, size_t count, int slot_id, const char *plate) {
    if (!slots || !plate) return false;
    for (size_t i = 0; i < count; ++i) {
        if (slots[i].slot_id == slot_id) {
            if (slots[i].status == SLOT_FREE) {
                slots[i].status = SLOT_OCCUPIED;
                strncpy(slots[i].vehicle_plate, plate, MAX_PLATE_LEN - 1);
                slots[i].vehicle_plate[MAX_PLATE_LEN - 1] = '\0';
                return true;
            }
            return false;
        }
    }
    return false;
}

bool release_slot(Slot *slots, size_t count, int slot_id) {
    if (!slots) return false;
    for (size_t i = 0; i < count; ++i) {
        if (slots[i].slot_id == slot_id) {
            slots[i].status = SLOT_FREE;
            slots[i].vehicle_plate[0] = '\0';
            return true;
        }
    }
    return false;
}

int count_free_slots(const Slot *slots, size_t count) {
    if (!slots) return 0;
    int free_count = 0;
    for (size_t i = 0; i < count; ++i) {
        if (slots[i].status == SLOT_FREE) {
            ++free_count;
        }
    }
    return free_count;
}
