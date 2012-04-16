# -*- cmake -*-

add_subdirectory(base)

add_subdirectory(webkit)

add_subdirectory(gstreamer010)

if (USE_QUICKTIME)
    add_subdirectory(quicktime)
endif (USE_QUICKTIME)

if (WINDOWS)
    add_subdirectory(winmmshim)
endif (WINDOWS)

add_subdirectory(example)
