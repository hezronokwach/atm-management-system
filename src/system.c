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

void success(struct User u, sqlite3 *db)
{
    char buffer[100];
    printf("\n✔ Success!\n\n");

    clearInputBuffer(); // Clear any previous input

    while (1)
    {
        printf("Enter 1 to go to the main menu and 0 to exit!\n");

        if (fgets(buffer, sizeof(buffer), stdin) == NULL)
        {
            printf("Error reading input.\n");
            continue;
        }
        buffer[strcspn(buffer, "\n")] = 0;

        // Check if input is exactly one character and is '0' or '1'
        if (strlen(buffer) != 1 || (buffer[0] != '0' && buffer[0] != '1'))
        {
            printf("Invalid input! Please enter only 0 or 1.\n\n"); // Added newline
            continue;                                               // This will immediately show the prompt again
        }

        if (buffer[0] == '0')
        {
            exit(1);
        }
        else
        { // Must be '1' at this point
            system("clear");
            mainMenu(&u, db);
        }
    }
}

int getValidAccountNumber()
{
    char buffer[100];
    int accId;

    while (1)
    {
        if (fgets(buffer, sizeof(buffer), stdin) == NULL)
        {
            printf("Error reading input.\n");
            printf("\nEnter the account number: ");
            continue;
        }

        // Check if input is too long (no newline found)
        if (strchr(buffer, '\n') == NULL)
        {
            // Clear the remaining input
            while (getchar() != '\n')
                ;
            printf("Input too long. Please enter a number with maximum 12 digits.\n");
            printf("\nEnter the account number: ");
            continue;
        }

        // Remove newline
        buffer[strcspn(buffer, "\n")] = 0;

        // Check for empty input
        if (strlen(buffer) == 0)
        {
            printf("Invalid input. Please enter a number.\n");
            printf("\nEnter the account number: ");
            continue;
        }

        // Check length (max 12 digits)
        if (strlen(buffer) > 12)
        {
            printf("Number too long. Please enter maximum 12 digits.\n");
            printf("\nEnter the account number: ");
            continue;
        }

        // Check if input contains only digits
        int valid = 1;
        for (int i = 0; buffer[i] != '\0'; i++)
        {
            if (!isdigit(buffer[i]))
            {
                valid = 0;
                break;
            }
        }

        if (!valid)
        {
            printf("Invalid input. Please enter only digits.\n");
            printf("\nEnter the account number: ");
            continue;
        }

        // Convert to integer after validation
        accId = atoi(buffer);

        // Additional validation for reasonable account number range
        if (accId <= 0)
        {
            printf("Invalid account number. Please enter a positive number.\n");
            printf("\nEnter the account number: ");
            continue;
        }

        return accId;
    }
}

int getYesNoChoice(struct User *u, sqlite3 *db, const char *prompt)
{
    char continue_choice[10];

    do
    {
        printf("\n%s (y/n): ", prompt);

        if (fgets(continue_choice, sizeof(continue_choice), stdin) == NULL)
        {
            printf("Error reading input.\n");
            continue;
        }

        // Remove newline
        continue_choice[strcspn(continue_choice, "\n")] = 0;

        // Check if input is exactly "y" or "n"
        if (strcmp(continue_choice, "y") != 0 && strcmp(continue_choice, "n") != 0)
        {
            printf("Please enter a valid option ('y' or 'n')!\n");
            continue;
        }

        // At this point, input must be exactly "y" or "n"
        return (strcmp(continue_choice, "y") == 0) ? 1 : 0;

    } while (1);
}
// Helper function to validate date format
int isLeapYear(int year)
{
    return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}

