#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>
#include "header.h"
#include <ctype.h>

#define MAX_USERNAME_LENGTH 20
#define MAX_PASSWORD_LENGTH 20
#define BUFFER_SIZE 1024

int getUserId(const char *username, sqlite3 *db) {
    const char *sql = "SELECT id FROM users WHERE name = ?;";
    sqlite3_stmt *stmt;
    int userId = -1; // Default to -1 if not found

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) != SQLITE_OK) {
        fprintf(stderr, "Error preparing statement: %s\n", sqlite3_errmsg(db));
        return userId;
    }

    sqlite3_bind_text(stmt, 1, username, -1, SQLITE_STATIC);

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        userId = sqlite3_column_int(stmt, 0);
    }

    sqlite3_finalize(stmt);
    return userId;
}

const char *getPassword(const char *username, sqlite3 *db) {
    const char *sql_password = "SELECT password FROM users WHERE name = ?;";
    sqlite3_stmt *stmt_pass;
    char *password = NULL;

    if (sqlite3_prepare_v2(db, sql_password, -1, &stmt_pass, 0) != SQLITE_OK) {
        fprintf(stderr, "Error preparing statement: %s\n", sqlite3_errmsg(db));
        return NULL;
    }

    sqlite3_bind_text(stmt_pass, 1, username, -1, SQLITE_STATIC);

    if (sqlite3_step(stmt_pass) == SQLITE_ROW) {
        const char *temp_password = (const char *)sqlite3_column_text(stmt_pass, 0);
        if (temp_password) {
            password = malloc(strlen(temp_password) + 1);
            if (password) {
                strcpy(password, temp_password);
            }
        }
    }

    sqlite3_finalize(stmt_pass);
    return password;
}
void loginMenu(sqlite3 *db) {
    struct User u;
    const char *stored_password;
    char buffer[BUFFER_SIZE];
    int valid_input = 0;
 //clearInputBuffer();
    system("clear");
    printf("\n\n\n\t\t\t\t   Bank Management System\n\t\t\t\t\t User Login:");

    // Clear any leftover input
   

    // Username validation
    while (!valid_input) {
        printf("\nEnter your username: ");
        if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
            printf("Error reading input.\n");
            continue;
        }
        buffer[strcspn(buffer, "\n")] = 0;  // Remove newline

        if (strlen(buffer) == 0) {
            printf("Username cannot be empty.\n");
            continue;
        }

        if (strlen(buffer) > MAX_USERNAME_LENGTH) {
            printf("Username is too long. Maximum %d characters allowed.\n", MAX_USERNAME_LENGTH);
            continue;
        }

        valid_input = 1;
        for (int i = 0; buffer[i] != '\0'; i++) {
            if (!isalnum(buffer[i]) && buffer[i] != '_') {
                printf("Username should contain only letters, numbers, and underscores.\n");
                valid_input = 0;
                break;
            }
        }
    }
    strncpy(u.name, buffer, MAX_USERNAME_LENGTH);
    u.name[MAX_USERNAME_LENGTH - 1] = '\0';

    // Password input with hidden characters
    struct termios oflags, nflags;
    tcgetattr(fileno(stdin), &oflags);
    nflags = oflags;
    nflags.c_lflag &= ~ECHO;
    nflags.c_lflag |= ECHONL;

    if (tcsetattr(fileno(stdin), TCSANOW, &nflags) != 0) {
        perror("tcsetattr");
        return;
    }

    // Password validation
    valid_input = 0;
    while (!valid_input) {
        printf("\nEnter the password to login: ");
        if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
            printf("Error reading input.\n");
            continue;
        }
        buffer[strcspn(buffer, "\n")] = 0;  // Remove newline

        if (strlen(buffer) == 0) {
            printf("Password cannot be empty.\n");
            continue;
        }

        if (strlen(buffer) > MAX_PASSWORD_LENGTH) {
            printf("Password is too long. Maximum %d characters allowed.\n", MAX_PASSWORD_LENGTH);
            continue;
        }

        valid_input = 1;
    }
    strncpy(u.password, buffer, MAX_PASSWORD_LENGTH);
    u.password[MAX_PASSWORD_LENGTH - 1] = '\0';

    // Restore terminal settings
    tcsetattr(fileno(stdin), TCSANOW, &oflags);

    // Authentication
    stored_password = getPassword(u.name, db);
    if (stored_password == NULL) {
        printf("\nUser not found\n");
        return;
    }

    if (strcmp(stored_password, u.password) == 0) {
        printf("\nLogin successful!\n");
        u.id = getUserId(u.name, db);
        if (u.id == -1) {
            printf("Failed to retrieve user ID.\n");
            free((void*)stored_password);
            return;
        }
        free((void*)stored_password);
        mainMenu(&u, db);
    } else {
        printf("\nWrong password\n");
        free((void*)stored_password);
        return;
    }
}

