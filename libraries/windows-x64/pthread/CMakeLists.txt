find_library(
	PTHREADVC2_LIBRARY
	NAMES "pthreadVC2"
	PATHS .
	PATH_SUFFIXES "lib"
	NO_DEFAULT_PATH
)

add_library(pthreadVC2 INTERFACE)
target_link_libraries(pthreadVC2 INTERFACE ${PTHREADVC2_LIBRARY})