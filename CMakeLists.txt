cmake_minimum_required(VERSION 3.16)
project(ASCII_ISP CXX)

# Standardize C++ Environment
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find Dependencies
find_package(OpenCV REQUIRED)

# Define the Executable
# Standardizing the name to 'ascii_isp' for consistency across platforms.
add_executable(ascii_isp main.cpp)

# Link Libraries & Include Directories
target_link_libraries(ascii_isp PRIVATE ${OpenCV_LIBS})
target_include_directories(ascii_isp PRIVATE ${OpenCV_INCLUDE_DIRS})

# Windows-Specific DLL Deployment
# This block is automatically skipped on macOS/Linux but is 
# required on Windows for shared OpenCV builds to run properly.
if(WIN32 AND NOT OpenCV_STATIC)
  set(_opencv_bindir "${OpenCV_DIR}/${OpenCV_ARCH}/${OpenCV_RUNTIME}/bin")
  file(GLOB _opencv_world_dlls "${_opencv_bindir}/opencv_world*.dll")
  foreach(_dll ${_opencv_world_dlls})
    add_custom_command(
      TARGET ascii_isp
      POST_BUILD
      COMMAND "${CMAKE_COMMAND}" -E copy_if_different "${_dll}" "$<TARGET_FILE_DIR:ascii_isp>"
      VERBATIM)
  endforeach()
endif()
