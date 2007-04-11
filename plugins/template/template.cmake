find_program(MSGFMT_EXECUTABLE msgfmt)
find_program(ZIP_EXECUTABLE zip)

MESSAGE(STATUS ${template_files})

EXEC_PROGRAM(${ZIP_EXECUTABLE} ${src}
    ARGS -X ${src}/${template} ${template_files})
FILE(MAKE_DIRECTORY locale)
FILE(GLOB dirs RELATIVE "${src}/locale/" "${src}/locale/??")
FOREACH(dir ${dirs})
  FILE(MAKE_DIRECTORY locale/${dir})
  FILE(MAKE_DIRECTORY locale/${dir}/LC_MESSAGES)
  EXEC_PROGRAM(${MSGFMT_EXECUTABLE}
      ARGS ${src}/locale/${dir}/LC_MESSAGES/template.po -o ${dest}/locale/${dir}/LC_MESSAGES/template.mo)
  EXEC_PROGRAM(${MSGFMT_EXECUTABLE}
      ARGS ${src}/locale/${dir}/LC_MESSAGES/ui.po -o ${dest}/locale/${dir}/LC_MESSAGES/ui.mo)
ENDFOREACH(dir)
EXEC_PROGRAM(${ZIP_EXECUTABLE} ${dest}
    ARGS -X ${dest}/${template} -r locale)
