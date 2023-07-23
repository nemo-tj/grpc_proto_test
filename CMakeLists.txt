cmake_minimum_required(VERSION 3.5)
 
project(test)
 
set(CMAKE_CXX_STANDARD 11)
set(PROTO_PATH ${CMAKE_CURRENT_SOURCE_DIR}/pb_cc)
 
file(GLOB PROTO_FILES ${PROTO_PATH}/*.cc)
 
find_package(Threads REQUIRED)
find_package(Protobuf CONFIG REQUIRED)
find_package(gRPC CONFIG REQUIRED)
 
message(STATUS "Using protobuf-${Protobuf_VERSION}")
message(STATUS "Using gRPC-${gRPC_VERSION}")
 
include_directories(${PROTO_PATH})
 
foreach(TARGET client server)
    add_executable(${TARGET} "${TARGET}.cc"
        ${PROTO_FILES}
        )
 
    target_link_libraries(${TARGET}
        gRPC::grpc++
        gRPC::grpc++_reflection
        protobuf::libprotobuf)
endforeach()

#        ${HW_PROTO_SRCS}
#        ${HW_GRPC_SRCS}
#        ${PROTO_FILES}
