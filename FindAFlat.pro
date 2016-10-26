QT += core gui xml network widgets

TARGET = FindAFlat
TEMPLATE = app

SOURCES += main.cpp\
        mainwindow.cpp \
    filter.cpp \
    filtermodel.cpp \
    listing.cpp

HEADERS  += mainwindow.h \
    main.h \
    filter.h \
    filtermodel.h \
    listing.h

FORMS    += mainwindow.ui

RESOURCES += \
    resources.qrc

win32:RC_FILE = icon.rc

TRANSLATIONS = i18n_lv_LV.ts
