#include "header.h"
#include <string.h> // Include this for strcpy
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
            mainMenu(u, db);
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
        mainMenu(u, db);
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
        mainMenu(u, db);
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

#include <stdbool.h> // Include this for using bool, true, and false
#include "header.h"
#include "database.h"

// Your existing code...

void createNewAcc(struct User u, sqlite3 *db)
{
    struct Record r;
    const char *sql_check = "SELECT account_type FROM accounts WHERE user_id =? AND account_number =?;";
    const char *sql_insert = "INSERT INTO accounts (user_id, account_number, balance, account_type, phone_number, deposit_date, country) VALUES (?,?,?,?,?,?,?);";
    sqlite3_stmt *stmt_check;
    sqlite3_stmt *stmt_insert;

noAccount:
    system("clear");
    printf("\t\t\t===== New record =====\n");

    printf("\nEnter today's date (YYYY-MM-DD): ");
    scanf("%s", r.deposit_date);

    printf("\nEnter the account number: ");
    scanf("%d", &r.accountNbr);

    // Prepare and execute check statement
    if (sqlite3_prepare_v2(db, sql_check, -1, &stmt_check, 0) != SQLITE_OK)
    {
        fprintf(stderr, "Error preparing statement: %s\n", sqlite3_errmsg(db));
        return;
    }

    sqlite3_bind_int(stmt_check, 1, u.id);
    sqlite3_bind_int(stmt_check, 2, r.accountNbr);

    if (sqlite3_step(stmt_check) == SQLITE_ROW)
    {
        printf("Account already exists\n\n");
        sqlite3_finalize(stmt_check);
        return;
    }

    sqlite3_finalize(stmt_check);

    printf("\nEnter the country: ");
    scanf("%s", r.country);

    printf("\nEnter the phone number: ");
    scanf("%d", &r.phone);

    printf("\nEnter amount to deposit: $");
    scanf("%lf", &r.amount);

    printf("\nChoose the type of account:\n\t-> saving\n\t-> current\n\t-> fixed01(for 1 year)\n\t-> fixed02(for 2 years)\n\t-> fixed03(for 3 years)\n\n\tEnter your choice: ");
    scanf("%s", r.accountType);

    // Validate account type input
    while (true)
    {
        if (strcmp(r.accountType, "saving") == 0 || strcmp(r.accountType, "current") == 0 ||
            strcmp(r.accountType, "fixed01") == 0 || strcmp(r.accountType, "fixed02") == 0 ||
            strcmp(r.accountType, "fixed03") == 0)
        {
            break;
        }
        else
        {
            printf("Invalid account type. Please choose from the provided options.\n");
            printf("\nChoose the type of account:\n\t-> saving\n\t-> current\n\t-> fixed01(for 1 year)\n\t-> fixed02(for 2 years)\n\t-> fixed03(for 3 years)\n\n\tEnter your choice: ");
            scanf("%s", r.accountType);
        }
    }

    // Prepare and execute insert statement
    if (sqlite3_prepare_v2(db, sql_insert, -1, &stmt_insert, 0) != SQLITE_OK)
    {
        fprintf(stderr, "Error preparing statement: %s\n", sqlite3_errmsg(db));
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
    int newphone;
    const char *sql_select = "SELECT * FROM accounts WHERE account_number = ?;";
    const char *sql_update_country = "UPDATE accounts SET country = ? WHERE account_number = ?;";
    const char *sql_update_phone = "UPDATE accounts SET phone_number = ? WHERE account_number = ?;";

    sqlite3_stmt *stmt_retrieve;
    sqlite3_stmt *stmt_update_phone;
    sqlite3_stmt *stmt_update_country;

    system("clear");
    printf("\nEnter the account number you want to update:");
    scanf("%d", &accID);

    // Prepare the select statement
    if (sqlite3_prepare_v2(db, sql_select, -1, &stmt_retrieve, 0) != SQLITE_OK)
    {
        fprintf(stderr, "Error preparing statement: %s\n", sqlite3_errmsg(db));
        return;
    }
    sqlite3_bind_int(stmt_retrieve, 1, accID);

    // Execute the select statement
    if (sqlite3_step(stmt_retrieve) != SQLITE_ROW)
    {
        printf("No account found with ID %d\n", accID);
        sqlite3_finalize(stmt_retrieve);
        return;
    }

    printf("\nWhich field do you want to update:");
    printf("\n1 -- Phone number\n");
    printf("2 -- Country\n");
    scanf("%d", &choice);

    if (choice == 1)
    {
        printf("Enter new phone number: ");
        scanf("%d", &newphone);

        // Prepare the update phone statement
        if (sqlite3_prepare_v2(db, sql_update_phone, -1, &stmt_update_phone, 0) != SQLITE_OK)
        {
            fprintf(stderr, "Error preparing statement: %s\n", sqlite3_errmsg(db));
            sqlite3_finalize(stmt_retrieve);
            return;
        }
        sqlite3_bind_int(stmt_update_phone, 1, newphone);
        sqlite3_bind_int(stmt_update_phone, 2, accID);

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
        printf("Enter new country:");
        scanf("%s", newcountry);

        // Prepare the update country statement
        if (sqlite3_prepare_v2(db, sql_update_country, -1, &stmt_update_country, 0) != SQLITE_OK)
        {
            fprintf(stderr, "Error preparing statement: %s\n", sqlite3_errmsg(db));
            sqlite3_finalize(stmt_retrieve);
            return;
        }
        sqlite3_bind_text(stmt_update_country, 1, newcountry, -1, SQLITE_STATIC);
        sqlite3_bind_int(stmt_update_country, 2, accID);

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
    const char *sql_select_account = "SELECT * FROM accounts WHERE account_number = ?;";
    const char *sql_select_user = "SELECT id FROM users WHERE name = ?;";
    sqlite3_stmt *stmt_select_account;
    sqlite3_stmt *stmt_select_user;
    int accID;
    char newName[100];

    system("clear");
    printf("\nEnter the account number you want to transfer: ");
    scanf("%d", &accID);

    // Begin transaction
    if (sqlite3_exec(db, "BEGIN TRANSACTION;", NULL, NULL, NULL) != SQLITE_OK)
    {
        fprintf(stderr, "Error starting transaction: %s\n", sqlite3_errmsg(db));
        return;
    }

    // Check if the account exists
    if (sqlite3_prepare_v2(db, sql_select_account, -1, &stmt_select_account, 0) != SQLITE_OK)
    {
        fprintf(stderr, "Error preparing select account statement: %s\n", sqlite3_errmsg(db));
        goto rollback;
    }
    sqlite3_bind_int(stmt_select_account, 1, accID);

    if (sqlite3_step(stmt_select_account) != SQLITE_ROW)
    {
        printf("No account found with ID %d\n", accID);
        sqlite3_finalize(stmt_select_account);
        goto rollback;
    }
    sqlite3_finalize(stmt_select_account);

    printf("\nWhich name do you want to transfer to: ");
    scanf("%s", newName);

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
        goto rollback;
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

    // Immediately check all accounts after transfer
    // checkAllAccounts(u, db); // Fetch the latest account data

    success(u, db);
    return;

rollback:
    // Rollback transaction in case of error
    sqlite3_exec(db, "ROLLBACK;", NULL, NULL, NULL);
}

void checkAccountsDetails(int accId, sqlite3 *db)
{
    const char *sql_select = "SELECT account_number, deposit_date, country, phone_number, balance, account_type FROM accounts WHERE account_number = ?;";
    sqlite3_stmt *stmt_select;
    int accNumber;
    char depositDate[11]; // Format: YYYY-MM-DD
    char country[100];
    int phone;
    char accountType[20];
    double amount;
    double interest;
    int interestDay;

    // Prepare the SQL statement
    if (sqlite3_prepare_v2(db, sql_select, -1, &stmt_select, 0) != SQLITE_OK)
    {
        fprintf(stderr, "Error preparing statement: %s\n", sqlite3_errmsg(db));
        return;
    }

    // Bind the account ID
    sqlite3_bind_int(stmt_select, 1, accId);

    // Execute the statement and check for results
    if (sqlite3_step(stmt_select) != SQLITE_ROW)
    {
        printf("No account found with ID %d\n", accId);
        sqlite3_finalize(stmt_select);
        return;
    }

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
           accNumber,
           depositDate,
           country,
           phone,
           amount,
           accountType);

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

    // Finalize the statement
    sqlite3_finalize(stmt_select);
    struct User u;
    success(u, db);
}

void deleteAccount(int accId, sqlite3 *db)
{
    const char *sql_delete = "DELETE FROM accounts WHERE account_number = ?;";
    sqlite3_stmt *stmt_delete;
    if (sqlite3_prepare_v2(db, sql_delete, -1, &stmt_delete, 0) != SQLITE_OK)
    {

        fprintf(stderr, "Error preparing statement: %s\n", sqlite3_errmsg(db));
        return;
    }
    sqlite3_bind_int(stmt_delete, 1, accId);
    if (sqlite3_step(stmt_delete) != SQLITE_DONE)
    {
        printf("No account found");
        sqlite3_finalize(stmt_delete);
        return;
    }
    else
    {
        printf("Deleted successfully\n");
    }
    sqlite3_finalize(stmt_delete);
    struct User u;
    success(u, db);
}

void makeTransaction(struct User u, sqlite3 *db)
{
    int accID;
    int choice;
    char transactionType[10];
    double amount;
    double balance;
    char accountType[20];
    const char *sql_select = "SELECT account_type, balance, account_number FROM accounts WHERE account_number = ?;";
    const char *sql_update = "UPDATE accounts SET balance = ? WHERE account_number = ?;";
    sqlite3_stmt *stmt_select;
    sqlite3_stmt *stmt_update;

    // Step 1: Prompt for Account ID
    printf("Enter the account ID you want to make a transaction on: ");
    scanf("%d", &accID);

    // Step 2: Validate Account ID
    if (sqlite3_prepare_v2(db, sql_select, -1, &stmt_select, 0) != SQLITE_OK)
    {
        fprintf(stderr, "Error preparing statement: %s\n", sqlite3_errmsg(db));
        return;
    }
    sqlite3_bind_int(stmt_select, 1, accID);

    if (sqlite3_step(stmt_select) != SQLITE_ROW)
    {
        printf("No account found with ID %d\n", accID);
        sqlite3_finalize(stmt_select);
        return;
    }

    // Step 3: Retrieve Account Details
    strncpy(accountType, (const char *)sqlite3_column_text(stmt_select, 0), sizeof(accountType) - 1);
    balance = sqlite3_column_double(stmt_select, 1);

    sqlite3_finalize(stmt_select);

    // Step 4: Validate Account Type
    if (strcmp(accountType, "fixed01") == 0 || strcmp(accountType, "fixed02") == 0 || strcmp(accountType, "fixed03") == 0)
    {
        printf("Transactions are not allowed for this account type.\n");
        return;
    }

    // Step 5: Prompt for Transaction Type and Amount
    printf("Do you want to withdraw or deposit money? (withdraw/deposit) ");
    printf("\n1 --> withdraw\n");
    printf("2 --> deposit\n");
    scanf("%d", &choice);
    printf("Enter the amount for the transaction: ");
    scanf("%lf", &amount);

    // Step 6: Perform Transaction
    if (choice == 1)
    {
        if (amount <= balance)
        {
            balance -= amount;
        }
        else
        {
            printf("Insufficient balance.\n");
            return;
        }
    }
    else if ((choice == 2))
    {
        balance += amount;
    }
    else
    {
        printf("Invalid transaction type.\n");
        return;
    }

    // Step 7: Update Database
    if (sqlite3_prepare_v2(db, sql_update, -1, &stmt_update, 0) != SQLITE_OK)
    {
        fprintf(stderr, "Error preparing statement: %s\n", sqlite3_errmsg(db));
        return;
    }
    sqlite3_bind_double(stmt_update, 1, balance);
    sqlite3_bind_int(stmt_update, 2, accID);

    if (sqlite3_step(stmt_update) != SQLITE_DONE)
    {
        fprintf(stderr, "Execution failed: %s\n", sqlite3_errmsg(db));
    }

    sqlite3_finalize(stmt_update);

    // Step 8: Display Transaction Details
    printf("Transaction type: %s\n", transactionType);
    printf("Amount: %.2f\n", amount);
    printf("Updated balance: $%.2f\n", balance);

    // Step 9: Return to Main Menu
    success(u, db);
}
