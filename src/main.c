#include "header.h"
#include "database.h"
#include <stdbool.h>

void mainMenu(struct User *u, sqlite3 *db)
{

    int option;
    system("clear");
    printf("\n\n\t\t======= ATM =======\n\n");
    printf("\n\t\t-->> Feel free to choose one of the options below <<--\n");
    printf("\n\t\t[1]- Create a new account\n");
    printf("\n\t\t[2]- Update account information\n");
    printf("\n\t\t[3]- Check accounts\n");
    printf("\n\t\t[4]- Check list of owned account\n");
    printf("\n\t\t[5]- Make Transaction\n");
    printf("\n\t\t[6]- Remove existing account\n");
    printf("\n\t\t[7]- Transfer ownership\n");
    printf("\n\t\t[8]- Exit\n");
    scanf("%d", &option);

    switch (option)
    {
    case 1:
        createNewAcc(*u, db);
        break;
    case 2:
        update(*u, db);
        // student TODO : add your **Update account information** function
        // here
        break;
    case 3:

        // int accID;
        // printf("Enter the account number you want to check: ");
        // scanf("%d", &accID);
        checkAccountsDetails(u, db);
        // student TODO : add your **Check the details of existing accounts** function
        // here
        break;
    case 4:
        checkAllAccounts(*u, db);
        break;
    case 5:
    makeTransaction(*u,db);
        // student TODO : add your **Make transaction** function
        // here
        break;
    case 6:
        // int accId;
        // printf("Enter the account ID you want to delete: ");
        // scanf("%d", &accId);
        deleteAccount(u, db);
        // student TODO : add your **Remove existing account** function
        // here
        break;
    case 7:
    transferAcc(*u,db);
        // student TODO : add your **Transfer owner** function
        // here
        break;
    case 8:
        exit(1);
        break;
    default:
        printf("Invalid operation!\n");
    }
}

void initMenu(struct User *u, sqlite3 *db)
{
    bool validInput = false;
    int option;
    while (!validInput)
    {
        system("clear");
        printf("\n\n\t\t======= ATM =======\n");
        printf("\n\t\t-->> Feel free to login / register :\n");
        printf("\n\t\t[1]- login\n");
        printf("\n\t\t[2]- register\n");
        printf("\n\t\t[3]- exit\n");

        scanf("%d", &option);
        switch (option)
        {
        case 1:
            loginMenu(db);
            validInput = true;
            break;
        case 2:

            registerAcc(db);
            validInput = true;

            // student TODO : add your **Registration** function
            // here
            break;
        case 3:
            exit(1);
            break;
        default:
            printf("Insert a valid operation!\n");
            validInput = true;
            break;
        }
    }
}
int main()
{
    struct User u;
    sqlite3 *db; // Declare the database pointer
    int rc;

    // Initialize the database and create tables
    rc = initializeDatabase(&db);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to initialize database. Error: %s\n", sqlite3_errmsg(db));
        return 1;
    }

    // Check current user IDs (for debugging purposes)
    checkUserIds(db);

    // Main program loop
    while (1) {
        initMenu(&u, db);
        
        // After each session, ask if the user wants to continue
        char continue_choice;
        printf("\nDo you want to perform another operation? (y/n): ");
        scanf(" %c", &continue_choice);
        if (continue_choice != 'y' && continue_choice != 'Y') {
            break;
        }
    }

    // Close the database connection
    sqlite3_close(db);
    printf("Thank you for using our ATM system. Goodbye!\n");
    return 0;
}