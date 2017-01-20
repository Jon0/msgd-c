
# util library files
file(GLOB SHARE_SRC_FILES
	"*.c"
)


file(GLOB SHARE_INCLUDE_FILES
	"*.h"
)


add_library(
	${SHARE_LIBRARY_NAME}
	SHARED
	${SHARE_SRC_FILES}
	${SHARE_INCLUDE_FILES}
)


target_link_libraries(
	${SHARE_LIBRARY_NAME}
	${UTIL_LIBRARY_NAME}
)
