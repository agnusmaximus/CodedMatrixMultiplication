# CodedMatrixMultiplication
Distributed matrix multiplication using codes to reduce the effects of stragglers.
https://arxiv.org/pdf/1512.02673.pdf

# Usage

It's beneficial to have this whole directory in a shared fileysystem such as NFS.


Run

```
make generate_uncoded_data
```

to generate a square synthetic matrix multiplication problem given
`N_PROCS`, `N_ROWS`, and `N_COLS` as defined in the Makefile. An error
will be thrown if the number of rows is not evenly divisible by the
number of non-parity workers. The generated synthetic data will be
used by the code.


Run
```
make coded1
```
or
```
make naive
```
or
```
make coded2
```
to generate the MPI executable for the respective distributed matrix multiplication routine.


Run
```
mpirun -np NPROCS ./executable
```
to run the respective binary.