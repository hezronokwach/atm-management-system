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
    char name[100];
    char country[100];
    int phone;
    char accountType[10];
    int accountNbr;
    double amount;
    char deposit_date[11];
    char withdraw_date[11];
};

struct User
{
    int id;
    char name[50];
    char password[50];
};

// authentication functions
void loginMenu(sqlite3 *db);
int getUserId(const char *username, sqlite3 *db);
void registerAcc(sqlite3 *db);
//void registerMenu(char a[50], char pass[50]);
const char *getPassword(const char *username, sqlite3 *db) ;

// system function
void createNewAcc(struct User u, sqlite3 *db);
void mainMenu(struct User u, sqlite3 *db);
void checkAllAccounts(struct User u,sqlite3 *db);
void update(struct User u, sqlite3 *db);
void checkAccountsDetails(int accId, sqlite3 *db);
void deleteAccount(int accId, sqlite3 *db);
void transferAcc(struct User u, sqlite3 *db);
//void completeTransfer(int accID, char *receiverName, struct User u, sqlite3 *db);
void makeTransaction(struct User u, sqlite3 *db);


#endif // HEADER_H