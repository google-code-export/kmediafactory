include_directories(${KOSTORE_INCLUDES})

if(QCA2_FOUND)
    include_directories(${QCA2_INCLUDE_DIR})
    add_definitions(-DQCA2)
endif(QCA2_FOUND)

########### next target ###############

set(kstore_LIB_SRCS
    KoStore.cpp
    KoTarStore.cpp
    KoDirectoryStore.cpp
    KoZipStore.cpp
    KoStoreBase.cpp
    KoXmlReader.cpp
    KoXmlWriter.cpp
    KoEncryptedStore.cpp
    KoEncryptedStore.cpp
    KoEncryptionChecker.cpp
)

kde4_add_library(kmediafactorykstore SHARED ${kstore_LIB_SRCS})

target_link_libraries(kmediafactorykstore ${KDE4_KIO_LIBS})

if(QCA2_FOUND)
target_link_libraries(kmediafactorykstore ${QCA2_LIBRARIES})
endif(QCA2_FOUND)

set_target_properties(kmediafactorykstore PROPERTIES VERSION ${GENERIC_KMF_LIB_VERSION} SOVERSION ${GENERIC_KMF_LIB_SOVERSION})
install(TARGETS kmediafactorykstore DESTINATION ${LIB_INSTALL_DIR})
