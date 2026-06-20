# Use an alma9 image to install CrestApi
FROM registry.cern.ch/docker.io/almalinux:9.5

# set up environment
ENV USR=crestclient
ENV USER=crestclient
ENV home_dir=/home/${USR}
RUN mkdir -p ${home_dir}/build

WORKDIR ${home_dir}
# Install the necessary packages
#RUN dnf install -y curl

# Install the CrestApi
COPY CrestApi ${home_dir}/CrestApi
# COPY nlohmann ${home_dir}/nlohmann
COPY CMakeLists.txt ${home_dir}/CMakeLists.txt
COPY cmake ${home_dir}/cmake
COPY src ${home_dir}/src
COPY tools ${home_dir}/tools

# Build the CrestApi
RUN dnf install -y gcc-c++ make cmake
RUN dnf install -y boost-devel curl-devel nc git openssl openssl-devel
# Install nlohmann/json from source
RUN git clone https://github.com/nlohmann/json.git /tmp/json && \
    cd /tmp/json && \
    cmake -B build -S . && \
    cmake --install build --prefix /usr/local && \
    rm -rf /tmp/json

COPY test ${home_dir}/test
COPY examples ${home_dir}/examples
RUN cd ${home_dir}/build && cmake ..
RUN cd ${home_dir}/build && make && make install

CMD [ "bash" ]