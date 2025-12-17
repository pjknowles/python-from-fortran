FROM continuumio/miniconda3
RUN conda install fortran-compiler c-compiler ninja cxx-compiler cmake