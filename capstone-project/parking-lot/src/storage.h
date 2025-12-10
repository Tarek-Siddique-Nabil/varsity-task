// storage.h
#ifndef STORAGE_H
#define STORAGE_H

#include "common.h"

// Data file paths (relative to repo root)
#define USERS_CSV "data/users.csv"
#define SLOTS_CSV "data/slots.csv"
#define VEHICLES_CSV "data/vehicles.csv"
#define LOGS_CSV "data/logs.csv"

// Capacity limits for in-memory collections
#define MAX_USERS 128
#define MAX_SLOTS 512
#define MAX_VEHICLES 1024
#define MAX_LOGS 2048

bool load_users(const char *path, User *out, size_t max_count, size_t *out_count);
bool save_users(const char *path, const User *arr, size_t count);

bool load_slots(const char *path, Slot *out, size_t max_count, size_t *out_count);
bool save_slots(const char *path, const Slot *arr, size_t count);

bool load_vehicles(const char *path, VehicleRecord *out, size_t max_count, size_t *out_count);
bool save_vehicles(const char *path, const VehicleRecord *arr, size_t count);

bool load_logs(const char *path, LogEntry *out, size_t max_count, size_t *out_count);
bool append_log(const char *path, const LogEntry *entry);

#endif // STORAGE_H
