Developer Guide
===============

This page provides a tutorial in the basics of using OP-PIC for unstructured-mesh PIC application development.

Example Application
-------------------

The tutorial will use the FemPIC application, an electrostatic 3D unstructured mesh finite element PIC code (https://github.com/ExCALIBUR-NEPTUNE/Documents/blob/main/reports/2057699/TN-03-3.pdf).

It is based on tetrahedral mesh cells, nodes and faces forming a duct. 
Faces on one end of the duct are designated as inlet faces and the outer wall is fixed at a higher potential to retain the ions within the duct. 
Charged particles are injected at a constant rate from the inlet faces of the duct (one-stream) at a fixed velocity, and the particles move through the duct under the influence of the electric field. 
The particles are removed when they leave the boundary face. Overall Mini-FEM-PIC has 1 degree of freedom (DOF) per cell, 2 DOFs per node and 7 DOFs per particle.

FemPIC consists of six main loops, ``inject_ions`` , ``calculate_new_pos_vel`` , ``move`` , ``deposit_charge_on_nodes``, ``compute_node_charge_density`` and ``compute_electric_field``, within a time-marching iterative loop. 
Additionally, it contain a ``init_boundary_pot`` loop and a ``get_final_max_values`` loop used during initialzing and finalizing stages.
In addition to the above loops, FemPIC consists of a linear sparse matrix field solver, which is implemented using PETSc library.

Out of these, ``compute_node_charge_density``, ``init_boundary_pot`` and ``get_final_max_values`` are what we classify as direct loops where all the data accessed in the loop is defined on the mesh element over which the loop iterates over. 
Thus for example in a direct loop, a loop over nodes will only access data defined on nodes. 

All the other loops are indirect loops. 
In this case when looping over a given type of elements, data on other types of elements will be accessed indirectly, using mapping tables. 
There are two types of indirect loops. 
The ``compute_electric_field`` loop iterates over cells and read data on nodes, accessing them indirectly via a mapping table that gives the explicit connectivity information between cells and nodes. 
Similarly, ``calculate_new_pos_vel`` loop iterates over particles and read data on cells, accessing them indirectly via a mapping between particles and cells.
The other kind of indirect loop is double-indirect. 
For example ``deposit_charge_on_nodes`` loop iterates over particles and increments data on nodes. But these nodes are not directly related to particles though one mapping. 
Thus we may need to use two mappings, the first from particles to cells (p2c_map) and the second from cells to nodes (c2n_map).

Within these above mentioned loop, there is a special loop which will move particles to cells accoring to the particle position, and the further details will be discussed in a later section.

* Go to the ``OP-PIC/app_fempic`` directory and open the ``fempic.cpp`` file to view the original application.
* Use the information in the readme file of that directory to code-generate and run the application.

Original - Load mesh and initialization
---------------------------------------
The original code begins with allocating memory to hold the mesh data and then initializing them by reading in the mesh data, form the text file. 

Go to the ``OP-PIC/app_fempic/fempic_misc_mesh_loader.h`` to see the complete mesh loader, where we use the original fempic code to read from file and store in the data storage class ``DataPointers``.

.. code-block:: c++

    std::shared_ptr<DataPointers> m = load_mesh();

In this tutorial, the main focus is to show how the OP-PIC API is used, hence the user may implement their own code for mesh loading.

Step 1 - Preparing to use OP-PIC
--------------------------------

First, include the following header files, then initialize OP-PIC and finalize it as follows:

.. code-block:: c++

    #include "opp_templates.h"
    ...
    ...
    int main(int argc, char **argv) {
        opp_init(argc, argv); //Initialise the OP-PIC library, passing runtime args
        {
            ...
            ...
            ...
        }  
        opp_exit(); //Finalising the OP-PIC library
    }

Step 2 - OP-PIC Declaration
---------------------------
**Declare sets** - 
The FemPIC application consists of three mesh element types (which we call sets): ``cells``, ``nodes``, and ``inlet-faces``. 
These needs to be declared using the ``opp_decl_set`` API call together with the number of elements for each of these sets.

In addition, FemPIC contains a particle set, that is defined using ``opp_decl_particle_set`` API call together with the number of particles and a mesh cell set. 

The speciality of a particle set is that they can be resized (the set size can be increased or reduced during the simulation).
Other than the main particle set, we have used a temporary dummy particle set to hold some random data for particle injection initialization.

.. code-block:: c++

    // declare sets
    opp_set node_set       = opp_decl_set(m->n_nodes, "mesh_nodes");
    opp_set cell_set       = opp_decl_set(m->n_cells, "mesh_cells");
    opp_set iface_set      = opp_decl_set(m->n_ifaces, "inlet_faces_cells");
    opp_set particle_set   = opp_decl_particle_set("particles", cell_set); 
    opp_set dummy_part_set = opp_decl_particle_set("dummy particles", cell_set);

Later, we will see how the number of mesh elements can be read in directly from an hdf5 file using the ``opp_decl_set_hdf5`` and ``opp_decl_particle_set_hdf5`` call.

When developing your own application with OP-PIC, or indeed converting an application to use OP-PIC, you will need to decide on what mesh element types, i.e. sets will need to be declared to define the full mesh. 
A good starting point for this design is to see what mesh elements are used the loops over the mesh.

**Declare maps** - Looking at the original Mini-FEM-PIC application's loops we see that mappings between cells and nodes, cells and cells, inlet-faces and nodes, inlet-faces and cells, and cells and nodes are required. 
In addition, a particles to cells mapping is required. 

This can be observed by the indirect access to data in each of the loops in the main iteration loops. 
These connectivity information needs to be declared via the ``opp_decl_map`` API call:

.. code-block:: c++

    //declare maps
    opp_map c2n_map  = opp_decl_map(cell_set,  node_set, 4, m->c_to_n, "c_v_n_map");
    opp_map c2c_map  = opp_decl_map(cell_set,  cell_set, 4, m->c_to_c,  "c_v_c_map"); 
    opp_map if2c_map = opp_decl_map(iface_set, cell_set, 1, m->if_to_c, "if_v_c_map"); 
    opp_map if2n_map = opp_decl_map(iface_set, node_set, 4, m->if_to_n, "if_v_n_map");

    opp_map p2c_map  = opp_decl_map(particle_set, cell_set, 1, nullptr, "p2c_map");

The ``opp_decl_map`` requires the names of the two sets for which the mapping is declared, its arity, mapping data (as in this case allocated in integer blocks of memory) and a string name.
A map created with a particle set is capable of changing its length during the simulation and other maps are static.

Note that we have declared ``p2c_map`` with a ``nullptr`` since ``particle_set`` is defined without a particle count (i.e. zero), since we anticipate to inject particles during the simulation.

**Declare data** - All data declared on sets should be declared using the ``opp_decl_dat`` API call. For FemPIC this consists of seven cell dats, six node dats, six inlet-face dats and three particle dats (+1 dummy particle dat).

.. code-block:: c++

  //declare data on sets
    opp_dat c_det       = opp_decl_dat(cell_set, 16, DT_REAL, m->c_det,      "c_det");  
    opp_dat c_volume    = opp_decl_dat(cell_set, 1,  DT_REAL, m->c_vol,      "c_volume");        
    opp_dat c_ef        = opp_decl_dat(cell_set, 3,  DT_REAL, m->c_ef,       "c_ef");
    opp_dat c_sd        = opp_decl_dat(cell_set, 12, DT_REAL, m->c_sd,       "c_shape_deri"); 
    opp_dat c_gbl_id    = opp_decl_dat(cell_set, 1,  DT_INT,  m->c_id,       "c_gbl_id"); 
    opp_dat c_colors    = opp_decl_dat(cell_set, 1,  DT_INT,  m->c_col,      "c_colors");
    opp_dat c_centroids = opp_decl_dat(cell_set, 3,  DT_REAL, m->c_centroid, "c_centroids");

    opp_dat n_volume     = opp_decl_dat(node_set, 1, DT_REAL, m->n_vol,     "n_vol");        
    opp_dat n_potential  = opp_decl_dat(node_set, 1, DT_REAL, m->n_pot,     "n_potential");     
    opp_dat n_charge_den = opp_decl_dat(node_set, 1, DT_REAL, m->n_ion_den, "n_charge_den");
    opp_dat n_pos        = opp_decl_dat(node_set, 3, DT_REAL, m->n_pos,     "n_pos");     
    opp_dat n_type       = opp_decl_dat(node_set, 1, DT_INT,  m->n_type,    "n_type");
    opp_dat n_bnd_pot    = opp_decl_dat(node_set, 1, DT_REAL, m->n_bnd_pot, "n_bnd_pot");

    opp_dat if_v_norm  = opp_decl_dat(iface_set, 3,  DT_REAL, m->if_v_norm, "iface_v_norm");
    opp_dat if_u_norm  = opp_decl_dat(iface_set, 3,  DT_REAL, m->if_u_norm, "iface_u_norm");
    opp_dat if_norm    = opp_decl_dat(iface_set, 3,  DT_REAL, m->if_norm,   "iface_norm");  
    opp_dat if_area    = opp_decl_dat(iface_set, 1,  DT_REAL, m->if_area,   "iface_area");
    opp_dat if_distrib = opp_decl_dat(iface_set, 1,  DT_INT,  m->if_dist,   "iface_dist");
    opp_dat if_n_pos   = opp_decl_dat(iface_set, 12, DT_REAL, m->if_n_pos,  "iface_n_pos");

    opp_dat p_pos   = opp_decl_dat(particle_set, 3, DT_REAL, nullptr, "p_position");
    opp_dat p_vel   = opp_decl_dat(particle_set, 3, DT_REAL, nullptr, "p_velocity");
    opp_dat p_lc    = opp_decl_dat(particle_set, 4, DT_REAL, nullptr, "p_lc");

    opp_dat dp_rand = opp_decl_dat(dummy_part_set, 2, DT_REAL, nullptr, "dummy_part_rand");

Note that we have declared particle dats with a ``nullptr`` since ``particle_set`` is defined without a particle count (i.e. zero), since we anticipate to inject particles during the simulation.

**Declare constants** - Finally global constants that are used in any of the computations in the loops needs to be declared.
This is required due to the fact that when using code-generation later for parallelizations such as on GPUs (e.g. using CUDA or HIP), global constants needs to be copied over to the GPUs before they can be used in a GPU kernel. 

Declaring them using the ``opp_decl_const<type>`` API call will indicate to the OP-PIC code-generator that these constants needs to be handled in a special way, generating code for copying them to the GPU for the relevant back-ends.
The template types could be ``OPP_REAL``, ``OPP_INT``, ``OPP_BOOL``.

.. code-block:: c++

    //declare global constants
    opp_decl_const<OPP_REAL>(1, &spwt,           "CONST_spwt");
    opp_decl_const<OPP_REAL>(1, &ion_velocity,   "CONST_ion_velocity");
    opp_decl_const<OPP_REAL>(1, &dt,             "CONST_dt");
    opp_decl_const<OPP_REAL>(1, &plasma_den,     "CONST_plasma_den");
    opp_decl_const<OPP_REAL>(1, &mass,           "CONST_mass");
    opp_decl_const<OPP_REAL>(1, &charge,         "CONST_charge");
    opp_decl_const<OPP_REAL>(1, &wall_potential, "CONST_wall_potential");

The constants can be accessed in the kernels with the same literals used in the string name. 
An example can be seen in the next section (Step 3).

Step 3 - Parallel loop : ``opp_par_loop``
------------------------------------------

Direct loop
~~~~~~~~~~~

We can now convert a direct loop to use the OP-PIC API. 

We have chosen ``compute_node_charge_density`` to demostrate a direct loop.
It iterates over nodes, ``multiply node_charge_den`` with (``CONST_spwt`` / ``node_volume``) and saves to multiply ``node_charge_den``.

.. code-block:: c++

    //compute_node_charge_density : iterates over nodes
    for (int iteration = 0; iteration < (nnodes * 1); ++iteration) {
        node_charge_den[iteration] *= (CONST_spwt[0] / node_volume[iteration]);
    }

This is a direct loops due to the fact that all data accessed in the computation are defined on the set that the loop iterates over. In this case the iteration set is nodes.

To convert to the OP-PIC API we first outline the loop body (elemental kernel) to a subroutine:

.. code-block:: c++

    //outlined elemental kernel
    inline void compute_ncd_kernel(double *ncd, const double *nv) {
        ncd[0] *= (CONST_spwt[0] / nv[0]);
    }
    //compute_node_charge_density : iterates over nodes
    for (int iteration = 0; iteration < (nnodes * 1); ++iteration) {
        compute_ncd_kernel(&node_charge_den[iteration], &node_volume[iteration]);
    }

Now we can directly declare the loop with the ``opp_par_loop`` API call:

.. code-block:: c++

    //outlined elemental kernel
    inline void compute_ncd_kernel(double *ncd, const double *nv) {
        ncd[0] *= (CONST_spwt[0] / nv[0]);
    }

    opp_par_loop(compute_ncd_kernel, "compute_node_charge_density", node_set, OPP_ITERATE_ALL,
        opp_arg_dat(n_charge_den,  OPP_RW), 
        opp_arg_dat(n_volume,      OPP_READ));

Note how we have:

- indicated the elemental kernel ``compute_ncd_kernel`` in the first argument to ``opp_par_loop``.
- used the ``opp_dat``s names ``n_charge_den`` and ``n_volume`` in the API call.
- noted the iteration set ``node_set`` (3rd argument) and iteration type ``OPP_ITERATE_ALL`` (4th argument).
- indicated the direct access of ``n_charge_den`` and ``n_volume`` without any mappings provided to ``opp_arg_dat``.
- indicated that ``n_volume`` is read only (``OP_READ``) and ``n_charge_den`` is read & write (``OPP_RW``), by looking through the elemental kernel and identifying how they are used/accessed in the kernel.
- given that ``n_volume`` is read only we also indicate this by the key word ``const`` for ``compute_ncd_kernel`` elemental kernel.
- note that we have accessed a const value ``CONST_spwt`` that we declared using ``opp_decl_const<OPP_REAL>()`` API call.

Indirect loop (single indirection)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

We have selected two loops in FemPIC to demonstrate single indirections. 

First, we use ``compute_electric_field`` calculation to showcase the mesh set to mesh set mapping indirections.
Here we iterate over cells set, access node potentials through indirect accesses using ``c2n_map``.
Note that one cell in FemPIC is linked with 4 surrounding nodes and ``n_potential`` has a dimension of one.

.. code-block:: c++

    //compute_electric_field : iterates over cells
    for (int iter = 0; iter < ncell; ++iter) {
        const int map1idx = c2n_map[iter * 4 + 0];
        const int map2idx = c2n_map[iter * 4 + 1];
        const int map3idx = c2n_map[iter * 4 + 2];
        const int map4idx = c2n_map[iter * 4 + 3];
        
        for (int dim = 0; dim < 3; dim++) { 
            c_ef[3 * iter + dim] = c_ef[12 * iter + dim] - 
                ((c_sd[12 * iter + (0 + dim)] * n_potential[map1idx * 1 + 0])) + 
                (c_sd[12 * iter + (3 + dim)] * n_potential[map2idx * 1 + 0])) +
                (c_sd[12 * iter + (6 + dim)] * n_potential[map3idx * 1 + 0])) + 
                (c_sd[12 * iter + (9 + dim)] * n_potential[map4idx * 1 + 0])));
        }
    }

