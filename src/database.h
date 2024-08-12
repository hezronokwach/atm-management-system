#ifndef DATABASE_H
#define DATABASE_H

#include <sqlite3.h>
#include "header.h"

void initializeDatabase(sqlite3 **db);
void createNewAcc(struct User u, sqlite3 *db);
const char *getPassword(const char *username, sqlite3 *db) ;



#endif // DATABASE_H
