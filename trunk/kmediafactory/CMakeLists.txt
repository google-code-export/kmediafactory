include_directories(${CMAKE_CURRENT_SOURCE_DIR})

set(kmediafactoryinterfaces_LIB_SRCS
  plugin.cpp
  plugininterface.cpp
  object.cpp
  job.cpp
)

set(kmediafactoryinclude_HEADERS
  kmediafactory.h
  plugin.h
  plugininterface.h
  object.h
  job.h
)

kde4_add_library(kmediafactoryinterfaces SHARED ${kmediafactoryinterfaces_LIB_SRCS})
target_link_libraries(kmediafactoryinterfaces ${KDE4_KDECORE_LIBS} ${KDE4_KPARTS_LIBS} ${KDE4_THREADWEAVER_LIBRARIES})
set_target_properties(kmediafactoryinterfaces PROPERTIES VERSION ${KMF_LIB_VERSION} SOVERSION ${KMF_LIB_VERSION})
install(TARGETS kmediafactoryinterfaces DESTINATION ${LIB_INSTALL_DIR})
#kde4_install_libtool_file(${LIB_INSTALL_DIR} kmediafactoryinterfaces)
install(FILES kmediafactoryplugin.desktop DESTINATION ${SERVICETYPES_INSTALL_DIR})
install(FILES ${kmediafactoryinclude_HEADERS} DESTINATION ${INCLUDE_INSTALL_DIR}/kmediafactory)
