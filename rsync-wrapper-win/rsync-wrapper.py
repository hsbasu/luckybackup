#!/usr/bin/python

##To compile luckybackup modify C:\Qt\4.8.2\mkspecs\win32-g++\qmake.conf
## And LFLAGS on Makefile.release -Wl,-subsystem,windows -Wl,-subsystem,console
#ssh -o "StrictHostKeyChecking no"
import sys,os,random,tempfile,string,commands,subprocess
import unicodedata

validFilenameChars = "-_.() %s%s" % (string.ascii_letters, string.digits)

def removeDisallowedFilenameChars(filename):
    cleanedFilename = unicodedata.normalize('NFKD', filename).encode('ASCII', 'ignore')
    return ''.join(c for c in cleanedFilename if c in validFilenameChars)

tempdir=tempfile.gettempdir()
(fscript1,fscript1_name)=tempfile.mkstemp(".bat")
#(fscript2,fscript2_name)=tempfile.mkstemp(".bat")
(fsetvar,fsetvar_name)=tempfile.mkstemp(".cmd")
os.close(fsetvar)
(flogfile,flogfile_name)=tempfile.mkstemp("-error.log")
#os.close(flogfile)
  
homedir=os.path.abspath(os.path.dirname(sys.argv[0]))
myid=str(int(random.uniform(1,999999999)))
source=sys.argv[-2]
dest=sys.argv[-1]

os.write(flogfile,"\",\"".join(sys.argv)+"\n") 
params=sys.argv
params.pop(0)
#params.pop(-1)
#params.pop(-1)

filterstr=" "
#realparams=" --chmod=ugo=rwX "
realparams=" --chmod=u=rwX "
remotestr=" "
filename=" "
quote=False
vss=False
setacl_backuppath=" "
setacl_restorepath=" "
logfilestr=" "
backupfilestr=" "
createdir=False
restore=False
listdir=False
for i in params:
  if i[0:11] == "--list-only":
    listdir=True
  elif i[0:7] == "--mkdir":
    createdir=True
  elif i[0:2] == "-e":
    createcmd=i[2:].replace("'",'"')+" -o \"StrictHostKeyChecking no\" "
    str=i[0:2]+ " \""+ i[2:]
    remotestr=str.replace("'","\\\"")+" -o \\\"StrictHostKeyChecking no\\\"\" "
#  elif i[0:13] == "--backup-dir=":
#    realparams+=" --backup-dir=\""+":".split(i[13:])[1]+"\" "
#    backupfilestr=" --backup-dir=\""+i[13:]+"\" "
  elif i[0:11] == "--log-file=":
    logfilestr=" --log-file=\""+subprocess.check_output("\""+os.path.join(homedir,"cygpath.exe")+"\" \""+i[11:]+"\"",stderr=subprocess.STDOUT).strip()+"\" "
  elif i[0:9] == "--filter=":
    filterstr+= " --filter=\""+i[9:]+"\" "
#  elif i[0:11] == "--backup-nt":
#    setacl_backuppath= i[12:]
#  elif i[0:12] == "--restore-nt":
#    setacl_restorepath= i[13:]
  elif i[0:5] == "--vss ":
    vss=True
  elif i[0] != "-":
    os.write(flogfile, "\nOTHER: "+i+"\n")
  else:
    os.write(flogfile, "\nPARAM:"+i+"\n")
    realparams+=" \""+i+"\""
#print "RSYCNSTR:"+remotestr 
#print realparams
#print "SOURCE:"+source
#print "NTSTREAMS:"+setacl_backuppath+"-------"+setacl_restorepath

if listdir:
  os.write(flogfile,'"'+homedir+"/rsync.exe\" "+realparams+" "+
    logfilestr+backupfilestr+filterstr+" "+remotestr+dest)
  os.close(fscript1)
  #os.close(fscript2)
  print subprocess.call('"'+homedir+"/rsync.exe\" "+realparams+" "+
    logfilestr+backupfilestr+filterstr+" "+remotestr+dest,stderr=subprocess.STDOUT,shell=True) 
  os.remove(fscript1_name)
  #os.remove(fscript2_name)
  os.remove(fsetvar_name)
  os.close(flogfile)
  os.remove(flogfile_name)
  sys.exit(0)
  
if createdir:
  os.write(fscript1,createcmd+dest.replace(":"," \"mkdir -p ")+"\"")
  os.write(flogfile,createcmd+dest.replace(":"," \"mkdir -p ")+"\"")
  os.close(fscript1)
  #os.close(fscript2)
  print subprocess.call(fscript1_name,stderr=subprocess.STDOUT,shell=True) 
  os.remove(fscript1_name)
  #os.remove(fscript2_name)
  os.remove(fsetvar_name)
  os.close(flogfile)
  os.remove(flogfile_name)
  sys.exit(0)
  
