// reporting.h
#ifndef REPORTING_H
#define REPORTING_H

#include "common.h"

void report_slot_status(const Slot *slots, size_t count);
void report_revenue(const LogEntry *logs, size_t count);

#endif // REPORTING_H
