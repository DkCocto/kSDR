find_library(
    RTLSDR_LIBRARY
    NAMES "rtlsdr"
    PATHS .
    PATH_SUFFIXES "lib" "lib64"
    NO_DEFAULT_PATH)

add_library(rtlsdr INTERFACE)
target_include_directories(rtlsdr INTERFACE include/)
target_link_libraries(rtlsdr INTERFACE ${RTLSDR_LIBRARY})