for i in ['w','v','u','t','s','r','q','p','z','y','x','o','n','m','l','k','j','i']:
   mapdrive=i
   if not os.path.isdir(mapdrive+":/"):
       break   

if source.find("@") > -1:
  print "Doing restore command"
  os.write(flogfile,"Doing restore command")
  vss=False
  restore=True
if source[1] == ":":
  srcdrive=source[0]
  newsource=subprocess.check_output("\""+os.path.join(homedir,"cygpath.exe")+"\" \""+mapdrive+source[1:]+"\"",stderr=subprocess.STDOUT).strip() 
  source=subprocess.check_output("\""+os.path.join(homedir,"cygpath.exe")+"\" \""+source+"\"",stderr=subprocess.STDOUT).strip() 
  #newsource=source[:10]+mapdrive+source[11:]
elif source[:8] == "/cygdrive" :
  srcdrive=source[10]
  newsource=source[:10]+mapdrive+source[11:]
else:
  print "Invalid source drive for VSS"
  os.write(flogfile,"Invalid source drive for VSS")
  vss=False
if dest[1] == ":":
  if dest[0] == srcdrive :
    print "Cannot copy open files to same drive"
    os.write(flogfile,"Cannot copy open files to same drive")
    vss=False
  dest=subprocess.check_output("\""+os.path.join(homedir,"cygpath.exe")+"\" \""+dest+"\"",stderr=subprocess.STDOUT).strip() 

  




script1="""
@ECHO OFF
ECHO DOING RSYNC  
SETLOCAL ENABLEEXTENSIONS ENABLEDELAYEDEXPANSION

CALL {setvar} 

"{runfrom}\dosdev"  {mapdrive}  %SHADOW_DEVICE_1%  
REM dir {mapdrive} 
REM ECHO Backing up: {rsync_cmd} 
REM 2>&1
{rsync_cmd} >> {logfile} 2>&1
SET ACTERR=!ERRORLEVEL!
REM type {logfile}

REM ECHO Backing up completed: !ACTERR!. !DATE! !TIME!  

REM delete shadow device drive mapping
"{runfrom}\dosdev" -r -d {mapdrive} 2>NUL
rem del {setvar}
rem del %0
IF !ACTERR! GTR 0 (
ECHO ERROR on backup.  !DATE! !TIME!  
exit /b 1
)


""" .format(logfile=flogfile_name,setvar=fsetvar_name,rsync_cmd='"'+homedir+"/rsync.exe\" -v "+realparams+" "+logfilestr+backupfilestr+filterstr+" "+remotestr+" \""+newsource+"\" "+dest,runfrom=homedir,mapdrive=mapdrive+":")


