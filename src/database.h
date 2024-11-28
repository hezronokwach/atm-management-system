#ifndef DATABASE_H
#define DATABASE_H

#include <sqlite3.h>

// Function to initialize the database
int initializeDatabase(sqlite3 **db);

// Function to check and display user IDs
void checkUserIds(sqlite3 *db);

// You might want to add these additional functions if you plan to implement them
int resetUserIdCounter(sqlite3 *db);
int getMaxUserId(sqlite3 *db);

#endif // DATABASE_H