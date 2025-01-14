#ifndef HEADER_H
#define HEADER_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>

// all fields for each record of an account
struct Record
{
    int id;
    int userId;
    char name[20];
    char country[21];
    int phone;// max 12 digits
    char accountType[10];
    int accountNbr;// max 12 digits
    double amount; // max 10 digits
    char deposit_date[11];
    char withdraw_date[11];
}; 

struct User
{
    int id;
    char name[20];
    char password[20];
};

// authentication functions
void loginMenu(sqlite3 *db);
void registerAcc(sqlite3 *db);
const char *getPassword(const char *username, sqlite3 *db) ;
int getUserId(const char *username, sqlite3 *db);

// system function
int getYesNoChoice(struct User *u, sqlite3 *db, const char *prompt);

void createNewAcc(struct User u, sqlite3 *db);
void mainMenu(struct User *u, sqlite3 *db);
void checkAllAccounts(struct User u,sqlite3 *db);
void update(struct User u, sqlite3 *db);
void checkAccountsDetails(struct User *u, sqlite3 *db);
void deleteAccount(struct User *u, sqlite3 *db);
void transferAcc(struct User u, sqlite3 *db);
//void completeTransfer(int accID, char *receiverName, struct User u, sqlite3 *db);
void makeTransaction(struct User u, sqlite3 *db);
void clearInputBuffer();
void success(struct User u, sqlite3 *db);



#endif // HEADER_H