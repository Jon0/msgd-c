
# executable names
set(TESTU_NAME "testutil")


# client files
file(GLOB TESTU_FILES
	"testutil.c"
)


add_executable(
	${TESTU_NAME}
	${TESTU_FILES}
)


target_link_libraries(
	${TESTU_NAME}
	${UTIL_LIBRARY_NAME}
)
