#define _XOPEN_SOURCE
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include "header.h"
#include "database.h"
#include <stdbool.h>

void stayOrReturn(int notGood, void f(struct User u), struct User u, sqlite3 *db)
{
    int option;
    if (notGood == 0)
    {
        system("clear");
        printf("\n✖ Record not found!!\n");
    invalid:
        printf("\nEnter 0 to try again, 1 to return to main menu and 2 to exit:");
        scanf("%d", &option);
        if (option == 0)
            f(u);
        else if (option == 1)
            mainMenu(&u, db);
        else if (option == 2)
            exit(0);
        else
        {
            printf("Insert a valid operation!\n");
            goto invalid;
        }
    }
    else
    {
        printf("\nEnter 1 to go to the main menu and 0 to exit:");
        scanf("%d", &option);
    }
    if (option == 1)
    {
        system("clear");
        mainMenu(&u, db);
    }
    else
    {
        system("clear");
        exit(1);
    }
}

void success(struct User u, sqlite3 *db)
{
    int option;
    printf("\n✔ Success!\n\n");
invalid:
    printf("Enter 1 to go to the main menu and 0 to exit!\n");
    scanf("%d", &option);
    system("clear");
    if (option == 1)
    {
        mainMenu(&u, db);
    }
    else if (option == 0)
    {
        exit(1);
    }
    else
    {
        printf("Insert a valid operation!\n");
        goto invalid;
    }
}

// Helper function to validate date format
int validateDate(const char *date) {
    struct tm tm;
    return (strptime(date, "%Y-%m-%d", &tm) != NULL);
}

// Helper function to validate integer input
int getIntegerInput() {
    char input[100];
    int value;
    while (1) {
        if (fgets(input, sizeof(input), stdin) == NULL) {
            return -1;  // Error in input
        }
        if (sscanf(input, "%d", &value) == 1) {
            return value;
        }
        printf("Invalid input. Please enter a number: ");
    }
}

bool validateIntegerInput(int *value) {
    char buffer[50];
    if (scanf("%49s", buffer) != 1) {
        return false; // Invalid input
    }
    char *endptr;
    long int_value = strtol(buffer, &endptr, 10);
    if (*endptr != '\0') {
        return false; // Not a valid integer
    }
    *value = (int)int_value; // Store the valid integer
    return true;
}

bool validateDoubleInput(double *value) {
    char buffer[50];
    if (scanf("%49s", buffer) != 1) {
        return false; // Invalid input
    }
    char *endptr;
    double double_value = strtod(buffer, &endptr);
    if (*endptr != '\0') {
        return false; // Not a valid double
    }
    *value = double_value; // Store the valid double
    return true;
}

