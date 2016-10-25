QT += core gui xml network widgets

TARGET = FindAFlat
TEMPLATE = app

SOURCES += main.cpp\
        mainwindow.cpp \
    flat.cpp \
    filter.cpp \
    filtermodel.cpp

HEADERS  += mainwindow.h \
    flat.h \
    main.h \
    filter.h \
    filtermodel.h

FORMS    += mainwindow.ui

RESOURCES += \
    resources.qrc

win32:RC_FILE = icon.rc
