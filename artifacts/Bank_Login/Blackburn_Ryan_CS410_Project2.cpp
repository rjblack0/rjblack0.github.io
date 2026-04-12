#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <limits>
#include <functional>

using namespace std;

/* CS-410 Secure banking login Summary
    *Category Two: Algorithms and Data Structures
    **Replaced fixed-size username buffer with std::string for safer input handling
    **Reworked username and password validation before authentication is attempted
    **Stored credentials in a structured map
    **Compared hashed password values instead of plaintext passwords
    **Tracked failed login attempts per user with a map
    **Replaced separate client variables and conditional update chains with a vector of client records
*/

/*
    *Week 6 Refactor:
    To more strongly align with C++ design expectations, added object-oriented structure by introducing:
    1. CustomerRecord class
        Replaces the simple struct with a real class
        Encapsulates client data and related behavior
    2. AuthenticationSystem class
        Encapsulates credential storage
        Encapsulates failed login tracking
        Encapsulates username validation and password hashing
        Encapsulates authentication workflow
    3. InvestmentApp class
        Encapsulates client list and application flow
        Encapsulates menu display and update logic
*/


//Constants used throughout the application.
const int MAX_LOGIN_ATTEMPTS = 5;
const int BROKERAGE_SERVICE = 1;
const int RETIREMENT_SERVICE = 2;


// ###################################### //
/* CustomerRecord class
    Replaced struct ClientRecord with class-based version
    Demonstrates object-oriented design;
        Keeps both the client data and related behaviors in one place.
        Better encapsulation
        Cleaner access to client data
        Easier expansion.
*/

class CustomerRecord {
private:
    // CHANGE: These fields are now private instead of being directly accessed.
    string name;
    int serviceChoice;

public:
    // Constructor initializes each customer record when the app starts.
    CustomerRecord(const string& clientName, int clientServiceChoice)
        : name(clientName), serviceChoice(clientServiceChoice) {
    }

    // CHANGE: Getter for client name. Instead of directly reading clients[i].name like before,
    // the rest of the program now asks the object for its name.
    const string& GetName() const {
        return name;
    }

    // Getter for current service choice.
    int GetServiceChoice() const {
        return serviceChoice;
    }

    // Setter for service choice.
    // CHANGE: Now the object manages its own update through a setter.
    void SetServiceChoice(int newServiceChoice) {
        serviceChoice = newServiceChoice;
    }

    // CHANGE: Global / free function ServiceLabel now belongs to customer record itself.
    string GetServiceLabel() const {
        if (serviceChoice == BROKERAGE_SERVICE) {
            return "Brokerage";
        }
        return "Retirement";
    }
};


// ###################################### //
/* AuthenticationSystem class
    This class moves all authentication-related state and behavior into one unit.
        Removes global authentication state
        Groups related logic together
        Makes the code easier to read
*/

class AuthenticationSystem {
private:
    // CHANGE: Now private members of AuthenticationSystem; only the authentication system controls login data.
    map<string, size_t> credentialStore;
    map<string, int> failedAttempts;

    // CHANGE: Now private because only the authentication system needs it.
    size_t HashPassword(const string& password) const {
        return hash<string>{}(password);
    }

    // CHANGE: Now a private helper because username validation is part of authentication responsibility.
    bool IsValidUsername(const string& username) const {
        const size_t MIN_USERNAME_LENGTH = 3;
        const size_t MAX_USERNAME_LENGTH = 20;

        if (username.length() < MIN_USERNAME_LENGTH || username.length() > MAX_USERNAME_LENGTH) {
            return false;
        }

        for (char character : username) {
            bool isLowercaseLetter = (character >= 'a' && character <= 'z');
            bool isUppercaseLetter = (character >= 'A' && character <= 'Z');
            bool isDigit = (character >= '0' && character <= '9');
            bool isUnderscore = (character == '_');

            if (!(isLowercaseLetter || isUppercaseLetter || isDigit || isUnderscore)) {
                return false;
            }
        }

        return true;
    }

