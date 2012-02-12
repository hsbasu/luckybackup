/*-------------------------commandline.cpp------------------------------------------
 runs luckybackup in command line mode

===============================================================================================================================
===============================================================================================================================
     This file is part of "luckyBackup" project
     Copyright 2008-2011, Loukas Avgeriou
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
     along with luckyBackup.  If not,see <http://www.gnu.org/licenses/>.
 developer 		: luckyb 
 last modified 		: 17 Feb 2011
===============================================================================================================================
===============================================================================================================================
*/

#include "luckybackupwindow.cpp"
#include "commandline.h"

// class commandline Constructor=================================================================================================
commandline::commandline()
{
	writeToLog=FALSE;
	errorsFound = 0;	// Total number of errors from all tasks (in CLI, only 1 error per task is allowed !!)
	errorCount = 0;		// Number of errors from one task (max value is 1)
}

// class commandline Destructor=================================================================================================
commandline::~commandline()
{
}

// result===================================================================================================================
// calls functions intro, loadProfile, check_list, check_dirs, rsyncIT, thats_all and returns an integer to main()
int commandline::result()
{
	intro();
	
	rsyncIT();
	thats_all();

	if (errorsFound > 0)
		return 6;		// error at rsync or pre/post command execution

	return 0;
}

// intro===================================================================================================================
// displays an intro message
void commandline::intro()
{
	cout << "\n============================================================================================\n";
	cout << "		luckybackup version ";
	cout << appVersion;
	cout << " - command line mode \n";
	cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\n";
	cout << "This application will (hopefully) sync and backup everything you need in no time\n";
	cout << "(Well, it might take a little longer if run for the first time ;-)\n\n";
	cout << "\n";
	if (!NoQuestions)	//if questions are required
	{
		cout <<  "Make sure that:\n";
		cout <<  "You have already declared and mounted all the directories that need to be synced/backed-up\n";
		cout <<  "Please take a moment to examine all messages from checks done until now\n\n";
		cout <<  "Also have in mind that, to avoid errors, it is best to run this app as su\n\n";
		cout <<  "Hit 'enter' to continue otherwise press 'ctrl+c' to exit NOW\n";
		cin.get();
	}
	cout << "\n\n";
}



