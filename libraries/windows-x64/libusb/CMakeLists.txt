find_library(
	LIBUSB-1.0_LIBRARY
	NAMES "libusb-1.0"
	PATHS .
	PATH_SUFFIXES "lib"
	NO_DEFAULT_PATH
)

add_library(libusb-1.0 INTERFACE)
target_link_libraries(libusb-1.0 INTERFACE ${LIBUSB-1.0_LIBRARY})