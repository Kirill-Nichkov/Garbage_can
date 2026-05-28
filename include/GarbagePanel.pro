QT += widgets

CONFIG += c++17
TEMPLATE = app
TARGET = garbage-panel

INCLUDEPATH += include

SOURCES += \
    src/main.cpp \
    src/garbage_bin.cpp \
    src/fill_level_widget.cpp \
    src/map_widget.cpp \
    src/overview_window.cpp \
    src/main_window.cpp

HEADERS += \
    include/garbage_bin.h \
    include/fill_level_widget.h \
    include/map_widget.h \
    include/overview_window.h \
    include/main_window.h
