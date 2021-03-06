# luckybackup

## Overview

The main goal of luckyBackup, as its name states, is the creation of backups of your data.
Somebody once said:

" if you haven't backed up your data is like not having them at all !! ”

That is because, based at the laws of probability, some time, for some reason, something will happen and you will lose valuable data. Another guy once said:

“ computer users can be divided into 2 categories:
Those that have lost data
...and those that have not lost data YET ”

I really wish you to never come to that situation.
But if you do, then I hope that you backed up your data the very last minute either by using this application or not :-)

luckyBackup is an application that backs-up and/or synchronizes any directories with the power of rsync.

It is simple to use, fast (transfers over only changes made and not all data), safe (keeps your data safe by checking all declared directories before proceeding in any data manipulation ), reliable and fully customizable.


## Copyright

Copyright 2008-2018 Loukas Avgeriou
luckyBackup is distributed under the terms of the GNU General Public License

It is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, version 3 of the License.

It is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with luckyBackup.  If not, see <http://www.gnu.org/licenses/>.


## Features

### Backup
Create a "clone" of your data at another location in no time, safely.
Backup any directory (source) to another (destination).
luckyBackup copies over only the changes you've made to the source directory and nothing more.
You will be surprised when your huge source is backed up in seconds (after the first time !!).

Whatever changes you make to the source include adding, moving, deleting, modifing files / directories etc, will have the same effect to the destination.

Owner, group, time stamps, links and permissions of files are preserved.

### Snapshots
Create multiple backup "snapshots".

Each snapshot is an image of the source data that refers to a specific date-time.

Rolling back to any of the snapshots is possible.

### Sync
Sync any directories keeping the files that were most recently modified on both of them.

Useful if you modify files on more than one PCs (using a flash-drive and don't want to bother remembering what did you use last.

### Keep your data safe
luckyBackup first checks whether the directories you've declared exist or if they are empty and warns you accordingly.

You wouldn't want your 500GB music collection backup (that took half a day to create !!) vanish in a second if you forgot to mount the external drive that your source is in !!
You also wouldn't want to execute an rsync command if your destination folder is in an external drive that you also forgot to mount.

### Simple / advanced option
The add/modify operation dialog is quite simple and everybody can use it with confidence.
Hit on the "advanced" pushbutton at the operation properties window and a whole bunch of other options will appear.
If you know what you're doing change anything at will.

### Exclude option
Exclude any file, folder or pattern from the transfer.
You might not want to copy over backup files (*~), trash folders, system mount folders (/media & /mnt), some huge video files or anything else.

### Only include option
Use this option to only transfer over specific file(s), folder(s) or pattern(s) within your source directory and nothing else.

### Add/remove any rsync option
If you don't like the default rsync options that luckybackup uses, add or remove any option you wish.

### Remote connections
Remote connections are possible, either for use as a source or as a destination.

### Also execute
You can execute any command(s) before or after a specific task.

### Restore operation
Everybody wishes to never use this !!
But when this time comes, luckyBackup gives you the option to create an operation based on an existing one for restore purposes or use a wizard style procedure that will guide you through.

### Simulation option
If you are unsure of the effects on your data when executing an rsync command try the dry-run option. luckyBackup will perform a trial run that doesn't make any changes (and produces mostly the same output as a real run).
Note: The progress-bar might be misleading when using this.

### Include option – Execution order
A separate check-box can be used to include or not the operations within a profile.

The option to change the execution order of operation is also given.

### Profiles
You don't have to create all operations from scratch every time you start luckyBackup. You can save your preferences in a .profile file and load it whenever needed. Use as many profiles as you like.

### Scheduling
Scheduling for execution of already created profiles via cronjobs is supported.

### Logfile
After each operation a logfile is created in your home folder. You can have a look at it any time you want.

### e-mail
e-mail a report after a profile execution.

### Command line mode
luckyBackup can run in command line if you wish not to use the gui, but you have to first create the profile that is going to be executed.
Type "luckybackup --help" at a terminal to see usage and supported options.


## Installation from Source

IMPORTANT: luckyBackup copies files preserving their permissions, so you should run it with su privileges  if you want to transfer data that a normal user has no authorization on.

Dependencies
```
 libqtcore4 (>= 4.6)
 libqtgui4 (>= 4.6)
 libc6 or glibc (>= 2.7-1)
 libgcc1 (>= 4.1.1)
 libstdc++6 (>= 4.1.1)
 rsync
 ```

1. Download & extract the tarball.

2. Using the console go to the directory that the unzipped tarball lies eg:
```$
cd ./luckyBackup-0.X.Y
```

3. execute commands:
```$
qmake
make
```

4. Run the app with the command:
```$
./luckybackup
```


Other technical info
==============================================================================================

Project code is in c++ using Qt4.


Web
===============================================================================================
web page        : http://luckybackup.sourceforge.net/

download page   : https://sourceforge.net/projects/luckybackup/files/

To make sure that the file you've downloaded is not corrupted, you also need to download the appropriate md5sum file eg:
`luckyBackup-X.Y.tar.gz.md5`
and then test it with the command eg:
```$
md5sum -c luckyBackup-X.Y.tar.gz.md5
```


Contact
===============================================================================================

Should you have any comments (good work, that is completely useless etc), complains, feature requests, installation problems or anything, do not hesitate to contact me at :

luckyb69@users.sourceforge.net


Wish you good, fast, reliable, safe and usable backups
luckyb
:-)
