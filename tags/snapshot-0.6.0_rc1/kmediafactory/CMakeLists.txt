include_directories(${CMAKE_CURRENT_SOURCE_DIR})

set(kmediafactoryinterfaces_LIB_SRCS
  plugin.cpp
  projectinterface.cpp
  uiinterface.cpp
  kmfobject.cpp
)

set(kmediafactoryinclude_HEADERS
  plugin.h
  projectinterface.h
  uiinterface.h
  kmfobject.h
)

kde4_add_library(kmediafactoryinterfaces SHARED ${kmediafactoryinterfaces_LIB_SRCS})
target_link_libraries(kmediafactoryinterfaces ${KDE4_KDECORE_LIBS} ${KDE4_KPARTS_LIBS})
set_target_properties(kmediafactoryinterfaces PROPERTIES VERSION 1.0.0 SOVERSION 1)
install(TARGETS kmediafactoryinterfaces DESTINATION ${LIB_INSTALL_DIR})
#kde4_install_libtool_file(${LIB_INSTALL_DIR} kmediafactoryinterfaces)
install(FILES kmediafactoryplugin.desktop DESTINATION ${SERVICETYPES_INSTALL_DIR})
install(FILES ${kmediafactoryinclude_HEADERS} DESTINATION ${INCLUDE_INSTALL_DIR}/kmediafactory)
