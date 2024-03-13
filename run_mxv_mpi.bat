@echo off
for %%s in (64 128 256 512 1024 2048 4096 8192 16384 32768) do (
    for /l %%i in (1,1,10) do (
        echo MPI Matrix-Vector Multiplication (Size: %%s, Run: %%i)
        mpicc -o output.exe mxv_mpi.c
        mpiexec -n 4 output.exe %%s
        echo.
        echo --------------------------------------------
    )
)
pause
