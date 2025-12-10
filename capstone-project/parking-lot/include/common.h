// common.h
#ifndef COMMON_H
#define COMMON_H

#include <stdbool.h>
#include <stddef.h>

// Size limits for string fields (including null terminators)
#define MAX_USERNAME_LEN 32
#define MAX_PASSWORD_LEN 64
#define MAX_ROLE_LEN 16
#define MAX_PLATE_LEN 16
#define MAX_TICKET_LEN 16
#define MAX_TIMESTAMP_LEN 20  // YYYY-MM-DDTHH:MM:SS
#define MAX_DETAILS_LEN 256

typedef enum {
	ROLE_ADMIN,
	ROLE_ATTENDANT
} Role;

typedef enum {
	SLOT_FREE,
	SLOT_OCCUPIED
} SlotStatus;

typedef struct {
	char username[MAX_USERNAME_LEN];
	char password[MAX_PASSWORD_LEN];
	Role role;
} User;

typedef struct {
	int slot_id;
	SlotStatus status;
	char vehicle_plate[MAX_PLATE_LEN];
} Slot;

typedef struct {
	char ticket_id[MAX_TICKET_LEN];
	char plate[MAX_PLATE_LEN];
	char entry_ts[MAX_TIMESTAMP_LEN];
	char exit_ts[MAX_TIMESTAMP_LEN];
	int slot_id;
	bool active;   // true until exit is processed
	double fee;    // computed at exit
} VehicleRecord;

typedef struct {
	double rate_per_hour;
	double grace_minutes; // optional grace period before billing starts
} Rate;

typedef struct {
	char timestamp[MAX_TIMESTAMP_LEN];
	char event_type[MAX_ROLE_LEN]; // e.g., "entry", "exit", "error"
	char details[MAX_DETAILS_LEN];
	double amount;
} LogEntry;

#endif // COMMON_H
