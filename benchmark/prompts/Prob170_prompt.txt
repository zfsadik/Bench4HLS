Implement a 2-D Finite-Difference Time-Domain (FDTD) kernel in HLS. The module updates three 200×240 field arrays (ex, ey, hz) over tmax time steps, using a 1-D "fictitious" boundary array _fict_ of length 100.

Inputs:
	tmax (int): number of time steps (≤100)
	nx, ny (int): grid dimensions (≤200, ≤240)
	ex[200][240], ey[200][240], hz[200][240] (double): electric & magnetic fields
	_fict_[100] (double): boundary values for ey[0][*]

Outputs:
	Updated ex, ey, and hz in place
	valid (bool): high when all tmax steps finish

At each time step t from 0 to tmax–1, perform these updates:
	1. Set every element of the first row of ey to the corresponding entry from the _fict_ array (the "fictitious" boundary).
	2. For each cell below the first row, subtract half the difference between the magnetic field hz at that cell and the one immediately above.
	3. For each cell to the right of the first column, subtract half the difference between hz at that cell and its left neighbor.
	4. For every interior cell, subtract 0.7 times the sum of the change in ex to the right and the change in ey below (a discrete curl).

After all steps, raise valid to indicate the fields ex, ey, and hz now hold the updated solution.
Use this prototype:

The top-level function should have the following prototype:
void TopModule(int    tmax,
               int    nx,
               int    ny,
               double ex[200][240],
               double ey[200][240],
               double hz[200][240],
               double _fict_[100],
               bool   &valid)
