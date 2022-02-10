cpp_out_path := ../src/protocols
python_out_path := ../cv

.default:
	gen-cpp

gen-cpp:
	protoc -I . --cpp_out=$(cpp_out_path) --grpc_out=$(cpp_out_path) --plugin=protoc-gen-grpc=`which grpc_cpp_plugin` ./*.proto

gen-python:
	python3 -m grpc_tools.protoc -I./ --python_out=$(python_out_path) --grpc_python_out=$(python_out_path) ./*.proto

