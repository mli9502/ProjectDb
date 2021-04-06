BUILD := debug
CMAKE_DIR := cmake-build-${BUILD}

# For release build, use command "make BUILD=release <target>".

.PHONY: init_build main run_main tests run_tests build_and_run_docker pdf_regen_all zip_files clean

init_build:
	-@rm -rf $(CMAKE_DIR)
	mkdir $(CMAKE_DIR) && cd $(CMAKE_DIR) && cmake -DCMAKE_BUILD_TYPE=$(shell echo $(BUILD) | sed 's/./\U&/') ..

projectdb:
	cd $(CMAKE_DIR) && cmake --build . --target projectdb

main:
	cd $(CMAKE_DIR) && cmake --build . --target main

run_main:
	$(MAKE) -s main
	# -@rm -rf $(CMAKE_DIR)/projectdb
	cd $(CMAKE_DIR) && ./main

tests:
	cd $(CMAKE_DIR) && cmake --build . --target tests

run_tests:
	$(MAKE) -s tests
	cd $(CMAKE_DIR)/tests && ./tests

build_and_run_docker:
	docker build -t cpp_project .
	docker run cpp_project

pdf_regen_all:
	@for file in $(shell ls *.md); do pandoc $${file} -V geometry:margin=.5in --pdf-engine=xelatex -o ml4643_MengwenLi_$${file%.*}.pdf; done

zip_files:
	-rm *.zip
	zip ml4643_MengwenLi_hw5.zip utils/* main.cpp CMakeLists.txt Makefile ml4643_MengwenLi_hw5_report.pdf

clean:
	@rm -rf cmake-build-*
	@rm main main.debug main.release *.zip *.pdf *.o *.d $(OBJ_DIR)/*.o 2>/dev/null || true

