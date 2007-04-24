set(kmf_converter_SRCS
  converter.xba
  script.xlb
)

add_custom_command(
  OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/kmf_converter.zip
  COMMAND ${ZIP_EXECUTABLE} -X ${CMAKE_CURRENT_BINARY_DIR}/kmf_converter.oxt ${kmf_converter_SRCS}
  DEPENDS ${kmf_converter_SRCS}
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

add_custom_target(kmf_converter ALL echo
  DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/kmf_converter.zip
)

set(kmediafactory_SCRIPTS
  frame
  info
  make_mpeg
  oo2pdf
  tools
  "${CMAKE_CURRENT_BINARY_DIR}/kmf_converter.zip"
)

install(FILES ${kmediafactory_SCRIPTS} DESTINATION ${DATA_INSTALL_DIR}/kmediafactory/scripts)
