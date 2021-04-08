FROM ubuntu:20.04

RUN apt update && \
    apt-get install -y software-properties-common && \
    add-apt-repository ppa:ubuntu-toolchain-r/ppa && \
    apt update && \
    apt install -y g++-10

# Use gcc-10 and g++-10.
RUN update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-10 90 --slave /usr/bin/g++ g++ /usr/bin/g++-10

# Install cmake and make.
RUN apt install -y cmake
RUN apt install -y make

# Install gtest.
RUN apt install -y wget
RUN apt install -y unzip
RUN wget https://github.com/google/googletest/archive/release-1.10.0.zip && unzip release-1.10.0.zip && rm release-1.10.0.zip
RUN cd googletest-release-1.10.0 && \
    cp -r googletest/include/gtest /usr/include && \
    cp -r googlemock/include/gmock /usr/include && \
    mkdir cmake-build && \
    cd cmake-build && \
    cmake .. && \
    make && \
    cp lib/libg* /usr/lib && \
    cp lib/libg* /usr/local/lib
RUN rm -rf ./googletest-release-1.10.0

#WORKDIR /root

#CMD ["bash"]

copy . ./project_db

# init build directory.
run cd project_db && make init_build

# run test by default when launch the image.
cmd cd project_db && make run_main

