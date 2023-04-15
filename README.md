# grpc_proto_test
proto test, a demo for grpc

https://www.cnblogs.com/umichan0621/p/16501843.html

## 编写proto文件并生成4个C++文件
- 首先编写proto文件，命名为test.proto，保存在工作目录下
```cc
syntax = "proto3";
 
package test.idl;
 
message Student{
    int32 id=1;
    string name=2;
    int32 score=3;
}
 
message Class{
    int32 class_id=1;
    repeated Student students=2;
}
 
message AvgScore{
    int32 score=1;
}
 
service Test{
    rpc GetAvgScore(Class) returns (AvgScore){}
}
```
proto传输班级的数据结构，每个班级有0~n个学生，学生有个人的信息。
GetAvgScore会计算全班的平均分然后打印。

## 在工作目录下新建文件grpctool.sh，在里面写入
```shell
#! /bin/bash
protoc --cpp_out=./ *.proto
GRPC_CPP_PLUGIN=grpc_cpp_plugin
GRPC_CPP_PLUGIN_PATH=`which ${GRPC_CPP_PLUGIN}`
protoc --grpc_out=./ --plugin=protoc-gen-grpc=${GRPC_CPP_PLUGIN_PATH} *.proto 
```
在工作目录下执行
```
sh ./grpctool.sh
```
此时生成4个文件，在工作目录创建文件夹test_proto，将4个文件移动到文件夹

## 编写服务器代码
在工作目录下创建server.cpp，服务器接受客户端的消息后打印班级中的每个学生信息，然后计算平均分返回给客户端
```cpp
#include <iostream>
#include <memory>
#include <string>
 
#include <grpcpp/grpcpp.h>
#include <grpcpp/health_check_service_interface.h>
#include <grpcpp/ext/proto_server_reflection_plugin.h>
 
#include "test.grpc.pb.h"
using namespace std;
using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;
 
// Logic and data behind the server's behavior.
class ServiceImpl final : public test::idl::Test::Service {
    Status GetAvgScore(
        ServerContext* context, const test::idl::Class* cur_class, test::idl::AvgScore* avg_score) override {
        cout << "Class Id = " << cur_class->class_id() << endl;
        int size = cur_class->students_size();
        int sum = 0;
        for (int i = 0; i < size; ++i) {
            auto cur_student = cur_class->students(i);
            cout << "student:" << cur_student.id() << " " << cur_student.name() << " " << cur_student.score() << endl;
            sum += cur_student.score();
        }
        avg_score->set_score(sum / size);
        return Status::OK;
    }
};
 
void RunServer() {
    std::string server_address("0.0.0.0:50062");
    ServiceImpl service;
 
    grpc::EnableDefaultHealthCheckService(true);
    grpc::reflection::InitProtoReflectionServerBuilderPlugin();
    ServerBuilder builder;
    // Listen on the given address without any authentication mechanism.
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    // Register "service" as the instance through which we'll communicate with
    // clients. In this case it corresponds to an *synchronous* service.
    builder.RegisterService(&service);
    // Finally assemble the server.
    std::unique_ptr<Server> server(builder.BuildAndStart());
    std::cout << "Server listening on " << server_address << std::endl;
 
    // Wait for the server to shutdown. Note that some other thread must be
    // responsible for shutting down the server for this call to ever return.
    server->Wait();
}
 
int main(int argc, char** argv) {
    RunServer();
 
    return 0;
}
```

## 编写客户端代码
在工作目录下创建client.cpp，客户端发送班级和学生信息给服务器，然后接收平均分。
```cpp
#include <iostream>
#include <memory>
#include <string>
 
#include <grpcpp/grpcpp.h>
#include "test.grpc.pb.h"
 
using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;
 
class Client {
public:
    Client(std::shared_ptr<Channel> channel) : stub_(test::idl::Test::NewStub(channel)) {}
 
    int AvgScore() {
        std::vector<std::pair<int, std::pair<std::string, int>>> sudent_list = {
            {1, {"步梦", 90}}, {2, {"果林", 50}}, {3, {"雪菜", 98}}, {4, {"栞子", 97}}, {5, {"彼方", 95}}};
        test::idl::Class cur_class;
        cur_class.set_class_id(312);
        for (auto& st : sudent_list) {
            test::idl::Student* stu = cur_class.add_students();
            stu->set_id(st.first);
            stu->set_name(st.second.first);
            stu->set_score(st.second.second);
        }
 
        test::idl::AvgScore avg_score;
        ClientContext context;
 
        Status status = stub_->GetAvgScore(&context, cur_class, &avg_score);
 
        if (status.ok()) {
            return avg_score.score();
        } else {
            std::cout << status.error_code() << ": " << status.error_message() << std::endl;
            return -1;
        }
    }
 
private:
    std::unique_ptr<test::idl::Test::Stub> stub_;
};
 
int main() {
    std::string target = "localhost:50062";
    Client client(grpc::CreateChannel(target, grpc::InsecureChannelCredentials()));
    int reply = client.AvgScore();
    std::cout << "Avg score = " << reply << std::endl;
    return 0;
}
```
 
## 编写CMake文件
- 在工作目录创建CMakeLists.txt，写入内容如下
```cmake
cmake_minimum_required(VERSION 3.5)
 
project(test)
 
set(CMAKE_CXX_STANDARD 11)
set(PROTO_PATH ${CMAKE_CURRENT_SOURCE_DIR}/test_proto)
 
file(GLOB PROTO_FILES ${PROTO_PATH}/*.cc)
 
find_package(Threads REQUIRED)
find_package(Protobuf CONFIG REQUIRED)
find_package(gRPC CONFIG REQUIRED)
 
message(STATUS "Using protobuf-${Protobuf_VERSION}")
message(STATUS "Using gRPC-${gRPC_VERSION}")
 
include_directories(${PROTO_PATH})
 
foreach(TARGET client server)
    add_executable(${TARGET} "${TARGET}.cpp"
        ${HW_PROTO_SRCS}
        ${HW_GRPC_SRCS}
        ${PROTO_FILES})
 
    target_link_libraries(${TARGET}
        gRPC::grpc++
        gRPC::grpc++_reflection
        protobuf::libprotobuf)
endforeach()
```


