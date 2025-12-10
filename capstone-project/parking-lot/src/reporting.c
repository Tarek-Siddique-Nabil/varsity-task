// reporting.c
#include "reporting.h"

#include <stdio.h>

void report_slot_status(const Slot *slots, size_t count) {
    if (!slots || count == 0) {
        puts("No slots available.");
        return;
    }

    int free_count = 0, occupied_count = 0;
    
    puts("\n--- Parking Slot Status ---");
    for (size_t i = 0; i < count; ++i) {
        const char *status_str = slots[i].status == SLOT_FREE ? "FREE" : "OCCUPIED";
        printf("Slot %d: %s", slots[i].slot_id, status_str);
        if (slots[i].status == SLOT_OCCUPIED) {
            printf(" (Vehicle: %s)\n", slots[i].vehicle_plate);
            ++occupied_count;
        } else {
            puts("");
            ++free_count;
        }
    }
    printf("Total: %zu slots | Free: %d | Occupied: %d\n\n", count, free_count, occupied_count);
}

void report_revenue(const LogEntry *logs, size_t count) {
    if (!logs || count == 0) {
        puts("No revenue logs.");
        return;
    }

    double total_revenue = 0.0;
    puts("\n--- Revenue Report ---");
    for (size_t i = 0; i < count; ++i) {
        if (logs[i].amount > 0.0) {
            printf("%s | %s | Taka %.2f\n", logs[i].timestamp, logs[i].event_type, logs[i].amount);
            total_revenue += logs[i].amount;
        }
    }
    printf("Total Revenue: Taka %.2f\n\n", total_revenue);
}
