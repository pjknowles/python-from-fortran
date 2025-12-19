FROM continuumio/miniconda3
RUN conda install -c conda-forge -y fortran-compiler c-compiler ninja cxx-compiler cmake gdb make
ENV CMAKE_GENERATOR=Ninja