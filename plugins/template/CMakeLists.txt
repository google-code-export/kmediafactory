add_subdirectory(simple)
add_subdirectory(preview_6)
add_subdirectory(preview_3)
add_subdirectory(preview_1)

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_SOURCE_DIR}
  ${CMAKE_SOURCE_DIR}/lib
  ${CMAKE_BINARY_DIR}/lib
)

set(kmediafactory_template_SRCS
  templateplugin.cpp
  templateobject.cpp
  kmfwidget.cpp
  kmflabel.cpp
  kmfframe.cpp
  kmfmenupage.cpp
  kmfbutton.cpp
  kmfmenu.cpp
  kmfimage.cpp
  kmftemplatebase.cpp
  kmfwidgetfactory.cpp
  kmflayout.cpp
  kmfgeometry.cpp
  kmfintl.cpp
  kmftemplate.cpp
  kconfigxml.cpp
  newstuffobject.cpp
)

kde4_add_ui_files(kmediafactory_template_SRCS
  templateconfig.ui
)

kde4_add_kcfg_files(kmediafactory_template_SRCS
  templatepluginsettings.kcfgc
)

kde4_add_plugin(kmediafactory_template ${kmediafactory_template_SRCS})
target_link_libraries(kmediafactory_template ${KDE4_KDECORE_LIBS} ${KDE4_KNEWSTUFF2_LIBRARY} ${QT_QTUITOOLS_LIBRARY} kmediafactoryinterfaces kmf kmediafactorykstore)

install(TARGETS kmediafactory_template DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES kmediafactory_templateui.rc DESTINATION ${DATA_INSTALL_DIR}/kmediafactory)
install(FILES templateplugin.kcfg DESTINATION ${KCFG_INSTALL_DIR})
install(FILES kmediafactory_template.desktop DESTINATION ${SERVICES_INSTALL_DIR})
install(FILES kmediafactory_template.knsrc DESTINATION ${CONFIG_INSTALL_DIR})