int validateDate(const char *date)
{
    struct tm tm;
    time_t now = time(NULL);
    struct tm *current_time = localtime(&now);
    
    if (strptime(date, "%Y-%m-%d", &tm) == NULL)
    {
        printf("Invalid date format. Please use YYYY-MM-DD.\n");
        return 0;
    }

    int year = tm.tm_year + 1900;
    int month = tm.tm_mon + 1;
    int day = tm.tm_mday;

    int current_year = current_time->tm_year + 1900;
    int current_month = current_time->tm_mon + 1;
    int current_day = current_time->tm_mday;

    // Check if date is in the future
    if (year > current_year || 
        (year == current_year && month > current_month) ||
        (year == current_year && month == current_month && day > current_day))
    {
        printf("Error: Date cannot be in the future. Today is %d-%02d-%02d.\n", 
               current_year, current_month, current_day);
        return 0;
    }

    // Check year range
    if (year < 1900)
    {
        printf("Error: Year must be 1900 or later.\n");
        return 0;
    }

    // Check month range
    if (month < 1 || month > 12)
    {
        printf("Error: Month must be between 1 and 12.\n");
        return 0;
    }

    // Check day range
    int maxDay;
    switch (month)
    {
        case 4: case 6: case 9: case 11:
            maxDay = 30;
            break;
        case 2:
            maxDay = isLeapYear(year) ? 29 : 28;
            break;
        default:
            maxDay = 31;
    }

    if (day < 1 || day > maxDay)
    {
        printf("Error: Invalid day for the given month.\n");
        return 0;
    }

    return 1;
}

int isEmptyInput(const char *str) {
    // Return 1 if string is empty or only whitespace
    while (*str) {
        if (!isspace(*str)) {
            return 0;  // Found a non-space character
        }
        str++;
    }
    return 1;  // Only found spaces or empty string
}

