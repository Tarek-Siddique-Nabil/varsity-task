// storage.c
#include "storage.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Internal helpers
static void trim_newline(char *s) {
	if (!s) return;
	size_t len = strlen(s);
	if (len && (s[len - 1] == '\n' || s[len - 1] == '\r')) {
		s[len - 1] = '\0';
	}
}

static bool ensure_file_with_header(const char *path, const char *header) {
	FILE *f = fopen(path, "r");
	if (!f) {
		f = fopen(path, "w");
		if (!f) return false;
		fprintf(f, "%s\n", header);
		fclose(f);
		return true;
	}

	char buf[256];
	if (!fgets(buf, sizeof(buf), f)) {
		fclose(f);
		f = fopen(path, "w");
		if (!f) return false;
		fprintf(f, "%s\n", header);
		fclose(f);
		return true;
	}
	fclose(f);
	return true;
}

static Role role_from_string(const char *s) {
	if (s && strcmp(s, "admin") == 0) return ROLE_ADMIN;
	return ROLE_ATTENDANT;
}

static const char *role_to_string(Role r) {
	return r == ROLE_ADMIN ? "admin" : "attendant";
}

static SlotStatus slot_status_from_string(const char *s) {
	if (s && strcmp(s, "occupied") == 0) return SLOT_OCCUPIED;
	return SLOT_FREE;
}

static const char *slot_status_to_string(SlotStatus st) {
	return st == SLOT_OCCUPIED ? "occupied" : "free";
}

bool load_users(const char *path, User *out, size_t max_count, size_t *out_count) {
	if (!ensure_file_with_header(path, "username,password,role")) return false;

	FILE *f = fopen(path, "r");
	if (!f) return false;

	char line[256];
	// skip header
	if (!fgets(line, sizeof(line), f)) {
		fclose(f);
		*out_count = 0;
		return true;
	}

	size_t count = 0;
	while (fgets(line, sizeof(line), f) && count < max_count) {
		trim_newline(line);
		if (line[0] == '\0') continue;

		char username[MAX_USERNAME_LEN];
		char password[MAX_PASSWORD_LEN];
		char role_str[MAX_ROLE_LEN];

		if (sscanf(line, " %31[^,],%63[^,],%15[^,]", username, password, role_str) == 3) {
			strncpy(out[count].username, username, MAX_USERNAME_LEN);
			out[count].username[MAX_USERNAME_LEN - 1] = '\0';
			strncpy(out[count].password, password, MAX_PASSWORD_LEN);
			out[count].password[MAX_PASSWORD_LEN - 1] = '\0';
			out[count].role = role_from_string(role_str);
			count++;
		}
	}

	fclose(f);
	*out_count = count;
	return true;
}

bool save_users(const char *path, const User *arr, size_t count) {
	char tmp_path[256];
	snprintf(tmp_path, sizeof(tmp_path), "%s.tmp", path);

	FILE *f = fopen(tmp_path, "w");
	if (!f) return false;

	fprintf(f, "username,password,role\n");
	for (size_t i = 0; i < count; ++i) {
		fprintf(f, "%s,%s,%s\n", arr[i].username, arr[i].password, role_to_string(arr[i].role));
	}

	fclose(f);
	if (remove(path) != 0) {
		// ignore errors if file doesn't exist
	}
	if (rename(tmp_path, path) != 0) return false;
	return true;
}

bool load_slots(const char *path, Slot *out, size_t max_count, size_t *out_count) {
	if (!ensure_file_with_header(path, "slot_id,status,vehicle_plate")) return false;

	FILE *f = fopen(path, "r");
	if (!f) return false;

	char line[256];
	if (!fgets(line, sizeof(line), f)) {
		fclose(f);
		*out_count = 0;
		return true;
	}

	size_t count = 0;
	while (fgets(line, sizeof(line), f) && count < max_count) {
		trim_newline(line);
		if (line[0] == '\0') continue;

		int slot_id = 0;
		char status_str[16];
		char plate[MAX_PLATE_LEN] = "";

		int matched = sscanf(line, " %d,%15[^,],%15[^,]", &slot_id, status_str, plate);
		if (matched >= 2) {
			out[count].slot_id = slot_id;
			out[count].status = slot_status_from_string(status_str);
			strncpy(out[count].vehicle_plate, plate, MAX_PLATE_LEN);
			out[count].vehicle_plate[MAX_PLATE_LEN - 1] = '\0';
			count++;
		}
	}

	fclose(f);
	*out_count = count;
	return true;
}

bool save_slots(const char *path, const Slot *arr, size_t count) {
	char tmp_path[256];
	snprintf(tmp_path, sizeof(tmp_path), "%s.tmp", path);

	FILE *f = fopen(tmp_path, "w");
	if (!f) return false;

	fprintf(f, "slot_id,status,vehicle_plate\n");
	for (size_t i = 0; i < count; ++i) {
		fprintf(f, "%d,%s,%s\n", arr[i].slot_id, slot_status_to_string(arr[i].status), arr[i].vehicle_plate);
	}

	fclose(f);
	if (remove(path) != 0) {
		// ignore errors if file doesn't exist
	}
	if (rename(tmp_path, path) != 0) return false;
	return true;
}