    // CHANGE: Lockout check was previously performed inline using direct access to the global failedAttempts map.
    // It now has its own helper method for readability and encapsulation.
    bool IsLockedOut(const string& username) const {
        auto attemptRecord = failedAttempts.find(username);
        if (attemptRecord == failedAttempts.end()) {
            return false;
        }

        return attemptRecord->second >= MAX_LOGIN_ATTEMPTS;
    }

public:
    // Constructor sets up the test credential store.
    //
    // CHANGE: Constructor now initializes credentialStore inside the authentication class constructor.
    //That means authentication data is now owned by AuthenticationSystem.
    AuthenticationSystem() {
        credentialStore["advisor1"] = HashPassword("SecurePass123");
        credentialStore["manager1"] = HashPassword("Password456");
        credentialStore["analyst1"] = HashPassword("SNHU789");
    }

    /*
        User Authentication Sequence
        1. Read username
        2. Validate username format
        3. Check lockout status
        4. Confirm username exists
        5. Read password
        6. Compare hashed password values
        7. Update per-user attempt tracking
    */
    bool AuthenticateUser() {
        string username;
        string passwordInput;

        cout << "Enter your username:\n";
        cin >> username;

        if (!IsValidUsername(username)) {
            cout << "Invalid username format. Use 3-20 letters, digits, or underscores.\n";
            return false;
        }

        if (IsLockedOut(username)) {
            cout << "Account locked due to too many failed login attempts.\n";
            return false;
        }

        auto userRecord = credentialStore.find(username);
        if (userRecord == credentialStore.end()) {
            cout << "User not found.\n";
            return false;
        }

        cout << "Enter your password:\n";
        cin >> passwordInput;

        size_t hashedInput = HashPassword(passwordInput);

        if (hashedInput == userRecord->second) {
            failedAttempts[username] = 0;
            cout << "Login successful.\n";
            return true;
        }

        failedAttempts[username]++;

        if (failedAttempts[username] >= MAX_LOGIN_ATTEMPTS) {
            cout << "Account locked due to too many failed login attempts.\n";
        } else {
            cout << "Incorrect password. Attempts remaining: "
                 << (MAX_LOGIN_ATTEMPTS - failedAttempts[username]) << endl;
        }

        return false;
    }

    // CHANGE: New method moves responsibility of login loop to AuthenticationSystem class.
    // In the old version, main() manually looped through credentialStore and checked failedAttempts directly.

    bool AllTrackedUsersLocked() const {
        for (const auto& entry : credentialStore) {
            auto attemptRecord = failedAttempts.find(entry.first);
            int attempts = 0;

            if (attemptRecord != failedAttempts.end()) {
                attempts = attemptRecord->second;
            }

            if (attempts < MAX_LOGIN_ATTEMPTS) {
                return false;
            }
        }

        return true;
    }
};


// ###################################### //
/* InvestmentApp class
// This class now owns the client records and the overall application flow.
    Removes global client state
    Groups application behavior together
    Makes main() much simpler
    Better reflects a real object-oriented design
*/

class InvestmentApp {
private:
    // CHANGE: Authentication is now owned by the app as an object
    AuthenticationSystem authenticationSystem;

    // CHANGE: Client data is now owned by the application class.
    vector<CustomerRecord> clients;

    // CHANGE: Private helper used internally by the app.
    bool ReadIntegerInRange(int& value, int minValue, int maxValue, const string& errorMessage) {
        cin >> value;

        if (cin.fail()) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << errorMessage << endl;
            return false;
        }

        if (value < minValue || value > maxValue) {
            cout << errorMessage << endl;
            return false;
        }