void createNewAcc(struct User u, sqlite3 *db)
{
    struct Record r;
    char buffer[100];
    const char *sql_check = "SELECT COUNT(*) FROM accounts WHERE account_number = ?;";
    const char *sql_insert = "INSERT INTO accounts (user_id, account_number, balance, account_type, phone_number, deposit_date, country) VALUES (?,?,?,?,?,?,?);";
    sqlite3_stmt *stmt_check;
    sqlite3_stmt *stmt_insert;
    //clearInputBuffer();
    system("clear");
    printf("\t\t\t===== New record =====\n");

    // Date input with validation
    do
    {
        printf("\nEnter today's date (YYYY-MM-DD): ");
        if (fgets(buffer, sizeof(buffer), stdin) == NULL)
        {
            printf("Error reading input.\n");
            continue;
        }
        buffer[strcspn(buffer, "\n")] = 0; // Remove newline

        if (isEmptyInput(buffer)) {
        printf("Date cannot be empty. Please enter a valid date.\n");
        continue;
    }

        if (strlen(buffer) != 10 || !validateDate(buffer))
        {
            printf("Invalid date format. Please use YYYY-MM-DD.\n");
            continue;
        }
        if (strlen(buffer) > sizeof(r.deposit_date) - 1)
        {
            printf("Date input too long. Maximum length is %zu characters.\n", sizeof(r.deposit_date) - 1);
            continue;
        }
        strcpy(r.deposit_date, buffer);
        break;
    } while (1);

    // Account number input
    do
    {
        printf("\nEnter the account number (max 12 digits): ");
        if (fgets(buffer, sizeof(buffer), stdin) == NULL)
        {
            printf("Error reading input.\n");
            continue;
        }
        buffer[strcspn(buffer, "\n")] = 0; // Remove newline

         if (isEmptyInput(buffer)) {
        printf("Account number cannot be empty. Please enter a valid number.\n");
        continue;
    }

        if (strlen(buffer) > 12)
        {
            printf("Account number too long. Maximum length is 12 digits.\n");
            continue;
        }

        // Check if input contains only digits
        int valid = 1;
        for (int i = 0; buffer[i] != '\0'; i++)
        {
            if (!isdigit(buffer[i]))
            {
                valid = 0;
                break;
            }
        }

        if (!valid)
        {
            printf("Invalid account number. Please enter only digits.\n");
            continue;
        }

        r.accountNbr = atoi(buffer);

        // Check if account exists
        if (sqlite3_prepare_v2(db, sql_check, -1, &stmt_check, 0) != SQLITE_OK)
        {
            fprintf(stderr, "Error preparing statement: %s\n", sqlite3_errmsg(db));
            return;
        }

        sqlite3_bind_int(stmt_check, 1, r.accountNbr);

        if (sqlite3_step(stmt_check) == SQLITE_ROW)
        {
            int count = sqlite3_column_int(stmt_check, 0);
            if (count > 0)
            {
                printf("Account number already exists. Please choose a different number.\n");
                sqlite3_finalize(stmt_check);
                continue;
            }
        }
        sqlite3_finalize(stmt_check);
        break;
    } while (1);

    // Country input with validation
    do
    {
        printf("\nEnter the country (max 20 characters): ");
        if (fgets(buffer, sizeof(buffer), stdin) == NULL)
        {
            printf("Error reading input.\n");
            continue;
        }
        buffer[strcspn(buffer, "\n")] = 0; // Remove newline

        if (isEmptyInput(buffer)) {
        printf("Country name cannot be empty. Please enter a valid country name.\n");
        continue;
    }

        if (strlen(buffer) > 20)
        {
            printf("Country name too long. Maximum length is 20 characters.\n");
            continue;
        }

        // Check if input contains only letters and spaces
        int valid = 1;
        for (int i = 0; buffer[i] != '\0'; i++)
        {
            if (!isalpha(buffer[i]) && !isspace(buffer[i]))
            {
                valid = 0;
                break;
            }
        }

        if (!valid)
        {
            printf("Invalid country name. Please enter only letters and spaces.\n");
            continue;
        }

        strncpy(r.country, buffer, sizeof(r.country) - 1);
        r.country[sizeof(r.country) - 1] = '\0';
        break;
    } while (1);

    // Phone number input
    do
    {
        char phone_str[500];
        printf("\nEnter the phone number (max 12 digits): ");
        if (fgets(phone_str, sizeof(phone_str), stdin) == NULL)
        {
            printf("Error reading input. Please try again.\n");
            continue;
        }
        phone_str[strcspn(phone_str, "\n")] = 0; // Remove newline

        if (isEmptyInput(phone_str)) {
        printf("Phone number cannot be empty. Please enter a valid number.\n");
        continue;
    }

        if (strlen(phone_str) > 12)
        {
            printf("Phone number too long. Maximum length is 12 digits.\n");
            continue;
        }

        // Check if the input contains only digits
        int valid_phone = 1;
        for (int i = 0; phone_str[i] != '\0'; i++)
        {
            if (!isdigit(phone_str[i]))
            {
                valid_phone = 0;
                break;
            }
        }

        if (!valid_phone)
        {
            printf("Invalid phone number. Please enter only digits.\n");
            continue;
        }

        r.phone = atoi(phone_str);
        break;
    } while (1);

    // Amount input
    do
    {
        char amount_str[500];
        printf("\nEnter amount to deposit (max 10 digits): $");
        if (fgets(amount_str, sizeof(amount_str), stdin) == NULL)
        {
            printf("Error reading input. Please try again.\n");
            continue;
        }
        amount_str[strcspn(amount_str, "\n")] = 0; // Remove newline

        if (isEmptyInput(amount_str)) {
        printf("Amount cannot be empty. Please enter a valid amount.\n");
        continue;
    }

        if (strlen(amount_str) > 10)
        {
            printf("Amount too long. Maximum length is 10 digits.\n");
            continue;
        }

        // Check if the input is a valid float
        char *endptr;
        r.amount = strtod(amount_str, &endptr);
        if (*endptr != '\0' || r.amount < 0)
        {
            printf("Invalid amount. Please enter a positive number.\n");
            continue;
        }
        break;
    } while (1);

    // Account type input with validation
    do
    {
        char accountType_str[500]; // Increased buffer size to handle longer inputs
        printf("\nChoose the type of account:\n\t-> savings\n\t-> current\n\t-> fixed01(for 1 year)\n\t-> fixed02(for 2 years)\n\t-> fixed03(for 3 years)\n\n\tEnter your choice: ");
        if (fgets(accountType_str, sizeof(accountType_str), stdin) == NULL)
        {
            printf("Error reading input.\n");
            continue;
        }
        accountType_str[strcspn(accountType_str, "\n")] = 0; // Remove newline
         if (isEmptyInput(accountType_str)) {
        printf("Account type cannot be empty. Please choose from the options provided.\n");
        continue;
    }

        if (strlen(accountType_str) > 9)
        {
            printf("Account type too long. Maximum length is 9 characters.\n");
            clearInputBuffer(); // Clear any remaining input
            continue;
        }

        // Check if the input contains only letters and numbers
        int valid_account_type = 1;
        for (int i = 0; accountType_str[i] != '\0'; i++)
        {
            if (!isalpha(accountType_str[i]) && !isdigit(accountType_str[i]))
            {
                valid_account_type = 0;
                break;
            }
        }

        if (!valid_account_type)
        {
            printf("Invalid account type. Please enter only letters and numbers.\n");
            continue;
        }

        if (strcmp(accountType_str, "savings") != 0 &&
            strcmp(accountType_str, "current") != 0 &&
            strcmp(accountType_str, "fixed01") != 0 &&
            strcmp(accountType_str, "fixed02") != 0 &&
            strcmp(accountType_str, "fixed03") != 0)
        {
            printf("Invalid account type. Please choose from the provided options.\n");
            continue;
        }

        strncpy(r.accountType, accountType_str, sizeof(r.accountType) - 1);
        r.accountType[sizeof(r.accountType) - 1] = '\0'; // Ensure null-termination
        break;
    } while (1);

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
    mainMenu(&u, db);
}
void clearInputBuffer()
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF)
    {
    }
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
        //clearInputBuffer();
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
    char newcountry[500];
    char newphone[500];
    const char *sql_select = "SELECT * FROM accounts WHERE account_number = ? AND user_id = ?;";
    const char *sql_update_country = "UPDATE accounts SET country = ? WHERE account_number = ? AND user_id = ?;";
    const char *sql_update_phone = "UPDATE accounts SET phone_number = ? WHERE account_number = ? AND user_id = ?;";

    sqlite3_stmt *stmt_retrieve;
    sqlite3_stmt *stmt_update_phone;
    sqlite3_stmt *stmt_update_country;
    //clearInputBuffer();
    system("clear");
    while (1)
    {
        printf("\nEnter your account number you want to update: ");
        accID = getValidAccountNumber();
        if (sqlite3_prepare_v2(db, sql_select, -1, &stmt_retrieve, 0) != SQLITE_OK)
        {
            fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
            return;
        }

        sqlite3_bind_int(stmt_retrieve, 1, accID);
        sqlite3_bind_int(stmt_retrieve, 2, u.id);

        if (sqlite3_step(stmt_retrieve) == SQLITE_ROW)
        {
            printf("\nChoose what you want to update:");
            printf("\n1. Phone");
            printf("\n2. Country");
            printf("\nEnter your choice: ");
            scanf("%d", &choice);
            clearInputBuffer();

            switch (choice)
            {
            case 1:
            {
                int valid_phone = 0;
                while (!valid_phone)
                {
                    printf("Enter the new phone number: ");
                    if (fgets(newphone, sizeof(newphone), stdin) == NULL)
                    {
                        printf("Error reading input.\n");
                        continue;
                    }
                    newphone[strcspn(newphone, "\n")] = 0; // Remove newline

                    if (strlen(newphone) > 15)
                    {
                        printf("Phone number is too long. Maximum 15 digits allowed.\n");
                        continue;
                    }

                    valid_phone = 1;
                    for (int i = 0; newphone[i] != '\0'; i++)
                    {
                        if (!isdigit(newphone[i]))
                        {
                            printf("Phone number should contain only digits.\n");
                            valid_phone = 0;
                            break;
                        }
                    }
                }

                if (sqlite3_prepare_v2(db, sql_update_phone, -1, &stmt_update_phone, 0) != SQLITE_OK)
                {
                    fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
                    return;
                }
                sqlite3_bind_text(stmt_update_phone, 1, newphone, -1, SQLITE_STATIC);
                sqlite3_bind_int(stmt_update_phone, 2, accID);
                sqlite3_bind_int(stmt_update_phone, 3, u.id);
                if (sqlite3_step(stmt_update_phone) != SQLITE_DONE)
                {
                    fprintf(stderr, "Failed to execute statement: %s\n", sqlite3_errmsg(db));
                }
                else
                {
                    printf("Phone number updated successfully!\n");
                }
                sqlite3_finalize(stmt_update_phone);
                break;
            }
            case 2:
            {
                int valid_country = 0;
                while (!valid_country)
                {
                    printf("Enter the new country: ");
                    if (fgets(newcountry, sizeof(newcountry), stdin) == NULL)
                    {
                        printf("Error reading input.\n");
                        continue;
                    }
                    newcountry[strcspn(newcountry, "\n")] = 0; // Remove newline

                    if (strlen(newcountry) > 50)
                    {
                        printf("Country name is too long. Maximum 50 characters allowed.\n");
                        continue;
                    }

                    valid_country = 1;
                    for (int i = 0; newcountry[i] != '\0'; i++)
                    {
                        if (!isalpha(newcountry[i]) && newcountry[i] != ' ')
                        {
                            printf("Country name should contain only letters and spaces.\n");
                            valid_country = 0;
                            break;
                        }
                    }
                }

                if (sqlite3_prepare_v2(db, sql_update_country, -1, &stmt_update_country, 0) != SQLITE_OK)
                {
                    fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
                    return;
                }
                sqlite3_bind_text(stmt_update_country, 1, newcountry, -1, SQLITE_STATIC);
                sqlite3_bind_int(stmt_update_country, 2, accID);
                sqlite3_bind_int(stmt_update_country, 3, u.id);
                if (sqlite3_step(stmt_update_country) != SQLITE_DONE)
                {
                    fprintf(stderr, "Failed to execute statement: %s\n", sqlite3_errmsg(db));
                }
                else
                {
                    printf("Country updated successfully!\n");
                }
                sqlite3_finalize(stmt_update_country);
                break;
            }
            default:
                printf("Invalid choice.\n");
                break;
            }
        }
        else
        {
            printf("Account not found!\n");
        }
        sqlite3_finalize(stmt_retrieve);

        if (!getYesNoChoice(&u, db, "Do you want to perform another update operation?"))
        {
            printf("\nReturning to main menu...\n");
            mainMenu(&u, db);
            return;
        }
    }
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
    char choice[10];
    //clearInputBuffer();
    system("clear");
    do
    {
        printf("\nEnter the account number you want to transfer: ");
        accID = getValidAccountNumber();
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

            do
            {
                printf("\nDo you want to try another transfer? (y/n): ");
                if (fgets(choice, sizeof(choice), stdin) == NULL)
                {
                    printf("Error reading input.\n");
                    continue;
                }
                choice[strcspn(choice, "\n")] = 0; // Remove newline

                if (strlen(choice) != 1 || (choice[0] != 'y' && choice[0] != 'n'))
                {
                    printf("Please enter a valid option ('y' or 'n')!\n");
                }
            } while (strlen(choice) != 1 || (choice[0] != 'y' && choice[0] != 'n'));

            if (choice[0] == 'n')
            {
                mainMenu(&u,db);
                return;
            }
            continue;
        }
        sqlite3_finalize(stmt_select_account);

        printf("\nEnter the name of the user you want to transfer to: ");
        scanf("%99s", newName);
        clearInputBuffer();

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
        do
        {
            printf("\nDo you want to try another transfer? (y/n): ");
            if (fgets(choice, sizeof(choice), stdin) == NULL)
            {
                printf("Error reading input.\n");
                continue;
            }
            choice[strcspn(choice, "\n")] = 0; // Remove newline

            if (strlen(choice) != 1 || (choice[0] != 'y' && choice[0] != 'n'))
            {
                printf("Please enter a valid option ('y' or 'n')!\n");
            }
        } while (strlen(choice) != 1 || (choice[0] != 'y' && choice[0] != 'n'));

        if (choice[0] == 'n')
        {
            mainMenu(&u, db);
            return;
        }

    } while (choice[0] == 'y');
    mainMenu(&u, db);

