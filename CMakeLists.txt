cmake_minimum_required(VERSION 3.17)
project (qr_desktop)

find_package (OpenCV REQUIRED)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x -g -O3 -ftree-vectorize")

include_directories("./qr")
aux_source_directory ("./qr" QR_SRC_LIST)
aux_source_directory ("./qr/reedsolomon" RS_SRC_LIST)
set(COMMON_SRC ${QR_SRC_LIST} ${RS_SRC_LIST})

add_executable(qr_desktop ${COMMON_SRC} QRCode_Mac/QRCode_Mac/main.cpp)
target_link_libraries(qr_desktop ${OpenCV_LIBS} -pthread)
