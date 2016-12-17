
# util library files
file(GLOB UTIL_SRC_FILES
	"*.c"
)


file(GLOB UTIL_INCLUDE_FILES
	"*.h"
)


add_library(
	${UTIL_LIBRARY_NAME}
	SHARED
	${UTIL_SRC_FILES}
	${UTIL_INCLUDE_FILES}
)


target_link_libraries(
	${UTIL_LIBRARY_NAME}
)