Similar to the direct loop, we outline the loop body and call it within the loop as follows:

.. code-block:: c++

    //outlined elemental kernel
    inline void compute_ef_kernel(
        double *c_ef, const double *c_sd, const double *n_pot0,
        const double *n_pot1, const double *n_pot2, const double *n_pot3) {
        
        for (int dim = 0; dim < 3; dim++) { 
            c_ef[dim] = c_ef[dim] - 
                ((c_sd[0 + dim] * n_pot0[0])) + (c_sd[3 + dim] * n_pot1[0])) +
                (c_sd[6 + dim] * n_pot2[0])) + (c_sd[9 + dim] * n_pot3[0])));
        }    
    }
    //compute_electric_field : iterates over cells
    for (int iter = 0; iter < ncell; ++iter) {
        const int map1idx = c2n_map[iter * 4 + 0];
        const int map2idx = c2n_map[iter * 4 + 1];
        const int map3idx = c2n_map[iter * 4 + 2];
        const int map4idx = c2n_map[iter * 4 + 3];

        compute_ef_kernel(&c_ef[3 * iter], &c_sd[12 * iter], &n_potential[1 * map1idx],
            &n_potential[1 * map2idx], &n_potential[1 * map3idx], &n_potential[1 * map4idx]);
    }

Now, convert the loop to use the opp_par_loop API:

