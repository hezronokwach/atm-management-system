#include <termios.h>
#include "header.h"
#include <sqlite3.h>

void loginMenu(sqlite3 *db)
{
    struct termios oflags, nflags;
    struct User u;
    const char *retrieved;

    system("clear");
    printf("\n\n\n\t\t\t\t   Bank Management System\n\t\t\t\t\t User Login:");
    scanf("%s", u.name);

    // disabling echo
    tcgetattr(fileno(stdin), &oflags);
    nflags = oflags;
    nflags.c_lflag &= ~ECHO;
    nflags.c_lflag |= ECHONL;

    if (tcsetattr(fileno(stdin), TCSANOW, &nflags) != 0)
    {
        perror("tcsetattr");
        return exit(1);
    }
    printf("\n\n\n\n\n\t\t\t\tEnter the password to login:");
    scanf("%s", u.password);

    // restore terminal
    if (tcsetattr(fileno(stdin), TCSANOW, &oflags) != 0)
    {
        perror("tcsetattr");
        return exit(1);
    }
    retrieved = getPassword(u.name, db);
    if (retrieved == NULL)
    {
        printf("User not found\n");
        return;
    }
 printf("Entered Password: %s\n", u.password);
    printf("Retrieved Password: %s\n", retrieved);
    if (strcmp(retrieved, u.password) == 0)
    {
        printf("login success\n");
        mainMenu(u, db);
    }
    else
    {
        
        printf("wrong password\n");
        //printf(u.password);
        return;
    }
};

const char *getPassword(const char *username, sqlite3 *db) {
    const char *sql_password = "SELECT password FROM users WHERE name = ?;";
    sqlite3_stmt *stmt_pass;

    // Prepare the SQL statement
    if (sqlite3_prepare_v2(db, sql_password, -1, &stmt_pass, 0) != SQLITE_OK) {
        fprintf(stderr, "Error preparing statement: %s\n", sqlite3_errmsg(db));
        return NULL; // Return NULL on error
    }

    // Bind the username to the query
    sqlite3_bind_text(stmt_pass, 1, username, -1, SQLITE_STATIC);

    // Execute the query
    if (sqlite3_step(stmt_pass) == SQLITE_ROW) {
        const char *password = (const char *)sqlite3_column_text(stmt_pass, 0);
        sqlite3_finalize(stmt_pass);
            printf("Retrieved Password from DB: %s\n", password); // Debugging output
        return password; // Return the retrieved password
    } else {
        sqlite3_finalize(stmt_pass);
        return NULL; // Return NULL if no user is found
    }
}


void registerAcc(sqlite3 *db)
{
    struct User u;
    const char *sql_check = "SELECT name FROM users WHERE name = ?;";
    const char *sql_insert = "INSERT INTO users (id, name, password) VALUES (?, ?, ?);";

    sqlite3_stmt *stmt_check;
    sqlite3_stmt *stmt_insert;


    system("clear");
    printf("\n\n\n\t\t\t\t   Bank Management System\n\t\t\t\t\t User Name:");
    scanf("%s", u.name);
    if (sqlite3_prepare_v2(db, sql_check, -1, &stmt_check, 0) != SQLITE_OK)
    {
        fprintf(stderr, "Error preparing statement: %s\n", sqlite3_errmsg(db));
        return;
    }
    sqlite3_bind_text(stmt_check, 1, u.name,-1,SQLITE_STATIC);
    if (sqlite3_step(stmt_check) == SQLITE_ROW)
    {
        printf("Account already exists\n\n");
        sqlite3_finalize(stmt_check);
        return;
    }
    sqlite3_finalize(stmt_check);

    printf("\nCreate the password: ");
    scanf("%s", u.password);
      if (sqlite3_prepare_v2(db, sql_insert, -1, &stmt_insert, 0) != SQLITE_OK) {
        fprintf(stderr, "Error preparing statement: %s\n", sqlite3_errmsg(db));
        return;
    }
    u.id = 1;

     sqlite3_bind_int(stmt_check, 1, u.id);
    sqlite3_bind_text(stmt_check, 2, u.name, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt_check, 3, u.password, -1, SQLITE_STATIC);

     if (sqlite3_step(stmt_insert) != SQLITE_DONE) {
        fprintf(stderr, "Execution failed: %s\n", sqlite3_errmsg(db));
        return;
    } else {
        printf("✔ Account registered successfully!\n");
        mainMenu(u, db);
    }
    sqlite3_finalize(stmt_insert);

}
