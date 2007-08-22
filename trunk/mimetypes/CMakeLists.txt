find_package(SharedMimeInfo REQUIRED)

########### install files ###############
install(FILES x-kmediafactory.xml DESTINATION ${XDG_MIME_INSTALL_DIR})
update_xdg_mimetypes(${XDG_MIME_INSTALL_DIR})

