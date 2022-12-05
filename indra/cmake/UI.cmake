# -*- cmake -*-
include(Prebuilt)
include(FreeType)

if (USESYSTEMLIBS)
  include(FindPkgConfig)

  foreach(pkg ${PKGCONFIG_PACKAGES})
    pkg_check_modules(${pkg} REQUIRED ${pkg})
    include_directories(${${pkg}_INCLUDE_DIRS})
    link_directories(${${pkg}_LIBRARY_DIRS})
    list(APPEND UI_LIBRARIES ${${pkg}_LIBRARIES})
    add_definitions(${${pkg}_CFLAGS_OTHERS})
  endforeach(pkg)
else (USESYSTEMLIBS)

  include_directories (
      ${LIBS_PREBUILT_DIR}/include
      ${LIBS_PREBUILT_DIR}/include/cairo
      ${LIBS_PREBUILT_DIR}/include/pixman-1

      )
  foreach(include ${${LL_ARCH}_INCLUDES})
      include_directories(${LIBS_PREBUILT_DIR}/include/${include})
  endforeach(include)
endif (USESYSTEMLIBS)

