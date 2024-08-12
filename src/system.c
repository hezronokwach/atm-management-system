#include "header.h"
#include <string.h> // Include this for strcpy

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

void createNewAcc(struct User u, sqlite3 *db)
{
    struct Record r;
    char userName[50];
    char *errMsg;
    const char *sql_check = "SELECT account_type FROM accounts WHERE user_id = ? AND account_number = ?;";
    const char *sql_insert = "INSERT INTO accounts (user_id, account_number, balance, account_type, phone_number,deposit_date, country) VALUES (?, ?, ?, ?, ?,?,?);";
    sqlite3_stmt *stmt_check;
    sqlite3_stmt *stmt_insert;

noAccount:
    system("clear");
    printf("\t\t\t===== New record =====\n");

    printf("\nEnter today's date (YYYY-MM-DD): ");
    scanf("%s", r.deposit_date);
    printf("\nEnter the account number:");
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

    printf("\nEnter the country:");
    scanf("%s", r.country);
    printf("\nEnter the phone number:");
    scanf("%d", &r.phone);
    printf("\nEnter amount to deposit: $");
    scanf("%lf", &r.amount);
    printf("\nChoose the type of account:\n\t-> saving\n\t-> current\n\t-> fixed01(for 1 year)\n\t-> fixed02(for 2 years)\n\t-> fixed03(for 3 years)\n\n\tEnter your choice:");
    scanf("%s", r.accountType);

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
    const char *sql_retrieve = "SELECT account_number, deposit_date, country, phone_number, balance, account_type FROM accounts WHERE user_id = ?;";
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
            {
                strcpy(r.deposit_date, depositDate); // Copy deposit date
            }
            if (country)
            {
                strcpy(r.country, country); // Copy country
            }
            if (accountType)
            {
                strcpy(r.accountType, accountType); // Copy account type
            }

            // Display account information
            printf("_____________________\n");
            printf("\nAccount number: %d\nDeposit Date: %s\nCountry: %s\nPhone number: %d\nAmount deposited: $%.2f\nType Of Account: %s\n",
                   r.accountNbr,
                   r.deposit_date,
                   r.country,
                   r.phone,
                   r.amount,
                   r.accountType);

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
    success(u,db);
}
