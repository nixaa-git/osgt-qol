set(SOURCES
    src/buffer.c
    src/hook.c
    src/trampoline.c
    src/hde/hde64.c
)

add_library(minhook STATIC ${SOURCES})
target_include_directories(minhook PUBLIC include)
