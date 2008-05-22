find_program(MSGFMT_EXECUTABLE msgfmt)
find_program(ZIP_EXECUTABLE zip)

string(REGEX MATCHALL "([a-z/.]+)" files ${template_files})

execute_process(WORKING_DIRECTORY ${src}
                COMMAND ${ZIP_EXECUTABLE} -X ${dest}/${template} ${files}
                OUTPUT_QUIET)
file(MAKE_DIRECTORY locale)
file(GLOB dirs RELATIVE "${src}/locale/" "${src}/locale/??")
foreach(dir ${dirs})
  file(MAKE_DIRECTORY locale/${dir})
  file(MAKE_DIRECTORY locale/${dir}/LC_MESSAGES)
  execute_process(COMMAND ${MSGFMT_EXECUTABLE}
                          ${src}/locale/${dir}/LC_MESSAGES/template.po 
                          -o ${dest}/locale/${dir}/LC_MESSAGES/template.mo
                  OUTPUT_QUIET)
  execute_process(COMMAND ${MSGFMT_EXECUTABLE}
                          ${src}/locale/${dir}/LC_MESSAGES/ui.po 
                          -o ${dest}/locale/${dir}/LC_MESSAGES/ui.mo
                  OUTPUT_QUIET)
endforeach(dir)
execute_process(WORKING_DIRECTORY ${dest}
                COMMAND ${ZIP_EXECUTABLE} -X ${dest}/${template} -r locale
                OUTPUT_QUIET)