script2="""
REM ECHO INICIO
@ECHO OFF
SET tdebug=y
SET tdeleteshadows=y
SET runfrom={runfrom}
SET el=0
REM Determine Windows version WINVER 5.0=2000, 5.1=XP, 5.2=2003, 6.0=Vista, 6.1=7/2008
FOR /F "tokens=2* delims=[]" %%A IN ('VER') DO FOR /F "tokens=2,3 delims=. " %%B IN ("%%A") DO SET WINVER=%%B.%%C
REM Determine Windows 32-bit (x86) or 64-bit (x64) WINBIT
SET WINBIT=x86&&IF "%PROCESSOR_ARCHITECTURE%" == "AMD64" (SET WINBIT=x64) ELSE IF "%PROCESSOR_ARCHITEW6432%" == "AMD64" SET WINBIT=x64
IF %WINVER% LSS 5.1 (
   ECHO Sorry, %this% cannot run under this version of Windows %WINVER%-%WINBIT%.
   SET el=12
   GOTO :ennd
)
REM Set VSHADOWVER appropriately for the vshadow-n-[bit].exe programs
IF %WINVER%==5.1 SET VSHADOWVER=xp&&SET WINBIT=x86
IF %WINVER%==5.2 SET VSHADOWVER=2003&&SET WINBIT=x86
IF %WINVER%==6.0 SET VSHADOWVER=2008
IF %WINVER%==6.1 SET VSHADOWVER=2008-r2

REM ECHO %WINVER%

REM ---- Change current drive ----
REM For reasons that are undocumented - but probably related to the location of
REM snapshot data - vshadow must be run with a local, or the snapshot source,
REM drive as the current drive on the command line. So we must switch to source
REM drive and ensure that all calls to external programs are mapped back to the
REM original location  - which may for instance be on a network share
REM switch to %TEMP% if on mainsrcdr, otherwise to %mainsrcdr%:\
CD  {mainsrcdr}
CD %TEMP%
{mainsrcdr}
CD %runfrom%

REM ------- delete existing shadow copy ---------------------------------------
   IF ERRORLEVEL 1 SET el=109&&GOTO :endd
   IF NOT "%tdeleteshadows%"=="y" (
       IF "%tdebug%"=="y" ECHO Skipping deletion of any existing shadow copies
   ) ELSE (
       IF "%tdebug%"=="y" ECHO About to delete any existing shadow copies
       ECHO y|"%runfrom%\\vshadow-%VSHADOWVER%-%WINBIT%.exe" -da>nul
       IF ERRORLEVEL 1 (
           IF "%tdebug%"=="y" ECHO Error occurred: testing for administrator permissions
           IF EXIST "%windir%\\system32\\test" RMDIR "%windir%\\system32\\test" 2>nul
           VERIFY>NUL
           MKDIR "%windir%\\system32\\test" 2>nul
           IF ERRORLEVEL 1 (
               REM not running as administrator, this is cause of failure
               IF "%tdebug%"=="y" ECHO No administrator permissions
               SET /A el=11
           ) ELSE (
               REM running as administrator, there is a problem with vshadow
               RMDIR "%windir%\\system32\\test"
               SET /A el=7
           )
           GOTO :endd
       )
       IF "%tdebug%"=="y" ECHO Deleted any existing shadow copies
   )
REM ------------------------------------------------------------------------------
   REM "%runfrom%\\vshadow-%VSHADOWVER%-%WINBIT%.exe" -script={setvar} -exec={script} {mainsrcdr}
    "%runfrom%\\vshadow-%VSHADOWVER%-%WINBIT%.exe" -script={setvar} {mainsrcdr}
    call {setvar}
    "{runfrom}\dosdev"  {mapdrive}  %SHADOW_DEVICE_1%  
    dir {mapdrive} 
    {rsync_cmd}  2>&1
   IF ERRORLEVEL 1 SET /A el=%ERRORLEVEL%+20 
    "%runfrom%\\vshadow-%VSHADOWVER%-%WINBIT%.exe" -ds=%SHADOW_ID_1%
:ennd
  cd %runfrom%
  REM del {logfile}
  REM del {setvar}
  REM del {script}
  REM del %0 
  exit /b %el%

""" .format(logfile=flogfile_name,mainsrcdr=srcdrive+":",setvar=fsetvar_name,script=fscript1_name,runfrom=homedir,rsync_cmd='"'+homedir+"/rsync.exe\" -v "+realparams+" "+logfilestr+backupfilestr+filterstr+" "+remotestr+" \""+newsource+"\" "+dest,mapdrive=mapdrive+":")

script3="""
@ECHO OFF
SET tdebug=y
SET tdeleteshadows=y
SET runfrom={runfrom}
SET el=0
REM Determine Windows version WINVER 5.0=2000, 5.1=XP, 5.2=2003, 6.0=Vista, 6.1=7/2008
FOR /F "tokens=2* delims=[]" %%A IN ('VER') DO FOR /F "tokens=2,3 delims=. " %%B IN ("%%A") DO SET WINVER=%%B.%%C
REM Determine Windows 32-bit (x86) or 64-bit (x64) WINBIT
SET WINBIT=x86&&IF "%PROCESSOR_ARCHITECTURE%" == "AMD64" (SET WINBIT=x64) ELSE IF "%PROCESSOR_ARCHITEW6432%" == "AMD64" SET WINBIT=x64
IF %WINVER% LSS 5.1 (
   ECHO Sorry, %this% cannot run under this version of Windows %WINVER%-%WINBIT%.
   SET el=12
   GOTO :eof
)
REM Set VSHADOWVER appropriately for the vshadow-n-[bit].exe programs
IF %WINVER%==5.1 SET VSHADOWVER=xp&&SET WINBIT=x86
IF %WINVER%==5.2 SET VSHADOWVER=2003&&SET WINBIT=x86
IF %WINVER%==6.0 SET VSHADOWVER=2008
IF %WINVER%==6.1 SET VSHADOWVER=2008-r2
ECHO %runfrom%\\vshadow-%VSHADOWVER%-%WINBIT%.exe


""".format(runfrom=homedir)

(fscript2,fscript2_name)=tempfile.mkstemp(".bat")
os.write(fscript2,script3)
os.close(fscript2)
vshadow=subprocess.check_output(fscript2_name).strip()
os.remove(fscript2_name)

