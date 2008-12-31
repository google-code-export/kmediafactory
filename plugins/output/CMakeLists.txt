include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_SOURCE_DIR}
  ${CMAKE_SOURCE_DIR}/kstore
  ${CMAKE_SOURCE_DIR}/lib
  ${CMAKE_BINARY_DIR}/lib
)

set(kmediafactory_output_SRCS
  outputplugin.cpp
  dvddirectoryobject.cpp
  dvdauthorobject.cpp
  k3bobject.cpp
)

if(LIBDVDREAD_FOUND)
  set(kmediafactory_output_SRCS ${kmediafactory_output_SRCS} dvdinfo.cpp)
endif(LIBDVDREAD_FOUND)

kde4_add_ui_files(kmediafactory_output_SRCS
  dvdinfo.ui
)

kde4_add_plugin(kmediafactory_output ${kmediafactory_output_SRCS})
target_link_libraries(kmediafactory_output ${KDE4_KIO_LIBS} ${KDE4_THREADWEAVER_LIBRARY} kmediafactoryinterfaces kmf kmediafactorykstore)

install(TARGETS kmediafactory_output DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES kmediafactory_outputui.rc DESTINATION ${DATA_INSTALL_DIR}/kmediafactory)
install(FILES kmediafactory_output.desktop DESTINATION ${SERVICES_INSTALL_DIR})
