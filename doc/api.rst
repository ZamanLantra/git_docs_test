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
   
   :param from: Source set.
   :param to: Destination set.
   :param dim: Number of mappings per source element.
   :param imap: Mapping table.
   :param name: A name to be used for output diagnostics.

   .. note::
      imap should contain a valid data array pointer. The only exception is to provide `nullptr` given the from set is a particle set with zero particles in it.
   
.. c:function:: opp_dat opp_decl_dat(opp_set set, int dim, opp_data_type dtype, void *data, char const *name)

   This routine defines a dataset.

   :param set: The set the data is associated with (can be a mesh set or a particle set).
   :param dim: Number of data elements per set element.
   :param type: The datatype, This can be a type in :c:expr:`opp_data_type` enum (:c:expr:`DT_INT` or :c:expr:`DT_REAL`, other type may be added later).
   :param data: Input data of type :c:type:`T` (checked for consistency with **type** at run-time). The data must be provided in AoS form with each of the **dim** elements per set element contiguous in memory.
   :param name: A name to be used for output diagnostics.

   .. note::
      At present **dim** must be an integer literal or a :c:expr:`#define`

.. c:function:: template <typename T> void opp_decl_const(int dim, T* data, const char* name)

   This routine defines constant data with global scope that can be used in kernel functions.

   :param dim: Number of data elements. For maximum efficiency this should be an integer literal or a :c:expr:`#define`.
   :param data: A pointer to the data, checked for type consistency at run-time.
   :param name: The name as a string that the kernels access it, (should be :c:expr:`CONST_+<var_name>`).

   .. note::
      The variable is available in the kernel functions with type :c:expr:`T` with type :c:expr:`T*`. Hence even if **dim** is :c:expr:`1`, it should be accessed as :c:expr:`CONST_+<var_name>[0]` within the kernel.

.. c:function:: void opp_partition(std::string lib_name, opp_set prime_set, opp_map prime_map, opp_dat dat)

   This routine controls the partitioning of the sets used for distributed memory parallel execution.

   :param lib_name: The partitioning library to use, see below.
   :param prime_set: Specifies the set to be partitioned.
   :param prime_map: Specifies the map to be used to create adjacency lists for the **prime_set**. Required if using :c:expr:`"KWAY"` or :c:expr:`"GEOMKWAY"` (defaulted to :c:expr:`nullptr`).
   :param dat: Specifies the :c:expr:`opp_dat` required for the partitioning strategy (defaulted to :c:expr:`nullptr`).

   The current options for **lib_name** are:

   - :c:expr:`"PTSCOTCH"`: The `PT-Scotch <https://www.labri.fr/perso/pelegrin/scotch/>`_ library.
   - :c:expr:`"PARMETIS"`: The `ParMETIS <http://glaros.dtc.umn.edu/gkhome/metis/parmetis/overview>`_ library.    geometric coordinates of the **prime_set**. Required if using :c:expr:`"GEOM"` or :c:expr:`"GEOMKWAY"`.
   - :c:expr:`"EXTERNAL"`: External partitioning optionally read in when using HDF5 I/O.
   - :c:expr:`"RANDOM"`: Random partitioning, intended for debugging purposes.

   The options for **lib_routine** when using :c:expr:`"PTSCOTCH"` or :c:expr:`"KAHIP"` are:

   - :c:expr:`"KWAY"`: K-way graph partitioning.

   The options for **lib_routine** when using :c:expr:`"PARMETIS"` are:

   - :c:expr:`"KWAY"`: K-way graph partitioning.
   - :c:expr:`"GEOM"`: Geometric graph partitioning.
   - :c:expr:`"GEOMKWAY"`: Geometric followed by k-way graph partitioning.


