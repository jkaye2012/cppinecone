FROM ubuntu:jammy
USER root

RUN apt update && apt install -y cmake ninja-build git libclang-common-14-dev libclang-cpp14 clang-14 clangd-14 clang-14-doc libclang1-14  \
    clang-tidy-14 clang-format-14 libncurses-dev libreadline8 libstdc++-11-dev libtinfo-dev llvm-14 llvm-14-dev \
    llvm-14-linker-tools llvm-14-runtime llvm-14-tools readline-common libssl-dev python3 python3-pip doxygen graphviz inotify-tools
RUN ln -sf /usr/bin/clang-14 /usr/local/bin/clang
RUN ln -sf /usr/bin/clang++-14 /usr/local/bin/clang++
RUN ln -sf /usr/bin/clangd-14 /usr/local/bin/clangd
RUN ln -sf /usr/bin/clang-tidy-14 /usr/local/bin/clang-tidy
RUN ln -sf /usr/bin/clang-format-14 /usr/local/bin/clang-format

RUN pip install mkdocs-material
EXPOSE 8000

ENV CC=clang CXX=clang++
WORKDIR /app

CMD [ "/bin/bash" ]

