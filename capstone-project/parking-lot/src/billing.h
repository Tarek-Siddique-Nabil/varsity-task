// billing.h
#ifndef BILLING_H
#define BILLING_H

#include "common.h"

double calculate_fee(const char *entry_ts, const char *exit_ts, double rate_per_hour);
char *format_receipt(const VehicleRecord *vehicle, double fee, char *out_buf, size_t bufsize);

#endif // BILLING_H
