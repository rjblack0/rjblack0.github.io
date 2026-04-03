# Program redesign expects to load credentials securely using environment variables

##### To login using Environmental variables: #####

## Navigate to the folder containing the code files and enter the following:

 # Windows (Command Prompt):
   set AAC_USERNAME=aacuser
   set AAC_PASSWORD=[your_password_here]

 # Windows (PowerShell):
   $env:AAC_USERNAME="aacuser"
   $env:AAC_PASSWORD=["your_password_here"]

 # MacOS / Linux (Terminal):
   export AAC_USERNAME=aacuser
   export AAC_PASSWORD=[your_password_here]

# Then run the file using by entering:
   python -m notebook

# For the purpose of testing environment, a JSON file has been included which can be used for storing login credentials
### This file would NOT be included in any release versions.

##### Jupyter Notebook Requirements #####
It's recommended to import these files into the Codio Enviroment for CS-340, as the 
dependencies are already installed.

# To test this on your local system:
Navigate to the project folder
## Check Python version using:
    python --version
## Then install Jupyter Notebook:
    pip install notebook
## and all install all dependencies:
    python -m pip install numpy pandas matplotlib dash dash-leaflet plotly pymongo jupyter-dash


##### MongoDB Requirements #####
### Mongosh ###
https://www.mongodb.com/try/download/shell
https://www.mongodb.com/try/download/database-tools

# Create the MongoDB User Account:

# Open Mongosh
    mongosh
# Open the project
use AAC
# Create user:

db.createUser({
  user: "aacuser",
  pwd: "your_password_here",
  roles: [
    { role: "readWrite", db: "AAC" }
  ]
})

# Type exit to quit

# Edit db_config.json
## If not using local credentials, the password here must match the one you used when you created the user
{
  "username": "aacuser",
  "password": ["your_password_here"]
}

# Import Databases

Your actual command might change depending on your computer layout and the location that you are running these files from

[Full Path to mongoimport.exe] --db AAC --collection animals --type csv --headerline --file [Full path to the .csv file inside of your project/datasets folder]