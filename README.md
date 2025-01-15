## ATM Management System
![](https://img.freepik.com/free-vector/convenience-joy-using-self-service-machines-set-flat-isolates-illustration_1284-65895.jpg?ga=GA1.1.1553843546.1721645701&semt=ais_hybrid)

A C-based ATM management system with SQLite3 database integration that allows users to manage their bank accounts and perform various banking operations securely.

## Features
### User Authentication
* Login: Secure access to user accounts through SQLite3 database verification
* Registration: Create new user accounts with unique usernames stored in SQLite3 database

## Account Management
* Create Account: Open new bank accounts with different types:
  * Savings (7% interest rate)
  * Current (no interest)
  * Fixed Deposit:
    * 1 year (4% interest rate)
    * 2 years (5% interest rate)
    * 3 years (8% interest rate)

* View Accounts:
  * List all owned accounts
  * Check detailed information of specific accounts
  * View interest calculations for savings and fixed deposit accounts

* Update Account Information:
  * Modify phone number
  * Update country information
  * Delete Account: Remove existing accounts from database

* Transactions
  * Deposit: Add funds to accounts
  * Withdraw: Remove funds from accounts
  * Transfer Ownership: Transfer account ownership to another user
  * Note: Fixed deposit accounts cannot perform transactions

### Technical Details

#### Dependencies
* SQLite3 database
* C compiler (gcc recommended)
* Make utility

### Database Schema:
The system uses SQLite3 database with the following tables:

```sql
Users Table
CREATE TABLE users (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    username TEXT UNIQUE NOT NULL,
    password TEXT NOT NULL
);

Accounts Table
CREATE TABLE accounts (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    user_id INTEGER,
    account_type TEXT NOT NULL,
    balance REAL DEFAULT 0.0,
    creation_date TEXT,
    country TEXT,
    phone TEXT,
    FOREIGN KEY (user_id) REFERENCES users(id)
);

### Setup and Installation:

Clone the repository:

`git clone https://learn.zone01kisumu.ke/git/hokwach/atm-management-system.git`


Ensure SQLite3 is installed on your system:

Navigate to the project directory:

`cd atm-management-system`

Compile the project using make:

`make`


Run the application:

`./atm`


### Usage
* Start the Application
* Choose between login or register. New users will be added to the SQLite database

### Options
1. Main Menu
2. Create new account
3. View all accounts
4. Check specific account details
5. Update account information
6. Make transactions
7. Transfer account ownership
8. Delete account
9. Exit

### Account Operations
* For viewing account details:
  * Enter the account ID
* For transactions:
  * Select account and choose deposit or withdraw
* For updates:
  * Select account and choose field to update
* For transfers:
  * Enter account ID and recipient username

### Security Features
* SQLite3 database for secure data storage
* Unique username enforcement through database constraints
* Input validation for all operations
* Transaction validation for fixed deposit accounts
* Balance verification for withdrawals

### Error Handling
The system includes comprehensive error handling for:
* Database connection errors
* SQL query execution errors
* Invalid inputs
* Insufficient funds
* Non-existent accounts
* Unauthorized operations

### Contributing
1. Fork the repository
2. Create your feature branch
3. Commit your changes
4. Push to the branch
5. Create a new Pull Request


