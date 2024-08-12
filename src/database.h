#ifndef DATABASE_H
#define DATABASE_H

#include <sqlite3.h>
#include "header.h"

void initializeDatabase(sqlite3 **db);

#endif // DATABASE_H