.. code-block:: c++

    //outlined elemental kernel
    inline void compute_ef_kernel(
        double *c_ef, const double *c_sd, const double *n_pot0,
        const double *n_pot1, const double *n_pot2, const double *n_pot3) {
        
        for (int dim = 0; dim < 3; dim++) { 
            c_ef[dim] = c_ef[dim] - 
                ((c_sd[0 * 3 + dim] * n_pot0[0])) + (c_sd[1 * 3 + dim] * n_pot1[0])) +
                (c_sd[2 * 3 + dim] * n_pot2[0])) + (c_sd[3 * 3 + dim] * n_pot3[0])));
        }    
    }

    opp_par_loop(compute_ef_kernel, "compute_electric_field", cell_set, OPP_ITERATE_ALL,
        opp_arg_dat(c_ef,                    OPP_RW), 
        opp_arg_dat(c_sd,                    OPP_READ),
        opp_arg_dat(n_potential, 0, c2n_map, OPP_READ),
        opp_arg_dat(n_potential, 1, c2n_map, OPP_READ),
        opp_arg_dat(n_potential, 2, c2n_map, OPP_READ),
        opp_arg_dat(n_potential, 3, c2n_map, OPP_READ));

Note in this case how the indirections are specified using the mapping declared as ``opp_map`` ``c2n_map``, indicating the to-set index (2nd argument), and access mode ``OPP_READ``.
That is, the thrid argument of the ``opp_par_loop`` is a read-only argument mapped from cells to nodes using the mapping at the 0th index of c2n_map (i.e. 1st mapping out of 4 nodes attached).
Likewise, the fourth argument of ``opp_par_loop`` is mapped from cells to nodes using the mapping at the 1th index of ``c2n_map`` (i.e. 2nd mapping out of 4 nodes attached) and so on.

