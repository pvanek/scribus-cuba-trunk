##############################################################################################################
########## CMake Setup                                                                              ##########

#Set our CMake minimum version
#Require 2.4.2 for Qt finding
#Require 2.4.3 for moc change detection and rpath updates for custom cairo install
CMAKE_MINIMUM_REQUIRED(VERSION 2.6.0 FATAL_ERROR)

option(WANT_PIN_CUBA "Custom build for PeopleInNeed.cz" FALSE)

#IF(WIN32)
#  IF (${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION} LESS 2.6)
#    MESSAGE(FATAL_ERROR "Windows builds require CMake 2.6")
#  ENDIF (${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION} LESS 2.6)
#ENDIF(WIN32)

#Pretty colors
SET(CMAKE_COLOR_MAKEFILE ON)
#Dont force verbose
SET(CMAKE_VERBOSE_MAKEFILE OFF)
#Include current dir
SET(CMAKE_INCLUDE_CURRENT_DIR TRUE)
#RPATH setup - more below too
IF (WANT_NORPATH OR WANT_DISTROBUILD)
  SET(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
ELSE (WANT_NORPATH OR WANT_DISTROBUILD)
  SET(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
ENDIF (WANT_NORPATH OR WANT_DISTROBUILD)
SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
SET(CMAKE_SKIP_RULE_DEPENDENCY TRUE)
SET(CMAKE_SKIP_BUILD_RPATH TRUE)

INCLUDE(CheckIncludeFile)
INCLUDE(CheckCCompilerFlag)
INCLUDE(CheckCXXCompilerFlag)
INCLUDE(CheckTypeSize)
INCLUDE(TestBigEndian)

# custom build for PeopleInNeed.cz
if (WANT_PIN_CUBA)
    add_definitions(-DWANT_PIN_CUBA)
    message(STATUS "")
    message(STATUS "Custom build for PeopleInNeed.cz is ON")
    message(STATUS "")
endif (WANT_PIN_CUBA)

#Set the custom CMake module directory where our include/lib finders are
SET(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules")

##############################################################################################################
########## toplevel compiler flags                                                                  ##########
MESSAGE(STATUS "Shared Library Flags: ${CMAKE_SHARED_LIBRARY_C_FLAGS}")
# CHECK_C_COMPILER_FLAG(" ${CMAKE_SHARED_LIBRARY_C_FLAGS}" C_HAVE_PIC)
# CHECK_CXX_COMPILER_FLAG(" ${CMAKE_SHARED_LIBRARY_C_FLAGS}" CXX_HAVE_PIC)
# IF (CXX_HAVE_PIC AND C_HAVE_PIC)
#     SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${CMAKE_SHARED_LIBRARY_C_FLAGS}")
#     SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CMAKE_SHARED_LIBRARY_C_FLAGS}")
# ELSE (CXX_HAVE_PIC AND C_HAVE_PIC)
#     MESSAGE(FATAL_ERROR "Shared library support is missing")
# ENDIF (CXX_HAVE_PIC AND C_HAVE_PIC)

# SET(CMAKE_SHARED_MODULE_LINK_FLAGS "${CMAKE_SHARED_MODULE_LINK_FLAGS} -export-dynamic")
# SET(CMAKE_SHARED_MODULE_LINK_CXX_FLAGS "${CMAKE_SHARED_MODULE_LINK_CXX_FLAGS} -export-dynamic")
# SET(CMAKE_SHARED_LIBRARY_C_FLAGS "${CMAKE_SHARED_LIBRARY_C_FLAGS} -export-dynamic")
# SET(CMAKE_SHARED_LIBRARY_CXX_FLAGS "${CMAKE_SHARED_LIBRARY_CXX_FLAGS} -export-dynamic")
# MESSAGE(STATUS ${CMAKE_SHARED_MODULE_LINK_FLAGS})
# MESSAGE(STATUS ${CMAKE_SHARED_MODULE_LINK_CXX_FLAGS})
# MESSAGE(STATUS ${CMAKE_SHARED_LIBRARY_C_FLAGS})
# MESSAGE(STATUS ${CMAKE_SHARED_LIBRARY_CXX_FLAGS})
# MESSAGE(STATUS ${CMAKE_CXX_CREATE_SHARED_MODULE})
# MESSAGE(STATUS ${CMAKE_CXX_CREATE_SHARED_LIBRARY})

#IMPORT FROM COMMANDLINE
# SET with cmake -DWANT_CAIRO=1, etc
# MESSAGE(STATUS ${WANT_CAIRO})
# MESSAGE(STATUS ${WANT_LIBART})
# MESSAGE(STATUS ${WANT_DEBUG})

# SET(WANT_CAIRO 0)
# SET(WANT_LIBART 1)

#Project Setup
PROJECT(scribus)
#Set our version values
#Final version is ${VERSION} = ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}.${VERSION_SUFFIX}
#where VERSION_SUFFIX is of the form "", "svn" or "Nsvn" (N being the minor patch level)
SET (VERSION_MAJOR "1")
SET (VERSION_MINOR "5")
SET (VERSION_PATCH "0")
SET (VERSION_SUFFIX "svn")
SET (VERSION ${VERSION_MAJOR})
IF (VERSION_MINOR GREATER -1)
  SET (VERSION ${VERSION}.${VERSION_MINOR})
ENDIF (VERSION_MINOR GREATER -1)
IF (VERSION_PATCH GREATER -1)
  SET (VERSION ${VERSION}.${VERSION_PATCH})
ENDIF (VERSION_PATCH GREATER -1)
IF (VERSION_SUFFIX)
  SET (VERSION ${VERSION}.${VERSION_SUFFIX})
ENDIF (VERSION_SUFFIX)
MESSAGE(STATUS "Scribus ${VERSION} will be built to install into ${CMAKE_INSTALL_PREFIX}")

#Set the permissions to be used when installing plugins
SET(PLUGIN_PERMISSIONS WORLD_EXECUTE GROUP_EXECUTE OWNER_EXECUTE WORLD_READ GROUP_READ OWNER_READ OWNER_WRITE)

#Our main directory is scribus
SET(MAIN_DIR_NAME "scribus")

##############################################################################################################
########## check for the CPU we build for                                                           ##########

EXECUTE_PROCESS(
    COMMAND ${CMAKE_C_COMPILER} -dumpmachine
    OUTPUT_VARIABLE MACHINE
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
MESSAGE(STATUS  "Machine: ${MACHINE}, void pointer size: ${CMAKE_SIZEOF_VOID_P}")
STRING(REGEX MATCH "(i?86-*)|(athlon-*)|(pentium-*)" _machine_x86 "${MACHINE}")
IF (_machine_x86)
    MESSAGE(STATUS "Found target X86")
    SET(ARCH_X86 1)
ENDIF (_machine_x86)
IF (APPLE AND CMAKE_SIZEOF_VOID_P EQUAL 8 AND ARCH_X86 EQUAL 1)
  MESSAGE(STATUS "Found Snow Leopard Target: Apple, 64 bit, X86")
  UNSET(ARCH_X86)
  SET(ARCH_X86_64 1)
  SET(APPLE_10_6_X 1 CACHE TYPE BOOL)
ENDIF (APPLE AND CMAKE_SIZEOF_VOID_P EQUAL 8 AND ARCH_X86 EQUAL 1)

STRING(REGEX MATCH "(x86_64-*)|(X86_64-*)|(AMD64-*)|(amd64-*)" _machine_x86_64 "${MACHINE}")
IF (_machine_x86_64)
    MESSAGE(STATUS "Found target X86_64")
    SET(ARCH_X86_64 1)
ENDIF (_machine_x86_64)

STRING(REGEX MATCH "(sparc64-*)|(SPARC64-*)" _machine_sparc_64 "${MACHINE}")
IF (_machine_sparc_64)
    MESSAGE(STATUS "Found target SPARC 64")
    SET(ARCH_SPARC_64 1)
ENDIF (_machine_sparc_64)

STRING(REGEX MATCH "(mips64-*)|(MIPS64-*)" _machine_mips_64 "${MACHINE}")
IF (_machine_mips_64)
    MESSAGE(STATUS "Found target MIPS 64")
    SET(ARCH_MIPS_64 1)
ENDIF (_machine_mips_64)

STRING(REGEX MATCH "(ppc-*)|(powerpc-*)" _machine_ppc "${MACHINE}")
IF (_machine_ppc)
    MESSAGE(STATUS "Found target PPC")
    SET(ARCH_PPC 1)
ENDIF (_machine_ppc)

STRING(REGEX MATCH "(ppc64-*)|(PPC64-*)|(powerpc64-*)" _machine_ppc_64 "${MACHINE}")
IF (_machine_ppc_64)
    MESSAGE(STATUS "Found target PPC64")
    SET(ARCH_PPC_64 1)
ENDIF (_machine_ppc_64)

STRING(REGEX MATCH "(sparc-*)" _machine_sparc "${MACHINE}")
IF (_machine_sparc)
    MESSAGE(STATUS "Found target Sparc")
    SET(ARCH_SPARC 1)
ENDIF (_machine_sparc)

STRING(REGEX MATCH "(sparcv9-*)" _machine_sparcv9 "${MACHINE}")
IF (_machine_sparcv9)
    MESSAGE(STATUS "Found target Sparc v9")
    SET(ARCH_SPARCV9 1)
ENDIF (_machine_sparcv9)

STRING(REGEX MATCH "(sparc64-*)" _machine_sparc64 "${MACHINE}")
IF (_machine_sparc64)
    MESSAGE(STATUS "Found target Sparc64")
    SET(ARCH_SPARC64 1)
    SET(ARCH64BIT 1)
ENDIF (_machine_sparc64)

#convert any 64 bit build into generic 64 tag for below
IF (ARCH_X86_64 OR ARCH_SPARC_64 OR ARCH_MIPS_64 OR ARCH_PPC_64) 
  SET(ARCH64BIT 1)
ENDIF (ARCH_X86_64 OR ARCH_SPARC_64 OR ARCH_MIPS_64 OR ARCH_PPC_64) 

MESSAGE(STATUS "Building for target ${MACHINE}")

##############################################################################################################
########## Versioning Setup                                                                         ##########

#On Apple, we ignore the versioning tag so all items are "scribus" not "scribus-version"
IF (NOT BUILD_OSX_BUNDLE)
  SET(BUILD_OSX_BUNDLE ${APPLE} CACHE TYPE BOOL)
ENDIF (NOT BUILD_OSX_BUNDLE)

#Simplify future conditionals for Apple
IF (APPLE AND BUILD_OSX_BUNDLE)
  SET(APPLEBUNDLE TRUE CACHE TYPE BOOL)
ENDIF (APPLE AND BUILD_OSX_BUNDLE)

#Announce we cached a version request before, overridden below for OSX
IF(TAG_VERSION)
  IF (NOT APPLEBUNDLE)
    MESSAGE(STATUS "Previously selected version tag: ${TAG_VERSION}")
  ENDIF (NOT APPLEBUNDLE)
  SET(WANT_VERSIONING 1)
  SET(CACHED_VERSIONING 1)
ENDIF(TAG_VERSION)

#Remove version tags on OSX so our bundle is Scribus.app
IF (APPLEBUNDLE)
  IF (WANT_VERSIONING OR CACHED_VERSIONING)
    MESSAGE(STATUS "Ignoring version tag on OSX when building bundle")
    SET(WANT_VERSIONING 0)
    SET(CACHED_VERSIONING 0)
  ENDIF (WANT_VERSIONING OR CACHED_VERSIONING)
ENDIF (APPLEBUNDLE)

#Store our version string if require
IF (WANT_VERSIONING AND NOT CACHED_VERSIONING)
  IF (NOT CUSTOM_VERSIONTAG)
    SET(TAG_VERSION "-${VERSION}" CACHE TYPE STRING)
  ELSE (NOT CUSTOM_VERSIONTAG)
    SET(TAG_VERSION ${CUSTOM_VERSIONTAG} CACHE TYPE STRING)
  ENDIF (NOT CUSTOM_VERSIONTAG)
ENDIF (WANT_VERSIONING AND NOT CACHED_VERSIONING)

##############################################################################################################
########## Install/Directory Setup                                                                  ##########

IF (APPLEBUNDLE)
  IF ("${CMAKE_INSTALL_PREFIX}" MATCHES "[.]app/Contents/?$")
    # all ok
  ELSE ("${CMAKE_INSTALL_PREFIX}" MATCHES "[.]app/Contents/?$")
    IF ("${CMAKE_INSTALL_PREFIX}" MATCHES "[.]app/?$")
      # add "/Contents" subpath
      SET(CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}/Contents")
    ELSE ("${CMAKE_INSTALL_PREFIX}" MATCHES "[.]app/?$")
      # no bundle name spcified, use appname and version
      SET(CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}/Scribus${VERSION}.app/Contents")
    ENDIF ("${CMAKE_INSTALL_PREFIX}" MATCHES "[.]app/?$")
  ENDIF ("${CMAKE_INSTALL_PREFIX}" MATCHES "[.]app/Contents/?$")

  MESSAGE(STATUS "APPLEBUNDLE install prefix: ${CMAKE_INSTALL_PREFIX}")
ENDIF (APPLEBUNDLE)


#Setp all the directories we will use
#MAN
CMAKE_POLICY(SET CMP0005 OLD)
SET(MANDIR "share/man/")
IF(WANT_VERSIONING)
    SET(SHAREDIR "share/${MAIN_DIR_NAME}${TAG_VERSION}/")
ELSE(WANT_VERSIONING)
    SET(SHAREDIR "share/${MAIN_DIR_NAME}/")
ENDIF(WANT_VERSIONING)
ADD_DEFINITIONS("-DSHAREDIR=\\\"${CMAKE_INSTALL_PREFIX}/${SHAREDIR}\\\"")

#SHARE - use the default on Apple as TAG_VERSION is empty
IF(WIN32 OR OS2)
  SET(DOCDIR "share/doc/")
ELSE (WIN32 OR OS2)
  IF(TAG_VERSION OR BUILD_OSX_BUNDLE)
    SET(DOCDIR "share/doc/${MAIN_DIR_NAME}${TAG_VERSION}/")
  ELSE(TAG_VERSION OR BUILD_OSX_BUNDLE)
        IF(NOT WANT_VERSIONING)
            SET(DOCDIR "share/doc/${MAIN_DIR_NAME}/")
        ELSE(NOT WANT_VERSIONING)
            SET(DOCDIR "share/doc/${MAIN_DIR_NAME}-${VERSION}/")
        ENDIF(NOT WANT_VERSIONING)
  ENDIF (TAG_VERSION OR BUILD_OSX_BUNDLE)
ENDIF (WIN32 OR OS2)
ADD_DEFINITIONS("-DDOCDIR=\\\"${CMAKE_INSTALL_PREFIX}/${DOCDIR}\\\"")

#ICON
IF(WIN32)
  SET(ICONDIR "share/icons/")
ELSE(WIN32)
  SET(ICONDIR "share/${MAIN_DIR_NAME}${TAG_VERSION}/icons/")
ENDIF(WIN32)
ADD_DEFINITIONS("-DICONDIR=\\\"${CMAKE_INSTALL_PREFIX}/${ICONDIR}\\\"")

#SAMPLES
SET(SAMPLESDIR "share/${MAIN_DIR_NAME}${TAG_VERSION}/samples/")
ADD_DEFINITIONS("-DSAMPLESDIR=\\\"${CMAKE_INSTALL_PREFIX}/${SAMPLESDIR}\\\"")

#SCRIPTS
SET(SCRIPTSDIR "share/${MAIN_DIR_NAME}${TAG_VERSION}/scripts/")
ADD_DEFINITIONS("-DSCRIPTSDIR=\\\"${CMAKE_INSTALL_PREFIX}/${SCRIPTSDIR}\\\"")

#TEMPLATES
IF(WIN32)
  SET(TEMPLATEDIR "share/templates/")
ELSE(WIN32)
  SET(TEMPLATEDIR "share/${MAIN_DIR_NAME}${TAG_VERSION}/templates/")
ENDIF(WIN32)
ADD_DEFINITIONS("-DTEMPLATEDIR=\\\"${CMAKE_INSTALL_PREFIX}/${TEMPLATEDIR}\\\"")

#DESKTOP
SET(DESKTOPDIR "share/applications/")
ADD_DEFINITIONS("-DDESKTOPDIR=\\\"${CMAKE_INSTALL_PREFIX}/${DESKTOPDIR}\\\"")

# APPLICATION_DATA_DIR
# It allows specify the user configuration directory in the compile time.
# Example: I want to use special ~/.scribus135svn config directory for my trunk
#          cmake -DAPPLICATION_DATA_DIR=.scribus135svn ...
IF (APPLICATION_DATA_DIR)
	MESSAGE(STATUS "Using ApplicationDataDir ${APPLICATION_DATA_DIR} instead of the platform default.")
    ADD_DEFINITIONS("-DAPPLICATION_DATA_DIR=\\\"${APPLICATION_DATA_DIR}\\\"")
ELSE (APPLICATION_DATA_DIR)
    MESSAGE(STATUS "Using standard ApplicationDataDir. You can change it with -DAPPLICATION_DATA_DIR")
ENDIF (APPLICATION_DATA_DIR)

#LIB LOCATION - lib or lib64 normally
#LIB
IF(WANT_LIB64)
  IF(NOT ARCH_X86_64)
    MESSAGE(STATUS "WARNING: BUILDING 64 Bit build on non 64 bit architecture")
  ENDIF(NOT ARCH_X86_64)
  SET(LIB_SUFFIX "64" CACHE TYPE STRING)
ENDIF(WANT_LIB64)
SET(LIB_DIR_NAME "lib${LIB_SUFFIX}")
SET(LIBDIR "${LIB_DIR_NAME}/${MAIN_DIR_NAME}${TAG_VERSION}/")
ADD_DEFINITIONS("-DLIBDIR=\\\"${CMAKE_INSTALL_PREFIX}/${LIBDIR}\\\"")

#PLUGINS
IF(WIN32)
  SET(PLUGINDIR "plugins")
ELSE(WIN32)
  SET(PLUGINDIR "${LIB_DIR_NAME}/${MAIN_DIR_NAME}${TAG_VERSION}/plugins/")
ENDIF(WIN32)
ADD_DEFINITIONS("-DPLUGINDIR=\\\"${CMAKE_INSTALL_PREFIX}/${PLUGINDIR}\\\"")

#INCLUDE
SET(INCLUDEDIR "include/${MAIN_DIR_NAME}${TAG_VERSION}/")
#NO ADD DEFINITIONS, WE ONLY INSTALL WITH IT

#RPATH
IF (WANT_NORPATH OR WANT_DISTROBUILD)
  set(CMAKE_INSTALL_RPATH "")
ELSE (WANT_NORPATH OR WANT_DISTROBUILD)
  set(CMAKE_INSTALL_RPATH ${PLUGINDIR})
ENDIF (WANT_NORPATH OR WANT_DISTROBUILD)

# Set the XDG mime stuff
SET(MIMEDIR "share/mime/packages/")
SET(MIMELNKDIR "share/mimelnk/application/")

##############################################################################################################
########## Build Setup                                                                              ##########

#Convert our simpler command line option to the CMake style
#None, Debug, Release, .. or custom ones
# SET(WANT_DEBUG 1)
IF(WANT_DEBUG)
  SET(CMAKE_BUILD_TYPE Debug)
ENDIF(WANT_DEBUG)
IF (WANT_RELEASEWITHDEBUG)
  SET(CMAKE_BUILD_TYPE RelWithDebInfo)
ENDIF (WANT_RELEASEWITHDEBUG)
IF(NOT WANT_DEBUG AND NOT WANT_RELEASEWITHDEBUG)
  SET(CMAKE_BUILD_TYPE RELEASE)
ENDIF(NOT WANT_DEBUG AND NOT WANT_RELEASEWITHDEBUG)



#Based on our build type, setup our build options
IF(APPLE)
  IF(APPLE_10_6_X)
    SET(OSXMINVER "10.6" CACHE TYPE STRING)
  ELSE(APPLE_10_6_X)
    SET(OSXMINVER "10.4" CACHE TYPE STRING)
  ENDIF(APPLE_10_6_X)
  # gcc/mac
  IF(WANT_UNIVERSAL_BUNDLE)
    MESSAGE("Setting Universal (PPC/i386) Architecture for OSX Build/Bundle")
    SET(CMAKE_OSX_ARCHITECTURES "ppc;i386" )
    SET(CMAKE_TRY_COMPILE_OSX_ARCHITECTURES "ppc;i386" )
    SET(CMAKE_OSX_SYSROOT "/Developer/SDKs/MacOSX10.4u.sdk")
	SET(CMAKE_OSX_DEPLOYMENT_TARGET "10.4")
  ELSE(WANT_UNIVERSAL_BUNDLE)
    IF(ARCH_PPC OR ARCH_PPC_64)
      MESSAGE("Setting PPC Architecture for OSX Build/Bundle")
      SET(CMAKE_OSX_ARCHITECTURES "ppc" )
      SET(CMAKE_TRY_COMPILE_OSX_ARCHITECTURES "ppc" )
	ELSE(ARCH_PPC OR ARCH_PPC_64)
	  IF(APPLE_10_6_X)
		MESSAGE("Setting x86_64 Architecture for OSX Build/Bundle")
		SET(CMAKE_OSX_ARCHITECTURES "x86_64" )
		SET(CMAKE_TRY_COMPILE_OSX_ARCHITECTURES "x86_64" )
	  ELSE(APPLE_10_6_X)
		MESSAGE("Setting i386 Architecture for OSX Build/Bundle")
		SET(CMAKE_OSX_ARCHITECTURES "i386" )
		SET(CMAKE_TRY_COMPILE_OSX_ARCHITECTURES "i386" )
	  ENDIF(APPLE_10_6_X)
    ENDIF(ARCH_PPC OR ARCH_PPC_64)
  ENDIF(WANT_UNIVERSAL_BUNDLE)
  MESSAGE(STATUS "${CMAKE_OSX_ARCHITECTURES} ${CMAKE_OSX_SYSROOT}")
  SET(ENV{MACOSX_DEPLOYMENT_TARGET} ${OSXMINVER} )
  SET( ENV{LD_PREBIND} 1 )
  SET( ENV{LD_PREBIND_ALLOW_OVERLAP} 1 )
  IF (WANT_DEBUG)
	SET(CMAKE_CXX_FLAGS_DEBUG "-O0 -g3 -fno-inline-functions -Wall -mmacosx-version-min=${OSXMINVER}")
	SET(CMAKE_C_FLAGS_DEBUG "-O0 -g3 -fno-inline-functions -Wall -mmacosx-version-min=${OSXMINVER}")
  ENDIF (WANT_DEBUG)
  IF (WANT_RELEASEWITHDEBUG)
	SET(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -g3 -fno-inline-functions -Wall -mmacosx-version-min=${OSXMINVER}")
	SET(CMAKE_C_FLAGS_RELWITHDEBINFO "-O2 -g3 -fno-inline-functions -Wall -mmacosx-version-min=${OSXMINVER}")
  ENDIF (WANT_RELEASEWITHDEBUG)
  IF (NOT WANT_DEBUG AND NOT WANT_RELEASEWITHDEBUG)
	SET(CMAKE_CXX_FLAGS_RELEASE "-O2 -fno-inline-functions -Wall -mmacosx-version-min=${OSXMINVER}")
	SET(CMAKE_C_FLAGS_RELEASE "-O2 -fno-inline-functions -Wall -mmacosx-version-min=${OSXMINVER}")
  ENDIF (NOT WANT_DEBUG AND NOT WANT_RELEASEWITHDEBUG)
ELSE(APPLE)
  IF(${CMAKE_GENERATOR} MATCHES "^(Visual Studio|NMake).*")
    # Windows build with Visual Studio
    # Surely there's a better way to identify the compiler?
    SET(CMAKE_CXX_FLAGS_DEBUG)
    SET(CMAKE_C_FLAGS_DEBUG)
  ELSE(${CMAKE_GENERATOR} MATCHES "^(Visual Studio|NMake).*")
    # vanilla gcc
    SET(CMAKE_CXX_FLAGS_DEBUG "-O0 -g3 -Wall -fstrict-aliasing")
    SET(CMAKE_C_FLAGS_DEBUG "-O0 -g3 -Wall -fstrict-aliasing")
    
    IF (_machine_x86 OR _machine_x86_64)
      SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fPIC")
      SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -fPIC")
    ENDIF (_machine_x86 OR _machine_x86_64)
    
  ENDIF(${CMAKE_GENERATOR} MATCHES "^(Visual Studio|NMake).*")
ENDIF(APPLE)

IF(${CMAKE_GENERATOR} MATCHES "^(Visual Studio|NMake).*")
  SET(CMAKE_CXX_FLAGS_RELEASE)
  SET(CMAKE_C_FLAGS_RELEASE)
ELSE(${CMAKE_GENERATOR} MATCHES "^(Visual Studio|NMake).*")
  SET(CMAKE_CXX_FLAGS_RELEASE "-O2 -Wall -fPIC")
  SET(CMAKE_C_FLAGS_RELEASE "-O2 -Wall -fPIC")
  ADD_DEFINITIONS(-DNDEBUG)
ENDIF(${CMAKE_GENERATOR} MATCHES "^(Visual Studio|NMake).*")

IF(WIN32)
  ADD_DEFINITIONS(-DWIN32_LEAN_AND_MEAN
                  -DAVOID_WIN32_FILEIO
                  -D_CRT_SECURE_NO_DEPRECATE
                  -DCOMPILE_PLUGIN_AS_DLL
  )
  SET(BUILD_WINDOWS_VERSION 1)
  SET(SC_USE_GDI 1)
  IF(NOT SC_SHOULD_NOT_USE_GDIPLUS)
    SET(SC_USE_GDIPLUS 1)
  ENDIF(NOT SC_SHOULD_NOT_USE_GDIPLUS)
  SET(DLL_USE_NATIVE_API 1)
ENDIF(WIN32)

##############################################################################################################
########## Find Dependencies                                                                        ##########

#<< Qt
SET (QT_MT_REQUIRED TRUE)
SET (QT_MIN_VERSION "4.6.0")
IF (WANT_QT4MAC_COCOA)
  SET (QT_MAC_USE_COCOA TRUE)
ENDIF (WANT_QT4MAC_COCOA)
IF (WANT_QT3SUPPORT)
  SET (QT_USE_QT3SUPPORT TRUE)
ELSE (WANT_QT3SUPPORT)
  SET (QT_USE_QT3SUPPORT FALSE)
ENDIF (WANT_QT3SUPPORT)
SET (QT_USE_QTXML TRUE )
SET (QT_USE_QTWEBKIT TRUE )
SET (QT_USE_QTOPENGL TRUE )
IF (APPLE)
  SET( QT_USE_QTSQL TRUE )
ENDIF (APPLE)
SET( QT_USE_QTNETWORK TRUE )
FIND_PACKAGE(Qt4 REQUIRED)
INCLUDE( ${QT_USE_FILE} )
IF (QT_FOUND)
  MESSAGE("Qt4 Found OK")
  # we wont turn around, lets make it required
  IF(QT_QTWEBKIT_FOUND)
  MESSAGE(STATUS "QtWebkit Found OK")
  ELSE(QT_QTWEBKIT_FOUND)
  MESSAGE(FATAL_ERROR "QtWebKit not found")
  ENDIF(QT_QTWEBKIT_FOUND)

  IF (QT_VERSION_MAJOR EQUAL 4 AND QT_VERSION_MINOR EQUAL 4 AND QT_VERSION_PATCH EQUAL 0)
  MESSAGE(STATUS "Warning: Qt 4.4.0 has some bugs relating to decimal notation and line drawing. Please refer to Scribus bug 6999 and 7187 for information and to check if this may affect your use. Please use 4.4.1+ on release for your platform/distribution.")
  ENDIF (QT_VERSION_MAJOR EQUAL 4 AND QT_VERSION_MINOR EQUAL 4 AND QT_VERSION_PATCH EQUAL 0)
  IF (QT_VERSION_MAJOR EQUAL 4 AND QT_VERSION_MINOR EQUAL 3)
    MESSAGE(STATUS "Warning: Some versions of Qt 4.3.x have a bug influencing layout in the Scribus Properties Palette (refer to Scribus bug 7107). Please upgrade to Qt 4.4.1+ on release (4.4.0 has a decimal notation bug)")
  ENDIF (QT_VERSION_MAJOR EQUAL 4 AND QT_VERSION_MINOR EQUAL 3)
  IF (WANT_QT3SUPPORT)
    MESSAGE(STATUS "Building with Qt3 Support")
  ELSE (WANT_QT3SUPPORT)
    MESSAGE(STATUS "Building without Qt3 Support (GOOD)")
  ENDIF (WANT_QT3SUPPORT)
  #Add our Qt definitions
  ADD_DEFINITIONS(
    ${QT_DEFINITIONS}
    -DQT_CORE_LIB
    -DQT_GUI_LIB
    -DQT_THREAD_LIB
    -DQT_XML_LIB
    -DQT_GL_LIB
  )
ELSE(QT_FOUND)
  MESSAGE(FATAL_ERROR "No Qt4 found")
ENDIF(QT_FOUND)
#>> Qt

#<< ZLIB, PNG
IF (WIN32)
  # On win32 we can use Qt's zlib and libpng, so we use some
  # custom cmake includes. This permits us to honour the 
  # USE_QT_ZLIB_PNGLIB flag if passed.
  SET(PNG_DIR ${CMAKE_MODULE_PATH})
  SET(ZLIB_DIR ${CMAKE_MODULE_PATH})
ENDIF (WIN32)
FIND_PACKAGE(ZLIB REQUIRED)
IF (ZLIB_FOUND)
  MESSAGE("ZLIB Library Found OK")
  SET(HAVE_LIBZ 1)
ENDIF(ZLIB_FOUND)
FIND_PACKAGE(PNG REQUIRED)
IF (PNG_FOUND)
  MESSAGE("PNG Library Found OK")
ENDIF(PNG_FOUND)
#>> ZLIB, PNG

IF (WANT_NOOSG)
  MESSAGE("Building without 3D Extension")
ELSE (WANT_NOOSG)
  FIND_PACKAGE(OSG)
  IF(OSG_FOUND)
    MESSAGE("OSG found OK")
    SET(HAVE_OSG 1)
    FIND_PACKAGE(OpenGL)
  ELSE(OSG_FOUND)
	MESSAGE("No OSG found, building without 3D Extension")
  ENDIF(OSG_FOUND)
ENDIF (WANT_NOOSG)

#<< JPEG, TIFF, PYTHON
FIND_PACKAGE(JPEG REQUIRED)
IF (JPEG_FOUND)
  MESSAGE("JPEG Library Found OK")
ENDIF(JPEG_FOUND)
FIND_PACKAGE(TIFF REQUIRED)
IF (TIFF_FOUND)
  MESSAGE("TIFF Library Found OK")
  SET(HAVE_TIFF 1)
ELSE(TIFF_FOUND)
  MESSAGE(FATAL_ERROR "Could not find the TIFF Library")
ENDIF(TIFF_FOUND)
FIND_PACKAGE(PythonLibs REQUIRED)
IF (PYTHON_LIBRARY)
  MESSAGE("Python Library Found OK")
  SET(HAVE_PYTHON 1)
  SET(COMPILE_PYTHON 1)
ENDIF(PYTHON_LIBRARY)
#>> JPEG, TIFF, PYTHON

#<< FreeType2
IF (${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION} LESS 2.6)
  SET(FREETYPE_DIR ${CMAKE_MODULE_PATH})
  FIND_PACKAGE(FREETYPE REQUIRED)
  IF (FREETYPE_FOUND)
    MESSAGE("FreeType2 Library Found OK")
    INCLUDE(CheckLibraryExists)
    CHECK_LIBRARY_EXISTS(${FREETYPE_LIBRARY} FT_Get_First_Char "" HAVE_FREETYPE_FIRST)
    CHECK_LIBRARY_EXISTS(${FREETYPE_LIBRARY} FT_Get_Next_Char "" HAVE_FREETYPE_NEXT)
  ENDIF(FREETYPE_FOUND)
  SET(CFLAGS "${CFLAGS}"."${LIBFREETYPE_CFLAGS}")
ELSE(${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION} LESS 2.6)
  FIND_PACKAGE(Freetype REQUIRED)
  IF (FREETYPE_FOUND)
    MESSAGE(STATUS "FreeType2 Library Found OK")
    INCLUDE(CheckLibraryExists)
    CHECK_LIBRARY_EXISTS(${FREETYPE_LIBRARY} FT_Get_First_Char "" HAVE_FREETYPE_FIRST)
    CHECK_LIBRARY_EXISTS(${FREETYPE_LIBRARY} FT_Get_Next_Char "" HAVE_FREETYPE_NEXT)
    # here we have an alternative
    # a) Setup CFLAGS with the FREETYPE_INCLUDE_DIR_freetype2 provided by cmake
    # b) Insert ${FREETYPE_INCLUDE_DIRS} in include sections of CMakeLists that need it
    # I prefer (b) and at some point & from my opinion it should  be just needed in scribus/fonts/CmakeLists.txt - pm
  ELSE (FREETYPE_FOUND)
    MESSAGE(FATAL_ERROR "No Freetype Found")
  ENDIF(FREETYPE_FOUND)
ENDIF(${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION} LESS 2.6)
#>> FreeType2

#<<Render Library
# Select our rendering library
# As of Scribus 1.5.0, we can use cairo installed on system or cairo private branch
# If we have a RENDER_LIB selected from a previous cmake run, then use it
# otherwise, we should be defaulting to cairo private branch
IF(WANT_PRIVATE_CAIRO)
 MESSAGE("Using private CAIRO Library")
 SET(RENDER_LIB "CAIRO_PRIVATE" CACHE TYPE STRING)
 SET(HAVE_CAIRO 1)
 SET(HAVE_PRIVATE_CAIRO 1)
 SET( CAIRO_FOUND "YES" )
ELSE(WANT_PRIVATE_CAIRO)
 IF(RENDER_LIB)
  MESSAGE(STATUS "Previously selected rendering system: ${RENDER_LIB}")
  IF (RENDER_LIB STREQUAL "QTARTHUR")
   MESSAGE("Qt4's Arthur as renderer is not supported anymore, performing renderer detection again")
   SET(OLDRENDERLIBLOST TRUE)
  ENDIF (RENDER_LIB STREQUAL "QTARTHUR")
 ENDIF(RENDER_LIB)

 IF (WANT_SYSTEM_CAIRO OR RENDER_LIB STREQUAL "CAIRO_SYSTEM")
  SET(OLDRENDERLIBLOST FALSE)
  SET(CAIRO_DIR ${CMAKE_MODULE_PATH})
  FIND_PACKAGE(CAIRO)
  IF(CAIRO_FOUND)
   MESSAGE("CAIRO Library Found OK")
   SET(RENDER_LIB "CAIRO_SYSTEM" CACHE TYPE STRING)
  ELSE(CAIRO_FOUND)
   SET(OLDRENDERLIBLOST TRUE)
  ENDIF(CAIRO_FOUND)
 ENDIF (WANT_SYSTEM_CAIRO OR RENDER_LIB STREQUAL "CAIRO_SYSTEM")

 IF (OLDRENDERLIBLOST OR NOT RENDER_LIB OR RENDER_LIB STREQUAL "CAIRO_PRIVATE")
  SET(RENDER_LIB "CAIRO_PRIVATE" CACHE TYPE STRING)
  SET(HAVE_CAIRO 1)
  SET(HAVE_PRIVATE_CAIRO 1)
  SET(WANT_PRIVATE_CAIRO 1)
  SET( CAIRO_FOUND "YES" )
  MESSAGE("Cairo private branch selected for renderer")
  IF (OLDRENDERLIBLOST)
   MESSAGE("Old render lib could no longer be found, replaced by cairo private branch")
  ENDIF (OLDRENDERLIBLOST)
 ENDIF (OLDRENDERLIBLOST OR NOT RENDER_LIB OR RENDER_LIB STREQUAL "CAIRO_PRIVATE")
 
 #Cairo is required on Windows for printing system
 IF (WIN32)
  IF (NOT HAVE_CAIRO)
   MESSAGE(FATAL_ERROR "Cairo >= 1.5.16 is required for Windows build")  
  ENDIF (NOT HAVE_CAIRO)
 ENDIF (WIN32)
ENDIF(WANT_PRIVATE_CAIRO)
#>>Render Library

#<<CUPS
IF(NOT WIN32)
  IF (${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION} LESS 2.5)
    SET(Cups_DIR ${CMAKE_MODULE_PATH})
  ENDIF (${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION} LESS 2.5)
  FIND_PACKAGE(Cups REQUIRED)
  IF(CUPS_FOUND)
    SET(HAVE_CUPS 1)
    MESSAGE("CUPS Library Found OK")
  ENDIF(CUPS_FOUND)
ELSE(NOT WIN32)
  # Windows doesn't use CUPS
  SET(CUPS_INCLUDE_DIR)
ENDIF(NOT WIN32)
#>>CUPS

#<<LibXML2
IF (${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION} LESS 2.5)
  SET(LibXml2_DIR ${CMAKE_MODULE_PATH})
ENDIF (${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION} LESS 2.5)
FIND_PACKAGE(LibXml2 REQUIRED)
IF(LIBXML2_FOUND)
  SET(HAVE_XML 1)
  MESSAGE("LIBXML2 Library Found OK")
ENDIF(LIBXML2_FOUND)
#>>LibXML2

#<<LittleCMS
SET(LCMS_DIR ${CMAKE_MODULE_PATH})
  FIND_PACKAGE(LCMS)
  IF(LCMS_FOUND)
    SET(HAVE_LCMS1 1)
  ENDIF(LCMS_FOUND)
FIND_PACKAGE(LCMS2)
IF(LCMS2_FOUND)
  SET(HAVE_LCMS2 1)
  SET(HAVE_CMS 1)
  SET(CMS_INC "\"lcms2.h\"")
  MESSAGE("LittleCMS-2 Library Found OK")
ELSE(LCMS2_FOUND)
  FIND_PACKAGE(LCMS REQUIRED)
  IF(LCMS_FOUND)
    SET(HAVE_CMS 1)
    SET(HAVE_LCMS1 1)
    SET(CMS_INC "\"lcms.h\"")
    MESSAGE("LittleCMS Library Found OK")
  ELSE(LCMS_FOUND)
    MESSAGE(FATAL_ERROR "Could not find the LittleCMS Library")
  ENDIF(LCMS_FOUND)
ENDIF(LCMS2_FOUND)
#>>LittleCMS

#<<FontConfig
IF(NOT WIN32)
  SET(FONTCONFIG_DIR ${CMAKE_MODULE_PATH})
  FIND_PACKAGE(FONTCONFIG)
  IF(FONTCONFIG_FOUND)
	MESSAGE("FontConfig Found OK")
	SET(HAVE_FONTCONFIG 1)
  ENDIF(FONTCONFIG_FOUND)
ELSE(NOT WIN32)
  # Windows builds neither use nor require fontconfig
  SET(FONTCONFIG_INCLUDE_DIR)
ENDIF(NOT WIN32)
#>>FontConfig

#<<ASPELL for Speelling support
OPTION(WITH_ASPELL "Enable support for spell checking" ON)
IF (WITH_ASPELL)
  FIND_PACKAGE(ASPELL)
  IF (ASPELL_FOUND)
	MESSAGE("ASpell Found OK")
	SET(HAVE_ASPELL 1)
  ELSE (ASPELL_FOUND)
	MESSAGE("ASpell or its developer libraries NOT found - Disabling support for spell checking")
  ENDIF (ASPELL_FOUND)
ENDIF (WITH_ASPELL)
#>>ASPELL for Speelling support


#<<PoDoFo for AI PDF import
OPTION(WITH_PODOFO "Enable support for PDF embedded in AI" ON)
IF (WITH_PODOFO)
  FIND_PACKAGE(LIBPODOFO)
  IF (LIBPODOFO_FOUND)
	MESSAGE("PoDoFo found OK")
	SET(HAVE_PODOFO 1)
  ELSE (LIBPODOFO_FOUND)
	MESSAGE("PoDoFo NOT found - Disabling support for PDF embedded in AI")
  ENDIF (LIBPODOFO_FOUND)
ENDIF (WITH_PODOFO)
#>>PoDoFo for AI PDF import

#<<Boost for 2Geom Tools
OPTION(WITH_BOOST "Enable support for Boost based enhancements" ON)
IF (WITH_BOOST)
  FIND_PACKAGE(Boost)
  IF (Boost_FOUND)
	MESSAGE("Boost Library Found OK")
	SET(HAVE_BOOST 1)
  ELSE(Boost_FOUND)
	MESSAGE("Boost: NOT FOUND, not building 2geomtools")
  ENDIF (Boost_FOUND)
ENDIF (WITH_BOOST)
#>>Boost for 2Geom Tools

#<<GraphicksMagick for image import
IF (WANT_GRAPHICSMAGICK)
  SET(GMAGICK_DIR ${CMAKE_MODULE_PATH})
  FIND_PACKAGE(GMAGICK)
  IF(GMAGICK_FOUND)
	MESSAGE("GraphicksMagick Found OK")
  ELSE(GMAGICK_FOUND)
	MESSAGE("GraphicksMagick NOT found! Building without additional image format support!")
  ENDIF(GMAGICK_FOUND)
ELSE (WANT_GRAPHICSMAGICK)
  MESSAGE("Building without GraphicksMagick (use -DWANT_GRAPHICSMAGICK=1 to enable)")
ENDIF (WANT_GRAPHICSMAGICK)
#>>GraphicksMagick for image import

#<<Poppler for PDF import
FIND_PACKAGE(POPPLER)
IF (POPPLER_FOUND)
	IF (FREETYPE_FOUND)
		SET( HAVE_POPPLER 1 )
		MESSAGE(STATUS "Found poppler")
		MESSAGE(STATUS "Found poppler libs: ${POPPLER_LIBRARY}")
		MESSAGE(STATUS "Found poppler includes: ${POPPLER_INCLUDE_DIR}")
	ELSE (FREETYPE_FOUND)
		MESSAGE("Poppler NOT found - Disabling support for native PDF import")
	ENDIF (FREETYPE_FOUND)
ELSE (POPPLER_FOUND)
	MESSAGE("Poppler NOT found - Disabling support for native PDF import")
ENDIF (POPPLER_FOUND)
#>>Poppler for PDF import


##############################################################################################################
########## Include Setup                                                                            ##########

TEST_BIG_ENDIAN(WORDS_BIGENDIAN)
  
CHECK_INCLUDE_FILE("dlfcn.h" HAVE_DLFCN_H)
IF(HAVE_DLFCN_H)
  ADD_DEFINITIONS(-DHAVE_DLFCN_H)
ENDIF(HAVE_DLFCN_H)

CHECK_INCLUDE_FILE("unistd.h" HAVE_UNISTD_H)
IF(HAVE_UNISTD_H)
  ADD_DEFINITIONS(-DHAVE_UNISTD_H)
ENDIF(HAVE_UNISTD_H)

CHECK_INCLUDE_FILE("sys/types.h" HAVE_SYS_TYPES_H)
# IF(HAVE_SYS_TYPES_H)
#   ADD_DEFINITIONS(-DHAVE_SYS_TYPES_H)
# ENDIF(HAVE_SYS_TYPES_H)

CHECK_INCLUDE_FILE("sys/stat.h" HAVE_SYS_STAT_H)
# IF(HAVE_SYS_STAT_H)
#   ADD_DEFINITIONS(-DHAVE_SYS_STAT_H)
# ENDIF(HAVE_SYS_STAT_H)
#>>Test for existing include files


#Create configure files.. config.h and uninstall
#config.h
INCLUDE (ConfigureChecks.cmake)
  IF(WIN32)
    SET(CONFIG_NAME win-config.h)
  ELSE(WIN32)
    SET(CONFIG_NAME config.h)
  ENDIF(WIN32)
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/config.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/${CONFIG_NAME})
ADD_DEFINITIONS("-DHAVE_CONFIG_H")



#Set up include dirs with all found packages
INCLUDE_DIRECTORIES(
  ${CMAKE_BINARY_DIR}
  ${CMAKE_BINARY_DIR}/scribus
  ${QT_INCLUDES}
  ${CUPS_INCLUDE_DIR}
  ${FREETYPE_INCLUDE_DIR}
  ${FONTCONFIG_INCLUDE_DIR}
  ${JPEG_INCLUDE_DIR}
  ${LCMS_INCLUDE_DIR}
  ${LIBXML2_INCLUDE_DIR}
  ${PYTHON_INCLUDE_PATH}
  ${TIFF_INCLUDE_DIR}
  ${ZLIB_INCLUDE_DIR}
  ${OPENGL_INCLUDE_DIR}
  ${OSG_INCLUDE_DIR}
  ${GSL_INCLUDE_DIR}
)

IF (HAVE_POPPLER)
  INCLUDE_DIRECTORIES(
	${POPPLER_INCLUDE_DIR}
  )
ENDIF (HAVE_POPPLER)

##############################################################################################################
########## Uninstall Setup                                                                          ##########

CONFIGURE_FILE(
  "${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
  IMMEDIATE @ONLY
)

ADD_CUSTOM_TARGET(uninstall
  "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake")
  
  
##############################################################################################################
########## Add our subdirs                                                                          ##########

#Add our source subdirs
ADD_SUBDIRECTORY(scribus)
ADD_SUBDIRECTORY(doc)
ADD_SUBDIRECTORY(resources/dicts)
ADD_SUBDIRECTORY(resources/editorconfig)
ADD_SUBDIRECTORY(resources/icons)
ADD_SUBDIRECTORY(resources/keysets)
ADD_SUBDIRECTORY(resources/loremipsum)
ADD_SUBDIRECTORY(resources/profiles)
ADD_SUBDIRECTORY(resources/swatches)
ADD_SUBDIRECTORY(resources/templates)
#ADD_SUBDIRECTORY(resources/translations/qm)
ADD_SUBDIRECTORY(resources/translations/po)

#Install our READMEs etc.
INSTALL(FILES
  AUTHORS
  BUILDING
  ChangeLog
  ChangeLogSVN
  COPYING
  LINKS
  NEWS
  PACKAGING
  README
  README.MacOSX
  TODO
  TRANSLATION
  DESTINATION ${DOCDIR}
)

#Install our MIME data
INSTALL(FILES
  scribus.xml
  DESTINATION ${MIMEDIR}
)
#KDE workaround until it starts supporting freedesktop spec
INSTALL(FILES
  vnd.scribus.desktop
  DESTINATION ${MIMELNKDIR}
)

## PL We may not need this anymore
#Install the desktop file
## INSTALL(FILES
 # scribus.desktop
 # #DESTINATION ${DESKTOPDIR}
#)

#If building an Apple bundle, install these specific files
IF(APPLEBUNDLE)
  INSTALL(FILES
    Scribus.app/Contents/Info.plist
    DESTINATION ${CMAKE_INSTALL_PREFIX}
  )
  INSTALL(FILES
    Scribus.app/Contents/Resources/Scribus.icns
    Scribus.app/Contents/Resources/Scribus-doc.icns
    DESTINATION ${CMAKE_INSTALL_PREFIX}/Resources
  )
ENDIF(APPLEBUNDLE)

##############################################################################################################
########## Install/CPack Setup                                                                      ##########

# If the cmake version includes cpack, use it for tarball building
IF(EXISTS "${CMAKE_ROOT}/Modules/CPack.cmake")
  SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Scribus is an open source publishing application for Linux, Mac OSX and Windows")
  SET(CPACK_PACKAGE_VENDOR "Scribus Team")
  SET(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/README")
  SET(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/README")
  SET(CPACK_PACKAGE_VERSION_MAJOR "${VERSION_MAJOR}")
  SET(CPACK_PACKAGE_VERSION_MINOR "${VERSION_MINOR}")
  SET(CPACK_PACKAGE_VERSION_PATCH "${VERSION_PATCH}")
  SET(CPACK_PACKAGE_INSTALL_DIRECTORY "Scribus ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}")
  SET(CPACK_PACKAGE_EXECUTABLES "scribus${TAG_VERSION}")
  SET(CPACK_SOURCE_PACKAGE_FILE_NAME "scribus-${VERSION}${VERSION_SUFFIX}")
  SET(CPACK_SOURCE_GENERATOR TBZ2)
  SET(CPACK_SYSTEM_NAME "")
  SET(CPACK_TOPLEVEL_TAG "")
  SET(CPACK_SOURCE_IGNORE_FILES
    CMakeCache.txt
    scribus-1.5.0.svn.tar.bz2
    scribus-1.5.0.svn.tar.Z
    scribus-1.5.0.svn.tar.gz
    "~$"
    "\\\\.cvsignore$"
    "\\\\.o$"
    "\\\\.svn-base$"
    "\\\\.svn$"
    "^${PROJECT_SOURCE_DIR}.*/CVS/"
    "^${PROJECT_SOURCE_DIR}/debian/"
    "^${PROJECT_SOURCE_DIR}/old/"
    "^${PROJECT_SOURCE_DIR}.*/CVSROOT/"
    "^${PROJECT_SOURCE_DIR}/admin/"
  )
  INCLUDE(CPack)
ENDIF(EXISTS "${CMAKE_ROOT}/Modules/CPack.cmake")

