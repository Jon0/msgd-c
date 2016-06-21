
# executable names
set(MSGD_NAME "msgd")


# client files
file(GLOB MSGD_FILES
	"main.c"
)


add_executable(
	${MSGD_NAME}
	${MSGD_FILES}
)


target_link_libraries(
	${MSGD_NAME}
	${UTIL_LIBRARY_NAME}
	${MSG_LIBRARY_NAME}
)
