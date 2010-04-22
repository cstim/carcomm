# - Try to find Boost
# Input variables:
#  BOOST_USE_STATIC_LIBS - Windows: Use the static versions of the
#                          Boost libraries (not needed on Linux)
#
# Once done this will define
#
#  BOOST_FOUND - System has Boost
#  BOOST_INCLUDE_DIR - Boost include directory
#  BOOST_LIBRARY_DIR - The path to where the Boost library files are.
#  BOOST_VERSION - Boost version, as defined in boost/version.hpp. For 1.33.1 it is 103301.
#
#  BOOST_FILESYSTEM_LIBRARY            The Boost Filesystem libary.
#  BOOST_PROGRAM_OPTIONS_LIBRARY       The Boost Program libary.
#  BOOST_SIGNALS_LIBRARY               The Boost Signals libary.
#  BOOST_SYSTEM_LIBRARY               The Boost System libary.
#  BOOST_THREAD_LIBRARY                The Boost Thread libary.
#
# The following libraries are searched for, but currently hidden as
# "advanced" because they are not needed by the Ibeo projects:
#  BOOST_DATE_TIME_LIBRARY             The Boost Date Time libary.
#  BOOST_REGEX_LIBRARY                 The Boost Regex libary.
#  BOOST_SERIALIZATION_LIBRARY         The Boost Serialization libary.
#  BOOST_UNIT_TEST_FRAMEWORK_LIBRARY   The Boost Unit Test Framework libary.
#
# Copyright (c) 2006 Andreas Schneider <mail@cynapses.org>
#  Copyright (c) 2007 Wengo
#  Copyright (c) 2009 Christian Stimming
#
#  Redistribution and use is allowed according to the terms of the New
#  BSD license.
#  For details see the accompanying COPYING-CMAKE-SCRIPTS file.
#


OPTION (BOOST_USE_MULTITHREADED "Use the multithreaded versions of the Boost libraries" ON)

# ############################################################

MACRO (_Boost_ADJUST_LIB_VARS basename)
  IF (BOOST_${basename}_LIBRARY_DEBUG AND BOOST_${basename}_LIBRARY_RELEASE)
    # if the generator supports configuration types then set
    # optimized and debug libraries, or if the CMAKE_BUILD_TYPE has a value
    IF (CMAKE_CONFIGURATION_TYPES OR CMAKE_BUILD_TYPE)
      SET(BOOST_${basename}_LIBRARY optimized ${BOOST_${basename}_LIBRARY_RELEASE} debug ${BOOST_${basename}_LIBRARY_DEBUG})
    ELSE(CMAKE_CONFIGURATION_TYPES OR CMAKE_BUILD_TYPE)
      # if there are no configuration types and CMAKE_BUILD_TYPE has no value
      # then just use the release libraries
      SET(BOOST_${basename}_LIBRARY ${BOOST_${basename}_LIBRARY_RELEASE} )
    ENDIF(CMAKE_CONFIGURATION_TYPES OR CMAKE_BUILD_TYPE)
    GET_FILENAME_COMPONENT(BOOST_LIBRARY_DIR "${BOOST_${basename}_LIBRARY_RELEASE}" PATH)
  ENDIF (BOOST_${basename}_LIBRARY_DEBUG AND BOOST_${basename}_LIBRARY_RELEASE)

  # if only the release version was found, set the debug variable also to the release version
  IF (BOOST_${basename}_LIBRARY_RELEASE AND NOT BOOST_${basename}_LIBRARY_DEBUG)
    SET(BOOST_${basename}_LIBRARY_DEBUG ${BOOST_${basename}_LIBRARY_RELEASE})
    SET(BOOST_${basename}_LIBRARY       ${BOOST_${basename}_LIBRARY_RELEASE})
    GET_FILENAME_COMPONENT(BOOST_LIBRARY_DIR "${BOOST_${basename}_LIBRARY}" PATH)
  ENDIF (BOOST_${basename}_LIBRARY_RELEASE AND NOT BOOST_${basename}_LIBRARY_DEBUG)

  # if only the debug version was found, set the release variable also to the debug version
  IF (BOOST_${basename}_LIBRARY_DEBUG AND NOT BOOST_${basename}_LIBRARY_RELEASE)
    SET(BOOST_${basename}_LIBRARY_RELEASE ${BOOST_${basename}_LIBRARY_DEBUG})
    SET(BOOST_${basename}_LIBRARY         ${BOOST_${basename}_LIBRARY_DEBUG})
    GET_FILENAME_COMPONENT(BOOST_LIBRARY_DIR "${BOOST_${basename}_LIBRARY}" PATH)
  ENDIF (BOOST_${basename}_LIBRARY_DEBUG AND NOT BOOST_${basename}_LIBRARY_RELEASE)
  
  IF (BOOST_${basename}_LIBRARY)
    SET(BOOST_${basename}_LIBRARY ${BOOST_${basename}_LIBRARY} CACHE FILEPATH "The Boost ${basename} library")
	SET (BOOST_LIBRARY_DIR ${BOOST_LIBRARY_DIR} CACHE PATH "Path to the directory with the boost library files")
  ENDIF (BOOST_${basename}_LIBRARY)

  # Make variables changeble to the advanced user
  MARK_AS_ADVANCED(
      BOOST_${basename}_LIBRARY_RELEASE
      BOOST_${basename}_LIBRARY_DEBUG
  )
