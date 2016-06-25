
# executable names
set(TEST_NAME "testutil")


# client files
file(GLOB TEST_FILES
	"testutil.c"
)


add_executable(
	${TEST_NAME}
	${TEST_FILES}
)


target_link_libraries(
	${TEST_NAME}
	${UTIL_LIBRARY_NAME}
)
