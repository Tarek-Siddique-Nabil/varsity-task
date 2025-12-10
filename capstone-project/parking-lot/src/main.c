// main.c
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "auth.h"
#include "billing.h"
#include "reporting.h"
#include "slots.h"
#include "storage.h"
#include "vehicle.h"

static Slot g_slots[MAX_SLOTS];
static size_t g_slot_count = 0;
static VehicleRecord g_vehicles[MAX_VEHICLES];
static size_t g_vehicle_count = 0;
static LogEntry g_logs[MAX_LOGS];
static size_t g_log_count = 0;
static Rate g_rate = {50.0, 0.0};

static void trim_newline(char *s) {
    if (!s) return;
    size_t len = strlen(s);
    if (len && (s[len - 1] == '\n' || s[len - 1] == '\r')) {
        s[len - 1] = '\0';
    }
}

static void read_line(char *buf, size_t size) {
    if (!buf || size == 0) return;
    if (fgets(buf, size, stdin)) {
        trim_newline(buf);
    } else {
        buf[0] = '\0';
    }
}

static void append_audit_log(const char *event_type, const char *details, double amount) {
    if (g_log_count >= MAX_LOGS) return;

    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    strftime(g_logs[g_log_count].timestamp, MAX_TIMESTAMP_LEN, "%Y-%m-%dT%H:%M:%S", t);
    strncpy(g_logs[g_log_count].event_type, event_type, MAX_ROLE_LEN - 1);
    strncpy(g_logs[g_log_count].details, details, MAX_DETAILS_LEN - 1);
    g_logs[g_log_count].amount = amount;
    ++g_log_count;
    
    append_log(LOGS_CSV, &g_logs[g_log_count - 1]);
}

static void print_banner(void) {
    puts("=============================");
    puts(" Parking Lot Management CLI ");
    puts("=============================");
}

static void show_main_menu(void) {
    puts("1) Login (Admin/Attendant)");
    puts("0) Exit");
}

static void admin_menu(void) {
    bool in_admin = true;
    while (in_admin) {
        puts("\n--- Admin Menu ---");
        puts("1) View Slot Status");
        puts("2) Configure Number of Slots");
        puts("3) Set Parking Rate");
        puts("4) View Revenue Report");
        puts("0) Logout");
        printf("Select: ");

        char choice[8];
        read_line(choice, sizeof(choice));

        switch (choice[0]) {
            case '1': {
                report_slot_status(g_slots, g_slot_count);
                break;
            }
            case '2': {
                printf("Enter number of slots (current: %zu): ", g_slot_count);
                char buf[8];
                read_line(buf, sizeof(buf));
                int new_count = atoi(buf);
                if (new_count > 0 && new_count <= MAX_SLOTS) {
                    if (new_count > (int)g_slot_count) {
                        for (int i = (int)g_slot_count; i < new_count; ++i) {
                            g_slots[i].slot_id = i + 1;
                            g_slots[i].status = SLOT_FREE;
                            g_slots[i].vehicle_plate[0] = '\0';
                        }
                    }
                    g_slot_count = new_count;
                    save_slots(SLOTS_CSV, g_slots, g_slot_count);
                    printf("Slots configured to %d\n", new_count);
                    append_audit_log("config", "slots_updated", 0.0);
                } else {
                    puts("Invalid count.");
                }
                break;
            }
            case '3': {
                printf("Enter rate per hour (current: Taka %.2f): ", g_rate.rate_per_hour);
                char buf[16];
                read_line(buf, sizeof(buf));
                double new_rate = atof(buf);
                if (new_rate > 0) {
                    g_rate.rate_per_hour = new_rate;
                    printf("Rate updated to Taka %.2f per hour\n", g_rate.rate_per_hour);
                    append_audit_log("config", "rate_updated", new_rate);
                } else {
                    puts("Invalid rate.");
                }
                break;
            }
            case '4': {
                report_revenue(g_logs, g_log_count);
                break;
            }
            case '0': {
                in_admin = false;
                break;
            }
            default:
                puts("Unknown option.");
                break;
        }
    }
}

