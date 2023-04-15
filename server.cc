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