void createNewAcc(struct User u, sqlite3 *db) {
    struct Record r;
    const char *sql_check = "SELECT COUNT(*) FROM accounts WHERE account_number = ?;";
    const char *sql_insert = "INSERT INTO accounts (user_id, account_number, balance, account_type, phone_number, deposit_date, country) VALUES (?,?,?,?,?,?,?);";
    sqlite3_stmt *stmt_check;
    sqlite3_stmt *stmt_insert;

noAccount:
    system("clear");
    printf("\t\t\t===== New record =====\n");

    // Date input with validation
    do {
        printf("\nEnter today's date (YYYY-MM-DD): ");
        if (scanf("%10s", r.deposit_date) != 1 || !validateDate(r.deposit_date)) {
            printf("Invalid date format. Please use YYYY-MM-DD.\n");
            while (getchar() != '\n'); // Clear input buffer
            continue;
        }
        break; // Valid date entered
    } while (true);

    // Account number input with uniqueness check
    while (true) {
        printf("\nEnter the account number: ");
        if (!validateIntegerInput(&r.accountNbr)) {
            printf("Invalid input. Please enter a valid integer.\n");
            while (getchar() != '\n'); // Clear input buffer
            continue;
        }

        // Check if account number already exists
        if (sqlite3_prepare_v2(db, sql_check, -1, &stmt_check, 0) != SQLITE_OK) {
            fprintf(stderr, "Error preparing statement: %s\n", sqlite3_errmsg(db));
            return;
        }

        sqlite3_bind_int(stmt_check, 1, r.accountNbr);

        int step_result = sqlite3_step(stmt_check);
        if (step_result == SQLITE_ROW) {
            int count = sqlite3_column_int(stmt_check, 0);
            if (count > 0) {
                printf("Account number already exists. Please choose a different number.\n");
                sqlite3_finalize(stmt_check);
                continue;
            }
        } else {
            fprintf(stderr, "Error checking account number: %s\n", sqlite3_errmsg(db));
            sqlite3_finalize(stmt_check);
            return;
        }

        sqlite3_finalize(stmt_check);
        break; // Valid account number entered
    }

    // Country input
    printf("\nEnter the country: ");
    if (scanf("%49s", r.country) != 1) {
        printf("Invalid input. Please try again.\n");
        while (getchar() != '\n'); // Clear input buffer
        goto noAccount;
    }

    // Phone number input
    while (true) {
        printf("\nEnter the phone number: ");
        if (!validateIntegerInput(&r.phone)) {
            printf("Invalid input. Please enter a valid integer for phone number.\n");
            continue; // Loop continues until valid input is received
        }
        break; // Valid phone number entered
    }

    // Amount input
    while (true) {
        printf("\nEnter amount to deposit: $");
        if (!validateDoubleInput(&r.amount)) {
            printf("Invalid input. Please enter a valid number for the amount.\n");
            continue; // Loop continues until valid input is received
        }
        break; // Valid amount entered
    }

    // Account type input with validation
    do {
        printf("\nChoose the type of account:\n\t-> savings\n\t-> current\n\t-> fixed01(for 1 year)\n\t-> fixed02(for 2 years)\n\t-> fixed03(for 3 years)\n\n\tEnter your choice: ");
        
        if (scanf("%19s", r.accountType) != 1 || 
            (strcmp(r.accountType, "savings") != 0 && 
             strcmp(r.accountType, "current") != 0 &&
             strcmp(r.accountType, "fixed01") != 0 && 
             strcmp(r.accountType, "fixed02") != 0 && 
             strcmp(r.accountType, "fixed03") != 0)) 
        {
            printf("Invalid account type. Please choose from the provided options.\n");
            while (getchar() != '\n'); // Clear input buffer
            continue; 
        }
        
        break; // Valid account type entered
    } while (true);

    // Prepare and execute insert statement
    if (sqlite3_prepare_v2(db, sql_insert, -1, &stmt_insert, 0) != SQLITE_OK)
    {
        fprintf(stderr, "Error preparing insert statement: %s\n", sqlite3_errmsg(db));
        return;
    }

    sqlite3_bind_int(stmt_insert, 1, u.id);
    sqlite3_bind_int(stmt_insert, 2, r.accountNbr);
    sqlite3_bind_double(stmt_insert, 3, r.amount);
    sqlite3_bind_text(stmt_insert, 4, r.accountType, -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt_insert, 5, r.phone);
    sqlite3_bind_text(stmt_insert, 6, r.deposit_date, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt_insert, 7, r.country, -1, SQLITE_STATIC);

    if (sqlite3_step(stmt_insert) != SQLITE_DONE)
    {
        fprintf(stderr, "Execution failed: %s\n", sqlite3_errmsg(db));
    }
    else
    {
        printf("✔ Account created successfully!\n");
    }

    sqlite3_finalize(stmt_insert);

   success(u, db); 
}