ENDMACRO (_Boost_ADJUST_LIB_VARS)

# ############################################################

if (BOOST_INCLUDE_DIR AND BOOST_LIBRARY_DIR AND BOOST_VERSION AND DEFINED BOOST_IOSTREAMS_LIBRARY)
  # in cache already
  set(BOOST_FOUND TRUE)
else (BOOST_INCLUDE_DIR AND BOOST_LIBRARY_DIR AND BOOST_VERSION AND DEFINED BOOST_IOSTREAMS_LIBRARY)

  # The list of versions which are searched for in the paths and
  # library names. These will have to be updated whenever a new Boost
  # version comes out.
  set (BOOST_VERSION_LIST
	1_42_1
	1_42
	1_41_1
	1_41
	1_40_1
	1_40
	1_39_1
	1_39_0
	1_39
	1_38
	1_37
	1_36
	1_35
	1_34_1
    1_34
    1_33_1
  )

  # Needed macros
  INCLUDE (MacroAppendForeach)

  # ############################################################
  # Look up the header include path

  if (WIN32)
    set(BOOST_INCLUDE_SEARCH_DIRS
      $ENV{BOOSTINCLUDEDIR}
	  "C:/Programme/boost"
      C:/boost/include
      /usr/include
      )
    MACRO_APPEND_FOREACH (BOOST_INCLUDE_SEARCH_DIRS
	  "C:/Programme/boost/boost_" ""
	  ${BOOST_VERSION_LIST})
  endif (WIN32)

  MACRO_APPEND_FOREACH (BOOST_PATH_SUFFIX "boost-" "" ${BOOST_VERSION_LIST})

  IF (BOOST_INCLUDE_DIR)
    # Already given? Make sure to store this value in the cache
    SET (BOOST_INCLUDE_DIR ${BOOST_INCLUDE_DIR} CACHE PATH "Path to a file")
  ELSE (BOOST_INCLUDE_DIR)

	# The header file boost/version.hpp must exist always (hopefully)
	find_path(BOOST_INCLUDE_DIR
      NAMES
      boost/version.hpp
      PATHS
      ${BOOST_INCLUDE_SEARCH_DIRS}
      PATH_SUFFIXES
      ${BOOST_PATH_SUFFIX}
	  )

  ENDIF (BOOST_INCLUDE_DIR)

  IF (BOOST_LIBRARY_DIR)
    # Make sure to store this value in the cache
	SET (BOOST_LIBRARY_DIR ${BOOST_LIBRARY_DIR} CACHE PATH "Path to the directory with the boost library files")
  ENDIF (BOOST_LIBRARY_DIR)

  # Continue only if BOOST_INCLUDE_DIR was found
  IF (BOOST_INCLUDE_DIR)

    # Look for a line like this: '#define BOOST_VERSION 103301' in version.hpp
    find_file (BOOST_INCLUDE_VERSION_HPP version.hpp PATHS ${BOOST_INCLUDE_DIR}/boost .)
    if (BOOST_INCLUDE_VERSION_HPP)
      file (READ ${BOOST_INCLUDE_VERSION_HPP} _boost_version_hpp)
    else (BOOST_INCLUDE_VERSION_HPP)
      message (SEND_ERROR "Include file boost/version.hpp was not found at BOOST_INCLUDE_DIR=${BOOST_INCLUDE_DIR}")
    endif (BOOST_INCLUDE_VERSION_HPP)
    string(REGEX REPLACE ".*\n#define BOOST_VERSION ([0-9]+).*" "\\1" BOOST_VERSION "${_boost_version_hpp}")
    STRING(REGEX REPLACE ".*#define BOOST_LIB_VERSION \"([0-9_]+)\".*" "\\1" BOOST_LIB_VERSION "${_boost_version_hpp}")

	SET (BOOST_VERSION ${BOOST_VERSION} CACHE PATH "Version number of the boost library which is installed")

    #message (STATUS "BOOST_LIB_VERSION=${BOOST_LIB_VERSION}")


	# ############################################################
	# Look up the libraries

	# We already found the headers. Maybe the libraries are under the
	# same directory but with /lib appended?
	get_filename_component (_BOOST_INCLUDE_PARENT ${BOOST_INCLUDE_DIR} PATH)
	set (BOOST_LIBRARIES_SEARCH_DIRS
	  ${BOOST_LIBRARY_DIR}
	  ${_BOOST_INCLUDE_PARENT}
	  "${_BOOST_INCLUDE_PARENT}/lib"
	  "${_BOOST_INCLUDE_PARENT}/../lib"
	  )

	# Setting some more suffixes for the library
	SET (_boost_LIB_PREFIX "")
	IF (WIN32 AND BOOST_USE_STATIC_LIBS)
      SET (_boost_LIB_PREFIX "lib")
	ENDIF (WIN32 AND BOOST_USE_STATIC_LIBS)

	IF (BOOST_USE_MULTITHREADED )
	  SET (_boost_MULTITHREADED_TAG "-mt")
	ELSE (BOOST_USE_MULTITHREADED )
      SET (_boost_MULTITHREADED_TAG "")
	ENDIF (BOOST_USE_MULTITHREADED )

	SET (_boost_COMPILER_TAG "-gcc")
	SET (_boost_STATIC_TAG "")
	SET (_boost_ABI_TAG "")
	IF (WIN32)
      IF(MSVC)
		SET (_boost_ABI_TAG "g")
      ENDIF(MSVC)
      IF( BOOST_USE_STATIC_LIBS )
		SET (_boost_STATIC_TAG "-s")
      ENDIF( BOOST_USE_STATIC_LIBS )
	ENDIF(WIN32)
	SET (_boost_ABI_TAG "${_boost_ABI_TAG}d")

	if (WIN32)
      # In windows, automatic linking is performed, so you do not have
      # to specify the libraries.
      # If you are linking to a dynamic runtime, then you can choose to
      # link to either a static or a dynamic Boost library, the default
      # is to do a static link.  You can alter this for a specific
      # library "whatever" by defining BOOST_WHATEVER_DYN_LINK to force
      # Boost library "whatever" to be linked dynamically. Alternatively
      # you can force all Boost libraries to dynamic link by defining
      # BOOST_ALL_DYN_LINK.

      # This feature can be disabled for Boost library "whatever" by
      # defining BOOST_WHATEVER_NO_LIB, or for all of Boost by defining
      # BOOST_ALL_NO_LIB.

      # If you want to observe which libraries are being linked against
      # then defining BOOST_LIB_DIAGNOSTIC will cause the auto-linking
      # code to emit a #pragma message each time a library is selected
      # for linking.
      set(BOOST_LIB_DIAGNOSTIC_DEFINITIONS "-DBOOST_LIB_DIAGNOSTIC")

      set(BOOST_LIBRARIES_SEARCH_DIRS ${BOOST_LIBRARIES_SEARCH_DIRS}
		$ENV{BOOSTLIBDIR}
		C:/boost/lib
		/usr/lib
		)
      MACRO_APPEND_FOREACH (BOOST_LIBRARIES_SEARCH_DIRS
		"C:/Programme/boost/boost_" "/lib"
		${BOOST_VERSION_LIST})

	  IF (MSVC90)
		SET (_boost_COMPILER_TAG "-vc90")
	  ELSEIF (MSVC80)
		SET (_boost_COMPILER_TAG "-vc80")
	  ELSEIF (MSVC71)
		SET (_boost_COMPILER_TAG "-vc71")
	  ENDIF(MSVC90)
	  IF (MINGW)
		EXEC_PROGRAM(${CMAKE_CXX_COMPILER}
		  ARGS -dumpversion
		  OUTPUT_VARIABLE _boost_COMPILER_VERSION
		  )
		STRING(REGEX REPLACE "([0-9])\\.([0-9])\\.[0-9]" "\\1\\2"
		  _boost_COMPILER_VERSION ${_boost_COMPILER_VERSION})
		SET (_boost_COMPILER_TAG "-mgw${_boost_COMPILER_VERSION}")
	  ENDIF(MINGW)

	endif (WIN32)


	# ############################################################

	# Actually find all libraries now
	SET (_boost_COMPONENT_LIST
	  date_time
	  filesystem
	  iostreams
	  program_options
	  regex
	  serialization
	  signals
	  system
	  thread
	  unit_test
	  )

	# Support preference of static libs by adjusting CMAKE_FIND_LIBRARY_SUFFIXES
	IF( BOOST_USE_STATIC_LIBS )
      SET( _boost_ORIG_CMAKE_FIND_LIBRARY_SUFFIXES ${CMAKE_FIND_LIBRARY_SUFFIXES})
      IF(WIN32)
		SET(CMAKE_FIND_LIBRARY_SUFFIXES .lib .a ${CMAKE_FIND_LIBRARY_SUFFIXES})
      ELSE(WIN32)
		SET(CMAKE_FIND_LIBRARY_SUFFIXES .a ${CMAKE_FIND_LIBRARY_SUFFIXES})
      ENDIF(WIN32)
	ENDIF( BOOST_USE_STATIC_LIBS )

	FOREACH (COMPONENT ${_boost_COMPONENT_LIST})
      STRING(TOUPPER ${COMPONENT} UPPERCOMPONENT)
      if (NOT BOOST_${UPPERCOMPONENT}_LIBRARY)

		find_library (BOOST_${UPPERCOMPONENT}_LIBRARY_DEBUG
          NAMES ${_boost_LIB_PREFIX}boost_${COMPONENT}${_boost_COMPILER_TAG}${_boost_MULTITHREADED_TAG}-${_boost_ABI_TAG}-${BOOST_LIB_VERSION}
               ${_boost_LIB_PREFIX}boost_${COMPONENT}${_boost_COMPILER_TAG}${_boost_MULTITHREADED_TAG}${_boost_STATIC_TAG}${_boost_ABI_TAG}-${BOOST_LIB_VERSION}
               ${_boost_LIB_PREFIX}boost_${COMPONENT}${_boost_MULTITHREADED_TAG}-${_boost_ABI_TAG}
               ${_boost_LIB_PREFIX}boost_${COMPONENT}${_boost_MULTITHREADED_TAG}${_boost_STATIC_TAG}${_boost_ABI_TAG}
               ${_boost_LIB_PREFIX}boost_${COMPONENT}-${_boost_ABI_TAG}

          HINTS ${BOOST_LIBRARIES_SEARCH_DIRS}
		  )

		find_library (BOOST_${UPPERCOMPONENT}_LIBRARY_RELEASE
          NAMES ${_boost_LIB_PREFIX}boost_${COMPONENT}${_boost_COMPILER_TAG}${_boost_MULTITHREADED_TAG}-${BOOST_LIB_VERSION}
               ${_boost_LIB_PREFIX}boost_${COMPONENT}${_boost_COMPILER_TAG}${_boost_MULTITHREADED_TAG}${_boost_STATIC_TAG}-${BOOST_LIB_VERSION}
               ${_boost_LIB_PREFIX}boost_${COMPONENT}${_boost_MULTITHREADED_TAG}
               ${_boost_LIB_PREFIX}boost_${COMPONENT}${_boost_MULTITHREADED_TAG}${_boost_STATIC_TAG}
               ${_boost_LIB_PREFIX}boost_${COMPONENT}

          HINTS ${BOOST_LIBRARIES_SEARCH_DIRS}
		  )

		_Boost_ADJUST_LIB_VARS(${UPPERCOMPONENT})

      else (NOT BOOST_${UPPERCOMPONENT}_LIBRARY)

		# This is needed if the variable was given on the cmake command
		# line so that it is cached for subsequent cmake runs.
	  
		_Boost_ADJUST_LIB_VARS(${UPPERCOMPONENT})

      endif (NOT BOOST_${UPPERCOMPONENT}_LIBRARY)

	ENDFOREACH (COMPONENT)

	IF( BOOST_USE_STATIC_LIBS )
      SET(CMAKE_FIND_LIBRARY_SUFFIXES ${_boost_ORIG_CMAKE_FIND_LIBRARY_SUFFIXES})
	ENDIF( BOOST_USE_STATIC_LIBS )

	# ############################################################

	if (BOOST_FOUND)
      if (NOT Boost_FIND_QUIETLY)
		message(STATUS "Found Boost version ${BOOST_VERSION}: ${BOOST_INCLUDE_DIR}")
      endif (NOT Boost_FIND_QUIETLY)
	else (BOOST_FOUND)
      if (Boost_FIND_REQUIRED)
		message(FATAL_ERROR "Could not find Boost")
      endif (Boost_FIND_REQUIRED)
	endif (BOOST_FOUND)

	# show this only in the advanced view
	mark_as_advanced(BOOST_USE_MULTITHREADED BOOST_VERSION)

	# Those libraries are so far unused in the Ibeo projects and also
	# don't have to be shown in the non-advanced view
	MARK_AS_ADVANCED(
	  BOOST_DATE_TIME_LIBRARY
	  BOOST_REGEX_LIBRARY
	  BOOST_SERIALIZATION_LIBRARY
	  BOOST_UNIT_TEST_FRAMEWORK_LIBRARY)

  ENDIF (BOOST_INCLUDE_DIR)

  IF (NOT BOOST_LIBRARY_DIR AND NOT BOOST_FILESYSTEM_LIBRARY)
	SET (BOOST_LIBRARY_DIR BOOST_LIBRARY_DIR-NOTFOUND CACHE PATH "Path to the directory with the boost library files")
  ENDIF (NOT BOOST_LIBRARY_DIR AND NOT BOOST_FILESYSTEM_LIBRARY)

endif (BOOST_INCLUDE_DIR AND BOOST_LIBRARY_DIR AND BOOST_VERSION AND DEFINED BOOST_IOSTREAMS_LIBRARY)

# The previous output variable was called include_dirS with a trailing
# S, so set this variable as well
IF (BOOST_INCLUDE_DIRS)
  MARK_AS_ADVANCED(BOOST_INCLUDE_DIRS)
ENDIF (BOOST_INCLUDE_DIRS)
SET (BOOST_INCLUDE_DIRS ${BOOST_INCLUDE_DIR})
