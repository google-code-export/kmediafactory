include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_SOURCE_DIR}
  ${CMAKE_SOURCE_DIR}/lib
  ${CMAKE_BINARY_DIR}/lib
)

set(kmediafactory_plugin_kross_SRCS
  krossplugin.cpp
  krossplugininterface.cpp
  krossmediaobject.cpp
  krosstemplateobject.cpp
  krossoutputobject.cpp
  krossjob.cpp
  objectmapper.cpp
)

kde4_add_plugin(kmediafactory_plugin_kross ${kmediafactory_plugin_kross_SRCS})
target_link_libraries(kmediafactory_plugin_kross
    ${KDE4_KDECORE_LIBS}
    ${KDE4_KROSSCORE_LIBS}
    kmediafactoryinterfaces
    kmf
)

install(TARGETS kmediafactory_plugin_kross DESTINATION ${PLUGIN_INSTALL_DIR})
