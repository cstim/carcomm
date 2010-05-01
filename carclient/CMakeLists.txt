# Carclient project
# Copyright (C) Christian Stimming, 2010

PROJECT (carclient)   # The name of the Project
CMAKE_MINIMUM_REQUIRED (VERSION 2.6)

# Extra cmake macros
SET (CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake_modules;${CMAKE_MODULE_PATH}")

SET (QT_USE_QTWEBKIT true) # enable QtWebKit, available in qt >= 4.4.0
SET (QT_USE_QTNETWORK true) # enable QtNetwork, available in qt >= 4.4.0
SET (QT_MIN_VERSION "4.5.0") # We need at least 4.5.0 (because only this is LGPL)
find_package (Qt4 REQUIRED) # find and setup Qt4 for this project
include_directories (${QT_INCLUDES})
include_directories (${CMAKE_CURRENT_BINARY_DIR})

# Set up include and link directories
include(${QT_USE_FILE})

IF (UNIX OR MINGW)
  SET( CMAKE_CXX_FLAGS "-Wall -Werror ${CMAKE_CXX_FLAGS}")
ENDIF (UNIX OR MINGW)

SET (carclient_SOURCES
  GPS_Serial.cpp
  NMEAParser.cpp
  PositionWGS84.cpp
  carmainwindow.cpp
  main.cpp
  slicesender.cpp
  )
SET (carclient_QOBJECT_HEADERS
  GPS_Serial.hpp
  carmainwindow.hpp
  slicesender.hpp
  )
SET (carclient_HEADERS ${carclient_QOBJECT_HEADERS}
  NMEAParser.hpp
  PositionWGS84.hpp
  )
SET (carclient_FORMS
  carmainwindow.ui
  )
SET (carclient_RESOURCES
  ressources.qrc
  )

# Generation of the ui source files
QT4_WRAP_UI (carclient_FORMS_HEADERS ${carclient_FORMS})

# Generation of the foo_MOC_SRCS = moc_Class1.cxx moc_Class2.cxx
QT4_WRAP_CPP (carclient_MOC_SOURCES ${carclient_QOBJECT_HEADERS})

# Generation of resource files
QT4_ADD_RESOURCES (carclient_RESOURCE ${carclient_RESOURCES})

ADD_EXECUTABLE (carclient ${win32_option}
  ${carclient_SOURCES}
  ${carclient_HEADERS}
  ${carclient_MOC_SOURCES}
  ${carclient_FORMS_HEADERS}
  ${carclient_RESOURCE}
  )
INSTALL (TARGETS carclient RUNTIME DESTINATION bin)

# Link against QT
TARGET_LINK_LIBRARIES (carclient ${QT_LIBRARIES})
#TARGET_LINK_LIBRARIES (rlaserview ${QT_QTWEBKIT_LIBRARY})

# Link against qtmain as well
IF(MSVC)
  target_link_libraries(carclient ${QT_QTMAIN_LIBRARY})
ENDIF(MSVC)

if (MINGW)
  SET_TARGET_PROPERTIES(carclient PROPERTIES
    LINK_FLAGS -Wl,-enable-runtime-pseudo-reloc )
endif (MINGW)

# ############################################################
# Boost

if (MSVC)
  # MSVC should not link against boost-regex and boost-date_time.
  ADD_DEFINITIONS( -DBOOST_ALL_NO_LIB )
  ADD_DEFINITIONS( -D_USE_MATH_DEFINES )

  # Use the boost libraries in static version, but only if this is
  # purely a static build or we are on windows/msvc.
  SET (Boost_USE_STATIC_LIBS ON)
endif (MSVC)

if (WIN32)
  # Boost-1.37.0 needs a specific version here
  ADD_DEFINITIONS (-D_WIN32_WINNT=0x0502)	# 0x0502 sets the minimum Windows Version to "Windows XP with SP2 / Windows Server 2003 with SP1"
  ADD_DEFINITIONS (-DWIN32_LEAN_AND_MEAN)
endif (WIN32)

# Check for boost, using our local cmake_modules/FindBoost.cmake file.
FIND_PACKAGE (Boost)
INCLUDE_DIRECTORIES (${Boost_INCLUDE_DIRS})
INCLUDE_DIRECTORIES (${BOOST_INCLUDE_DIR})

IF (BOOST_VERSION LESS 103600)
  MESSAGE (SEND_ERROR "Boost is too old - boost 1.36.0 is needed, but ${BOOST_VERSION} was found")
ENDIF (BOOST_VERSION LESS 103600)
IF (NOT BOOST_THREAD_LIBRARY)
  MESSAGE(SEND_ERROR "*** boost_thread not found, but is required.")
ENDIF (NOT BOOST_THREAD_LIBRARY)

SET (Boost_LIBRARIES ${BOOST_THREAD_LIBRARY} ${BOOST_SYSTEM_LIBRARY})

TARGET_LINK_LIBRARIES(carclient ${Boost_LIBRARIES})

IF (WIN32)
  # Find the wsock2 DLL for Windows
  #FIND_LIBRARY (WSOCK2_DLL ws2_32)
  # I have no idea why the above doesn't work!?!
  SET (WSOCK2_DLL "ws2_32")
  SET (MSWSOCK_DLL "mswsock")
  #TARGET_LINK_LIBRARIES(carclient ${WSOCK2_DLL} ${MSWSOCK_DLL})
ENDIF (WIN32)

# ############################################################
# Package creation rules

SET (CPACK_PACKAGE_NAME "Carclient")
SET (CPACK_PACKAGE_DESCRIPTION_SUMMARY "C2C Car Information Laptop Client")
SET (CPACK_PACKAGE_VENDOR "Christian Stimming")
SET (CPACK_PACKAGE_VERSION_MAJOR "1")
SET (CPACK_PACKAGE_VERSION_MINOR "0")
SET (CPACK_PACKAGE_VERSION_PATCH "0")
SET (CPACK_SOURCE_IGNORE_FILES "/\\\\.svn/;/\\\\.git/;.*~;build.*;html;Debug;Release")
SET (CPACK_STRIP_FILES "bin/carclient")
#SET (CPACK_PACKAGE_EXECUTABLES "bin/carclient")
IF(UNIX)
  SET(CPACK_GENERATOR "TGZ")
ELSE(UNIX)
  SET(CPACK_GENERATOR "NSIS")
ENDIF(UNIX)

SET(__cpack_system_name ${CMAKE_SYSTEM_NAME})
IF(${__cpack_system_name} MATCHES Windows)
  IF(CMAKE_CL_64)
    SET(__cpack_system_name win64)
  ELSE(CMAKE_CL_64)
    SET(__cpack_system_name win32)
  ENDIF(CMAKE_CL_64)
ENDIF(${__cpack_system_name} MATCHES Windows)
IF (MSVC80)
  SET (CPACK_SYSTEM_NAME "${__cpack_system_name}-msvc80")
ENDIF (MSVC80)
IF (MSVC90)
  SET (CPACK_SYSTEM_NAME "${__cpack_system_name}-msvc90")
  # Install also the Visual Studio Redistributable package
  INSTALL (FILES ${CMAKE_CURRENT_SOURCE_DIR}/res/vcredist_x86_9.0.21022_eng.exe DESTINATION .)
  SET(CPACK_NSIS_EXTRA_INSTALL_COMMANDS "ExecWait '\\\"$INSTDIR\\\\vcredist_x86_9.0.21022_eng.exe\\\" /q'")
ENDIF (MSVC90)
IF (MINGW)
  SET (CPACK_SYSTEM_NAME "${__cpack_system_name}-mingw")
ENDIF (MINGW)

INCLUDE (CPack)

# Also install the dependencies
IF (WIN32)
  # Make sure to have the DLLs instead of the LIBs.
  # QtWebKit requires QtNetwork and phonon4 as well.
  GET_FILENAME_COMPONENT (qtbindir ${QT_QMAKE_EXECUTABLE} PATH)

  # The non-optional parts
  FOREACH (_lib QtCore QtGui QtWebKit QtNetwork)
    SET (_libdebug pkg_${_lib}d4_dll)
    FIND_FILE (${_libdebug} "${_lib}d4.dll" ${qtbindir} NO_DEFAULT_PATH)
    FIND_FILE (${_libdebug} "${_lib}d4.dll")
    INSTALL (FILES ${${_libdebug}}
      DESTINATION bin
      CONFIGURATIONS Debug)

    SET (_librel pkg_${_lib}4_dll)
    FIND_FILE (${_librel} "${_lib}4.dll" ${qtbindir} NO_DEFAULT_PATH)
    FIND_FILE (${_librel} "${_lib}4.dll")
    INSTALL (FILES ${${_librel}}
      DESTINATION bin
      CONFIGURATIONS Release RelWithDebInfo MinSizeRel)
  ENDFOREACH (_lib)

  # And the qt >= 4.6.0 parts
  IF (${QTVERSION} VERSION_GREATER 4.5.99)
    FOREACH (_lib QtXmlPatterns phonon)
      SET (_libdebug pkg_${_lib}d4_dll)
      FIND_FILE (${_libdebug} "${_lib}d4.dll" ${qtbindir} NO_DEFAULT_PATH)
      FIND_FILE (${_libdebug} "${_lib}d4.dll")
      INSTALL (FILES ${${_libdebug}}
	DESTINATION bin
	CONFIGURATIONS Debug)

      SET (_librel pkg_${_lib}4_dll)
      FIND_FILE (${_librel} "${_lib}4.dll" ${qtbindir} NO_DEFAULT_PATH)
      FIND_FILE (${_librel} "${_lib}4.dll")
      INSTALL (FILES ${${_librel}}
	DESTINATION bin
	CONFIGURATIONS Release RelWithDebInfo MinSizeRel)
    ENDFOREACH (_lib)
  ENDIF (${QTVERSION} VERSION_GREATER 4.5.99)

  If (MSVC)
    STRING (REPLACE ".lib" ".dll" install_bthread_dll "${BOOST_THREAD_LIBRARY_RELEASE}")
    STRING (REPLACE ".LIB" ".DLL" install_bthread_dll "${install_bthread_dll}")
    STRING (REPLACE "libboost" "boost" install_bthread_dll "${install_bthread_dll}")
    STRING (REPLACE ".lib" ".dll" install_bsystem_dll "${BOOST_SYSTEM_LIBRARY_RELEASE}")
    STRING (REPLACE ".LIB" ".DLL" install_bsystem_dll "${install_bsystem_dll}")
    STRING (REPLACE "libboost" "boost" install_bsystem_dll "${install_bsystem_dll}")
    INSTALL (FILES ${install_bthread_dll} ${install_bsystem_dll}
      DESTINATION bin)
  ENDIf (MSVC)

  IF (MINGW)
    FIND_LIBRARY (mingwm10_dll NAMES mingwm10)
    INSTALL (FILES ${mingwm10_dll} DESTINATION bin)
    #INSTALL (FILES ${zlib1_dll} DESTINATION bin)
  ENDIF (MINGW)
ENDIF (WIN32)
