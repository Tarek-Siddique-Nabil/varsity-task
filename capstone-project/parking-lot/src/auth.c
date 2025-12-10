// auth.c
#include "auth.h"

#include <string.h>

bool authenticate(const char *username, const char *password, const User *users, size_t count, User *out_user) {
	if (!username || !password || !users) return false;
	for (size_t i = 0; i < count; ++i) {
		if (strcmp(username, users[i].username) == 0 && strcmp(password, users[i].password) == 0) {
			if (out_user) {
				*out_user = users[i];
			}
			return true;
		}
	}
	return false;
}

const char *role_name(Role role) {
	return role == ROLE_ADMIN ? "Admin" : "Attendant";
}
