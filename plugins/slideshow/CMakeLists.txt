include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/../video
  ${CMAKE_SOURCE_DIR}/lib
  ${CMAKE_BINARY_DIR}/lib
)

if(LIBDVDREAD_FOUND)
   include_directories(${KEXIV2_INCLUDE_DIR})
endif(LIBDVDREAD_FOUND)

set(kmediafactory_slideshow_SRCS
  slideshowplugin.cpp
  slideshowobject.cpp
  slideshowproperties.cpp
  ../video/subtitleoptions.cpp
)

kde4_add_ui_files(kmediafactory_slideshow_SRCS
  slideshowproperties.ui
  slideshowconfig.ui
  ../video/subtitleoptions.ui
)

kde4_add_kcfg_files(kmediafactory_slideshow_SRCS
  slideshowpluginsettings.kcfgc
)

kde4_add_plugin(kmediafactory_slideshow ${kmediafactory_slideshow_SRCS})
target_link_libraries(kmediafactory_slideshow ${KDE4_KIO_LIBS} ${KDE4_THREADWEAVER_LIBRARY} kmediafactoryinterfaces kmf)
if(KEXIV2_FOUND)
   target_link_libraries(kmediafactory_slideshow ${KEXIV2_LIBRARIES})
endif(KEXIV2_FOUND)

install(TARGETS kmediafactory_slideshow DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES kmediafactory_slideshowui.rc DESTINATION ${DATA_INSTALL_DIR}/kmediafactory)
install(FILES slideshowplugin.kcfg DESTINATION ${KCFG_INSTALL_DIR})
install(FILES kmediafactory_slideshow.desktop DESTINATION ${SERVICES_INSTALL_DIR})

