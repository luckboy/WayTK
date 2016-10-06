aux_source_directory("${CMAKE_CURRENT_SOURCE_DIR}" waytk_sources)

list(APPEND waytk_include_directories ${CAIRO_INCLUDE_DIRS})
list(APPEND waytk_include_directories ${WAYLAND_CLIENT_INCLUDE_DIRS})
list(APPEND waytk_include_directories ${WAYLAND_CURSOR_INCLUDE_DIRS})
list(APPEND waytk_include_directories ${XKBCOMMON_INCLUDE_DIRS})
list(APPEND waytk_include_directories ${YAML_INCLUDE_DIRS})

list(APPEND waytk_link_directories ${CAIRO_LIBRARY_DIRS})
list(APPEND waytk_link_directories ${WAYLAND_CLIENT_LIBRARY_DIRS})
list(APPEND waytk_link_directories ${WAYLAND_CURSOR_LIBRARY_DIRS})
list(APPEND waytk_link_directories ${XKBCOMMON_LIBRARY_DIRS})
list(APPEND waytk_link_directories ${YAML_LIBRARY_DIRS})

list(APPEND waytk_libraries ${CAIRO_LIBRARIES})
list(APPEND waytk_libraries ${WAYLAND_CLIENT_LIBRARIES})
list(APPEND waytk_libraries ${WAYLAND_CURSOR_LIBRARIES})
list(APPEND waytk_libraries ${XKBCOMMON_LIBRARIES})
list(APPEND waytk_libraries ${YAML_LIBRARIES})

include_directories(${waytk_include_directories})
link_directories(${waytk_link_directories})

if(BUILD_SHARED_LIBS)
	add_library(waytk SHARED ${waytk_sources})
	target_link_libraries(waytk ${waytk_libraries})
endif(BUILD_SHARED_LIBS)

if(BUILD_STATIC_LIBS)
	add_library(waytk_static STATIC ${waytk_sources})
	set_target_properties(waytk_static PROPERTIES OUTPUT_NAME waytk)
	target_link_libraries(waytk_static ${waytk_libraries})
endif(BUILD_STATIC_LIBS)
