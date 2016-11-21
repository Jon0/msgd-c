
# executable names
set(MSGCMD_NAME "msgcmd")


# client files
file(GLOB MSGCMD_FILES
	"main.c"
)


add_executable(
	${MSGCMD_NAME}
	${MSGCMD_FILES}
)


target_link_libraries(
	${MSGCMD_NAME}
	${UTIL_LIBRARY_NAME}
	${MSG_LIBRARY_NAME}
)
