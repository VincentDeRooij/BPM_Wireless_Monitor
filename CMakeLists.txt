cmake_minimum_required(VERSION 3.0.2)

project(BPMMonitor)

file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
# include all the src files of the BPM Monitor
file(GLOB_RECURSE BPM_SENSOR_SRC_FILES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/HeartSensor/*.cpp)
# include all the src files of the TFT display 
file(GLOB_RECURSE TFTDisplay_SRC_FILES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/TFTDisplay/*.cpp)

# include all the include files of the BPM Monitor library
#file(GLOB_RECURSE BPM_MONITOR_INCLUDES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/include/HeartSensor/*.h)
# include all the include files of the TFTDisplay library
#file(GLOB_RECURSE TFTDisplay_INCLUDES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/include/TFTDisplay/*.h)

add_executable(main_program demo.cpp ${SRC_FILES} ${BPM_SENSOR_SRC_FILES} ${TFTDisplay_SRC_FILES})

# link the wiringPi library
#target_link_libraries(main_program ${catkin_LIBRARIES} -lwiringPi)

target_include_directories(main_program PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(main_program PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include/HeartSensor)
target_include_directories(main_program PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include/TFTDisplay)

# https://techoverflow.net/2021/09/05/how-to-fix-c-error-undefined-reference-to-pthread_rwlock_init/
target_link_libraries(main_program pthread) 