void checkAllAccounts(struct User u, sqlite3 *db)
{
    const char *sql_retrieve =
        "SELECT account_number, deposit_date, country, phone_number, balance, account_type "
        "FROM accounts "
        "WHERE user_id = ?;";

    sqlite3_stmt *stmt_retrieve;

    // Prepare the SQL statement
    if (sqlite3_prepare_v2(db, sql_retrieve, -1, &stmt_retrieve, 0) != SQLITE_OK)
    {
        fprintf(stderr, "Error preparing statement: %s\n", sqlite3_errmsg(db));
        return;
    }

    // Bind the user ID to the query
    sqlite3_bind_int(stmt_retrieve, 1, u.id);

    // Execute the query
    if (sqlite3_step(stmt_retrieve) == SQLITE_ROW)
    {
        system("clear");
        printf("\t\t====== All accounts for user, %s =====\n\n", u.name);

        // Loop through the results
        do
        {
            struct Record r;

            // Retrieve account details
            r.accountNbr = sqlite3_column_int(stmt_retrieve, 0);
            const char *depositDate = (const char *)sqlite3_column_text(stmt_retrieve, 1);
            const char *country = (const char *)sqlite3_column_text(stmt_retrieve, 2);
            r.phone = sqlite3_column_int(stmt_retrieve, 3);
            r.amount = sqlite3_column_double(stmt_retrieve, 4);
            const char *accountType = (const char *)sqlite3_column_text(stmt_retrieve, 5);

            // Copy strings into the Record structure
            if (depositDate)
                strcpy(r.deposit_date, depositDate);
            if (country)
                strcpy(r.country, country);
            if (accountType)
                strcpy(r.accountType, accountType);

            // Display account information
            printf("_____________________\n");
            printf("\nAccount number: %d\nDeposit Date: %s\nCountry: %s\nPhone number: %d\nAmount deposited: $%.2f\nType Of Account: %s\n",
                   r.accountNbr, r.deposit_date, r.country, r.phone, r.amount, r.accountType);
        } while (sqlite3_step(stmt_retrieve) == SQLITE_ROW);
    }
    else
    {
        printf("No accounts found for user: %s\n", u.name);
    }

    // Finalize the statement
    sqlite3_finalize(stmt_retrieve);
    success(u, db);
}

