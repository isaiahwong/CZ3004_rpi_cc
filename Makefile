default:
	cmake -S ./src -B ./build
	make -C build 
	./build/RPi