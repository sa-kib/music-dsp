set(LIB_SOURCES
    nnls.c
)

add_library(${EXT_NNLS_TARGET} STATIC ${LIB_SOURCES})
