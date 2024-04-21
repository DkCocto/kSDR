find_library(
    FFTW_LIBRARY
    NAMES "libfftw3-3"
    PATHS .
    PATH_SUFFIXES "lib" "lib64"
    NO_DEFAULT_PATH)

add_library(fftw INTERFACE)
target_include_directories(fftw INTERFACE include/)
target_link_libraries(fftw INTERFACE ${FFTW_LIBRARY})