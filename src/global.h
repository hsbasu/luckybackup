/* ------------------ global.h --------------------------------------------------------------------------------------
 header file for luckyBackup project

===============================================================================================================================
===============================================================================================================================
     This file is part of "luckyBackup" project
     Copyright, Loukas Avgeriou
     luckyBackup is distributed under the terms of the GNU General Public License
     luckyBackup is free software: you can redistribute it and/or modify
     it under the terms of the GNU General Public License as published by
     the Free Software Foundation, either version 3 of the License, or
     (at your option) any later version.

     luckyBackup is distributed in the hope that it will be useful,
     but WITHOUT ANY WARRANTY; without even the implied warranty of
     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
     GNU General Public License for more details.

     You should have received a copy of the GNU General Public License
     along with luckyBackup.  If not, see <http://www.gnu.org/licenses/>.


 project version    : Please see "main.cpp" for project version

 developer          : luckyb
 last modified      : 22 May 2016
===============================================================================================================================
===============================================================================================================================
*/


#ifndef GLOBAL_H
#define GLOBAL_H

#include <QString>
#include <QDir>
#include <QFile>
#include <QLocale>
#include <QTranslator>
#include <QLibraryInfo>
#include <QCoreApplication>

class operation;

namespace Global {
  extern QString myHome;                      // user's home directory
  extern QString currentUser;                 // will hold the current username (definition at luckybackupwindow.cpp)
  extern QString const appName;               // application name
  extern double const appVersion;             // application version
  extern QString appVersionString;            // application version
  extern double const validProfileVersion;    // profile version before this a profile won't be loaded
  extern double const validScheduleVersion;   // schedule version before this a schedule won't be loaded
  extern double const validSettingsVersion;   // Settings file valid version
  extern QString luckyBackupDir;              // app working directory
  extern QString luckyBackupDefaultDir;       // same as above. WINDOWS use
  extern QString settingsFile;                // settings file
  extern QString profileDir;                  // profiles directory
  extern QString defaultProfile;              // default profile
  extern QString standardDefaultProfile;      // standard default profile
  extern QString logDir;                      // logs directory
  extern QString logfilename;                 // logfile filename - this will change according to the profile an task executed
  extern QString cronlogString;               // cron logfile full filename: "ProfileName-LastCronLog.log"
  extern QString emailLogString;              // logfile to send by email (all task logs merged) full filename: "ProfileName-email.log"
  extern QString emailLastLogString;          // email logfile full filename: "ProfileName-email-LastLog.log"
  extern QString snapDefaultDir;              // This is the directory where snapshots are placed. It lies inside the destination
  extern QString snapChangesDir;              // Snapshots changes files are stored here. It lies inside ~
  extern QString snapEmptyDir;                // empty directory used to delete older snapshots
  extern QString snapchangesfilename;         // Snapshot changes filename. This will change according to the snapshot
  extern QString snapChangesString;           // The log lines format of the "changes" file that stores changes made for every snapshot
  extern QString scheduleDir;                 // schedule directory
  extern QString schedulefilename;            // cron data filename
  extern QString cronfilename;                // cron filename
  extern QString relativeTransDir;            // translation directory when app executed but not installed
  extern QString systemTransDir;              // system translation directory (when app is installed)
  extern QDir transDir;                       // the current translations directory
  extern QTranslator appTranslator;           // QTranslator that holds the applications translation
  extern QTranslator translator_qt;           //QTranslator that holds the standard Qt translations
  extern QString locale;                      // String that holds the current language locale
  extern QString relativeLicense;             // licence directory when app executed but not installed
  extern QString systemLicense;               // system licence directory (when app is installed)
  extern QString suseLicense;                 // system licence directory (when app is installed) - openSuse
  extern QString debianLicense;               // system licence directory (when app is installed) - debian
  extern QString relativeManual;              // manual directory when app executed but not installed
  extern QString systemManual;                // system manual directory (when app is installed)
  extern QString suseManual;                  // system manual directory (when app is installed) - opensuse

