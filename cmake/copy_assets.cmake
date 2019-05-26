# Copy contents of assets directory to executable output directory
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory
	"${PROJECT_SOURCE_DIR}/assets"
	"$<TARGET_FILE_DIR:${PROJECT_NAME}>")
