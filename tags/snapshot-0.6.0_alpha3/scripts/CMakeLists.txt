set(kmediafactory_SCRIPTS
  kmf_frame
  kmf_info
  kmf_make_mpeg
  kmf_oo2pdf
  tools
  kmf_converter.oxt
)

install(FILES ${kmediafactory_SCRIPTS} DESTINATION ${DATA_INSTALL_DIR}/kmediafactory/scripts)
