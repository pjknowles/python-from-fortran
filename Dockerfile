FROM continuumio/miniconda3
RUN conda install -c conda-forge -y fortran-compiler c-compiler ninja cxx-compiler cmake gdb make python=3.10
ENV CMAKE_GENERATOR=Ninja
RUN apt update
RUN apt-get install -y python3-dev
