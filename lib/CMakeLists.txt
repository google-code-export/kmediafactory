add_subdirectory(pics)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

set(kmf_LIB_SRCS
  qdvdinfo.cpp
  qmpegvideo.cpp
  rect.cpp
  dvdauthorparser.cpp
  kmflanguagewidgets.cpp
  kmfimageview.cpp
  kmftime.cpp
  kmffontchooser.cpp
  kmftools.cpp
  kmfmultiurldialog.cpp
  kmftoolbutton.cpp
)

kde4_add_ui_files(kmf_LIB_SRCS
  kmfmultiurldialog.ui
)

kde4_automoc(${kmf_LIB_SRCS})
add_library(kmf SHARED ${kmf_LIB_SRCS})
target_link_libraries(kmf ${KDE4_KDECORE_LIBS} ${QT_QTXML_LIBRARY} ${KDE4_KUTILS_LIBS} ${XINE_LIBRARY} ${FONTCONFIG_LIBRARIES})
set_target_properties(kmf PROPERTIES VERSION 1.0.0 SOVERSION 1)
install(TARGETS kmf DESTINATION ${LIB_INSTALL_DIR})
kde4_install_libtool_file(${LIB_INSTALL_DIR} kmf)

set(kmfwidgets_SRCS)
kde4_add_widget_files(kmfwidgets_SRCS kmediafactory.widgets)
kde4_add_plugin(kmfwidgets ${kmfwidgets_SRCS})
kde4_install_libtool_file(${PLUGIN_INSTALL_DIR} kmfwidgets)
target_link_libraries(kmfwidgets ${KDE4_KDECORE_LIBS} kmf)
install(TARGETS kmfwidgets DESTINATION ${PLUGIN_INSTALL_DIR})
