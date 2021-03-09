BUILD := debug
CMAKE_DIR := cmake-build-${BUILD}

.PHONY: init_build main run_main tests run_tests build_and_run_docker clean

init_build:
	-@rm -rf $(CMAKE_DIR)
	mkdir $(CMAKE_DIR) && cd $(CMAKE_DIR) && cmake -DCMAKE_BUILD_TYPE=$(shell echo $(BUILD) | sed 's/./\U&/') ..

main:
	cd $(CMAKE_DIR) && cmake --build . --target main

run_main:
	$(MAKE) -s main
	cd $(CMAKE_DIR) && ./main

tests:
	cd $(CMAKE_DIR) && cmake --build . --target tests

run_tests:
	$(MAKE) -s tests
	cd $(CMAKE_DIR)/tests && ./tests

build_and_run_docker:
	docker build -t cpp_project .
	docker run cpp_project

clean:
	@rm -rf cmake-build-*
	@rm main main.debug main.release *.zip *.pdf *.o *.d $(OBJ_DIR)/*.o 2>/dev/null || true