rollback:
    sqlite3_exec(db, "ROLLBACK;", NULL, NULL, NULL);
    printf("Transfer operation failed.\n");
}

void checkAccountsDetails(struct User *u, sqlite3 *db)
{
    int accId;
    char choice[10];
    char buffer[100];
   // clearInputBuffer();
    system("clear");
    do
    {
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
        printf("\nEnter the account number you want to check: ");
        accId = getValidAccountNumber();

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
        int stepResult = sqlite3_step(stmt_select);
        if (stepResult != SQLITE_ROW)
        {
            if (stepResult == SQLITE_DONE)
            {
                printf("No account found with ID %d for this user.\n", accId);
            }
            else
            {
                fprintf(stderr, "Error executing statement: %s\n", sqlite3_errmsg(db));
            }
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

        if (getYesNoChoice(u, db, "Do you want to check another account?"))
        {
            continue; // Continue with outer loop
        }
        else
        {
            mainMenu(u, db); // Go to main menu
            return;
        }
    } while (1);

     mainMenu(u,db);
}

void deleteAccount(struct User *u, sqlite3 *db)
{
    int accId;
    char choice;
    //clearInputBuffer();
    system("clear");
    do
    {

        printf("Enter the account ID you want to delete: ");
        accId = getValidAccountNumber();

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

    } while (getYesNoChoice(u, db, "Do you want to delete another account?"));
    mainMenu(u, db);
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
    int transaction_completed = 0; // Flag to track if a transaction was completed
   // clearInputBuffer();
    system("clear");
    do
    {
        transaction_completed = 0;
        printf("Enter the account ID you want to make a transaction on: ");
        accID = getValidAccountNumber();

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
            if (!getYesNoChoice(&u, db, "Do you want to try again?"))
            {
                mainMenu(&u, db);
                return;
            }
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
            if (!getYesNoChoice(&u, db, "Do you want to try another account?"))
            {
                mainMenu(&u, db);
                return;
            }
            continue;
        }

        printf("\nCurrent Balance: $%.2f\n", balance);
        printf("Account Type: %s\n", accountType);

        do
        {
            printf("\nDo you want to:\n");
            printf("1. Deposit\n");
            printf("2. Withdraw\n");
            printf("3. Cancel transaction\n");
            printf("Enter choice: ");
            if (scanf("%d", &choice) != 1)
            {
                printf("Invalid input. Please enter a valid choice.\n");
                clearInputBuffer();
                choice = 0;
            }
            else if (choice < 1 || choice > 3)
            {
                printf("Invalid choice. Please enter 1, 2, or 3.\n");
                choice = 0;
            }
            else
            {
                clearInputBuffer();
            }
        } while (choice == 0);

        if (choice == 3)
        {
            printf("Transaction cancelled.\n");
            sqlite3_finalize(stmt_select);
            break;
        }

        // Input and validate amount
        int valid_amount = 0;
        while (!valid_amount)
        {
            printf("Enter amount: $");
            if (fgets(amountStr, sizeof(amountStr), stdin) == NULL)
            {
                printf("Error reading input.\n");
                continue;
            }

            // Remove newline character if present
            size_t len = strlen(amountStr);
            if (len > 0 && amountStr[len - 1] == '\n')
            {
                amountStr[len - 1] = '\0';
            }
            else if (len == sizeof(amountStr) - 1)
            {
                printf("Input too long. Please enter a smaller amount.\n");
                clearInputBuffer();
                continue;
            }

            // Check if the input contains only digits and optionally one decimal point
            valid_amount = 1;
            int decimal_count = 0;
            for (int i = 0; amountStr[i] != '\0'; i++)
            {
                if (amountStr[i] == '.')
                {
                    decimal_count++;
                    if (decimal_count > 1)
                    {
                        valid_amount = 0;
                        break;
                    }
                }
                else if (!isdigit(amountStr[i]))
                {
                    valid_amount = 0;
                    break;
                }
            }

            if (!valid_amount)
            {
                printf("Invalid amount. Please enter a valid number.\n");
            }
            else
            {
                amount = atof(amountStr);
                if (amount <= 0)
                {
                    printf("Amount must be greater than zero.\n");
                    valid_amount = 0;
                }
            }
        }

        if (choice == 1)
        {
            balance += amount;
            printf("\n$%.2f has been deposited to your account\n", amount);
        }
        else if (choice == 2)
        {
            if (balance < amount)
            {
                printf("\nInsufficient balance\n");
                sqlite3_finalize(stmt_select);
                if (!getYesNoChoice(&u, db, "Do you want to try another transaction?"))
                {
                    mainMenu(&u, db);
                    return;
                }
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
            transaction_completed = 1;
        }

        // Finalize the statements
        sqlite3_finalize(stmt_select);
        sqlite3_finalize(stmt_update);

        if (transaction_completed)
        {
            if (!getYesNoChoice(&u, db, "Do you want to make another transaction?"))
            {
                break;
            }
        }

    } while (1);

    mainMenu(&u, db);
}