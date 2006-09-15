include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_SOURCE_DIR}/lib
  ${CMAKE_BINARY_DIR}/lib
)

set(kmediafactory_slideshow_SRCS
  slideshowplugin.cpp
  slideshowobject.cpp
  slideshowproperties.cpp
)

kde4_add_ui_files(kmediafactory_slideshow_SRCS
  slideshowproperties.ui
)

kde4_add_kcfg_files(kmediafactory_slideshow_SRCS
  slideshowpluginsettings.kcfgc
)

kde4_automoc(${kmediafactory_slideshow_SRCS})
kde4_add_plugin(kmediafactory_slideshow ${kmediafactory_slideshow_SRCS})
kde4_install_libtool_file(${PLUGIN_INSTALL_DIR} kmediafactory_slideshow)
target_link_libraries(kmediafactory_slideshow ${KDE4_KDECORE_LIBS} kmediafactoryinterfaces kmf)

install(TARGETS kmediafactory_slideshow DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES kmediafactory_slideshowui.rc DESTINATION ${DATA_INSTALL_DIR}/kmediafactory_slideshow)
install(FILES slideshowplugin.kcfg DESTINATION ${KCFG_INSTALL_DIR})
install(FILES kmediafactory_slideshow.desktop DESTINATION ${SERVICES_INSTALL_DIR})

