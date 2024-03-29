INCLUDE_DIRECTORIES(
${CMAKE_SOURCE_DIR}
${CMAKE_SOURCE_DIR}/scribus
)

ADD_SUBDIRECTORY(samples)
ADD_SUBDIRECTORY(scripts)

SET(SCRIPTER_PLUGIN_UI_SRC
  pconsole.ui
  runscriptdialog.ui
  scripterprefsgui.ui
  prefs_scripterbase.ui
)

SET(SCRIPTER_PLUGIN_MOC_CLASSES
  pconsole.h
  scriptplugin.h
  scriptercore.h
  scripterprefsgui.h
  runscriptdialog.h
  prefs_scripter.h
)

SET(SCRIPTER_PLUGIN_SOURCES
  cmdcell.cpp
  cmdcolor.cpp
  cmddialog.cpp
  cmddoc.cpp
  cmdgetprop.cpp
  cmdgetsetprop.cpp
  cmdmani.cpp
  cmdmisc.cpp
  cmdobj.cpp
  cmdpage.cpp
  cmdsetprop.cpp
  cmdstyle.cpp
  cmdtable.cpp
  cmdtext.cpp
  cmdutil.cpp
  guiapp.cpp
  objimageexport.cpp
  objpdffile.cpp
  objprinter.cpp
  pconsole.cpp
  prefs_scripter.cpp
  runscriptdialog.cpp
  scriptercore.cpp
  scripterprefsgui.cpp
  scriptplugin.cpp
  svgimport.cpp
)

SET(SCRIBUS_SCRIPTER_PLUGIN "scriptplugin")

QT4_WRAP_UI(SCRIPTER_PLUGIN_UI_SOURCES ${SCRIPTER_PLUGIN_UI_SRC} )
QT4_WRAP_CPP(SCRIPTER_PLUGIN_MOC_SOURCES ${SCRIPTER_PLUGIN_MOC_CLASSES})

ADD_LIBRARY(${SCRIBUS_SCRIPTER_PLUGIN} MODULE 
  ${SCRIPTER_PLUGIN_SOURCES}
  ${SCRIPTER_PLUGIN_MOC_SOURCES}
  ${SCRIPTER_PLUGIN_UI_SOURCES})

TARGET_LINK_LIBRARIES(${SCRIBUS_SCRIPTER_PLUGIN}
  ${PYTHON_LIBRARIES}
)

TARGET_LINK_LIBRARIES(${SCRIBUS_SCRIPTER_PLUGIN} ${PLUGIN_LIBRARIES})

INSTALL(TARGETS ${SCRIBUS_SCRIPTER_PLUGIN}
  LIBRARY
  DESTINATION ${PLUGINDIR}
  PERMISSIONS ${PLUGIN_PERMISSIONS}
)

ADD_DEPENDENCIES(${SCRIBUS_SCRIPTER_PLUGIN} ${EXE_NAME})

# SET_TARGET_PROPERTIES(${SCRIBUS_SCRIPTER_PLUGIN} PROPERTIES VERSION "0.0.0")