void update(struct User u, sqlite3 *db)
{
    int accID;
    int choice;
    char newcountry[100]; // Use fixed-size array for country
    char newphone[20]; // Use a string to handle phone number input
    const char *sql_select = "SELECT * FROM accounts WHERE account_number = ? AND user_id = ?;";
    const char *sql_update_country = "UPDATE accounts SET country = ? WHERE account_number = ? AND user_id = ?;";
    const char *sql_update_phone = "UPDATE accounts SET phone_number = ? WHERE account_number = ? AND user_id = ?;";

    sqlite3_stmt *stmt_retrieve;
    sqlite3_stmt *stmt_update_phone;
    sqlite3_stmt *stmt_update_country;

    system("clear");
    printf("\nEnter your account number you want to update: ");
    if (scanf("%d", &accID) != 1) {
        printf("Invalid input. Please enter a valid account number.\n");
        while (getchar() != '\n'); // Clear input buffer
        return;
    }

    // Prepare the select statement
    if (sqlite3_prepare_v2(db, sql_select, -1, &stmt_retrieve, 0) != SQLITE_OK)
    {
        fprintf(stderr, "Error preparing statement: %s\n", sqlite3_errmsg(db));
        return;
    }
    sqlite3_bind_int(stmt_retrieve, 1, accID);
    sqlite3_bind_int(stmt_retrieve, 2, u.id); // Ensure the account belongs to the current user

    // Execute the select statement
    if (sqlite3_step(stmt_retrieve) != SQLITE_ROW)
    {
        printf("No account found with ID %d for this user\n", accID);
        sqlite3_finalize(stmt_retrieve);
        return;
    }

    printf("\nWhich field do you want to update:");
    printf("\n1 -- Phone number\n");
    printf("2 -- Country\n");
    if (scanf("%d", &choice) != 1) {
        printf("Invalid input. Please enter a valid choice.\n");
        while (getchar() != '\n'); // Clear input buffer
        sqlite3_finalize(stmt_retrieve);
        return;
    }

    if (choice == 1)
    {
        int valid_phone = 0;
        while (!valid_phone) {
            printf("Enter new phone number: ");
            if (scanf("%19s", newphone) != 1) {
                printf("Invalid input. Please enter a valid phone number.\n");
                while (getchar() != '\n'); // Clear input buffer
                continue;
            }

            // Check if the input contains only digits
            valid_phone = 1;
            for (int i = 0; newphone[i] != '\0'; i++) {
                if (!isdigit(newphone[i])) {
                    valid_phone = 0;
                    break;
                }
            }

            if (!valid_phone) {
                printf("Invalid phone number. Please enter only digits.\n");
            }
        }

        // Prepare the update phone statement
        if (sqlite3_prepare_v2(db, sql_update_phone, -1, &stmt_update_phone, 0) != SQLITE_OK)
        {
            fprintf(stderr, "Error preparing statement: %s\n", sqlite3_errmsg(db));
            sqlite3_finalize(stmt_retrieve);
            return;
        }
        sqlite3_bind_text(stmt_update_phone, 1, newphone, -1, SQLITE_STATIC);
        sqlite3_bind_int(stmt_update_phone, 2, accID);
        sqlite3_bind_int(stmt_update_phone, 3, u.id);

        // Execute the update phone statement
        if (sqlite3_step(stmt_update_phone) != SQLITE_DONE)
        {
            fprintf(stderr, "Execution failed: %s\n", sqlite3_errmsg(db));
            sqlite3_finalize(stmt_retrieve);
            sqlite3_finalize(stmt_update_phone);
            return;
        }
        else
        {
            printf("Phone number successfully updated\n");
        }

        // Finalize the statements
        sqlite3_finalize(stmt_retrieve);
        sqlite3_finalize(stmt_update_phone);
    }
    else if (choice == 2)
    {
        printf("Enter new country: ");
        if (scanf("%99s", newcountry) != 1) {
            printf("Invalid input. Please enter a valid country name.\n");
            while (getchar() != '\n'); // Clear input buffer
            sqlite3_finalize(stmt_retrieve);
            return;
        }

        // Prepare the update country statement
        if (sqlite3_prepare_v2(db, sql_update_country, -1, &stmt_update_country, 0) != SQLITE_OK)
        {
            fprintf(stderr, "Error preparing statement: %s\n", sqlite3_errmsg(db));
            sqlite3_finalize(stmt_retrieve);
            return;
        }
        sqlite3_bind_text(stmt_update_country, 1, newcountry, -1, SQLITE_STATIC);
        sqlite3_bind_int(stmt_update_country, 2, accID);
        sqlite3_bind_int(stmt_update_country, 3, u.id);

        // Execute the update country statement
        if (sqlite3_step(stmt_update_country) != SQLITE_DONE)
        {
            fprintf(stderr, "Execution failed: %s\n", sqlite3_errmsg(db));
            sqlite3_finalize(stmt_retrieve);
            sqlite3_finalize(stmt_update_country);
            return;
        }
        else
        {
            printf("Country successfully updated\n");
        }

        // Finalize the statements
        sqlite3_finalize(stmt_retrieve);
        sqlite3_finalize(stmt_update_country);
    }
    else
    {
        printf("Wrong choice\n");
        sqlite3_finalize(stmt_retrieve);
        return;
    }
    success(u, db);
}

