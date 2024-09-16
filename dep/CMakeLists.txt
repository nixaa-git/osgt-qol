# Build dependencies.

# MinHook - function hooking library.
set(SOURCES_MINHOOK
  "minhook/src/buffer.c"
  "minhook/src/hook.c"
  "minhook/src/trampoline.c"
  "minhook/src/hde/hde64.c"
)
add_library(minhook ${SOURCES_MINHOOK})
target_include_directories(minhook PUBLIC "minhook/include")