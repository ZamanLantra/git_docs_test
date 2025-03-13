Introduction
============

Overview
--------

`OP-PIC <https://github.com/OP-DSL/OP-PIC>`_ is a high-level embedded domain specific language (eDSL) for writing **unstructured mesh** Particle in Cell algorithms with automatic parellelisation on multi-core and many-core architectures. The API is embedded in C/C++.

The current OP-PIC eDSL supports generating code targeting multi-core CPUs with OpenMP threading, many-core GPUs with CUDA offloading, and distributed memory cluster variants of these using MPI. 
Particularly parallelizations are, sequential, OpenMP, MPI, CUDA, HIP, SYCL and distributed memory OpenMP+MPI, CUDA+MPI, HIP+MPI, SYCL+MPI.

These pages provide detailed documentation on using OP-PIC, including an installation guide, an overview of the C++ API, a walkthrough of the development of an example application, and developer documentation.

Licencing
---------

OP-PIC is released as an open-source project under the BSD 3-Clause License. See the `LICENSE <https://github.com/OP-DSL/OP-PIC/blob/main/LICENSE>`_ file for more information.

Citing
------

To cite OP-PIC, please reference the following paper:

`Zaman Lantra, Steven A. Wright, and Gihan R. Mudalige. 2024. OP-PIC - an Unstructured-Mesh Particle-in-Cell DSL for Developing Nuclear Fusion Simulations. In Proceedings of the 53rd International Conference on Parallel Processing (ICPP '24). Association for Computing Machinery, New York, NY, USA, 294–304. <https://doi.org/10.1145/3673038.3673130>`_

BibTeX citation:
---------------

.. code-block:: bibtex

    @inproceedings{10.1145/3673038.3673130,
      author = {Lantra, Zaman and Wright, Steven A. and Mudalige, Gihan R.},
      title = {OP-PIC - an Unstructured-Mesh Particle-in-Cell DSL for Developing Nuclear Fusion Simulations},
      year = {2024},
      isbn = {9798400717932},
      publisher = {Association for Computing Machinery},
      address = {New York, NY, USA},
      url = {https://doi.org/10.1145/3673038.3673130},
      doi = {10.1145/3673038.3673130},
      pages = {294–304},
      numpages = {11},
      keywords = {DSL, OP-PIC, PIC, Particle-In-Cell, Unstructured-mesh},
      location = {Gotland, Sweden},
      series = {ICPP '24}
    }
