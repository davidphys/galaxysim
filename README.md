OMP multithreaded gravity simulation
===

This is a good starting point for a larger project involving 3D astrophysics/galaxy formation.
It does a tree code gravity simulation with some basic I/O. This is then multithreaded using some really trivial OpenMP parallelization (one "#pragma for").
Intended as a starting point for more realistic astrophysics simulations.

Some short-term TODOs:
 - Implement periodic boundary conditions. This is essential to getting the correct cosmic web structure.
 - Implement more advanced benchmarking, either in the code (timing stuff) or outside of it (nvprof). It would be nice to be able to tell for sure if a change improved the performance or decreased it, and by how much. I'd bet the code could be sped up by 50% without many changes, but we'd only know for sure with benchmarks.
 - Try moving stuff to the GPU using stupid code. I know four k80s can speed up the computation to lightspeed, even with stupid code.

Animation of a 2D disk with 400,000 particles and a large rotation parameter:
![](animation.gif)

Animation of a 3D sphere with 400,000 particles and a smaller rotation parameter:
![](animationSphere.gif)
