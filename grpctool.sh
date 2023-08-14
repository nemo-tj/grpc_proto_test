#! /bin/bash
set -x
gen_dir=./pb_cc
mkdir -p $gen_dir
proto_dir=proto
protoc --cpp_out=${gen_dir} ${proto_dir}/*.proto
GRPC_CPP_PLUGIN=grpc_cpp_plugin
GRPC_CPP_PLUGIN_PATH=`which ${GRPC_CPP_PLUGIN}`
protoc --grpc_out=${gen_dir} --plugin=protoc-gen-grpc=${GRPC_CPP_PLUGIN_PATH} ${proto_dir}/*.proto 

cd pb_cc
ln -sf ../proto/CMakeLists.txt .
cd -
