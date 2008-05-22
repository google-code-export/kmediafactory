add_subdirectory(pics)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
if(LIBDVDREAD_FOUND)
   include_directories(${LIBDVDREAD_INCLUDE_DIR})
endif(LIBDVDREAD_FOUND)

set(kmf_LIB_SRCS
  kmflanguagewidgets.cpp
  rect.cpp
  dvdauthorparser.cpp
  kmfimageview.cpp
  kmftime.cpp
  kmffontchooser.cpp
  kmftools.cpp
  kmfmultiurldialog.cpp
  kmftoolbutton.cpp
  qdvdinfo.cpp
  kmfmediafile.cpp
  run.cpp
  kmftoolbar.cpp
  Q4puGenericSignalSpy.cpp
)

set(kmf_HEADERS
  kmflanguagewidgets.h
  rect.h
  dvdauthorparser.h
  kmfimageview.h
  kmftime.h
  kmffontchooser.h
  kmftools.h
  kmfmultiurldialog.h
  kmftoolbutton.h
  qdvdinfo.h
  kmfmediafile.h
  run.h
  kmftoolbar.h
  kmflistmodel.h
)

kde4_add_ui_files(kmf_LIB_SRCS
  kmfmultiurldialog.ui
)

kde4_add_library(kmf SHARED ${kmf_LIB_SRCS})
target_link_libraries(kmf ${KDE4_KDECORE_LIBS} ${QT_QTXML_LIBRARY} ${KDE4_KUTILS_LIBS} ${XINE_LIBRARY} ${FONTCONFIG_LIBRARIES})
if(LIBDVDREAD_FOUND)
   target_link_libraries(kmf ${LIBDVDREAD_LIBRARY})
endif(LIBDVDREAD_FOUND)
set_target_properties(kmf PROPERTIES VERSION ${KMF_LIB_VERSION} SOVERSION ${KMF_LIB_VERSION})
install(TARGETS kmf DESTINATION ${LIB_INSTALL_DIR})

set(kmfwidgets_PART_SRCS
  ${CMAKE_CURRENT_BINARY_DIR}/kmfwidgets.cpp
)
kde4_add_widget_files(${kmfwidgets_PART_SRCS} kmf.widgets)
qt4_add_resources(kmfwidgets_PART_SRCS kmfwidgets.qrc)
kde4_add_plugin(kmfwidgets ${kmfwidgets_PART_SRCS})
target_link_libraries(kmfwidgets ${KDE4_KDECORE_LIBS} kmf)
install(TARGETS kmfwidgets DESTINATION ${PLUGIN_INSTALL_DIR}/plugins/designer)

