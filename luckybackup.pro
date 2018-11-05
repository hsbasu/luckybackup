#check Qt version

message( "Qt version" $$[QT_VERSION] )
message( "Qmake version" $$[QMAKE_VERSION] )


lessThan(QT_MAJOR_VERSION, 4) {
    lessThan(QT_MINOR_VERSION, 6) {
      error("Sorry, you need at least Qt version 4.6.0")
        }
    }

QMAKE_STRIP = echo

VERSION = 0.5.0

target.path = /usr/bin
TARGET = luckybackup

menu.path = /usr/share/applications
menu.files = menu/luckybackup.desktop menu/luckybackup-su.desktop

polkit.path = /usr/share/polkit-1/actions
polkit.files = menu/net.luckybackup.su.policy

polkitscript.path = /usr/bin
polkitscript.files = menu/luckybackup-pkexec

pixmap.path = /usr/share/pixmaps
pixmap.files = menu/luckybackup.xpm menu/luckybackup.png

documentation.path = /usr/share/doc/luckybackup
documentation.files = manual

manpage.path = /usr/share/man/man8
manpage.files = manpage/luckybackup.8.gz manpage/luckybackup-pkexec.8

translations.path = /usr/share/luckybackup
translations.files = translations

license.path = /usr/share/doc/luckybackup
license.files = license

INSTALLS += target menu polkit polkitscript pixmap documentation manpage translations license

system(gzip -c manpage/luckybackup.8 > manpage/luckybackup.8.gz)
QMAKE_CLEAN = Makefile $${TARGET} manpage/luckybackup.8.gz
system(gzip -c manpage/luckybackup-pkexec.8 > manpage/luckybackup-pkexec.8.gz)
QMAKE_CLEAN = Makefile $${TARGET} manpage/luckybackup-pkexec.8.gz

greaterThan(QT_MAJOR_VERSION, 4) {
    QT += widgets
} 

QT += network

# Actions for specific distros ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
UNAMEA = $$system( uname -a )
ISUBUNTU = $$find(UNAMEA, "ubuntu")
ISFEDORA = $$find(UNAMEA, "fedora")
APTGETOP = $$system (apt-get --help)
ISNOTDEB = $$find(APTGETOP, "command not found")

!isEmpty ( ISFEDORA ) {
    message( "You are running fedora" )
 }
exists( /etc/SuSE-release ) {
    message( "You are running suse" )
    message( "Altering the documentation default install directory..." )
    license.path = /usr/share/doc/packages/luckybackup
    documentation.path = /usr/share/doc/packages/luckybackup
 }
!isEmpty ( ISUBUNTU ) {
    message( "You are running ubuntu" )
 }
!isEmpty ( ISNOTDEB ) {
    message( "You are not running a deb based distro" )
 }
win32 {
     message( "You are running windows" )
 }

macx {
     message( "You are running OSX" )
 }
 
HEADERS	= src/operationClass.h \
    src/luckybackupwindow.h \
    src/modifyDialog.h \
    src/textDialog.h \
    src/scheduleDialog.h \
    src/emailDialog.h \
    src/commandline.h \
    src/about.h \
    src/global.h \
    src/manage.h \
    src/calcDiffThread.h \
    src/logDialog.h \
    src/readLogFile.h \
    src/patternEditor.h \
    src/manageWizard.h \
    src/helpBrowser.h \
    src/winDialog.h \
    src/RsyncDirModel.h \
    src/RsyncDirModel_p.h \
    src/qurlinfo.h \
    src/qdirdialog.h
 
FORMS 	= ui/luckybackupwindow.ui \
    ui/modifyDialog.ui \
    ui/helpBrowser.ui \
    ui/scheduleDialog.ui \
    ui/emailDialog.ui \
    ui/about.ui \
    ui/manage.ui \
    ui/textDialog.ui \
    ui/patternEditor.ui \
    ui/manageWizard.ui \
    ui/logDialog.ui \
    ui/winDialog.ui

SOURCES	= src/modifyDialog.cpp \
    src/helpBrowser.cpp \
    src/textDialog.cpp \
    src/executenow.cpp \
    src/scheduleDialog.cpp \
    src/emailDialog.cpp \
    src/commandline.cpp \
    src/about.cpp \
    src/manage.cpp \
    src/calcDiffThread.cpp \
    src/functions.cpp \
    src/global.cpp \
    src/logDialog.cpp \
    src/readLogFile.cpp \
    src/patternEditor.cpp \
    src/manageWizard.cpp \
    src/winDialog.cpp \
    src/luckybackupwindow.cpp \
    src/main.cpp \
    src/RsyncDirModel.cpp \
    src/RsyncDirModel_p.cpp \
    src/qurlinfo.cpp \
    src/qdirdialog.cpp
 
RESOURCES = resources/luckybackup.qrc

 
TRANSLATIONS = translations/luckybackup_ara.ts \
    translations/luckybackup_bs.ts \
    translations/luckybackup_bg.ts \
    translations/luckybackup_ca.ts \
    translations/luckybackup_zh_TW.ts \
    translations/luckybackup_cs.ts \
    translations/luckybackup_nl.ts \
    translations/luckybackup_en.ts \
    translations/luckybackup_eo.ts \
    translations/luckybackup_et.ts \
    translations/luckybackup_fr.ts \
    translations/luckybackup_gl.ts \
    translations/luckybackup_de.ts \
    translations/luckybackup_el.ts \
    translations/luckybackup_it.ts \
    translations/luckybackup_ja_JP.ts \
    translations/luckybackup_no.ts \
    translations/luckybackup_pl.ts \
    translations/luckybackup_pt_BR.ts \
    translations/luckybackup_ro.ts \
    translations/luckybackup_ru.ts \
    translations/luckybackup_sk.ts \
    translations/luckybackup_sl.ts \
    translations/luckybackup_es.ts \
    translations/luckybackup_sv.ts \
    translations/luckybackup_tr.ts \
    translations/luckybackup_vi.ts \
