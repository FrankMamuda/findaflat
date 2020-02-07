QT += core gui xml network widgets

TARGET = FindAFlat
TEMPLATE = app

SOURCES += main.cpp\
        mainwindow.cpp \
    filter.cpp \
    filtermodel.cpp \
    listing.cpp \
    settings.cpp \
    listingmodel.cpp

HEADERS  += mainwindow.h \
    main.h \
    filter.h \
    filtermodel.h \
    listing.h \
    settings.h \
    listingmodel.h

FORMS    += mainwindow.ui

RESOURCES += \
    resources.qrc

win32:RC_FILE = icon.rc

TRANSLATIONS = i18n_lv_LV.ts

# NOTE: using libs from https://bintray.com/vszakats/generic/openssl
win32:INCLUDEPATH += C:/openssl-win64/include/openssl
win32:LIBS += -LC:/openssl-win64/lib -lcrypto -lssl
