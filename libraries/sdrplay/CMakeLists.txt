find_library(
    SDRPLAY_LIBRARY
    NAMES "sdrplay_api"
    PATHS .
    PATH_SUFFIXES "lib" "lib64"
    NO_DEFAULT_PATH)

add_library(sdrplay INTERFACE)
target_include_directories(sdrplay INTERFACE include/)
target_link_libraries(sdrplay INTERFACE ${SDRPLAY_LIBRARY})