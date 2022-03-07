mkfile_path := $(abspath $(lastword $(MAKEFILE_LIST)))
mkfile_dir := $(dir $(mkfile_path))

default:
	if [ ! -d "build" ]; then \
		mkdir build; \
	fi
	make -C protos
	cmake -S . -B ./build
	make -C build 
	cp ./build/RPi ./bin


run_rpi:
	./bin/RPi --serial="/dev/pts/3"\
		--vision=localhost:50051\
		--cameraopen=1\
		--instructiondelay=350\
		--failcapturedelay=2000\
		--captureretries=3

run_camera:
	python cv/main.py

gen-protos:
	make -C protos

run-serial:
	socat -d -d pty,raw,echo=0 pty,raw,echo=0

# make_cv:
# 	cmake -D CMAKE_BUILD_TYPE=RELEASE \
# 		-D CMAKE_INSTALL_PREFIX=/usr/local \
# 		-D OPENCV_EXTRA_MODULES_PATH=../../opencv_contrib/modules \
# 		-D ENABLE_NEON=ON \
# 		-D ENABLE_VFPV3=ON \
# 		-D BUILD_TESTS=OFF \
# 		-D INSTALL_PYTHON_EXAMPLES=OFF \
# 		-D OPENCV_ENABLE_NONFREE=ON \
# 		-D CMAKE_SHARED_LINKER_FLAGS='-latomic' \
# 		-D BUILD_EXAMPLES=OFF ..
