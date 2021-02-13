# **TroopTreasurer**

Qt/C++ based application to provide supplementary functions to aid a BSA Troop Treasurer to perform needed duties that a primary troop management system may lack.  

This is a personal project I created to aid myself with the duties of being the treasurer of a local BSA Troop.  Our troop utilizes a troop management solution that has some functional capabilities for the treasurer to track and manage financial (troop and scout) accounts as well as transactions against said accounts.  What the solution does not provide is a overall solution to coordinate the physical bank account interactions such as tracking payments and deposits and when they actually occur against the account, this makes reporting on the state of the accounts using an accrual model difficult.  Additionally the Troop management solution provides no features for balance sheet reporting to the committee, nor the ability to track scout accounts at a household level where more than one scout from a household exist.  

While the project from my perspective is aligned to a specific troop management solution, in reality data from any solution could be integrated and utilized if it can be made to fit the import data structure.

When utilizing this system in tandem with any solution the following guidelines should be followed as to avoid data issues when the transactions are imported into this system for reporting:

- Never for any reason should a transaction be deleted within the troop management solutions' transaction system. This not only causes balance issues within the troop management platform but will heighten the potential of data issues with this system when historical and current data are joined.  It is advised to add additional adjustment transactions to back out an issue as this preserves the data state, but additionally provides greater transparency and clarity to others as well.
  
- Never back date transactions, transactions activity date should be recorded as current day or greater only.  This system utilizes two data tables to hold transaction data, a historical archive, and a current active month set.  Every month the treasurer should download the month's data from the troop management solution and import it into this application.  This act will load the data set into the current month table archiving the previous contents to the archive set.  If any transactions are back dated there is a high potential for data irregularities and inaccuracies due to missed/duplicate records being present in the database.  



As stated this is a personal project and has no goal to be anything beyond a simple tool for personal use, I offer it here to help anyone out there looking for a method to help them with their treasurer responsibilities. Please feel free to fork and modify to your hearts delight, if you add/mod/tweak any aspect please drop me a pull request.  The **Main** branch will always be the current release.  

**Note:** The project has no overarching design standard in mind, I design as I go with my only intent to learn and try new ideas.  The only pattern I do try to follow always is the PImpl idiom using smart pointers to make sure that impl class instances are deleted appropriately.

------

### To Build

This project is built using the open source version of the [Qt Platform](https://www.qt.io/download-open-source?hsCtaTracking=9f6a2170-a938-42df-a8e2-a9f0b1d6cdce%7C6cb0de4f-9bb5-4778-ab02-bfb62735f3e5), and utilizes a [MySQL](https://www.mysql.com/) database version 8.0 or above.  To compile this project you must have Qt installed and the QSqlDatabase drivers compiled for MySQL as well, see [this link](https://doc.qt.io/qt-5/sql-driver.html) for more information.

**CMake & Make** are utilized to build, cmake 3.16 is the minimal version required.

Clone this repo to your machine.

`$ git clone https://github.com/karlmoad/TroopTreasurer.git`

Open project directory and create a subdirectory to perform build in. open build subdirectory. Issue cmake and make commands.

```
$ cd TroopTreasuer
$ mkdir build
$ cd build
$ cmake .. -DQT_CMAKE_DIR=<directory where Qt5cmake.config can be found>`
$ make
```

**QT_CMAKE_DIR** argument is supplied to cmake to direct it to where the Qt cmake configuration is located on the file system.  

**Platform compatibility:**  I work mainly on Apple OSX (intel), the build and execution of the project has been tested and verified to work on OSX 10.14 and above only on intel based macs.  While I have made attempts in the project to accommodate windows this largely remains untested and is not verified to work fully. I have avoided linux integration for the time being until I have identified a key store for credentials integration method, but the majority of the remaining code base is tested and functional. I have tested and built using Qt version 5.14 and 5.15 successfully.





















   