        return true;
    }

    // CHANGE: DisplayInfo() now belongs to the application because it operates on the app's client list
    void DisplayInfo() const {
        cout << "Client's Name    Service Selected" << endl;

        for (size_t i = 0; i < clients.size(); ++i) {
            cout << (i + 1) << ". "
                 << clients[i].GetName()
                 << " selected option "
                 << clients[i].GetServiceChoice()
                 << " (" << clients[i].GetServiceLabel() << ")"
                 << endl;
        }
    }

    // CHANGE: Previously the ChangeCustomerChoice(). Now belongs to the application because it modifies the app's client list.
    // Also note that it now uses CustomerRecord setters/getters instead of directly modifying public struct fields.
    void ChangeCustomerChoice() {
        int clientNumber = 0;
        int newService = 0;

        cout << "Enter the number of the client that you wish to change\n";
        if (!ReadIntegerInRange(
                clientNumber,
                1,
                static_cast<int>(clients.size()),
                "Invalid input. Client number must match one of the listed clients.")) {
            return;
        }

        cout << "Please enter the client's new service choice "
             << "(1 = Brokerage, 2 = Retirement)\n";
        if (!ReadIntegerInRange(
                newService,
                BROKERAGE_SERVICE,
                RETIREMENT_SERVICE,
                "Invalid input. Service choice must be 1 or 2.")) {
            return;
        }

        clients[clientNumber - 1].SetServiceChoice(newService);

        cout << clients[clientNumber - 1].GetName()
             << " was updated to "
             << clients[clientNumber - 1].GetServiceLabel()
             << ".\n";
    }

    // CHANGE: Method now authentication loop handling into the application class.
    bool RunAuthentication() {
        bool isAuthenticated = false;

        while (!isAuthenticated) {
            isAuthenticated = authenticationSystem.AuthenticateUser();

            if (!isAuthenticated) {
                cout << "Authentication failed. Please try again.\n";
            }

            if (authenticationSystem.AllTrackedUsersLocked()) {
                cout << "All tracked accounts are locked. Exiting program.\n";
                return false;
            }
        }

        return true;
    }

    // CHANGE: Moved MainMenu loop out of main, giving responsibility to application class.
    // Makes app flow easier to follow.
    void RunMenu() {
        int choice = 0;

        while (true) {
            cout << "\nWhat would you like to do?\n";
            cout << "DISPLAY the client list (Option: 1)\n";
            cout << "CHANGE a client's choice (Option: 2)\n";
            cout << "Exit the program (Option: 3)\n";

            if (!ReadIntegerInRange(choice, 1, 3, "Invalid input. Please enter Option: 1, 2, or 3.")) {
                continue;
            }

            cout << "You chose " << choice << endl;

            if (choice == 1) {
                DisplayInfo();
            } else if (choice == 2) {
                ChangeCustomerChoice();
            } else if (choice == 3) {
                break;
            }
        }
    }

public:
    // Constructor initializes the client list.
    //
    // CHANGE: Now initialized as part of the app object itself.
    InvestmentApp()
        : clients{
              CustomerRecord("Client One", BROKERAGE_SERVICE),
              CustomerRecord("Client Two", RETIREMENT_SERVICE),
              CustomerRecord("Client Three", BROKERAGE_SERVICE),
              CustomerRecord("Client Four", RETIREMENT_SERVICE),
              CustomerRecord("Client Five", BROKERAGE_SERVICE)
          } {
    }

    // Main application workflow.
    // CHANGE: This method centralizes startup, authentication, and menu flow.
    // The old version split those responsibilities across main() and free functions.
    void Run() {
        cout << "Created by Ryan Blackburn CS-410 Secure Bank Login\n";
        cout << "Hello! Welcome to our Investment Company\n";

        if (!RunAuthentication()) {
            return;
        }

        RunMenu();
    }
};


// ###################################### //
/* main

    The old main() performed:
        - The greeting
        - Authentication loop
        - Account lockout check
        - Menu loop
    Increased reduces responsiblities to application startup only.
*/
int main() {
    InvestmentApp app;
    app.Run();
    return 0;
}