// rsyncIT===================================================================================================================
// Performs the execution of operations
void commandline::rsyncIT()
{
	cout << "\n============================================================================================\n";
	cout << "				Executing tasks";
	if (DryRun)
		cout << "  (simulation mode)";
	cout << "\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\n";

	//some useful variables-----------------------------------------------------------------------
	QProcess *syncProcess;	syncProcess = new QProcess;	//create a new qprocess (for rsync)
	QString command="rsync";	//command to be executed. Normally this is "rsync"
	QStringList rsyncArguments;	// This stringList holds all arguments for the rsync command
	QString dirA;			//holds the first dir to be synced
	QString dirB;			//holds the second dir to be synced
	QString logText="";		//This text will be appended to the logfile
	
	currentOperation = 0;
	
	while (currentOperation < TotalOperations)
	{
		//if --skip-critical is given as argument and the task is CRITICAL
		if ( (Operation[currentOperation] -> GetCRITICAL()) && (SkipCritical) )	
			Operation[currentOperation] -> SetPerform(FALSE);	//don't perform this operation

		if ( (Operation[currentOperation] -> GetPerform()) || (Operation[currentOperation] -> GetByPassWARNING()) )
		{
			bool RemoteDestUsed = (Operation[currentOperation] -> GetRemoteDestination()) && (Operation[currentOperation] -> GetRemote());
			//update the last execution time + logfile actions ~~~~~~~~~~~~~~~~~~~~~
			if (!DryRun)
			{
				int maxSnaps = Operation[currentOperation] -> GetKeepSnapshots(); // this is the number of snapshots to keep
				if (maxSnaps < 1)
					maxSnaps = 1;
				int currentSnaps = Operation[currentOperation] -> GetSnapshotsListSize(); // this is the current number of snapshots
				if (currentSnaps < 1)
					currentSnaps = 1;

				// first remove the older logfiles and snapshots if max keep snapshots is reached
				if (currentSnaps >= maxSnaps)
				{
					cout << "\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";
					cout << " Removing old snapshots and logfiles of task: " <<
						QString((Operation[currentOperation] -> GetName()).toUtf8()).toStdString() << "\n";
				
					// ************** actually REMOVE DATA of all older snapshots ***************************
					// ******************************************************************************
					// First calculate the folder where snapshots go
					QStringList tempArguments = Operation[currentOperation] -> GetArgs();
					QString tempSource = tempArguments[tempArguments.size()-2];
					QString tempDestination = tempArguments[tempArguments.size()-1];
					QString sourceLast = tempSource;
					if (!tempSource.endsWith(SLASH))	// this means task is of type "backup dir by name"
					{
						if (sourceLast.contains(":"))	// this is normal for a remote directory
							sourceLast = sourceLast.right(tempSource.size()-sourceLast.lastIndexOf(":")-1);	//this is the remote source dir without the remote pc
						if (tempSource.contains(SLASH))	// this is normal for a directory unless it is remote
							sourceLast = sourceLast.right(tempSource.size()-sourceLast.lastIndexOf(SLASH)-1);	//this is the lowest dir of source
						
						tempSource.append(SLASH);
						tempDestination.append(sourceLast + SLASH);
					}
					tempDestination.append (snapDefaultDir);
					
					// increase the remove limit to include the source.size() if "backup dir by name" is used
					if (Operation[currentOperation] -> GetTypeDirName())
						removeCharLimit = 4 + sourceLast.size()+1;
					else
						removeCharLimit = 4;
					
					QProcess *rmProcess;	rmProcess  = new QProcess;
					QStringList rmArgs;
					rmArgs << "--progress" << "-r" << "--delete-after";
					int snapToKeep = currentSnaps-maxSnaps + 1;
					while ( snapToKeep < currentSnaps )
					{
						rmArgs.append("--filter=protect " + Operation[currentOperation] -> GetSnapshotsListItem(snapToKeep) + SLASH);
						snapToKeep++;
					}
					
					//also add all remote arguments exactly as used at normal backup
					if (RemoteDestUsed)
					{
						if ( Operation[currentOperation] -> GetRemotePassword() != "")
							rmArgs.append("--password-file=" + ( Operation[currentOperation] -> GetRemotePassword()) );
						if ( Operation[currentOperation] -> GetRemoteSSH())
						{
							if ( Operation[currentOperation] -> GetRemoteSSHPassword() != "")
								if ( Operation[currentOperation] -> GetRemoteSSHPort() != 0)
									rmArgs.append("-e ssh -i " +  Operation[currentOperation] -> GetRemoteSSHPassword() +" -p " +
												countStr.setNum( Operation[currentOperation] -> GetRemoteSSHPort()) );
								else
									rmArgs.append("-e ssh -i " +  Operation[currentOperation] -> GetRemoteSSHPassword());
							else
								if ( Operation[currentOperation] -> GetRemoteSSHPort() != 0)
									rmArgs.append("-e ssh -p " + countStr.setNum( Operation[currentOperation] -> GetRemoteSSHPort()) );
								else
									rmArgs.append("-e ssh");
						}
					}
					
					rmArgs.append(snapEmptyDir);
					rmArgs.append(tempDestination);
			
					rmProcess -> start ("rsync",rmArgs);
					rmProcess -> waitForFinished();
					
					if ((rmProcess -> exitCode()) == 0)
						cout << "\nRemoved  all older snapshots data";// << QString((tempDestination + Operation[currentOperation] -> GetSnapshotsListItem(0) + SLASH).toUtf8()).toStdString();
					else
						cout << "\nfailed to remove all older snapshots data";// << QString((tempDestination + Operation[currentOperation] -> GetSnapshotsListItem(0) + SLASH).toUtf8()).toStdString() << "\n";
				// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
				
					count = 0;
					while ( count < (currentSnaps -maxSnaps + 1 ) )
					{
						//remove the changes file
						snapchangesfilename = snapChangesDir + QString(profileName.toUtf8()) + "-" + 
								QString((Operation[currentOperation] -> GetName()).toUtf8()) + "-" +
								(Operation[currentOperation] -> GetSnapshotsListItem(0)) + ".changes.log";
						snapfile.setFileName(snapchangesfilename);
						if (snapfile.exists())
						{
							if (snapfile.remove())	// this is the old snapshot changes file
								cout << "\nRemoving " << QString(snapchangesfilename.toUtf8()).toStdString();
							else 
								cout << "\nfailed to remove " << QString(snapchangesfilename.toUtf8()).toStdString() << "\n";
						}
				
						//remove the oldest logfile
						logfilename = logDir + QString(profileName.toUtf8()) + "-" + 
								QString((Operation[currentOperation] -> GetName()).toUtf8()) + "-" +
								Operation[currentOperation] -> GetSnapshotsListItem(0) + ".log";
						logfile.setFileName(logfilename); // this is the old logfile
						if (logfile.exists())
						{
							if (logfile.remove())
								cout << "\nRemoving " << QString(logfilename.toUtf8()).toStdString();
							else
								cout << "\nfailed to remove " << QString(logfilename.toUtf8()).toStdString() << "\n";
						}
						
						//remove the oldest snapshot (0) from the list
						Operation[currentOperation] -> RemoveSnapshotsListItem (0);
						count++;
					}
				}

				//set the current date and time as the operation's last execution date-time
				Operation[currentOperation] -> SetLastExecutionTime (QDateTime::currentDateTime());
		
				// add a new snapshot with the last execution date-time
				Operation[currentOperation] ->
							AddSnapshotsListItem ((Operation[currentOperation] ->
									GetLastExecutionTime()).toString("yyyyMMddhhmmss"));
				currentSnaps = Operation[currentOperation] -> GetSnapshotsListSize();	// update the current number of snapshots

				// set a new changes file. This has a tag of the previous snapshot will include changes made to this snapshot
				snapchangesfilename = snapChangesDir + QString(profileName.toUtf8()) + "-" + 
					QString((Operation[currentOperation] -> GetName()).toUtf8()) + "-" +
					(Operation[currentOperation] -> GetSnapshotsListItem(currentSnaps - 1)) + ".changes.log";
				snapfile.setFileName(snapchangesfilename);
		
				// set a new logfile
				logfilename = logDir + QString(profileName.toUtf8()) + "-" +
					QString((Operation[currentOperation] -> GetName()).toUtf8()) + "-" +
					( Operation[currentOperation] -> GetSnapshotsListItem(currentSnaps - 1) ) + ".log";
				
				logfile.setFileName(logfilename); // this is the current logfile
				if (logfile.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append))	//create a new log file
				{
					writeToLog = TRUE;	//& if it's ok
					syncProcess -> setStandardOutputFile(logfilename, QIODevice::Append );
					syncProcess -> setStandardErrorFile(logfilename, QIODevice::Append );
				}
				else
					writeToLog = FALSE;
				
				// reset the error counter
				errorCount = 0;		// Number of errors from one task (max value is 1)
			}
			
			//pre-task commands execution ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			count = 0;
			bool StopTaskExecution = FALSE;
			
			while (Operation[currentOperation] -> GetExecuteBeforeListSize() > count)
			{
				cout << "\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";
				cout << " Now executing pre-task command:  "
					<< QString((Operation[currentOperation] -> GetExecuteBeforeListItem(count)).toUtf8()).toStdString() << "\n";
				
				logFileUpdate("pre-starting","",count);	//update the logfile

				//syncProcess -> execute (Operation[currentOperation] -> GetExecuteBeforeListItem(count));
				syncProcess -> start (Operation[currentOperation] -> GetExecuteBeforeListItem(count));
				
				//wait for the process to start and finish
				if (syncProcess -> waitForStarted (-1))	// the "-1" arguments are used to avoid timing out
				{
					cout << "\nThe process started successfully\n";
					syncProcess -> waitForFinished (-1);
				}
				else
				{
					cout << "\nThe process failed to start\n";
					errorCount++;
					if (Operation[currentOperation] -> GetExecuteBeforeListItemState(count) == TRUE) 
                    {
                        StopTaskExecution = TRUE;
                        break;
                    }
				}
				
				cout << "\nexit code	: " << syncProcess -> exitCode();
				cout << "\nexit status	: " << syncProcess -> exitStatus() << "\n";
				if (syncProcess -> exitCode() != 0)
				{
					errorCount++;
                    if (Operation[currentOperation] -> GetExecuteBeforeListItemState(count) == TRUE) 
                    {
                        StopTaskExecution = TRUE;
                        break;
                    }
				}
				
				logFileUpdate("pre-finished", "", count);
				
				count++;
			}
			
			//task execution
			if (StopTaskExecution)
			{
				logFileUpdate("pre-task-exited-with-error", "", 0);
				
				cout << "\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";
				cout << " Pre-task command exited with an error. Skipping task execution...";
				cout << "\n--------------------------------------------------------------------------------------------\n";
			}
			else
			{
				cout << "\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";
				cout << " Now executing :  " << QString((Operation[currentOperation] -> GetName()).toUtf8()).toStdString();
				cout << "\n--------------------------------------------------------------------------------------------\n";
				
				if (Operation[currentOperation] -> GetTypeSync())
					logFileUpdate("rsync-starting-syncAB", "", 0);
				else
					logFileUpdate("rsync-starting-backup", "", 0);

				// Create the destination if it does not exist
				bool DestCreateFail = FALSE;	 // This will become TRUE if destination does not exist and cannot be created
				
				rsyncArguments = AppendArguments(Operation[currentOperation]);	//set rsync arguments
				
				dirB = rsyncArguments[rsyncArguments.size()-1];
				QDir destCreate (dirB.toUtf8());
				if ( (!destCreate.exists()) && (!Operation[currentOperation] -> GetTypeSync()) && (!RemoteDestUsed) ) // local use ONLY
				{
					if (destCreate.mkpath(dirB.toUtf8()))
					{
						logFileUpdate("rsync-standard", "<br>Successfuly created destination directory", 0);
						cout << "\n\n Successfuly created destination directory\n";
					}
					else
					{
						logFileUpdate("rsync-error", "<br>Failed to create destination directory", 0);
						cout << "\n\n Failed to create destination directory\n";
						DestCreateFail = TRUE;
						errorCount++;
					}
				}
				
				if (!DestCreateFail)
				{
					//convert rsyncArguments to utf-8 for console compatibility
					count=0;
					while (count < rsyncArguments.size())
					{
						rsyncArguments[count] = QString(rsyncArguments[count].toUtf8());
						count++;
					}
					
					if (DryRun)
						rsyncArguments.insert(rsyncArguments.size()-2,"--dry-run");
					//if (writeToLog )
					//	rsyncArguments.insert(rsyncArguments.size()-2,"--log-file="+logfilename);

					//syncProcess -> execute (command,rsyncArguments);	// execute rsync command with rsyncArguments for backup or sync A->B
					syncProcess -> start (command,rsyncArguments);
					//wait for the process to start and finish
					if (syncProcess -> waitForStarted (-1))	// the "-1" arguments are used to avoid timing out
					{
						cout << "\nThe process started successfully\n";
						syncProcess -> waitForFinished (-1);
					}
					else
					{
						cout << "\nThe process failed to start\n";
						errorCount++;
					}
					
					cout << "\nrsync exit code		: " << syncProcess -> exitCode();
					cout << "\nrsync exit status	: " << syncProcess -> exitStatus() << "\n";
					if (syncProcess -> exitCode() != 0)
						errorCount++;

					if (Operation[currentOperation] -> GetTypeSync())
						logFileUpdate("rsync-finished-sync1", "", 0);
					else
						logFileUpdate("rsync-finished", "", 0);
					
					if (Operation[currentOperation] -> GetTypeSync())	//execute rsync for syncing B -> A
					{
						logFileUpdate("rsync-starting-syncBA", "", 0);
						
						dirA = rsyncArguments[rsyncArguments.size()-2];	//swap last 2 arguments (dir names)
						dirB = rsyncArguments[rsyncArguments.size()-1];
						rsyncArguments.removeLast();	rsyncArguments.removeLast();
						rsyncArguments.append(dirB);
						rsyncArguments.append(dirA);

						//syncProcess -> execute (command,rsyncArguments);	// execute rsync command with rsyncArguments B->A
						syncProcess -> start (command,rsyncArguments);	
						if (syncProcess -> waitForStarted (-1))	// the "-1" arguments are used to avoid timing out
						{
							cout << "\nThe process started successfully\n";
							syncProcess -> waitForFinished (-1);
						}
						else
						{
							cout << "\nThe process failed to start\n";
							errorCount++;
						}
					
						cout << "\nrsync exit code		: " << syncProcess -> exitCode();
						cout << "\nrsync exit status	: " << syncProcess -> exitStatus() << "\n";
						if (syncProcess -> exitCode() != 0)
							errorCount++;

						logFileUpdate("rsync-finished", "", 0);
					}

					//post-task commands execution
					count = 0;
					while (Operation[currentOperation] -> GetExecuteAfterListSize() > count)
					{
						cout << "\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";
						cout << " Now executing post-task command:  "
							<< QString(Operation[currentOperation] -> GetExecuteAfterListItem(count).toUtf8()).toStdString() << "\n";
							
						//update the logfile
						logFileUpdate("post-starting", "", count);
				
						//syncProcess -> execute (Operation[currentOperation] -> GetExecuteAfterListItem(count));

						syncProcess -> start (Operation[currentOperation] -> GetExecuteAfterListItem(count));
						if (syncProcess -> waitForStarted (-1))	// the "-1" arguments are used to avoid timing out
						{
							cout << "\nThe process started successfully\n";
							syncProcess -> waitForFinished (-1);
						}
						else
						{
							cout << "\nThe process failed to start\n";
							errorCount++;
                            if (Operation[currentOperation] -> GetExecuteAfterListItemState(count) == TRUE)
                            {
                                StopTaskExecution = TRUE;
                                break;
                            }
						}
					
						cout << "\nexit code	: " << syncProcess -> exitCode();
						cout << "\nexit status	: " << syncProcess -> exitStatus() << "\n";
                        if (syncProcess -> exitCode() != 0)
                        {
                            errorCount++;
                            if (Operation[currentOperation] -> GetExecuteAfterListItemState(count) == TRUE)
                            {
                                StopTaskExecution = TRUE;
                                break;
                            }
                        }
						
						logFileUpdate("post-finished", "", count);
						
						count++;
					}
					if (StopTaskExecution)
                    {
                        logFileUpdate("pre-task-exited-with-error", "", 0);
                        
                        cout << "\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";
                        cout << " Post-task command exited with an error. Skipping executing the rest of commands...";
                        cout << "\n--------------------------------------------------------------------------------------------\n";
                    }
					
				}
			}
			// close the logfile
			if (writeToLog )
				logfile.close();
			
			// Update the last execution errors and .changes file if real run
			if (!DryRun)
			{
				Operation[currentOperation] -> SetLastExecutionErrors (errorCount);	// update the last execution errors
				
				// strip unused lines from the snapshot changes file
/*				int removeLimit = 4;
				
				// increase the remove limit to include the source.size() if "backup dir by name" is used
				if (Operation[currentOperation] -> GetTypeDirName())
				{
					QString sourceLast = Operation[currentOperation] -> GetSource();
					if (sourceLast.contains(":"))	// this is normal for a remote directory
						sourceLast = sourceLast.right(sourceLast.size()-sourceLast.lastIndexOf(":")-1);	//this is the remote source dir without the remote pc
					if (sourceLast.contains(SLASH))	// this is normal for a directory unless it is remote
						sourceLast = sourceLast.right(sourceLast.size()-sourceLast.lastIndexOf(SLASH)-1);	//this is the lowest dir of source
					
					removeLimit = removeLimit + sourceLast.size()+1;
				}
*/
				QString filesAdded = "", snapLine = "";
				if (snapfile.open(QIODevice::ReadOnly | QIODevice::Text))
				{
					QTextStream in(&snapfile);
					while (!in.atEnd())
					{
						snapLine = in.readLine();
						if (snapLine.contains("+++++++", Qt::CaseInsensitive))
						{
							snapLine = snapLine.right(snapLine.size()-snapLine.lastIndexOf("[LB]")-removeCharLimit) + "\n";
							filesAdded.append(snapLine);
						}
					}
					snapfile.close();
				}
				if (snapfile.open(QIODevice::WriteOnly))
				{
					QTextStream out(&snapfile);
					out << filesAdded;
					snapfile.close();
				}
			}
			errorsFound = errorsFound + errorCount;
			errorCount = 0;
		}
		currentOperation++;
	}
}


