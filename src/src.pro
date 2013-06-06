TEMPLATE = app
TARGET = texsample-console

CONFIG += console release

QT = core network
BEQT = core network

isEmpty(BEQT_PREFIX) {
    #TODO: Add MacOS support
    mac|unix {
        BEQT_PREFIX=/usr/share/beqt
    } else:win32 {
        BEQT_PREFIX=$$(systemdrive)/PROGRA~1/BeQt
    }
}
include($${BEQT_PREFIX}/depend.pri)

isEmpty(TSMP_PREFIX) {
    #TODO: Add MacOS support
    mac|unix {
        TSMP_PREFIX=/usr/share/texsample
    } else:win32 {
        TSMP_PREFIX=$$(systemdrive)/PROGRA~1/TeXSample
    }
}
include($${TSMP_PREFIX}/depend.pri)

SOURCES += \
    main.cpp \
    terminaliohandler.cpp

HEADERS += \
    terminaliohandler.h

TRANSLATIONS += \
    ../translations/texsample-console_ru.ts

##############################################################################
################################ Generating translations #####################
##############################################################################

#Gets a file name
#Returns the given file name.
#On Windows slash characters will be replaced by backslashes
defineReplace(nativeFileName) {
    fileName=$${1}
    win32:fileName=$$replace(fileName, "/", "\\")
    return($${fileName})
}

translationsTs=$$files($${PWD}/../translations/*.ts)
for(fileName, translationsTs) {
    system(lrelease $$nativeFileName($${fileName}))
}

contains(TCSL_CONFIG, builtin_resources) {
    RESOURCES += \
        texsample_console.qrc \
        ../translations/texsample_console_translations.qrc
    DEFINES += BUILTIN_RESOURCES
}

##############################################################################
################################ Installing ##################################
##############################################################################

!contains(TCSL_CONFIG, no_install) {

#mac {
    #isEmpty(PREFIX):PREFIX=/Library
    #TODO: Add ability to create bundles
#} else:unix:!mac {
#TODO: Add MacOS support
mac|unix {
    isEmpty(PREFIX):PREFIX=/usr
    equals(PREFIX, "/")|equals(PREFIX, "/usr")|equals(PREFIX, "/usr/local") {
        isEmpty(BINARY_INSTALLS_PATH):BINARY_INSTALLS_PATH=$${PREFIX}/bin
        isEmpty(RESOURCES_INSTALLS_PATH):RESOURCES_INSTALLS_PATH=$${PREFIX}/share/texsample-console
    } else {
        isEmpty(BINARY_INSTALLS_PATH):BINARY_INSTALLS_PATH=$${PREFIX}
        isEmpty(RESOURCES_INSTALLS_PATH):RESOURCES_INSTALLS_PATH=$${PREFIX}
    }
} else:win32 {
    isEmpty(PREFIX):PREFIX=$$(systemdrive)/PROGRA~1/TeXSample-Console
    isEmpty(BINARY_INSTALLS_PATH):BINARY_INSTALLS_PATH=$${PREFIX}
}

##############################################################################
################################ Binaries ####################################
##############################################################################

target.path = $${BINARY_INSTALLS_PATH}
INSTALLS = target

##############################################################################
################################ Translations ################################
##############################################################################

!contains(TCSL_CONFIG, builtin_resources) {
    installsTranslations.files=$$files($${PWD}/../translations/*.qm)
    installsTranslations.path=$${RESOURCES_INSTALLS_PATH}/translations
    INSTALLS += installsTranslations
}

} #end !contains(TSRV_CONFIG, no_install)
