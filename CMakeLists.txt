cmake_minimum_required(VERSION 3.0.2)

project(BPMMonitor)

# include all the src files
file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS src/*.cpp)

# include all the include files of the WiringPi library 
file(GLOB_RECURSE WIREPI_INCLUDES CONFIGURE_DEPENDS /usr/local/include/*.h)

add_executable(main_program test.cpp ${SRC_FILES})

# link the wiringPi library
target_link_libraries(main_program ${catkin_LIBRARIES} -lwiringPi)
# https://techoverflow.net/2021/09/05/how-to-fix-c-error-undefined-reference-to-pthread_rwlock_init/
target_link_libraries(main_program pthread) 
target_include_directories(main_program PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)