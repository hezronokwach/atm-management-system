#include <stdio.h>
#include <sqlite3.h>
#include "database.h"

int initializeDatabase(sqlite3 **db)
{
    int rc = sqlite3_open("atm_system.db", db);
    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(*db));
        return rc;
    }

    const char *create_users_table = "CREATE TABLE IF NOT EXISTS users ("
                                     "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                                     "name TEXT NOT NULL UNIQUE,"
                                     "password TEXT NOT NULL);";

    const char *create_accounts_table = "CREATE TABLE IF NOT EXISTS accounts ("
                                        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                                        "user_id INTEGER,"
                                        "phone_number INTEGER,"
                                        "account_number INTEGER UNIQUE,"
                                        "balance REAL DEFAULT 0,"
                                        "account_type TEXT,"
                                        "deposit_date TEXT,"
                                        "withdraw_date TEXT,"
                                        "country TEXT,"
                                        "FOREIGN KEY (user_id) REFERENCES users(id));";

    char *errMsg = 0;

    rc = sqlite3_exec(*db, create_users_table, 0, 0, &errMsg);
    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "SQL error: %s\n", errMsg);
        sqlite3_free(errMsg);
        return rc;
    }

    rc = sqlite3_exec(*db, create_accounts_table, 0, 0, &errMsg);
    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "SQL error: %s\n", errMsg);
        sqlite3_free(errMsg);
        return rc;
    }

    return SQLITE_OK;
}

void checkUserIds(sqlite3 *db)
{
    const char *sql_dump_users = "SELECT id, name FROM users ORDER BY id;";
    sqlite3_stmt *stmt_dump_users;

    printf("Current users in the database:\n");

    if (sqlite3_prepare_v2(db, sql_dump_users, -1, &stmt_dump_users, 0) == SQLITE_OK) {
        while (sqlite3_step(stmt_dump_users) == SQLITE_ROW) {
            printf("User ID: %d, Name: %s\n",
                   sqlite3_column_int(stmt_dump_users, 0),
                   sqlite3_column_text(stmt_dump_users, 1));
        }
        sqlite3_finalize(stmt_dump_users);
    } else {
        fprintf(stderr, "Error preparing statement: %s\n", sqlite3_errmsg(db));
    }
}

int resetUserIdCounter(sqlite3 *db)
{
    const char *sql_reset = "DELETE FROM sqlite_sequence WHERE name='users';";
    char *errMsg = 0;
    int rc = sqlite3_exec(db, sql_reset, 0, 0, &errMsg);

    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "SQL error: %s\n", errMsg);
        sqlite3_free(errMsg);
        return rc;
    }

    printf("User ID counter has been reset.\n");
    return SQLITE_OK;
}

int getMaxUserId(sqlite3 *db)
{
    const char *sql_max_id = "SELECT MAX(id) FROM users;";
    sqlite3_stmt *stmt_max_id;
    int max_id = 0;

    if (sqlite3_prepare_v2(db, sql_max_id, -1, &stmt_max_id, 0) == SQLITE_OK) {
        if (sqlite3_step(stmt_max_id) == SQLITE_ROW) {
            max_id = sqlite3_column_int(stmt_max_id, 0);
        }
        sqlite3_finalize(stmt_max_id);
    } else {
        fprintf(stderr, "Error preparing statement: %s\n", sqlite3_errmsg(db));
    }

    return max_id;
}