#SETACL: backup windows permisions
#setacl -on d:\temp\test -ot file -actn list -lst f:sddl;w:d,s,o,g -rec cont -bckp d:\temp\backup.txt
#RESTORE:
#setacl -on d:\temp\test -ot file -actn restore -bckp d:\temp\backup.txt

#setacl='\"'+homedir+"/setacl.exe\" -on "
#setacl_backup=" -ot file -actn list -lst f:sddl;w:d,s,o,g -rec cont -bckp "
#setacl_restore=" -ot file -actn restore -bckp "

 
if source[0] == "/":
  #print os.path.join(homedir,"cygpath.exe")+" -w \""+source+"\""
  #winsource=subprocess.check_output("\""+os.path.join(homedir,"cygpath.exe")+"\" -w \""+source+"\"",stderr=subprocess.STDOUT).strip() 
  #print setacl+"\""+winsource+"\""+setacl_backup+"\""+winsource+"\\"+setacl_backuppath+"\""
  #if setacl_backuppath != "":
  #  print subprocess.check_output(setacl+"\""+winsource+"\""+setacl_backup+"\""+winsource+"\\"+setacl_backuppath+"\"",stderr=subprocess.STDOUT)
  #sys.exit(0)
  os.write(fscript1,script1)
  #os.write(fscript2,script2)
  os.close(fscript1)
  #os.close(fscript2)
  #print fscript2_name
  if vss:
    #print subprocess.call("cmd.exe /c " + fscript2_name,stderr=subprocess.STDOUT,shell=True)
    os.write(flogfile,"\""+vshadow+"\" -script=\""+fsetvar_name+"\" -exec=\""+fscript1_name+"\" "+srcdrive+": ")
    os.close(flogfile)
    flogfile=os.open(flogfile_name,os.O_RDONLY)
    #logpos=0
    #os.lseek(flogfile,0,0)
    cmderr=""
    cmdout=""
    try:
      runobj=subprocess.Popen([vshadow,"-script="+fsetvar_name,"-exec="+fscript1_name,srcdrive+":"],
                 bufsize=0, # fully buffered (default)
                 stdin=subprocess.PIPE,
                 stdout=subprocess.PIPE,
                 stderr=subprocess.PIPE,
#                 cwd=os.curdir,
                 env=os.environ)
#      cmdout,cmderr =  runobj.communicate()
#      ret=runobj.wait()
#      ret=runobj.returncode
      while True:
        ret=runobj.poll()
        if ret != None:
          break
        sys.stdout.write (os.read(flogfile,20))
        #cmderr+=runobj.stderr.readline()
        #cmdout+=runobj.stdout.readline()
    except OSError, e:
      print e
#      print cmdout
#      print cmderr
      ret=1
#    print cmdout,cmderr
    print runobj.stderr
    print runobj.stdout
    t=os.read(flogfile,100)
    while t:
      sys.stdout.write(t)
      t=os.read(flogfile,100)
    print cmdout
    print cmderr
  else:
    os.write(flogfile,'"'+homedir+"\\rsync.exe\" "+realparams+" "+logfilestr+backupfilestr+filterstr+" "+remotestr+" \""+source+"\" \""+dest+"\" ")
    ret=subprocess.call('"'+homedir+"\\rsync.exe\" "+realparams+" "+logfilestr+backupfilestr+filterstr+" "+" "+remotestr+" \""+source+"\" \""+dest+"\" ",stderr=subprocess.STDOUT,shell=True)

else:
  print "remote source"
  os.write(flogfile,"Remote source\n")
  os.write(flogfile,'"'+homedir+"/rsync.exe\" "+realparams+" "+logfilestr+backupfilestr+filterstr+" "+remotestr+" \""+source+"\" \""+dest+"\" ")
  os.close(fscript1)
  #os.close(fscript2)
  ret=subprocess.call('"'+homedir+"/rsync.exe\" "+realparams+" "+logfilestr+backupfilestr+filterstr+" "+remotestr+" \""+source+"\" \""+dest+"\" ",stderr=subprocess.STDOUT,shell=True)

#if dest[0] == "/" and setacl_restorepath != "":
#  windest=subprocess.check_output('"'+homedir+"/cygpath.exe\" -w \""+dest+"\"",stderr=subprocess.STDOUT)
#  print subprocess.check_output(setacl+"\""+windest+"\""+setacl_restore+"\""+windest+"\\"+setacl_restorepath+"\"",stderr=subprocess.STDOUT)


#if vss == False:
os.remove(fscript1_name)
#os.remove(fscript2_name)
os.remove(fsetvar_name)
os.close(flogfile)
os.remove(flogfile_name)
sys.exit(ret)
