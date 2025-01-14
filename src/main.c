#include "header.h"
#include "database.h"
#include <stdbool.h>

void mainMenu(struct User *u, sqlite3 *db)
{
    int option;
    system("clear");
    printf("\n\n\t\t\t\t======= ATM =======\n\n");
    printf("\n\t\t-->> Feel free to choose one of the options below <<--\n");
    printf("\n\t\t[1]- Create a new account\n");
    printf("\n\t\t[2]- Update account information\n");
    printf("\n\t\t[3]- Check accounts\n");
    printf("\n\t\t[4]- Check list of owned account\n");
    printf("\n\t\t[5]- Make Transaction\n");
    printf("\n\t\t[6]- Remove existing account\n");
    printf("\n\t\t[7]- Transfer ownership\n");
    printf("\n\t\t[8]- Exit\n");
     if (scanf("%d", &option) != 1) {
        printf("\n\t\tInvalid input! Please enter a number between 1 and 8.\n");
        exit(1);
    }

    if (option < 1 || option > 8) {
        printf("\n\t\tInvalid choice! Please enter a number between 1 and 8.\n");
        exit(1);
    }

    switch (option)
    {
    case 1:
        //clearInputBuffer();
        createNewAcc(*u, db);
        break;
    case 2:
        update(*u, db);
        break;
    case 3:
        checkAccountsDetails(u, db);
        break;
    case 4:
        checkAllAccounts(*u, db);
        break;
    case 5:
        makeTransaction(*u, db);
        break;
    case 6:
        deleteAccount(u, db);
        break;
    case 7:
        transferAcc(*u, db);
        break;
    case 8:
        exit(1);
        break;
    default:
        //printf("Invalid operation!\n");
        exit(1);
        break;
    }
}

    void initMenu(struct User * u, sqlite3 * db)
    {
        bool validInput = false;
        int option;
        while (!validInput)
        {
            system("clear");
             printf("\n\n\t\t\t======= ATM =======\n\n");
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
        if (rc != SQLITE_OK)
        {
            fprintf(stderr, "Failed to initialize database. Error: %s\n", sqlite3_errmsg(db));
            return 1;
        }
        initMenu(&u, db);

        // Close the database connection
        sqlite3_close(db);
        printf("Thank you for using our ATM system. Goodbye!\n");
        return 0;
    }