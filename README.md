OMP multithreaded gravity simulation
===

This is a good starting point for a larger project involving 3D astrophysics/galaxy formation.
It does a tree code gravity simulation with some basic I/O. This is then multithreaded using some really trivial OpenMP parallelization (one "#pragma for").
Intended as a starting point for more realistic astrophysics simulations.


Animation of a 2D disk with 400,000 particles and a large rotation parameter:
![](animation.gif)

Animation of a 3D sphere with 400,000 particles and a smaller rotation parameter:
![](animationSphere.gif)

Command line Interface
===
There are several command line arguments to make it easy to set up initial conditions. These are (in "commandname datatype (description)" format):

    -dt float (delta t for each substep)
    -nout int (number of output files to produce)
    -ksteps int (number of timesteps to take between each output file)
    -txtdir string (what directory to output text files. If no directory is given, no text file output will be produced. The directory must already exist or no output will be produced.)
    -imgdir string (what directory to output image files. If no directory is given, no image file output will be produced. The directory must already exist or no output will be produced.)
    -in string (what file to take in. The file is expected to be of 1 particle per line, m x y z vx vy vz.)
    -box int (Option to create a unit mass 3D unit box of n particles)
    -square int (Option to create a unit mass 2D unit square of n particles)
    -disk int (Option to create a unit mass 2D unit disk of n particles)
    -sphere int (Option to create a unit mass 3D unit sphere of n particles)

Example commands:

**Create a disk:**  `./galaxysim -disk 100000 -nout 100 -ksteps 5 -imgdir out`

This creates a disk of a hundred thousand particles, outputs 100 images to "./out/out000.bmp", and does 500 total timesteps with dt=0.01.

**Load a galaxy and run it:**  `./galaxysim -in galaxy.txt -nout 600 -ksteps 5 -txtdir out`

Loads a galaxy, outputs 600 images to "./out/out000.bmp", and does 3000 total timesteps with dt=0.01.


Some short-term TODOs:
===

 - Implement periodic boundary conditions. This is essential to getting the correct cosmic web structure.
 - Implement more advanced benchmarking, either in the code (timing stuff) or outside of it (nvprof). It would be nice to be able to tell for sure if a change improved the performance or decreased it, and by how much. I'd bet the code could be sped up by 50% without many changes, but we'd only know for sure with benchmarks.
 - Try moving stuff to the GPU using stupid code. I know four k80s can speed up the computation to lightspeed, even with stupid code.