void completeTransfer(int accID, char *receiverName, struct User u, sqlite3 *db)
{
    const char *sql_update_account = "UPDATE accounts SET user_id = (SELECT id FROM users WHERE name = ?) WHERE account_number = ?;";
    sqlite3_stmt *stmt_update_account;

    // Prepare and execute the update account statement
    if (sqlite3_prepare_v2(db, sql_update_account, -1, &stmt_update_account, 0) != SQLITE_OK)
    {
        fprintf(stderr, "Error preparing update account statement: %s\n", sqlite3_errmsg(db));
        return;
    }
    sqlite3_bind_text(stmt_update_account, 1, receiverName, -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt_update_account, 2, accID);

    if (sqlite3_step(stmt_update_account) != SQLITE_DONE)
    {
        fprintf(stderr, "Execution failed: %s\n", sqlite3_errmsg(db));
    }
    else
    {
        printf("Account ID %d successfully transferred to %s\n", accID, receiverName);
    }

    sqlite3_finalize(stmt_update_account);
}

void transferAcc(struct User u, sqlite3 *db)
{
    const char *sql_select_account = "SELECT * FROM accounts WHERE account_number = ? AND user_id = ?;";
    const char *sql_select_user = "SELECT id FROM users WHERE name = ?;";
    sqlite3_stmt *stmt_select_account;
    sqlite3_stmt *stmt_select_user;
    int accID;
    char newName[100];
    char choice;

    do {
        system("clear");
        printf("\nEnter the account number you want to transfer: ");
        if (scanf("%d", &accID) != 1) {
            printf("Invalid input. Please enter a valid account number.\n");
            while (getchar() != '\n'); // Clear input buffer
            continue;
        }

        // Begin transaction
        if (sqlite3_exec(db, "BEGIN TRANSACTION;", NULL, NULL, NULL) != SQLITE_OK)
        {
            fprintf(stderr, "Error starting transaction: %s\n", sqlite3_errmsg(db));
            return;
        }

        // Check if the account exists and belongs to the user
        if (sqlite3_prepare_v2(db, sql_select_account, -1, &stmt_select_account, 0) != SQLITE_OK)
        {
            fprintf(stderr, "Error preparing select account statement: %s\n", sqlite3_errmsg(db));
            goto rollback;
        }
        sqlite3_bind_int(stmt_select_account, 1, accID);
        sqlite3_bind_int(stmt_select_account, 2, u.id);

        if (sqlite3_step(stmt_select_account) != SQLITE_ROW)
        {
            printf("No account found with ID %d for this user\n", accID);
            sqlite3_finalize(stmt_select_account);
            goto ask_retry;
        }
        sqlite3_finalize(stmt_select_account);

        printf("\nEnter the name of the user you want to transfer to: ");
        scanf("%99s", newName);

        // Check if the receiver exists
        if (sqlite3_prepare_v2(db, sql_select_user, -1, &stmt_select_user, 0) != SQLITE_OK)
        {
            fprintf(stderr, "Error preparing select user statement: %s\n", sqlite3_errmsg(db));
            goto rollback;
        }
        sqlite3_bind_text(stmt_select_user, 1, newName, -1, SQLITE_STATIC);

        if (sqlite3_step(stmt_select_user) != SQLITE_ROW)
        {
            printf("No user found with name %s\n", newName);
            sqlite3_finalize(stmt_select_user);
            goto ask_retry;
        }
        sqlite3_finalize(stmt_select_user);

        // Complete the transfer process
        completeTransfer(accID, newName, u, db);

        // Commit transaction
        if (sqlite3_exec(db, "COMMIT;", NULL, NULL, NULL) != SQLITE_OK)
        {
            fprintf(stderr, "Error committing transaction: %s\n", sqlite3_errmsg(db));
            return;
        }

        // Run a simple query to refresh the connection state
        if (sqlite3_exec(db, "SELECT 1;", NULL, NULL, NULL) != SQLITE_OK)
        {
            fprintf(stderr, "Error executing SELECT 1: %s\n", sqlite3_errmsg(db));
        }

ask_retry:
        printf("\nDo you want to try another transfer? (y/n): ");
        scanf(" %c", &choice);
        while (getchar() != '\n'); // Clear input buffer

    } while (choice == 'y' || choice == 'Y');

    success(u, db);
    return;

rollback:
    sqlite3_exec(db, "ROLLBACK;", NULL, NULL, NULL);
    printf("Transfer operation failed.\n");
}
void checkAccountsDetails(struct User *u, sqlite3 *db)
{
    int accId;
    char choice;
    do {
        const char *sql_select = "SELECT account_number, deposit_date, country, phone_number, balance, account_type FROM accounts WHERE account_number = ? AND user_id = ?;";
        sqlite3_stmt *stmt_select;
        int accNumber;
        char depositDate[11]; // Format: YYYY-MM-DD
        char country[100];
        int phone;
        char accountType[20];
        double amount;
        double interest;
        int interestDay;

        system("clear");
        printf("\nEnter the account number you want to check: ");
        if (scanf("%d", &accId) != 1) {
            printf("Invalid input. Please enter a number.\n");
            while (getchar() != '\n'); // Clear input buffer
            continue;
        }

        // Prepare the SQL statement
        if (sqlite3_prepare_v2(db, sql_select, -1, &stmt_select, 0) != SQLITE_OK)
        {
            fprintf(stderr, "Error preparing statement: %s\n", sqlite3_errmsg(db));
            return;
        }

        // Bind the account ID and user ID
        sqlite3_bind_int(stmt_select, 1, accId);
        sqlite3_bind_int(stmt_select, 2, u->id);

        // Execute the statement and check for results
        if (sqlite3_step(stmt_select) != SQLITE_ROW)
        {
            printf("No account found with ID %d for this user.\n", accId);
            sqlite3_finalize(stmt_select);
        }
        else
        {
            // Retrieve account details
            accNumber = sqlite3_column_int(stmt_select, 0);
            strcpy(depositDate, (const char *)sqlite3_column_text(stmt_select, 1));
            strcpy(country, (const char *)sqlite3_column_text(stmt_select, 2));
            phone = sqlite3_column_int(stmt_select, 3);
            amount = sqlite3_column_double(stmt_select, 4);
            strcpy(accountType, (const char *)sqlite3_column_text(stmt_select, 5));

            // Extract the day from the deposit date
            interestDay = atoi(&depositDate[8]); // Assuming depositDate is in YYYY-MM-DD format

            // Display account details
            system("clear");
            printf("_____________________\n");
            printf("\nAccount number: %d\nDeposit Date: %s\nCountry: %s\nPhone number: %d\nAmount deposited: $%.2f\nType Of Account: %s\n",
                   accNumber, depositDate, country, phone, amount, accountType);

            // Calculate and display interest based on account type
            if (strcmp(accountType, "savings") == 0)
            {
                interest = (0.07 * amount) / 12; // Monthly interest
                printf("\nYou will get $%.2f interest on day %d of every month\n", interest, interestDay);
            }
            else if (strcmp(accountType, "fixed01") == 0)
            {
                interest = (0.04 * amount); // Total interest for 1 year
                printf("\nYou will get $%.2f interest on day %d of every month\n", interest, interestDay);
            }
            else if (strcmp(accountType, "fixed02") == 0)
            {
                interest = (0.05 * amount) * 2; // Total interest for 2 years
                printf("\nYou will get $%.2f interest on day %d of every month\n", interest, interestDay);
            }
            else if (strcmp(accountType, "fixed03") == 0)
            {
                interest = (0.08 * amount) * 3; // Total interest for 3 years
                printf("\nYou will get $%.2f interest on day %d of every month\n", interest, interestDay);
            }
            else if (strcmp(accountType, "current") == 0)
            {
                printf("You will not get interests because the account is of type current.\n");
            }

            sqlite3_finalize(stmt_select);
        }

        printf("\nDo you want to check another account? (y/n): ");
        scanf(" %c", &choice);
        while (getchar() != '\n'); // Clear input buffer

    } while (choice == 'y' || choice == 'Y');

    success(*u, db);
}

