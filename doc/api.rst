OP-PIC C++ API
==============

Initialisation and Termination
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. c:function:: void opp_init(int argc, char **argv)

   This routine must be called before all other OP-PIC routines. 
   If `USE_PETSC` is defined during compilation, `opp_init` will initialize PETSc library as well (using :c:func:`PetscInitialize()`).
   Under MPI back-ends, this routine also calls :c:func:`MPI_Init()` unless its already called previously.

   :param argc: The number of command line arguments.
   :param argv: The command line arguments, as passed to :c:func:`main()`.

.. c:function:: void opp_exit()

   This routine must be called last to cleanly terminate the OP-PIC runtime. 
   If `USE_PETSC` is defined during compilation, `opp_exit` will finalize PETSc library as well (using :c:func:`PetscFinalize()`).
   Under MPI back-ends, this routine also calls :c:func:`MPI_Finalize()` unless its has been called previously. 
   A runtime error will occur if :c:func:`MPI_Finalize()` is called after :c:func:`opp_exit()`.

.. c:function:: opp_set opp_decl_set(int size, char const *name)

   This routine declares a mesh set.

   :param size: Number of set elements.
   :param name: A name to be used for output diagnostics.
   :returns: A set ID.

.. c:function:: opp_set opp_decl_particle_set(char const *name, opp_set cells_set)

   This routine declares a particle set with zero particles in it.

   :param name: A name to be used for output diagnostics.
   :param size: The underlying mesh set related.
   :returns: A set ID.

.. c:function:: opp_set opp_decl_particle_set(int size, char const *name, opp_set cells_set)

   This routine declares a particle set with a given number of particles in it.

   :param size: Number of particles in the set.
   :param name: A name to be used for output diagnostics.
   :param size: The underlying mesh set related.
   :returns: A set ID.

.. c:function:: opp_map opp_decl_map(opp_set from, opp_set to, int dim, int *imap, char const *name)

   This routine defines a mapping between sets. 
   imap should contain a valid data array pointer. 
   The only exception is to provide `nullptr` given the from set is a particle set with zero particles in it.
         
   :param from: Source set.
   :param to: Destination set.
   :param dim: Number of mappings per source element.
   :param imap: Mapping table.
   :param name: A name to be used for output diagnostics.




.. c:function:: void op_partition(char *lib_name, char *lib_routine, op_set prime_set, op_map prime_map, op_dat coords)

   This routine controls the partitioning of the sets used for distributed memory parallel execution.

   :param lib_name: The partitioning library to use, see below.
   :param lib_routine: The partitioning algorithm to use. Required if using :c:expr:`"PTSCOTCH"`, :c:expr:`"PARMETIS"` or https://kahip.github.io/ as the **lib_name**.
   :param prime_set: Specifies the set to be partitioned.
   :param prime_map: Specifies the map to be used to create adjacency lists for the **prime_set**. Required if using :c:expr:`"KWAY"` or :c:expr:`"GEOMKWAY"`.
   :param coords: Specifies the geometric coordinates of the **prime_set**. Required if using :c:expr:`"GEOM"` or :c:expr:`"GEOMKWAY"`.

   The current options for **lib_name** are:

   - :c:expr:`"PTSCOTCH"`: The `PT-Scotch <https://www.labri.fr/perso/pelegrin/scotch/>`_ library.
   - :c:expr:`"PARMETIS"`: The `ParMETIS <http://glaros.dtc.umn.edu/gkhome/metis/parmetis/overview>`_ library.
   - :c:expr:`"KAHIP"`: The `KaHIP <https://kahip.github.io/>`_ library.
   - :c:expr:`"INERTIAL"`: Internal 3D recursive inertial bisection partitioning.
   - :c:expr:`"EXTERNAL"`: External partitioning optionally read in when using HDF5 I/O.
   - :c:expr:`"RANDOM"`: Random partitioning, intended for debugging purposes.

   The options for **lib_routine** when using :c:expr:`"PTSCOTCH"` or :c:expr:`"KAHIP"` are:

   - :c:expr:`"KWAY"`: K-way graph partitioning.

   The options for **lib_routine** when using :c:expr:`"PARMETIS"` are:

   - :c:expr:`"KWAY"`: K-way graph partitioning.
   - :c:expr:`"GEOM"`: Geometric graph partitioning.
   - :c:expr:`"GEOMKWAY"`: Geometric followed by k-way graph partitioning.

.. c:function:: void op_decl_const(int dim, char *type, T *dat)

   This routine defines constant data with global scope that can be used in kernel functions.

   :param dim: Number of data elements. For maximum efficiency this should be an integer literal.
   :param type: The type of the data as a string. This can be either intrinsic (:c:expr:`"float"`, :c:expr:`"double"`, :c:expr:`"int"`, :c:expr:`"uint"`, :c:expr:`"ll"`, :c:expr:`"ull"`, or :c:expr:`"bool"`) or user-defined.
   :param dat: A pointer to the data, checked for type consistency at run-time.

   .. note::
      If **dim** is :c:expr:`1` then the variable is available in the kernel functions with type :c:expr:`T`, otherwise it will be available with type :c:expr:`T*`.

   .. warning::
      If the executable is not preprocessed, as is the case with the development sequential build, then you must define an equivalent global scope variable to use the data within the kernels.

.. c:function:: op_dat op_decl_dat(op_set set, int dim, char *type, T *data, char *name)

   This routine defines a dataset.

   :param set: The set the data is associated with.
   :param dim: Number of data elements per set element.
   :param type: The datatype as a string, as with :c:func:`op_decl_const()`. A qualifier may be added to control data layout - see :ref:`api:Dataset Layout`.
   :param data: Input data of type :c:type:`T` (checked for consistency with **type** at run-time). The data must be provided in AoS form with each of the **dim** elements per set element contiguous in memory.
   :param name: A name to be used for output diagnostics.

   .. note::
      At present **dim** must be an integer literal. This restriction will be removed in the future but an integer literal will remain more efficient.

.. c:function:: op_dat op_decl_dat_temp(op_set set, int dim, char *type, T *data, char *name)

    Equivalent to :c:func:`op_decl_dat()` but the dataset may be released early with :c:func:`op_free_dat_temp()`.

.. c:function:: void op_free_dat_temp(op_dat dat)

   This routine releases a temporary dataset defined with :c:func:`op_decl_dat_temp()`

   :param dat: The dataset to free.