Second, we use ``calculate_new_pos_vel`` calculation to showcase the particle set to mesh set mapping indirections.
Here we iterate over particles set, access cell electric fields through indirect accesses using ``p2c_map``.
Note that one particle in FemPIC can be linked with only only one cell.

.. code-block:: c++

    //calculate_new_pos_vel : iterates over cells
    for (int iter = 0; iter < nparticles; ++iter) {
        const int p2c = p2c_map[iter];
        const double coef = CONST_charge[0] / CONST_mass[0] * CONST_dt[0];
        for (int dim = 0; dim < 3; dim++) {
            p_vel[3 * iter + dim] += (coef * c_ef[3 * p2c * dim]);   
            p_pos[3 * iter + dim] += p_vel[3 * iter + dim] * CONST_dt[0];                
        }
    }

Then, we outline the loop body and call it within the loop as follows:

.. code-block:: c++

    //outlined elemental kernel
    inline void calc_pos_vel_kernel(
        const double *cell_ef, double *part_pos, double *part_vel) {

        const double coef = CONST_charge[0] / CONST_mass[0] * CONST_dt[0];
        for (int dim = 0; dim < 3; dim++) {
            part_vel[dim] += (coef * cell_ef[dim]);   
            part_pos[dim] += part_vel[dim] * (CONST_dt[0]);                
        }  
    }
    //calculate_new_pos_vel : iterates over particles
    for (int iter = 0; iter < nparticles; ++iter) {
        const int p2c = p2c_map[iter];
        calc_pos_vel_kernel(&c_ef[3 * p2c], &p_pos[3 * iter], &p_vel[3 * iter]);
    }

