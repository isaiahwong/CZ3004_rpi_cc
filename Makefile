default:
	if [ ! -d "build" ]; then \
		mkdir build; \
	fi
	cmake -S . -B ./build
	make -C build 
	./build/RPi