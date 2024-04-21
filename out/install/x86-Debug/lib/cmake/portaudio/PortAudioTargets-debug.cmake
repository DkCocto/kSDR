#----------------------------------------------------------------
# Generated CMake target import file for configuration "Debug".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "PortAudio::PortAudio" for configuration "Debug"
set_property(TARGET PortAudio::PortAudio APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(PortAudio::PortAudio PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "C"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/lib/portaudio.lib"
  )

list(APPEND _cmake_import_check_targets PortAudio::PortAudio )
list(APPEND _cmake_import_check_files_for_PortAudio::PortAudio "${_IMPORT_PREFIX}/lib/portaudio.lib" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
