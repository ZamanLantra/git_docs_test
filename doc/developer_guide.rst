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

The speciality of a particle set is that they can be resized (the set size can be increased or reduced during the simulation.
In addition to the main particle set, we have used a temporary dummy particle set to hold some random data for particle injection initialization.

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

.. code-block:: C

    //declare maps
    opp_map c2n_map  = opp_decl_map(cell_set,  node_set, 4, m->c_to_n, "c_v_n_map");
    opp_map c2c_map  = opp_decl_map(cell_set,  cell_set, 4, m->c_to_c,  "c_v_c_map"); 
    opp_map if2c_map = opp_decl_map(iface_set, cell_set, 1, m->if_to_c, "if_v_c_map"); 
    opp_map if2n_map = opp_decl_map(iface_set, node_set, 4, m->if_to_n, "if_v_n_map");

    opp_map p2c_map  = opp_decl_map(particle_set, cell_set, 1, nullptr, "p2c_map");

The ``opp_decl_map`` requires the names of the two sets for which the mapping is declared, its arity, mapping data (as in this case allocated in integer blocks of memory) and a string name.
A map created with a particle set is capable of changing its length during the simulation and other maps are static.

**Declare data** - All data declared on sets should be declared using the ``opp_decl_dat`` API call. For FemPIC this consists of seven cell dats, six node dats, six inlet-face dats and three particle dats (+1 dummy particle dat).

.. code-block:: C

  //declare data on sets
    opp_dat c_det       = opp_decl_dat(cell_set, ALL_DET,     DT_REAL, m->c_det,      "c_det");  
    opp_dat c_volume    = opp_decl_dat(cell_set, ONE,         DT_REAL, m->c_vol,      "c_volume");        
    opp_dat c_ef        = opp_decl_dat(cell_set, DIM,         DT_REAL, m->c_ef,       "c_ef");
    opp_dat c_sd        = opp_decl_dat(cell_set, N_PER_C*DIM, DT_REAL, m->c_sd,       "c_shape_deri"); 
    opp_dat c_gbl_id    = opp_decl_dat(cell_set, ONE,         DT_INT,  m->c_id,       "c_gbl_id"); 
    opp_dat c_colors    = opp_decl_dat(cell_set, ONE,         DT_INT,  m->c_col,      "c_colors");
    opp_dat c_centroids = opp_decl_dat(cell_set, DIM,         DT_REAL, m->c_centroid, "c_centroids");

    opp_dat n_volume     = opp_decl_dat(node_set, ONE, DT_REAL, m->n_vol,     "n_vol");        
    opp_dat n_potential  = opp_decl_dat(node_set, ONE, DT_REAL, m->n_pot,     "n_potential");     
    opp_dat n_charge_den = opp_decl_dat(node_set, ONE, DT_REAL, m->n_ion_den, "n_charge_den");
    opp_dat n_pos        = opp_decl_dat(node_set, DIM, DT_REAL, m->n_pos,     "n_pos");     
    opp_dat n_type       = opp_decl_dat(node_set, ONE, DT_INT,  m->n_type,    "n_type");
    opp_dat n_bnd_pot    = opp_decl_dat(node_set, ONE, DT_REAL, m->n_bnd_pot, "n_bnd_pot");

    opp_dat if_v_norm  = opp_decl_dat(iface_set, DIM,          DT_REAL, m->if_v_norm, "iface_v_norm");
    opp_dat if_u_norm  = opp_decl_dat(iface_set, DIM,          DT_REAL, m->if_u_norm, "iface_u_norm");
    opp_dat if_norm    = opp_decl_dat(iface_set, DIM,          DT_REAL, m->if_norm,   "iface_norm");  
    opp_dat if_area    = opp_decl_dat(iface_set, ONE,          DT_REAL, m->if_area,   "iface_area");
    opp_dat if_distrib = opp_decl_dat(iface_set, ONE,          DT_INT,  m->if_dist,   "iface_dist");
    opp_dat if_n_pos   = opp_decl_dat(iface_set, N_PER_IF*DIM, DT_REAL, m->if_n_pos,  "iface_n_pos");

    opp_dat p_pos   = opp_decl_dat(particle_set, DIM,     DT_REAL, nullptr, "p_position");
    opp_dat p_vel   = opp_decl_dat(particle_set, DIM,     DT_REAL, nullptr, "p_velocity");
    opp_dat p_lc    = opp_decl_dat(particle_set, N_PER_C, DT_REAL, nullptr, "p_lc");

    opp_dat dp_rand = opp_decl_dat(dummy_part_set, 2, DT_REAL, nullptr, "dummy_part_rand");

**Declare constants** - Finally global constants that are used in any of the computations in the loops needs to be declared.
This is required due to the fact that when using code-generation later for parallelizations such as on GPUs (e.g. using CUDA or HIP), global constants needs to be copied over to the GPUs before they can be used in a GPU kernel. 
Declaring them using the ``opp_decl_const<type>`` API call will indicate to the OP-PIC code-generator that these constants needs to be handled in a special way, generating code for copying them to the GPU for the relevant back-ends.

.. code-block:: C

    //declare global constants
    opp_decl_const<OPP_REAL>(ONE, &spwt,           "CONST_spwt");
    opp_decl_const<OPP_REAL>(ONE, &ion_velocity,   "CONST_ion_velocity");
    opp_decl_const<OPP_REAL>(ONE, &dt,             "CONST_dt");
    opp_decl_const<OPP_REAL>(ONE, &plasma_den,     "CONST_plasma_den");
    opp_decl_const<OPP_REAL>(ONE, &mass,           "CONST_mass");
    opp_decl_const<OPP_REAL>(ONE, &charge,         "CONST_charge");
    opp_decl_const<OPP_REAL>(ONE, &wall_potential, "CONST_wall_potential");

Step 3 - First parallel loop : direct loop
------------------------------------------




