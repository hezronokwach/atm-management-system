#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>
#include "header.h"

// Encryption function (XOR-based for demonstration purposes)
void encrypt(const char *input, char *output, const char *key) {
    size_t input_len = strlen(input);
    size_t key_len = strlen(key);
    
    for (size_t i = 0; i < input_len; i++) {
        output[i] = input[i] ^ key[i % key_len]; // XOR operation
    }
    output[input_len] = '\0'; // Null-terminate the output string
}

// Decryption function (symmetric with encryption)
void decrypt(const char *input, char *output, const char *key) {
    encrypt(input, output, key); // XOR encryption is symmetric
}

void loginMenu(sqlite3 *db)
{
    struct termios oflags, nflags;
    struct User u;
    const char *retrieved;
    char decryptedPassword[50]; // Buffer for the decrypted password
    const char *key = "mysecretkey"; // Define the same key used for encryption

    system("clear");
    printf("\n\n\n\t\t\t\t   Bank Management System\n\t\t\t\t\t User Login:");
    printf("\nEnter your username: ");
    scanf("%s", u.name);

    // Disabling echo for password input
    tcgetattr(fileno(stdin), &oflags);
    nflags = oflags;
    nflags.c_lflag &= ~ECHO;
    nflags.c_lflag |= ECHONL;

    if (tcsetattr(fileno(stdin), TCSANOW, &nflags) != 0)
    {
        perror("tcsetattr");
        return exit(1);
    }
    printf("\nEnter the password to login: ");
    scanf("%s", u.password);

    // Restore terminal settings
    if (tcsetattr(fileno(stdin), TCSANOW, &oflags) != 0)
    {
        perror("tcsetattr");
        return exit(1);
    }

    // Retrieve the stored encrypted password
    retrieved = getPassword(u.name, db);
    if (retrieved == NULL)
    {
        printf("User not found\n");
        return;
    }

    // Decrypt the retrieved password
    decrypt(retrieved, decryptedPassword, key);

    // Compare the decrypted password with the entered password
    if (strcmp(decryptedPassword, u.password) == 0)
    {
        printf("Login successful\n");
        mainMenu(u, db);
    }
    else
    {
        printf("Wrong password\n");
        return;
    }
}

const char *getPassword(const char *username, sqlite3 *db) {
    const char *sql_password = "SELECT password FROM users WHERE name = ?;";
    sqlite3_stmt *stmt_pass;
    char *password = NULL; // Pointer to hold the retrieved password

    // Prepare the SQL statement
    if (sqlite3_prepare_v2(db, sql_password, -1, &stmt_pass, 0) != SQLITE_OK) {
        fprintf(stderr, "Error preparing statement: %s\n", sqlite3_errmsg(db));
        return NULL; // Return NULL on error
    }

    // Bind the username to the query
    sqlite3_bind_text(stmt_pass, 1, username, -1, SQLITE_STATIC);

    // Execute the query
    if (sqlite3_step(stmt_pass) == SQLITE_ROW) {
        const char *temp_password = (const char *)sqlite3_column_text(stmt_pass, 0);
        if (temp_password) {
            // Allocate memory for the password and copy it
            password = malloc(strlen(temp_password) + 1);
            if (password) {
                strcpy(password, temp_password);
            }
        }
    }

    sqlite3_finalize(stmt_pass); // Finalize the statement
    return password; // Return the allocated password or NULL if not found
}

void registerAcc(sqlite3 *db)
{
    struct User u;
    const char *sql_check = "SELECT name FROM users WHERE name = ?;";
    const char *sql_insert = "INSERT INTO users (id, name, password) VALUES (?, ?, ?);";
    char encryptedPassword[50]; // Buffer for the encrypted password
    const char *key = "mysecretkey"; // Define a key for encryption

    sqlite3_stmt *stmt_check;
    sqlite3_stmt *stmt_insert;

    system("clear");
    printf("\n\n\n\t\t\t\t   Bank Management System\n\t\t\t\t\t User Name:");
    printf("\nEnter your username: ");
    scanf("%s", u.name);
    if (sqlite3_prepare_v2(db, sql_check, -1, &stmt_check, 0) != SQLITE_OK)
    {
        fprintf(stderr, "Error preparing statement: %s\n", sqlite3_errmsg(db));
        return;
    }
    sqlite3_bind_text(stmt_check, 1, u.name, -1, SQLITE_STATIC);
    if (sqlite3_step(stmt_check) == SQLITE_ROW)
    {
        printf("Account already exists\n\n");
        sqlite3_finalize(stmt_check);
        return;
    }
    sqlite3_finalize(stmt_check);

    printf("\nCreate the password: ");
    scanf("%s", u.password);

    // Encrypt the password before storing it
    encrypt(u.password, encryptedPassword, key);

    if (sqlite3_prepare_v2(db, sql_insert, -1, &stmt_insert, 0) != SQLITE_OK) {
        fprintf(stderr, "Error preparing statement: %s\n", sqlite3_errmsg(db));
        return;
    }
   
    sqlite3_bind_int(stmt_insert, 1, u.id);
    sqlite3_bind_text(stmt_insert, 2, u.name, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt_insert, 3, encryptedPassword, -1, SQLITE_STATIC);

    if (sqlite3_step(stmt_insert) != SQLITE_DONE) {
        fprintf(stderr, "Execution failed: %s\n", sqlite3_errmsg(db));
        return;
    } else {
        printf("✔ Account registered successfully!\n");
        mainMenu(u, db);
    }
    sqlite3_finalize(stmt_insert);
}
