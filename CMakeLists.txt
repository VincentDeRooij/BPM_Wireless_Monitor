cmake_minimum_required(VERSION 3.0.2)

project(BPMMonitor)

# include all the src files of the BPM Monitor
file(GLOB_RECURSE BPM_SENSOR_SRC_FILES CONFIGURE_DEPENDS src/HeartSensor/*.cpp)
# include all the src files of the TFT display 
file(GLOB_RECURSE TFTDisplay_SRC_FILES CONFIGURE_DEPENDS src/TFTDisplay/*.cpp)

# include all the include files of the BPM Monitor library
file(GLOB_RECURSE BPM_MONITOR_INCLUDES CONFIGURE_DEPENDS include/HeartSensor/*.h)
# include all the include files of the TFTDisplay library
file(GLOB_RECURSE TFTDisplay_INCLUDES CONFIGURE_DEPENDS include/TFTDisplay/*.h)

add_executable(main_program main.cpp ${BPM_SENSOR_SRC_FILES} ${TFTDisplay_SRC_FILES})

# link the wiringPi library
#target_link_libraries(main_program ${catkin_LIBRARIES} -lwiringPi)

# https://techoverflow.net/2021/09/05/how-to-fix-c-error-undefined-reference-to-pthread_rwlock_init/
target_link_libraries(main_program pthread) 

target_include_directories(main_program PUBLIC ${BPM_MONITOR_INCLUDES} ${TFTDisplay_INCLUDES})
