PROJECT (carclient)   # The name of the Project
CMAKE_MINIMUM_REQUIRED (VERSION 2.6)

SET (QT_USE_QTWEBKIT true) # enable QtWebKit, available in qt >= 4.4.0
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
  main.cpp
)
SET (carclient_QOBJECT_HEADERS
  carmainwindow.hpp
)
SET (carclient_HEADERS ${carclient_QOBJECT_HEADERS}
)
SET (carclient_FORMS
  mainwindow.ui
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
