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