// thats_all===================================================================================================================
// Displays an "all done" message
void commandline::thats_all()
{
	cout << "============================================================================================\n";
	cout << "			" << QDateTime::currentDateTime().toString(Qt::SystemLocaleLongDate).toStdString() << "\n";
	cout <<  "			Syncing and backing up is finished";
	if (DryRun)
		cout << "  (simulation mode)";
	if (errorsFound == 0)
		cout << "\n			No errors found";
	else
		cout << "\n			errors found: " << countStr.setNum(errorsFound).toStdString();
	if (writeToLog)
		cout << "\n 	logfile(s) have been created under directory: " << logDir.toStdString();
	cout <<  "\n\n			hope everything went ok ;-)\n";
	cout << "============================================================================================\n";
	
	saveProfile(QString(currentProfile.toUtf8()));	// updates the execution time
    
    // send an email
    if ( (!emailNever) && (!DryRun) )
    {
        bool send = TRUE;
        if ( ((emailError) && (errorsFound == 0))   // do not send if the condition "error" is checked and there are no errors
                ||
             ((emailSchedule) && (!NoQuestions)) )  // do not send if the condition "scheduled" is checked
                                                                   // & --no-questions is NOT given as argument
            send = FALSE;
        
        // ***** gui mode and "schedule" is covered in executenow.cpp ******
        
        if (send)
        {
            cout << "\n\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";
            cout << "trying to send an email\n";
            cout << "       . . .\n\n";
            cout << sendEmailNow(FALSE).toStdString();
        }
    }
}


// end of commandline.cpp---------------------------------------------------------
 
