#-------------------------------------------------
#
# Project created by QtCreator 2010-05-03T21:45:32
#
#-------------------------------------------------

QT       += core gui network webkit

TARGET = carclient
TEMPLATE = app

SOURCES += \
  GPS_Serial.cpp \
  NMEAParser.cpp \
  PositionWGS84.cpp \
  carmainwindow.cpp \
  mapviewer.cpp \
  slicesender.cpp \
  main.cpp

HEADERS  += \
  GPS_Serial.hpp \
  carmainwindow.hpp \
  mapviewer.hpp \
  slicesender.hpp \
  NMEAParser.hpp \
  PositionWGS84.hpp

FORMS    += carmainwindow.ui

RESOURCES = ressources.qrc

CONFIG += mobility
MOBILITY = 
INCLUDEPATH += \
  "C:\Programme\Microsoft Visual Studio 9.0\VC\include" \
  "C:\Programme\Microsoft SDKs\Windows\v6.1\Include" \
  C:/Programme/boost/boost_1_42_0-msvc90/include/boost-1_42

LIBS += \
  -L"C:\Programme\Microsoft Visual Studio 9.0\VC\lib" \
  -L"C:\Programme\Microsoft SDKs\Windows\v6.1\Lib" \
  -LC:/Programme/boost/boost_1_42_0-msvc90/lib \
  -lboost_thread-vc90-mt-gd-1_42 -lboost_system-vc90-mt-gd-1_42

DEFINES = BOOST_ALL_NO_LIB _USE_MATH_DEFINES _WIN32_WINNT=0x0502 WIN32_LEAN_AND_MEAN

symbian {
    TARGET.UID3 = 0xeafe4019
    # TARGET.CAPABILITY += 
    TARGET.EPOCSTACKSIZE = 0x14000
    TARGET.EPOCHEAPSIZE = 0x020000 0x800000
}
