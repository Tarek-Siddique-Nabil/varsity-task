// billing.c
#include "billing.h"

#include <stdio.h>
#include <string.h>
#include <time.h>

static int parse_timestamp(const char *ts) {
    struct tm t = {0};
    if (sscanf(ts, "%d-%d-%dT%d:%d:%d", &t.tm_year, &t.tm_mon, &t.tm_mday, &t.tm_hour, &t.tm_min, &t.tm_sec) == 6) {
        t.tm_year -= 1900;
        t.tm_mon -= 1;
        time_t et = mktime(&t);
        return (int)et;
    }
    return 0;
}

double calculate_fee(const char *entry_ts, const char *exit_ts, double rate_per_hour) {
    if (!entry_ts || !exit_ts || rate_per_hour <= 0) return 0.0;

    time_t entry_time = (time_t)parse_timestamp(entry_ts);
    time_t exit_time = (time_t)parse_timestamp(exit_ts);

    if (entry_time <= 0 || exit_time <= 0 || exit_time <= entry_time) {
        return 0.0;
    }

    double minutes = difftime(exit_time, entry_time) / 60.0;
    double hours = minutes / 60.0;
    
    if (hours < 1.0) {
        hours = 1.0;
    }

    return hours * rate_per_hour;
}

char *format_receipt(const VehicleRecord *vehicle, double fee, char *out_buf, size_t bufsize) {
    if (!vehicle || !out_buf || bufsize == 0) return out_buf;

    snprintf(out_buf, bufsize,
             "===== PARKING RECEIPT =====\n"
             "Ticket ID: %s\n"
             "License Plate: %s\n"
             "Slot: %d\n"
             "Entry Time: %s\n"
             "Exit Time: %s\n"
             "Amount Due: Taka %.2f\n"
             "===========================\n",
             vehicle->ticket_id, vehicle->plate, vehicle->slot_id,
             vehicle->entry_ts, vehicle->exit_ts, fee);

    return out_buf;
}
