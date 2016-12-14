
# util library files
file(GLOB SRC_FILES
	"*.c"
)


file(GLOB INCLUDE_FILES
	"*.h"
)


add_library(
	${UTIL_LIBRARY_NAME}
	SHARED
	${SRC_FILES}
	${INCLUDE_FILES}
)


target_link_libraries(
	${UTIL_LIBRARY_NAME}
)
