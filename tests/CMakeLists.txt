
# executable names
set(TEST_NAME "msgd-test")


# client files
file(GLOB TEST_FILES
	"test.c"
)


add_executable(
	${TEST_NAME}
	${TEST_FILES}
)


target_link_libraries(
	${TEST_NAME}
	${UTIL_LIBRARY_NAME}
	${SYS_LIBRARY_NAME}
	${MSG_LIBRARY_NAME}
)
