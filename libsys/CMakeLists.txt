
# set rt link
set(SYS_RT_LIBS "rt")

find_package(Fuse REQUIRED)

set(CMAKE_C_FLAGS "-D_FILE_OFFSET_BITS=64 -DFUSE_USE_VERSION=26")


# util library files
file(GLOB SRC_FILES
	"*.c"
)


file(GLOB INCLUDE_FILES
	"*.h"
)


add_library(
	${SYS_LIBRARY_NAME}
	SHARED
	${SRC_FILES}
	${INCLUDE_FILES}
)


target_link_libraries(
	${SYS_LIBRARY_NAME}
	${CMAKE_THREAD_LIBS_INIT}
	${CMAKE_DL_LIBS}
	${FUSE_LIBRARIES}
    ${SYS_RT_LIBS}
)
