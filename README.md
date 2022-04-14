# RPi Module
The RPi module is writen in `C++` 

# Unix Dependencies

## Serial
```
git clone https://github.com/WiringPi/WiringPi.git
cd WiringPi
./build
```

## Bluetooth
```
sudo apt-get install bluez libbluetooth-dev
```

## gRPC
[Guide](https://github.com/grpc/grpc/blob/master/test/distrib/cpp/run_distrib_test_cmake_module_install.sh#L38)
```
# Install openssl (to use instead of boringssl)
$ apt-get update && apt-get install -y libssl-dev

$ sudo apt install -y build-essential autoconf libtool pkg-config

$ git clone --recurse-submodules -b v1.42.0 https://github.com/grpc/grpc

# Use externally provided env to determine build parallelism, otherwise use default.
$ GRPC_CPP_DISTRIBTEST_BUILD_COMPILER_JOBS=${GRPC_CPP_DISTRIBTEST_BUILD_COMPILER_JOBS:-4}

$ cd grpc
$ mkdir -p "cmake/build"
$ cd cmake/build

cmake \
  -DCMAKE_BUILD_TYPE=Release \
  -DgRPC_INSTALL=ON \
  -DgRPC_BUILD_TESTS=OFF \
  -DgRPC_SSL_PROVIDER=package \
  ../..

make "-j${GRPC_CPP_DISTRIBTEST_BUILD_COMPILER_JOBS}" install
```


# Getting Started
```
# Creates cmake build list and runs make on target
$ make  -j1
```

# Generating Protofiles
Note: Proto files have been generated and commited to src. 
```
$ make gen-protos
```
