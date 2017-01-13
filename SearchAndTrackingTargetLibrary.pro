#-------------------------------------------------
#
# Project created by QtCreator 2017-01-04T13:38:01
#
#-------------------------------------------------

QT       -= gui

TARGET = SearchAndTrackingTargetLibrary
TEMPLATE = lib

DEFINES += SEARCHANDTRACKINGTARGETLIBRARY_LIBRARY

SOURCES += SearchAndTrackingTargetLibrary.cpp \
    SearchAndTrackingTarget.cpp \
    Globals.cpp

HEADERS += SearchAndTrackingTargetLibrary.h\
        searchandtrackingtargetlibrary_global.h \
    SearchAndTrackingTarget.h \
    Globals.h

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}

INCLUDEPATH+=C://OpenCV31//buildmgw//install//include
LIBS+=C://OpenCV31//buildmgw//bin//libopencv_imgproc310.dll
LIBS+=C://OpenCV31//buildmgw//bin//libopencv_core310.dll
LIBS+=C://OpenCV31//buildmgw//bin//libopencv_imgcodecs310.dll

FORMS +=
