include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_SOURCE_DIR}/lib
  ${CMAKE_BINARY_DIR}/lib
)
set(kmediafactory_video_SRCS
  videoplugin.cpp
  videoobject.cpp
  videooptions.cpp
  chapters.cpp
  subtitleoptions.cpp
)

kde4_add_ui_files(kmediafactory_video_SRCS
  videooptions.ui
  chapters.ui
  autochapters.ui
  languageselection.ui
  videoconfig.ui
  subtitleoptions.ui
  addchapter.ui
)

kde4_add_kcfg_files(kmediafactory_video_SRCS
  videopluginsettings.kcfgc
)

kde4_automoc(${kmediafactory_video_SRCS})
kde4_add_plugin(kmediafactory_video ${kmediafactory_video_SRCS})
target_link_libraries(kmediafactory_video ${KDE4_KDECORE_LIBS} kmediafactoryinterfaces kmf)

install(TARGETS kmediafactory_video DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES kmediafactory_videoui.rc DESTINATION ${DATA_INSTALL_DIR}/kmediafactory_video)
install(FILES videoplugin.kcfg DESTINATION ${KCFG_INSTALL_DIR})
install(FILES kmediafactory_video.desktop DESTINATION ${SERVICES_INSTALL_DIR})
