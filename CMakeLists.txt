cmake_minimum_required(VERSION 3.0.2)

project(OpenCV_Basics)

# Find OpenCV package
find_package(OpenCV REQUIRED)

# Add the source file (referencing the relative path)
add_executable(main main.cpp)

# Link OpenCV libraries
target_link_libraries(main ${OpenCV_LIBS})
