cmake_minimum_required(VERSION 3.5)
 
project(test)
 
set(CMAKE_CXX_STANDARD 11)
 
 
find_package(Threads REQUIRED)
find_package(Protobuf CONFIG REQUIRED)
find_package(gRPC CONFIG REQUIRED)
 
message(STATUS "Using protobuf-${Protobuf_VERSION}")
message(STATUS "Using gRPC-${gRPC_VERSION}")
 
# add_subdirectory(pb_cc)

 
foreach(TARGET client server)
    add_executable(${TARGET} 
			"${TARGET}.cc")
 
    target_link_libraries(${TARGET}
				nemo::pb_cc
        gRPC::grpc++
        gRPC::grpc++_reflection
        protobuf::libprotobuf)
endforeach()

#        ${HW_PROTO_SRCS}
#        ${HW_GRPC_SRCS}
#        ${PROTO_FILES}

# set(PROTO_PATH ${CMAKE_CURRENT_SOURCE_DIR}/pb_cc)
# include_directories(${PROTO_PATH})
# file(GLOB PROTO_FILES ${PROTO_PATH}/proto/*.cc)
# message(STATUS "pro--------------> ${PROTO_FILES}")
# foreach(TA ${PROTO_FILES})
# 	message(STATUS "pro--------------> ${TA}")
# endforeach()
#file(GLOB CC_FILES "./*.cc")
