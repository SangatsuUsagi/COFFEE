add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_if_different
	"${LIBRARY_PATH}/libEGL.dll"
	"${LIBRARY_PATH}/libGLESv2.dll"
	"$<TARGET_FILE_DIR:${PROJECT_NAME}>")