static void attendant_menu(void) {
    bool in_attendant = true;
    while (in_attendant) {
        puts("\n--- Attendant Menu ---");
        puts("1) Vehicle Entry (Assign Slot)");
        puts("2) Vehicle Exit (Compute Fee)");
        puts("3) View Available Slots");
        puts("0) Logout");
        printf("Select: ");

        char choice[8];
        read_line(choice, sizeof(choice));

        switch (choice[0]) {
            case '1': {
                printf("Enter vehicle plate number: ");
                char plate[MAX_PLATE_LEN];
                read_line(plate, sizeof(plate));

                int slot_id = find_free_slot(g_slots, g_slot_count);
                if (slot_id < 0) {
                    puts("No free slots available.");
                    break;
                }

                if (g_vehicle_count >= MAX_VEHICLES) {
                    puts("Vehicle database full.");
                    break;
                }

                if (assign_slot(g_slots, g_slot_count, slot_id, plate)) {
                    VehicleRecord *vehicle = &g_vehicles[g_vehicle_count];
                    if (create_vehicle_entry(vehicle, plate, slot_id)) {
                        printf("Vehicle Entry Recorded\n");
                        printf("Ticket: %s\n", vehicle->ticket_id);
                        printf("Assigned Slot: %d\n", slot_id);
                        ++g_vehicle_count;
                        save_slots(SLOTS_CSV, g_slots, g_slot_count);
                        save_vehicles(VEHICLES_CSV, g_vehicles, g_vehicle_count);
                        
                        char details[MAX_DETAILS_LEN];
                        snprintf(details, sizeof(details), "plate=%s;slot=%d", plate, slot_id);
                        append_audit_log("entry", details, 0.0);
                    }
                }
                break;
            }
            case '2': {
                printf("Enter vehicle plate number: ");
                char plate[MAX_PLATE_LEN];
                read_line(plate, sizeof(plate));

                int idx = find_vehicle_by_plate(g_vehicles, g_vehicle_count, plate);
                if (idx < 0) {
                    puts("Vehicle not found or already exited.");
                    break;
                }

                VehicleRecord *vehicle = &g_vehicles[idx];
                double fee = calculate_fee(vehicle->entry_ts, vehicle->exit_ts, g_rate.rate_per_hour);
                
                time_t now = time(NULL);
                struct tm *t = localtime(&now);
                strftime(vehicle->exit_ts, MAX_TIMESTAMP_LEN, "%Y-%m-%dT%H:%M:%S", t);
                
                mark_vehicle_exit(vehicle, fee);
                release_slot(g_slots, g_slot_count, vehicle->slot_id);

                char receipt[512];
                format_receipt(vehicle, fee, receipt, sizeof(receipt));
                puts(receipt);

                save_vehicles(VEHICLES_CSV, g_vehicles, g_vehicle_count);
                save_slots(SLOTS_CSV, g_slots, g_slot_count);

                char details[MAX_DETAILS_LEN];
                snprintf(details, sizeof(details), "plate=%s;ticket=%s", plate, vehicle->ticket_id);
                append_audit_log("exit", details, fee);
                break;
            }
            case '3': {
                int free_count = count_free_slots(g_slots, g_slot_count);
                printf("Free slots: %d / %zu\n", free_count, g_slot_count);
                break;
            }
            case '0': {
                in_attendant = false;
                break;
            }
            default:
                puts("Unknown option.");
                break;
        }
    }
}

static void handle_login(const User *users, size_t user_count) {
    char username[MAX_USERNAME_LEN];
    char password[MAX_PASSWORD_LEN];

    printf("Username: ");
    read_line(username, sizeof(username));
    printf("Password: ");
    read_line(password, sizeof(password));

    User logged = {0};
    if (authenticate(username, password, users, user_count, &logged)) {
        printf("\nLogin successful. Role: %s\n", role_name(logged.role));
        if (logged.role == ROLE_ADMIN) {
            admin_menu();
        } else {
            attendant_menu();
        }
    } else {
        puts("Invalid credentials. Please try again.");
    }
}

int main(void) {
    User users[MAX_USERS];
    size_t user_count = 0;

    if (!load_users(USERS_CSV, users, MAX_USERS, &user_count)) {
        fprintf(stderr, "Failed to load users from %s\n", USERS_CSV);
        return 1;
    }

    if (!load_slots(SLOTS_CSV, g_slots, MAX_SLOTS, &g_slot_count)) {
        fprintf(stderr, "Failed to load slots from %s\n", SLOTS_CSV);
        return 1;
    }

    if (!load_vehicles(VEHICLES_CSV, g_vehicles, MAX_VEHICLES, &g_vehicle_count)) {
        fprintf(stderr, "Failed to load vehicles from %s\n", VEHICLES_CSV);
        return 1;
    }

    if (!load_logs(LOGS_CSV, g_logs, MAX_LOGS, &g_log_count)) {
        fprintf(stderr, "Failed to load logs from %s\n", LOGS_CSV);
        return 1;
    }

    append_audit_log("startup", "system_started", 0.0);

    bool running = true;
    while (running) {
        print_banner();
        show_main_menu();
        printf("Select an option: ");

        char choice[8];
        read_line(choice, sizeof(choice));

        switch (choice[0]) {
            case '1':
                handle_login(users, user_count);
                break;
            case '0':
                running = false;
                break;
            default:
                puts("Unknown option. Please choose again.");
                break;
        }

        puts("");
    }

    append_audit_log("shutdown", "system_stopped", 0.0);
    puts("Goodbye!");
    return 0;
}
