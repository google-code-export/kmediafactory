set(kmediafactory_SCRIPTS
  kmf_comment
  kmf_frame
  kmf_info
  kmf_make_mpeg
  kmf_oo2pdf
  tools
)

install(FILES kmf_converter.oxt DESTINATION ${DATA_INSTALL_DIR}/kmediafactory/scripts)
install(PROGRAMS ${kmediafactory_SCRIPTS} DESTINATION ${DATA_INSTALL_DIR}/kmediafactory/scripts)
