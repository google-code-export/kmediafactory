include_directories(${CMAKE_CURRENT_SOURCE_DIR})

set(kmediafactorykstore_LIB_SRCS
   KoStore.cpp
   KoTarStore.cpp
   KoDirectoryStore.cpp
   KoZipStore.cpp
   KoStoreBase.cpp
)

kde4_add_library(kmediafactorykstore SHARED ${kmediafactorykstore_LIB_SRCS})
target_link_libraries(kmediafactorykstore ${KDE4_KIO_LIBS})
set_target_properties(kmediafactorykstore PROPERTIES VERSION 1.0.0 SOVERSION 1)
install(TARGETS kmediafactorykstore DESTINATION ${LIB_INSTALL_DIR})
