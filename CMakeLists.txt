cmake_minimum_required(VERSION 2.8.3)
project(ipcamera)

set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-std=c++11 -Wall")
set(CMAKE_BUILD_TYPE RelWithDebInfo)
set(WITH_GSTREAMER ON)
set(WITH_FFMPEG ON)
set(WITH_FFMPEGS "ON")

find_package(OpenCV REQUIRED)
find_package(Boost REQUIRED)
find_package(catkin REQUIRED COMPONENTS
      roscpp 
      cv_bridge 
      image_transport
      sensor_msgs
      genmsg
      )


catkin_package(
      LIBRARIES ${PROJECT_NAME}
      CATKIN_DEPENDS roscpp cv_bridge image_transport sensor_msgs message_runtime
      DEPENDS OpenCV
      )


include_directories(
      include
      ${catkin_INCLUDE_DIRS}
      ${Boost_INCLUDE_DIRS}
      )

add_library(${PROJECT_NAME}
      src/ipcamera_publisher.cpp
      )

target_link_libraries(${PROJECT_NAME}
      ${catkin_LIBRARIES}
      )

add_executable(ipcamera_publisher src/ipcamera_publisher.cpp)

target_link_libraries(ipcamera_publisher
      ${PROJECT_NAME}
      ${catkin_LIBRARIES}
      ${OpenCV_LIBRARIES}
      )

add_dependencies(ipcamera_publisher
      ${catkin_EXPORTED_TARGETS}
      ${PROJECT_NAME}_gencpp
      ${${PROJECT_NAME}_EXPORTED_TARGETS}
      )