Now, convert the loop to use the opp_par_loop API:

.. code-block:: c++

    //outlined elemental kernel
    inline void calc_pos_vel_kernel(
        const double *cell_ef, double *part_pos, double *part_vel) {

        const double coef = CONST_charge[0] / CONST_mass[0] * CONST_dt[0];
        for (int dim = 0; dim < 3; dim++) {
            part_vel[dim] += (coef * cell_ef[dim]);   
            part_pos[dim] += part_vel[dim] * (CONST_dt[0]);                
        }  
    }

    opp_par_loop(calc_pos_vel_kernel, "calculate_new_pos_vel", particle_set, OPP_ITERATE_ALL,
        opp_arg_dat(c_ef, p2c_map, OPP_READ),
        opp_arg_dat(p_pos,         OPP_WRITE),
        opp_arg_dat(p_vel,         OPP_WRITE));

Note in this case how the indirections are specified using the mapping declared as ``opp_map`` ``p2c_map``, and access mode ``OPP_READ``.
That is, the first argument of the ``opp_par_loop`` is a read-only argument mapped from particles to cells, however a mapping index is not required since always particles to cells mapping has a dimension of one.

Double Indirect loop
~~~~~~~~~~~~~~~~~~~~

There could be instances where double indirection is required. 
For example in ``deposit_charge_on_nodes``, we may need to deposit charge from particles to nodes, but from particles we have a single mapping towards the cells, with another mapping from cells to nodes.

Note that one cell in FemPIC is linked with 4 surrounding nodes and ``n_charge_den`` has a dimension of one.




Step 4 - Move loop : ``opp_particle_move``
------------------------------------------



Step 5 - Global reductions
--------------------------

