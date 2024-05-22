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
Go to the ``app_fempic/fempic_misc_mesh_loader.h`` to see the complete mesh loader, where we use the original fempic code to read from file and store in the data storage class ``DataPointers``.

.. code-block:: C
    std::shared_ptr<DataPointers> m = load_mesh();

In this tutorial, the main focus is to show how the OP-PIC API is used, hence the user may implement their own code for mesh loading.

Step 1 - Preparing to use OP-PIC
--------------------------------

First, include the following header files, then initialize OP-PIC and finalize it as follows:

.. code-block:: C
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
