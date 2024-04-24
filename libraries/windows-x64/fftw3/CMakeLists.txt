find_library(
	FFTW3_LIBRARY
	NAMES "libfftw3-3"
	PATHS .
	PATH_SUFFIXES "lib"
	NO_DEFAULT_PATH
)

add_library(fftw3 INTERFACE)
target_include_directories(fftw3 INTERFACE include)
target_link_libraries(fftw3 INTERFACE ${FFTW3_LIBRARY})