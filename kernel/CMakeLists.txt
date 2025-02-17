cmake_minimum_required(VERSION 3.10)

project(easyai)

if (ENABLE_ASAN)
	message(STATUS "BUILD WITH ADDRESS SANITIZER")
	set (CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address")
	set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address")
	set (CMAKE_LINKER_FLAGS_DEBUG "${CMAKE_LINKER_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address")
endif ()

set(OpenCV_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty/opencv-mobile-4.9.0-luckfox-pico/lib/cmake/opencv4")
find_package(OpenCV REQUIRED)

set(rknpu2_yolov5_file rknpu2/yolov5.cc)
if (TARGET_SOC STREQUAL "rv1106" OR TARGET_SOC STREQUAL "rv1103")
    add_definitions(-DRV1106_1103)
    set(rknpu2_yolov5_file rknpu2/yolov5_rv1106_1103.cc)
    #dma
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty/allocator/dma)
    #alsa 
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty/alsa/include)
    # i2c
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty/i2clib/include)
endif()

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty/ 3rdparty.out)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../utils/ utils.out)

set(CMAKE_INSTALL_RPATH "$ORIGIN/lib")

file(GLOB SRCS ${CMAKE_CURRENT_SOURCE_DIR}/*.cc)

include_directories(${OpenCV_INCLUDE_DIRS})
add_library(algo SHARED
        postprocess.cc
    	${rknpu2_yolov5_file}
        )
target_include_directories(algo PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${LIBRKNNRT_INCLUDES}
    ${LIBALSA_INCLUDES}
    ../3rdparty/ipcm/baselib/
    ../3rdparty/rknpu2/include
    ../utils
    .././3rdparty/librga/include
)

add_executable(${PROJECT_NAME}
    #postprocess.cc
    core.cc
    ../app/lis3dh.cc
    ../app/main.cc
    ../app/device.cc
    ../app/alsa.cc
    ../app/is_night.cc
    ../3rdparty/ipcm/baselib/api.cc
    ../3rdparty/ipcm/baselib/mongoose.cc
)

target_link_libraries(${PROJECT_NAME}
    imageutils
    fileutils
    imagedrawing    
    ${LIBRKNNRT}
    ${OpenCV_LIBS}
    ${LIBALSA}
    ${LIBI2C}
    ${LIBALGO}
)

if (CMAKE_SYSTEM_NAME STREQUAL "Android")
    target_link_libraries(${PROJECT_NAME}
    log
)
endif()

if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set(THREADS_PREFER_PTHREAD_FLAG ON)
    find_package(Threads REQUIRED)
    target_link_libraries(${PROJECT_NAME} Threads::Threads)
endif()

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${LIBRKNNRT_INCLUDES}
    ${LIBALSA_INCLUDES}
    ../3rdparty/ipcm/baselib/
)

install(TARGETS ${PROJECT_NAME} DESTINATION .)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/model/labels_list.txt DESTINATION ./model)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/model/check_people.wav DESTINATION ./model)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/model/welcome.wav DESTINATION ./model)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/model/check_sleep.wav DESTINATION ./model)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/model/normal0.wav DESTINATION ./model)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/model/normal1.wav DESTINATION ./model)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/model/normal2.wav DESTINATION ./model)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/model/bb.wav DESTINATION ./model)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/model/sleep_v55.rknn DESTINATION ./model)
#file(GLOB RKNN_FILES "${CMAKE_CURRENT_SOURCE_DIR}/../model/*.rknn")
#install(FILES ${RKNN_FILES} DESTINATION model)
