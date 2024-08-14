#include <stdio.h>
#include <sqlite3.h>
#include "database.h"

// Function to initialize the database
void initializeDatabase(sqlite3 **db) {
    int rc = sqlite3_open("atm_system.db", db);
    if (rc) {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(*db));
        return;
    }

    const char *create_users_table = "CREATE TABLE IF NOT EXISTS users ("
                                      "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                                      "name TEXT NOT NULL UNIQUE,"
                                      "password TEXT NOT NULL,"
                                      "phone TEXT,"
                                      "country TEXT);";

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

    const char *create_transfers_table = "CREATE TABLE IF NOT EXISTS transfers ("
                                         "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                                         "account_number INTEGER,"
                                         "sender_id INTEGER,"
                                         "receiver_id INTEGER,"
                                         "transfer_date TEXT,"
                                         "FOREIGN KEY (sender_id) REFERENCES users(id),"
                                         "FOREIGN KEY (receiver_id) REFERENCES users(id),"
                                         "FOREIGN KEY (account_number) REFERENCES accounts(account_number));";

    char *errMsg;

    if (sqlite3_exec(*db, create_users_table, 0, 0, &errMsg) != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", errMsg);
        sqlite3_free(errMsg);
    }

    if (sqlite3_exec(*db, create_accounts_table, 0, 0, &errMsg) != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", errMsg);
        sqlite3_free(errMsg);
    }

    if (sqlite3_exec(*db, create_transfers_table, 0, 0, &errMsg) != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", errMsg);
        sqlite3_free(errMsg);
    }
}
