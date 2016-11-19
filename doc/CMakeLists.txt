configure_file(
	"${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in"
	"${CMAKE_CURRENT_BINARY_DIR}/Doxyfile" @ONLY)

file(GLOB_RECURSE headers "../include/*[^~]")

add_custom_command(
	OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/html"
	COMMAND "${DOXYGEN_EXECUTABLE}" "${CMAKE_CURRENT_BINARY_DIR}/Doxyfile"
	DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/Doxyfile" ${headers}
	WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
	VERBATIM)

add_custom_target(doc ALL DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/html")
