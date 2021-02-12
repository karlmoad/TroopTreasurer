# **TroopTreasurer**

Qt/C++ based application to provide supplimentary funtions to aid a BSA Troop Treasuer to perform needed duties that the primary troop management system does not provide.  

This is a personal proect I created to aid myself with the duties on being the treasurer of one of our local BSA Troops.  Our troop utilizes a troop managment platform that has some functional capabilites for the Treasuer to utilize to track and manage finaincial (troop and scout) accounts as well as transactions against said accounts.  What the system does notprovide is a overall solution to coordinate the physical bank account interactions such as tracking payments and deposits and when they acutally occur against the account.  Thsi makes reporting on the state of the accounts in relation using an accrual model.  Additionally the Troop managment platform provides no features in trems of reporting allowing the treasuere to provide a balance report to the committee.  

I created this project to supplimnet the troop managment platform to add functions to coverthe areas mentioned above and more.  When utilizing this system in tandem with the troop management platform the following guidelines should be follwoed as to avoid data issues when the transactions are imported into this system for reporting:

- Never for any reason should a transaction be deleted within the troop management platform's transaction system. This nto only causes balance issues witin the troop management platform but will highten the potential of data issues with this system when historical and current data are joined.  It is advised to add additioanl adjustment transactions to back out an issue as this preserves the daat state, but additioanlly provides greater transparency and clairity to others as well.
- Never back date transactions, transactions activity date shoudl be recorded as current day or greater only.  This system utiilizes two table a histroical archive and a current month table to hold the transaction data to avoid duplication issues.  Every month the Treasuer should download the month's data from the troop management platform and import it into thsi application.  This act will load the data set into the current month table archiving the previous contents to the archive set.  If any transactions are back dated there is a high potential for data irregularies and innaccuracies due to missed/duplicate records being present in the database.  



As stated this is a personal project and has no intent to be productized or anything beyond a simple tool to help me, I offer it here to help anyone out there looking for a method to help them with their troop treasuer responsibilities. Please feel free to frok and modify to your hearts delight if you would like.  If you add/mod/tweek or just generally imporive any aspect please drop me a pull request and I will happily evaulate it for inclusioninto the base code.  The master branch will alwasy be the current compilable release.  

**Note:** The project has no real design guidelines as I dev/design as I go with no overarching design standard in mind.  

------

### To Build

This project is built using the open source version of the [Qt Platform](https://www.qt.io/download-open-source?hsCtaTracking=9f6a2170-a938-42df-a8e2-a9f0b1d6cdce%7C6cb0de4f-9bb5-4778-ab02-bfb62735f3e5), and utilizes a [MySQL](https://www.mysql.com/) database version 8.0 or above.  To compile this project you must have Qt installed and the QSqlDatabase drivers compiled for MySQL as well, see [this link](https://doc.qt.io/qt-5/sql-driver.html) for more information.

 I have tested and built using Qt version 5.14 and 5.15 successfully.

**CMake & Make** are utilized to build, cmake 3.16 is the minimal version required.

Clone this repo to your machine.

`$ git clone https://github.com/karlmoad/TroopTreasurer.git`

Open project directory and create a subdirecotry to perform build in. open build subdirectory. Issue cmake and make commands.

```
$ cd TroopTreasuer
$ mkdir build
$ cd build
$ cmake ../TroopTreasuer -DQT_CMAKE_DIR=<directory where Qt5cmake.config can be found>`
$ make
```

<u>QT_CMAKE_DIR</u> argument is supplied ot cmake to direct it to where the Qt cmake configuration is located on the file system.  

**One final note:**  I develope and work on Apple platforms, the build and execution of the project has been tested and verified to work on OSX 10.14 and above.  While I have made attempts in the project to accomidate windows platforms this largely remains untested and is not verified to work fully. 





















   













