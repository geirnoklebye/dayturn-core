# -*- cmake -*-

project(media_plugin_example)

include(00-Common)
include(LLCommon)
include(LLImage)
include(LLPlugin)
include(LLMath)
include(LLRender)
include(LLWindow)
include(Linking)
include(PluginAPI)
include(MediaPluginBase)
include(OpenGL)

include(ExamplePlugin)

include_directories(
    ${LLPLUGIN_INCLUDE_DIRS}
    ${MEDIA_PLUGIN_BASE_INCLUDE_DIRS}
    ${LLCOMMON_INCLUDE_DIRS}
    ${LLMATH_INCLUDE_DIRS}
    ${LLIMAGE_INCLUDE_DIRS}
    ${LLRENDER_INCLUDE_DIRS}
    ${LLWINDOW_INCLUDE_DIRS}
)
include_directories(SYSTEM
    ${LLCOMMON_SYSTEM_INCLUDE_DIRS}
    )


### media_plugin_example

if(WINDOWS)
    ##add_definitions(/FIXED:NO)
else(WINDOWS) # not windows therefore DARWIN
    add_definitions(-fPIC)
endif(WINDOWS)

set(media_plugin_example_SOURCE_FILES
    media_plugin_example.cpp
    )

add_library(media_plugin_example
    SHARED
    ${media_plugin_example_SOURCE_FILES}
    )

target_link_libraries(media_plugin_example
  ${LLPLUGIN_LIBRARIES}
  ${MEDIA_PLUGIN_BASE_LIBRARIES}
  ${LLCOMMON_LIBRARIES}
  ${EXAMPLE_PLUGIN_LIBRARIES}
  ${PLUGIN_API_WINDOWS_LIBRARIES}
)

add_dependencies(media_plugin_example
  ${LLPLUGIN_LIBRARIES}
  ${MEDIA_PLUGIN_BASE_LIBRARIES}
  # Using ${LLCOMMON_LIBRARIES} here drags in a whole bunch of Boost stuff
  # that only produces CMake warnings about nonexistent dependencies.
  llcommon
)

if (WINDOWS)
  set_target_properties(
    media_plugin_example
    PROPERTIES
    LINK_FLAGS "/MANIFEST:NO /SAFESEH:NO /NODEFAULTLIB:LIBCMT"
    )
endif (WINDOWS)

if (DARWIN)
  # Don't prepend 'lib' to the executable name, and don't embed a full path in the library's install name
  set_target_properties(
    media_plugin_example
    PROPERTIES
    PREFIX ""
    # CA: The directive below invokes a CMP0068 warning on later CMAKE versions and appears to have
    # no actual effect on the output, so comment it
    # BUILD_WITH_INSTALL_RPATH 1
    INSTALL_NAME_DIR "@executable_path"
    LINK_FLAGS "-exported_symbols_list ${CMAKE_CURRENT_SOURCE_DIR}/../base/media_plugin_base.exp"
  )

endif (DARWIN)