bool load_vehicles(const char *path, VehicleRecord *out, size_t max_count, size_t *out_count) {
	if (!ensure_file_with_header(path, "ticket_id,plate,entry_ts,exit_ts,slot_id,active,fee")) return false;

	FILE *f = fopen(path, "r");
	if (!f) return false;

	char line[512];
	if (!fgets(line, sizeof(line), f)) {
		fclose(f);
		*out_count = 0;
		return true;
	}

	size_t count = 0;
	while (fgets(line, sizeof(line), f) && count < max_count) {
		trim_newline(line);
		if (line[0] == '\0') continue;

		char ticket[MAX_TICKET_LEN];
		char plate[MAX_PLATE_LEN];
		char entry[MAX_TIMESTAMP_LEN];
		char exit_ts[MAX_TIMESTAMP_LEN];
		int slot_id = 0;
		int active_int = 0;
		double fee = 0.0;

		int matched = sscanf(line, " %15[^,],%15[^,],%19[^,],%19[^,],%d,%d,%lf", ticket, plate, entry, exit_ts, &slot_id, &active_int, &fee);
		if (matched >= 5) {
			strncpy(out[count].ticket_id, ticket, MAX_TICKET_LEN);
			out[count].ticket_id[MAX_TICKET_LEN - 1] = '\0';
			strncpy(out[count].plate, plate, MAX_PLATE_LEN);
			out[count].plate[MAX_PLATE_LEN - 1] = '\0';
			strncpy(out[count].entry_ts, entry, MAX_TIMESTAMP_LEN);
			out[count].entry_ts[MAX_TIMESTAMP_LEN - 1] = '\0';
			strncpy(out[count].exit_ts, exit_ts, MAX_TIMESTAMP_LEN);
			out[count].exit_ts[MAX_TIMESTAMP_LEN - 1] = '\0';
			out[count].slot_id = slot_id;
			out[count].active = active_int != 0;
			out[count].fee = fee;
			count++;
		}
	}

	fclose(f);
	*out_count = count;
	return true;
}

bool save_vehicles(const char *path, const VehicleRecord *arr, size_t count) {
	char tmp_path[256];
	snprintf(tmp_path, sizeof(tmp_path), "%s.tmp", path);

	FILE *f = fopen(tmp_path, "w");
	if (!f) return false;

	fprintf(f, "ticket_id,plate,entry_ts,exit_ts,slot_id,active,fee\n");
	for (size_t i = 0; i < count; ++i) {
		fprintf(f, "%s,%s,%s,%s,%d,%d,%.2f\n",
				arr[i].ticket_id,
				arr[i].plate,
				arr[i].entry_ts,
				arr[i].exit_ts,
				arr[i].slot_id,
				arr[i].active ? 1 : 0,
				arr[i].fee);
	}

	fclose(f);
	if (remove(path) != 0) {
		// ignore errors if file doesn't exist
	}
	if (rename(tmp_path, path) != 0) return false;
	return true;
}

bool load_logs(const char *path, LogEntry *out, size_t max_count, size_t *out_count) {
	if (!ensure_file_with_header(path, "timestamp,event_type,details,amount")) return false;

	FILE *f = fopen(path, "r");
	if (!f) return false;

	char line[512];
	if (!fgets(line, sizeof(line), f)) {
		fclose(f);
		*out_count = 0;
		return true;
	}

	size_t count = 0;
	while (fgets(line, sizeof(line), f) && count < max_count) {
		trim_newline(line);
		if (line[0] == '\0') continue;

		char ts[MAX_TIMESTAMP_LEN];
		char event_type[32];
		char details[MAX_DETAILS_LEN];
		double amount = 0.0;

		int matched = sscanf(line, " %19[^,],%31[^,],%255[^,],%lf", ts, event_type, details, &amount);
		if (matched >= 3) {
			strncpy(out[count].timestamp, ts, MAX_TIMESTAMP_LEN);
			out[count].timestamp[MAX_TIMESTAMP_LEN - 1] = '\0';
			strncpy(out[count].event_type, event_type, MAX_ROLE_LEN);
			out[count].event_type[MAX_ROLE_LEN - 1] = '\0';
			strncpy(out[count].details, details, MAX_DETAILS_LEN);
			out[count].details[MAX_DETAILS_LEN - 1] = '\0';
			out[count].amount = amount;
			count++;
		}
	}

	fclose(f);
	*out_count = count;
	return true;
}

bool append_log(const char *path, const LogEntry *entry) {
	if (!ensure_file_with_header(path, "timestamp,event_type,details,amount")) return false;

	FILE *f = fopen(path, "a");
	if (!f) return false;
	fprintf(f, "%s,%s,%s,%.2f\n", entry->timestamp, entry->event_type, entry->details, entry->amount);
	fclose(f);
	return true;
}
