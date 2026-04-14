# Animal_Shelter Installation Notes

## Program redesign expects to load credentials securely using environment variables
### To login using Environmental variables:

Navigate to the folder containing the code files and enter the following:

Windows (Command Prompt):
```console
set AAC_USERNAME=aacuser
set AAC_PASSWORD=[your_password_here]
```
Windows (PowerShell):
```console
$env:AAC_USERNAME="aacuser"
$env:AAC_PASSWORD=["your_password_here"]
```

 MacOS / Linux (Terminal):
 ```console
 export AAC_USERNAME=aacuser
 export AAC_PASSWORD=[your_password_here]
```
Then run the file using by entering:
```console
python -m notebook
```

### For the purpose of testing environment, a JSON file has been included which can be used for storing login credentials
### This file would NOT be included in any release versions.

## Jupyter Notebook Requirements

### To test this on your local system:
Navigate to the project folder

Check Python version using:
```console
python --version
```
Then install Jupyter Notebook:
```console
pip install notebook
```
And all install all dependencies:
```console
python -m pip install numpy pandas matplotlib dash dash-leaflet plotly pymongo jupyter-dash
```

## MongoDB Requirements
### Mongosh
https://www.mongodb.com/try/download/shell

https://www.mongodb.com/try/download/database-tools

### Create the MongoDB User Account:

### Open Mongosh
```console
mongosh
```
### Open the project
```console
use AAC
```
### Create user:
```console
db.createUser({
  user: "aacuser",
  pwd: "your_password_here",
  roles: [
    { role: "readWrite", db: "AAC" }
  ]
})
```

### Type exit to quit

## Edit db_config.json
### If not using local credentials, the password here must match the one you used when you created the user
```console
{
  "username": "aacuser",
  "password": ["your_password_here"]
}
```

## Import Databases

Your actual command might change depending on your computer layout and the location that you are running these files from
```console
[Full Path to mongoimport.exe] --db AAC --collection animals --type csv --headerline --file [Full path to the .csv file inside of your project/datasets folder]
```

# Bank_Login Installation Notes

## Source File

The main application file is:

```console
Blackburn_Ryan_Bank_Login.cpp
```
### This project requires a C++ compiler that supports modern C++ features.

## Visual Studio
Open Visual Studio

Create a new Console App project in C++

Replace the default source file contents with the contents of Blackburn_Ryan_Bank_Login.cpp

Build and run the project using:
```console
ctrl+F5
```
## Login Credentials for Testing:
The program includes a built-in test credential store for demonstration purposes.

Available usernames and passwords are:

```console
advisor1   / SecurePass123
manager1   / Password456
analyst1   / SNHU789
```
## Program Workflow
After launching the application:

-Enter a valid username
-Enter the corresponding password
-If authentication succeeds, the program will display a menu with the following options:
```console
1 = Display the client list
2 = Change a client's service choice
3 = Exit the program
```

Username Rules

Usernames must meet the following validation requirements:

- between 3 and 20 characters long
- may contain:
- uppercase letters
- lowercase letters
- digits
- underscores

Invalid usernames will be rejected before authentication is attempted.

