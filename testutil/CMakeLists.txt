
# executable names
set(TESTUTIL_NAME "test-util")


# client files
file(GLOB TESTUTIL_FILES
	"testutil.c"
)


add_executable(
	${TESTUTIL_NAME}
	${TESTUTIL_FILES}
)


target_link_libraries(
	${TESTUTIL_NAME}
	${UTIL_LIBRARY_NAME}
)
