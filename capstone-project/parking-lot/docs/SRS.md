# Parking Lot Management System — Simplified SRS (1 page)

## 1. Goal

CLI parking lot manager in C to automate vehicle check-in/out, slot allocation, and billing with role-based access (Admin, Attendant). Data persists via file I/O (CSV). No DB, no GUI.

## 2. Users & Roles

- Admin: manage attendants, set rates, configure slots, view reports.
- Attendant: record vehicle entry/exit, assign slots, compute fees.
- Driver: receives assigned slot at entry, pays at exit.

## 3. In Scope (Core Features)

- Login for Admin/Attendant with role-based menus.
- Vehicle entry/exit registration; automatic assignment of first-free slot; slot release on exit.
- Billing from entry/exit timestamps; rates configurable by Admin; print/save receipt; append to revenue log.
- File-based persistence for users, slots, vehicles, logs (CSV headers human-readable).
- Reports: current availability, daily logs, revenue summary.

## 4. Out of Scope (Explicit)

No online payments, database, GUI/mobile, multi-floor simulation, GPS, cloud services.

## 5. Non-Functional Highlights

- Portable C11, gcc; low memory footprint; no external libs beyond stdlib/stdio/time.
- CLI menu UX with robust input validation; clear prompts/help.
- Modular code: `auth`, `slots`, `vehicle`, `billing`, `reporting`, `storage/common` utilities.
- Logging: append operations/errors to `data/logs.csv` (or separate audit file if needed).
- Safe file writes (temp + rename) to avoid corruption.

## 6. Data & Formats (CSV)

- `data/users.csv`: `username,password,role` (plaintext acceptable for coursework; note optional hashing).
- `data/slots.csv`: `slot_id,status,vehicle_plate` (`status` = free/occupied).
- `data/vehicles.csv`: `ticket_id,plate,entry_ts,slot_id` (ISO 8601 timestamps).
- `data/logs.csv`: `date,event_type,details,amount` (for exits/revenue and notable events).

## 7. Functional Requirements

1) Authentication: verify Admin/Attendant credentials; show role menu; Admin can add attendants.
2) Slot management: configure N slots; represent ID/status/vehicle ref; auto-assign nearest/first free; release on exit.
3) Vehicle handling: entry records plate/type/time/slot; exit lookup by plate or ticket; update records and logs.
4) Billing: Admin-set rates (per hour/min slabs acceptable); compute duration/fee; emit receipt; append revenue log.
5) Reporting: current status (occupied/free counts/list), daily entry/exit logs, revenue summary by session/day.
6) Persistence: load CSV on startup; write on each change or clean shutdown; handle missing files by creating with headers.
7) Admin utilities: create/edit attendants, set rates, configure slots.

## 8. Development Phases (Milestones)

0) Setup: layout, Makefile, README, basic `main.c` menu stub.
1) Models & storage: structs `User`, `Slot`, `VehicleRecord`, `Rate`, `LogEntry`; CSV load/save helpers.
2) Auth & menus: login flow; Admin/Attendant menus; add attendants.
3) Slot & entry: configure slots; vehicle entry; auto-assign slot; persist.
4) Exit & billing: lookup, duration/fee calc, release slot, revenue log, receipt output.
5) Reporting: status, daily logs, revenue; search parked vehicles; optional log export.
6) Testing/polish: billing unit tests; error handling; input sanitization; help/shortcuts; final docs/examples.

## 9. Acceptance (Done) Criteria

- Builds with `make` on gcc (C11) and runs via `make run` with no crashes on valid input.
- Supports Admin/Attendant login, slot auto-assignment/release, billing with configurable rates, CSV persistence, and basic reports.
- Tests for billing calculations run and pass (Phase 6).
