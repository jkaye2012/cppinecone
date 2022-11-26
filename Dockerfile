FROM ubuntu:jammy
USER root

RUN apt update
RUN apt install -y cmake ninja-build git libclang-common-14-dev libclang-cpp14 clang-14 clang-14-doc libclang1-14  \
    libncurses-dev libreadline8 libstdc++-11-dev libtinfo-dev llvm-14 llvm-14-dev llvm-14-linker-tools llvm-14-runtime \
    llvm-14-tools readline-common libssl-dev
RUN ln -sf /usr/bin/clang-14 /usr/local/bin/clang
RUN ln -sf /usr/bin/clang++-14 /usr/local/bin/clang++
ENV CC=clang CXX=clang++

CMD [ "/bin/bash" ]