  extern QString countStr;                    // this helps displaying integers as strings
  extern QString message;                     // this is really handy to hold messages for dialogs
  extern QString messageCLI;                  // this is really handy to hold messages for the Command-Line
  extern int errorsFound;                     // count errors when executing rync or pre/post command
  extern int filesTransfered;                 // count files transfered when executing rync
  extern unsigned long long int bytesTransfered;  // count bytes transfered when executing rync

  // Useful variables for operations ---------------------------------------------------------------------------------------------------
  extern int const maxOperations;             // The maximum number or operations permitted
  extern int TotalOperations;                 // The number of available operations "operations list"- starts from 1
  extern int currentOperation;                // this holds the current row from the operations list- starts from 0
  extern int removeCharLimit;                 // No of characters to remove from lines inside snapshot .changes file
  extern bool modifyOK;                       // bool variable that says create or modify opeartion went ok
  extern bool modifyConnected;                // if the operation under modification is connected to another op, this becomes true
  extern bool DryRun;                         // shows if dry-run is checked
  extern bool ask;                            // becomes true if we want to ask the user if he/she wants to continue with the execution of operations or not
  extern bool newTaskThisIs;                  // becomes true if "add task" is pressed and false if "modify" task is pressed
  extern bool NothingToDo;                    // Becomes true if there is nothing to do when start button pressed
  extern bool NothingIncluded;                // Becomes true if there is nothing checked ("include" checkboxes)
  extern bool crontabUpdated;                 // Becomes true if the user's crontab is updated
  extern QString currentProfile;              // This holds the filename of the currently used profile
  extern QString profileName;                 // just holds the profile name not the full path+filename.profile
  extern QString profileDescription;          // holds a description of the profile
  extern QFile profile;                       // this is the profile QFile
  extern QFile logfile;                       // this is the logfile QFile
  extern QFile snapfile;                      // this is the snapshot changes QFile
  extern QString CheckedData;                 // String holding the current task message (OK - WARNING - CRITICAL) for info window display
  extern QString CheckedDataCLI;              // String holding the current task message (OK - WARNING - CRITICAL) for CLI  display
  extern operation *Operation[];              //The array that holds name, source, destination & arguments of "Opereations list"

  extern bool console;                        // these become true depending on the argument given at command line
  extern bool NoQuestions;                    // do not ask any questions at console mode
  extern bool SkipCritical;                   // skip critical tasks
  extern bool silentMode;                     // start at silent mode
  extern bool runImmediately;                 // execute a profile immediately after gui loads
  extern bool validation;                     // becomes true if "validation check" button is pressed
  extern bool KDErunning;                     // becomes true if desktop running is KDE
  extern bool GNOMErunning;                   // becomes true if desktop running is GNOME

  // functions ------------------------------------------------------------------------------------------------------------------------
  extern bool argumentsTest(int , char**);             // checks the arguments given at command-line and returns true if everything is ok
  extern bool loadCurrentProfile();                    // loads an already created profile
  void declareRsyncCommand();                          // Declares the rsync & ssh commands (full path for windows)
  extern bool check_list();                            // check the operations list
  extern bool check_dirs();                            // check directories of declared profile data

