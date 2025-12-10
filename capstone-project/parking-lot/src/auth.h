// auth.h
#ifndef AUTH_H
#define AUTH_H

#include "common.h"

bool authenticate(const char *username, const char *password, const User *users, size_t count, User *out_user);
const char *role_name(Role role);

#endif // AUTH_H
