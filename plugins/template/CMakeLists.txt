add_subdirectory(artwork)
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
if (KDE_4_4)
target_link_libraries(kmediafactory_template ${KDE4_KIO_LIBS} ${KDE4_THREADWEAVER_LIBRARY} ${KDE4_KNEWSTUFF3_LIBRARY} ${QT_QTUITOOLS_LIBRARY} kmediafactoryinterfaces kmf kmediafactorykstore)
else (KDE_4_4)
target_link_libraries(kmediafactory_template ${KDE4_KIO_LIBS} ${KDE4_THREADWEAVER_LIBRARY} ${KDE4_KNEWSTUFF2_LIBRARY} ${QT_QTUITOOLS_LIBRARY} kmediafactoryinterfaces kmf kmediafactorykstore)
endif (KDE_4_4)

install(TARGETS kmediafactory_template DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES kmediafactory_templateui.rc DESTINATION ${DATA_INSTALL_DIR}/kmediafactory)
install(FILES templateplugin.kcfg DESTINATION ${KCFG_INSTALL_DIR})
install(FILES kmediafactory_template.desktop DESTINATION ${SERVICES_INSTALL_DIR})
if (KDE_4_4)
install(FILES kmediafactory_template-kns3.knsrc DESTINATION ${CONFIG_INSTALL_DIR} RENAME kmediafactory_template.knsrc)
else (KDE_4_4)
install(FILES kmediafactory_template.knsrc DESTINATION ${CONFIG_INSTALL_DIR})
endif (KDE_4_4)