void deleteAccount(struct User *u, sqlite3 *db)
{
    int accId;
    char choice;
    
    do {
        printf("Enter the account ID you want to delete: ");
        scanf("%d", &accId);

        const char *sql_check = "SELECT COUNT(*) FROM accounts WHERE account_number = ? AND user_id = ?;";
        const char *sql_delete = "DELETE FROM accounts WHERE account_number = ? AND user_id = ?;";
        sqlite3_stmt *stmt_check;
        sqlite3_stmt *stmt_delete;
        int account_exists = 0;

        // Check if the account belongs to the user
        if (sqlite3_prepare_v2(db, sql_check, -1, &stmt_check, 0) != SQLITE_OK)
        {
            fprintf(stderr, "Error preparing check statement: %s\n", sqlite3_errmsg(db));
            return;
        }

        sqlite3_bind_int(stmt_check, 1, accId);
        sqlite3_bind_int(stmt_check, 2, u->id);

        if (sqlite3_step(stmt_check) == SQLITE_ROW)
        {
            account_exists = sqlite3_column_int(stmt_check, 0);
        }

        sqlite3_finalize(stmt_check);

        if (!account_exists)
        {
            printf("No account found with ID %d for this user.\n", accId);
        }
        else
        {
            // Proceed with deletion
            if (sqlite3_prepare_v2(db, sql_delete, -1, &stmt_delete, 0) != SQLITE_OK)
            {
                fprintf(stderr, "Error preparing delete statement: %s\n", sqlite3_errmsg(db));
                return;
            }

            sqlite3_bind_int(stmt_delete, 1, accId);
            sqlite3_bind_int(stmt_delete, 2, u->id);

            if (sqlite3_step(stmt_delete) != SQLITE_DONE)
            {
                fprintf(stderr, "Error deleting account: %s\n", sqlite3_errmsg(db));
            }
            else
            {
                printf("Account deleted successfully.\n");
            }

            sqlite3_finalize(stmt_delete);
        }

        printf("\nDo you want to delete another account? (y/n): ");
        scanf(" %c", &choice);
        while (getchar() != '\n'); // Clear input buffer

    } while (choice == 'y' || choice == 'Y');

    success(*u, db);
}

