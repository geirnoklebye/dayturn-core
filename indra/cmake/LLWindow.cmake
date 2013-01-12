# -*- cmake -*-

include(OpenGL)
include(Prebuilt)

if (STANDALONE)
  include(FindSDL)

  # This should be done by FindSDL.  Sigh.
  mark_as_advanced(
      SDLMAIN_LIBRARY
      SDL_INCLUDE_DIR
      SDL_LIBRARY
      )
else (STANDALONE)
  use_prebuilt_binary(mesa)
  if (LINUX AND VIEWER)
    use_prebuilt_binary(SDL)#kokuafixme was:SDL-noartwork
    set (SDL_FOUND TRUE)
    set (SDL_INCLUDE_DIR ${LIBS_PREBUILT_DIR}/include)
    set (SDL_LIBRARY SDL)
    if (NOT ${ARCH} STREQUAL "x86_64")
      list(APPEND SDL_LIBRARY directfb fusion direct)
    endif (NOT ${ARCH} STREQUAL "x86_64")
  endif (LINUX AND VIEWER)
endif (STANDALONE)

if (SDL_FOUND)
  add_definitions(-DLL_SDL=1)
  include_directories(${SDL_INCLUDE_DIR})
endif (SDL_FOUND)

set(LLWINDOW_INCLUDE_DIRS
    ${GLEXT_INCLUDE_DIR}
    ${LIBS_OPEN_DIR}/llwindow
    )

if (SERVER AND LINUX)
  set(LLWINDOW_LIBRARIES
      llwindowheadless
      )
else (SERVER AND LINUX)
  set(LLWINDOW_LIBRARIES
      llwindow
      )
endif (SERVER AND LINUX)
