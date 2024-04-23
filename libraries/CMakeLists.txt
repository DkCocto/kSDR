add_subdirectory(glfw)
add_subdirectory(portaudio)
add_subdirectory(tinyxml2)
add_subdirectory(comport)
add_subdirectory(imgui)
add_subdirectory(stb)

if(WIN32 AND CMAKE_SIZEOF_VOID_P EQUAL 8) # Check if 64-bit windows
 add_subdirectory(windows-x64)
endif()