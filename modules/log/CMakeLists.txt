set(MODULE_NAME "log")


# module files
file(GLOB SRC_FILES
	"*.c"
)


file(GLOB INCLUDE_FILES
	"*.h"
)


add_library(
	${MODULE_NAME}
	SHARED
	${SRC_FILES}
	${INCLUDE_FILES}
)


target_link_libraries(
	${MODULE_NAME}
	${UTIL_LIBRARY_NAME}
    ${MSG_LIBRARY_NAME}
)