  void help();                                         // Displayes command-line usage
  void checkSyncDirs(QString, QString);                // function to check if the directories to be synced exist or are empty
  void checkBackupDirs(QString, QString);              // function to check if the source & dest dirs exist and are not empty
  int  loadProfile(QString);                           // loads an existing profile - text mode
  int  loadProfileQV(QString);                         // loads an existing profile - data mode
  extern bool saveProfile(QString);                    // saves an existing profile
  extern bool exportFullProfile(QString,QString);      // function to export the .profile file + logs + snaps
  extern int importFullProfile(QString,QString);       // function to import the .profile file + logs + snaps (whatever is available)
  extern bool checkTaskList();                         // Checks if the Task list is ok to proceed
  extern bool checkDeclaredDirs(bool);                 // Check if the declared data are ok by calling checkBackupDirs or checkSyncDirs
  QStringList AppendArguments(operation*);             // Set command's (normally rsync) arguments
  extern bool writeToLog;                              // if the log file is ok to write this becomes true
  extern QString logFileUpdate(QString, QString, int); // Update the logfile with some string
  extern bool checkMountPoint(QString);                // Check if a path belongs to a mountpoint or is a mountpoint itself
  void setTextMessages(QString,QString,bool,bool,QString,QString,QString);  // function to set text feedback for the user
  extern QString fixWinPathForRsync(QString,bool);     // Fixes a windows path for rsync use
  extern QString calculateLastPath(QString);           // Calculates the last part of a path eg path=user@host:destination\path\here -> returns "here"

  extern QString emailCommand;
  extern QString emailArguments;
  extern bool    emailNever;
  extern bool    emailError;
  extern bool    emailSchedule;
  extern bool    emailTLS;
  extern QString emailFrom;
  extern QString emailTo;
  extern QString emailSubject;
  extern QString emailSMTP;
  extern QString emailBody;
  extern QString emailDefaultSubject;         // Holds the default subject
  extern QString emailDefaultBody;            // Holds the default body text
  extern QString emailDefaultCommand;         // Holds the default email command
  extern QString emailDefaultArguments;       // Holds the default email arguments
  extern QString emailDefaultWinCommand;      // Holds the default email command - WINDOWS
  extern QString emailDefaultWinArguments;    // Holds the default email arguments - WINDOWS
  extern QString sendEmailNow(bool);          // Send an email after a profile run. bool is true if called for testing purposes

  extern QString rsyncDefaultCommand;         // Holds the default rsync command
  extern QString sshDefaultCommand;           // Holds the default ssh command

  // WINDOWS related variables. Also search variables above for "WINDOWS use"
  extern QString rsyncDefaultWinCommand;      // Holds the default rsync command for windows
  extern QString sshDefaultWinCommand;        // Holds the default ssh command for windows
  extern QString appPath;                     // This is used for windows app path. It's also causing a ...
                                              //"QCoreApplication::applicationDirPath: Please instantiate the QApplication object first" WARNING message
  //extern QString rsyncDefaultWinCommand;    // Holds the default rsync command for windows
  //extern QString sshDefaultWinCommand;      // Holds the default ssh command for windows
  extern QString mapdrive;
  extern QString vshadowDir;
  extern QString vshadowDefaultDir;
  extern QString dosdevCommand;
  extern QString dosdevDefaultCommand;
  extern QString cygpathCommand;
  extern QString cygpathDefaultCommand;
  extern bool isTempDirPath;
  extern QString tempDirPath;
  extern QString tempDefaultDirPath;
  void setAppDir(QString s);
  /* disable vss until
  int doVss=0;
  int vssPos=0;
  int vssErrPos=0;
  QTimer *vssTimer;
  int vssSleepTime=50;
  int vssReadSize=400;
  QFile *pipeVssFile;
  QFile *pipeVssErrFile;*/
  //QString createWinMkdirCommand(QString tempPath,bool vss,QStringList rsyncArgs,bool logGui);
  extern QString createWinRsyncCommand(QString tempPath,bool vss,QStringList rsyncArgs,bool logGui=true);
  // END of Windows related variables

  extern QString rsyncCommandPath;  // holds the full path of the rsync command for WINDOWS or just "rsync" for *nix
  extern QString sshCommandPath;    // holds the full path of the ssh command for WINDOWS or just "ssh" for *nix

  extern QString XnixSLASH;         // holds the default Xnix slash which is /

  extern bool OS2running;
  extern QString SLASH;
  extern bool WINrunning;
  extern bool notXnixRunning;
}
using namespace Global;
#endif
// end of global.h--------------------------------------------------------------