void makeTransaction(struct User u, sqlite3 *db)
{
    int accID;
    int choice;
    char amountStr[20];
    double amount;
    double balance;
    char accountType[20];
    const char *sql_select = "SELECT account_type, balance, account_number FROM accounts WHERE account_number = ? AND user_id = ?;";
    const char *sql_update = "UPDATE accounts SET balance = ? WHERE account_number = ? AND user_id = ?;";
    sqlite3_stmt *stmt_select;
    sqlite3_stmt *stmt_update;
    char retry;

    do {
        // Step 1: Prompt for Account ID
        printf("Enter the account ID you want to make a transaction on: ");
        if (scanf("%d", &accID) != 1) {
            printf("Invalid input. Please enter a valid account number.\n");
            while (getchar() != '\n'); // Clear input buffer
            continue;
        }

        // Prepare the select statement
        if (sqlite3_prepare_v2(db, sql_select, -1, &stmt_select, 0) != SQLITE_OK)
        {
            fprintf(stderr, "Error preparing statement: %s\n", sqlite3_errmsg(db));
            return;
        }
        sqlite3_bind_int(stmt_select, 1, accID);
        sqlite3_bind_int(stmt_select, 2, u.id);

        // Execute the select statement
        if (sqlite3_step(stmt_select) != SQLITE_ROW)
        {
            printf("No account found with ID %d for this user\n", accID);
            sqlite3_finalize(stmt_select);
            printf("Do you want to try again? (y/n): ");
            scanf(" %c", &retry);
            while (getchar() != '\n'); // Clear input buffer
            continue;
        }

        // Account found, retrieve account details
        strcpy(accountType, (const char *)sqlite3_column_text(stmt_select, 0));
        balance = sqlite3_column_double(stmt_select, 1);

        // Check if the account type is allowed for transactions
        if (strncmp(accountType, "fixed", 5) == 0)
        {
            printf("Error: Transactions are not allowed for %s accounts.\n", accountType);
            sqlite3_finalize(stmt_select);
            printf("Do you want to try another account? (y/n): ");
            scanf(" %c", &retry);
            while (getchar() != '\n'); // Clear input buffer
            continue;
        }

        printf("\nCurrent Balance: $%.2f\n", balance);
        printf("Account Type: %s\n", accountType);

        do {
            printf("\nDo you want to:\n");
            printf("1. Deposit\n");
            printf("2. Withdraw\n");
            printf("3. Cancel transaction\n");
            printf("Enter choice: ");
            if (scanf("%d", &choice) != 1) {
                printf("Invalid input. Please enter a valid choice.\n");
                while (getchar() != '\n'); // Clear input buffer
                choice = 0; // Set to invalid choice to repeat the loop
            } else if (choice < 1 || choice > 3) {
                printf("Invalid choice. Please enter 1, 2, or 3.\n");
                choice = 0; // Set to invalid choice to repeat the loop
            }
        } while (choice == 0);

        if (choice == 3) {
            printf("Transaction cancelled.\n");
            sqlite3_finalize(stmt_select);
            break;
        }

        // Input and validate amount
        int valid_amount = 0;
        while (!valid_amount) {
            printf("Enter amount: $");
            if (scanf("%19s", amountStr) != 1) {
                printf("Invalid input. Please enter a valid amount.\n");
                while (getchar() != '\n'); // Clear input buffer
                continue;
            }

            // Check if the input contains only digits and optionally one decimal point
            valid_amount = 1;
            int decimal_count = 0;
            for (int i = 0; amountStr[i] != '\0'; i++) {
                if (amountStr[i] == '.') {
                    decimal_count++;
                    if (decimal_count > 1) {
                        valid_amount = 0;
                        break;
                    }
                } else if (!isdigit(amountStr[i])) {
                    valid_amount = 0;
                    break;
                }
            }

            if (!valid_amount) {
                printf("Invalid amount. Please enter a valid number.\n");
            } else {
                amount = atof(amountStr);
                if (amount <= 0) {
                    printf("Amount must be greater than zero.\n");
                    valid_amount = 0;
                }
            }
        }

        if (choice == 1) {
            balance += amount;
            printf("\n$%.2f has been deposited to your account\n", amount);
        } else if (choice == 2) {
            if (balance < amount) {
                printf("\nInsufficient balance\n");
                sqlite3_finalize(stmt_select);
                printf("Do you want to try another transaction? (y/n): ");
                scanf(" %c", &retry);
                while (getchar() != '\n'); // Clear input buffer
                continue;
            }
            balance -= amount;
            printf("\n$%.2f has been withdrawn from your account\n", amount);
        }

        // Prepare the update statement
        if (sqlite3_prepare_v2(db, sql_update, -1, &stmt_update, 0) != SQLITE_OK)
        {
            fprintf(stderr, "Error preparing statement: %s\n", sqlite3_errmsg(db));
            sqlite3_finalize(stmt_select);
            return;
        }

        sqlite3_bind_double(stmt_update, 1, balance);
        sqlite3_bind_int(stmt_update, 2, accID);
        sqlite3_bind_int(stmt_update, 3, u.id);

        // Execute the update statement
        if (sqlite3_step(stmt_update) != SQLITE_DONE)
        {
            fprintf(stderr, "Execution failed: %s\n", sqlite3_errmsg(db));
        }
        else
        {
            printf("\nTransaction successful. New balance: $%.2f\n", balance);
        }

        // Finalize the statements
        sqlite3_finalize(stmt_select);
        sqlite3_finalize(stmt_update);

        printf("Do you want to make another transaction? (y/n): ");
        scanf(" %c", &retry);
        while (getchar() != '\n'); // Clear input buffer

    } while (retry == 'y' || retry == 'Y');

    success(u, db);
}