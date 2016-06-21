
# main library files
file(GLOB SRC_FILES
	"*.c"
)


file(GLOB INCLUDE_FILES
	"*.h"
)


add_library(
	${MSG_LIBRARY_NAME}
	SHARED
	${SRC_FILES}
	${INCLUDE_FILES}
)


target_link_libraries(
	${MSG_LIBRARY_NAME}
	${UTIL_LIBRARY_NAME}
)
