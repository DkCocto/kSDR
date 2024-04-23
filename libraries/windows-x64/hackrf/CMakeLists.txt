find_library(
	HACKRF_LIBRARY
	NAMES "hackrf"
	PATHS .
	PATH_SUFFIXES "lib"
	NO_DEFAULT_PATH
)

add_library(hackrf INTERFACE)
target_include_directories(hackrf INTERFACE include)
target_link_libraries(hackrf INTERFACE ${HACKRF_LIBRARY})