void registerAcc(sqlite3 *db) {
    struct User u;
    const char *sql_check = "SELECT name FROM users WHERE name = ?;";
    const char *sql_insert = "INSERT INTO users (name, password) VALUES (?, ?);";
    char buffer[BUFFER_SIZE];

    sqlite3_stmt *stmt_check;
    sqlite3_stmt *stmt_insert;

    system("clear");
    printf("\n\n\n\t\t\t\t   Bank Management System\n\t\t\t\t\t User Name:");

   // clearInputBuffer();

    // Get username
    int valid_username = 0;
    while (!valid_username) {
        printf("\nEnter your username: ");
        if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
            printf("Error reading input.\n");
            continue;
        }
        buffer[strcspn(buffer, "\n")] = 0;

        if (strlen(buffer) == 0) {
            printf("Username cannot be empty.\n");
            continue;
        }

        if (strlen(buffer) > MAX_USERNAME_LENGTH) {
            printf("Username is too long. Maximum %d characters allowed.\n", MAX_USERNAME_LENGTH);
            continue;
        }

        valid_username = 1;
        for (int i = 0; buffer[i] != '\0'; i++) {
            if (!isalnum(buffer[i]) && buffer[i] != '_') {
                printf("Username should contain only letters, numbers, and underscores.\n");
                valid_username = 0;
                break;
            }
        }
    }
    strncpy(u.name, buffer, MAX_USERNAME_LENGTH);
    u.name[MAX_USERNAME_LENGTH - 1] = '\0';

    // Check if username exists
    if (sqlite3_prepare_v2(db, sql_check, -1, &stmt_check, 0) != SQLITE_OK) {
        fprintf(stderr, "Error preparing statement: %s\n", sqlite3_errmsg(db));
        return;
    }

    sqlite3_bind_text(stmt_check, 1, u.name, -1, SQLITE_STATIC);
    
    if (sqlite3_step(stmt_check) == SQLITE_ROW) {
        printf("Account already exists\n\n");
        sqlite3_finalize(stmt_check);
        return;
    }
    sqlite3_finalize(stmt_check);

    // Get password
    int valid_password = 0;
    while (!valid_password) {
        printf("\nCreate the password: ");
        if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
            printf("Error reading input.\n");
            continue;
        }
        buffer[strcspn(buffer, "\n")] = 0;

        if (strlen(buffer) == 0) {
            printf("Password cannot be empty.\n");
            continue;
        }

        if (strlen(buffer) > MAX_PASSWORD_LENGTH) {
            printf("Password is too long. Maximum %d characters allowed.\n", MAX_PASSWORD_LENGTH);
            continue;
        }

        valid_password = 1;
    }
    strncpy(u.password, buffer, MAX_PASSWORD_LENGTH);
    u.password[MAX_PASSWORD_LENGTH - 1] = '\0';

    // Store password directly without encryption
    if (sqlite3_prepare_v2(db, sql_insert, -1, &stmt_insert, 0) != SQLITE_OK) {
        fprintf(stderr, "Error preparing statement: %s\n", sqlite3_errmsg(db));
        return;
    }

    sqlite3_bind_text(stmt_insert, 1, u.name, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt_insert, 2, u.password, -1, SQLITE_STATIC);

    if (sqlite3_step(stmt_insert) != SQLITE_DONE) {
        fprintf(stderr, "Execution failed: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt_insert);
        return;
    }
    sqlite3_finalize(stmt_insert);

    int userId = sqlite3_last_insert_rowid(db);
    printf("✔ Account registered successfully!\n");

    u.id = userId;
    mainMenu(&u, db);
}