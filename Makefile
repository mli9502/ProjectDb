CMAKE_DIR_PREFIX = cmake-build-
BUILD := release
CMAKE_DIR := ${CMAKE_DIR_PREFIX}${BUILD}

# For release build, use command "make BUILD=release <target>".

.PHONY: init_build main run_main benchmark run_benchmark tests run_tests docker_test docker_coverage pdf_regen_all zip_files clean

init_build:
	-@rm -rf $(CMAKE_DIR)
	mkdir $(CMAKE_DIR) && cd $(CMAKE_DIR) && cmake -DCMAKE_BUILD_TYPE=$(shell echo $(BUILD) | sed 's/./\U&/') ..

projectdb:
	cd $(CMAKE_DIR) && cmake --build . --target projectdb

main:
	cd $(CMAKE_DIR) && cmake --build . --target main

run_main:
	$(MAKE) -s main
	-@rm -rf $(CMAKE_DIR)/projectdb
	cd $(CMAKE_DIR) && ./main

benchmark:
	cd $(CMAKE_DIR) && cmake --build . --target benchmark

run_benchmark:
	$(MAKE) -s benchmark
	cd $(CMAKE_DIR)/benchmark && ./benchmark

# Force debug build for test.
tests:
	cd $(CMAKE_DIR) && cmake --build . --target tests

run_tests:
	$(MAKE) -s tests
	cd $(CMAKE_DIR)/tests && ./tests

coverage:
	cd ${CMAKE_DIR_PREFIX}coverage && cmake --build . --target tests
	cd ${CMAKE_DIR_PREFIX}coverage/tests && ./tests
	# exp for exclude: https://github.com/gcovr/gcovr/issues/151
	gcovr cmake-build-coverage/ -r . --exclude-directories '.*tests.*' --html -o docs/coverage.html

# Target used by CI.
docker_test:
	docker build -t projectdb_img .
	docker run --rm -v ${PWD}:/projectdb projectdb_img test

# Target used by CI.
# Runs coverage and copy the generated report from docker to docs/.
docker_coverage:
	docker build -t projectdb_img .
	docker run --rm -v ${PWD}:/projectdb projectdb_img coverage

pdf_regen_all:
	@for file in $(shell ls *.md); do pandoc $${file} -V geometry:margin=.5in --pdf-engine=xelatex -o $${file%.*}.pdf; done

zip_files:
	-rm *.zip
	zip ml4643_MengwenLi_hw5.zip utils/* main.cpp CMakeLists.txt Makefile ml4643_MengwenLi_hw5_report.pdf

clean:
	@rm -rf cmake-build-*
	@rm main main.debug main.release *.zip *.pdf *.o *.d $(OBJ_DIR)/*.o 2>/dev/null || true

