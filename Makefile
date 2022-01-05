build:
	mpicxx -fopenmp -c main.c -o main.o
	# note: cFunctions.c does not use mpi so we could 
	# compile it using g++ instead of mpicxx 
	mpicxx -fopenmp -c cFunctions.c -o cFunctions.o
	nvcc -c cudaFunctions.cu -o cudaFunctions.o
	mpicxx -fopenmp -o mpiCudaOpenMPI  main.o cFunctions.o cudaFunctions.o  /usr/local/cuda/lib64/libcudart_static.a -ldl -lrt
	
clean:
	rm -f *.o ./mpiCudaOpenMPI

run:
	mpiexec -np 2 ./mpiCudaOpenMPI

#don't use this (at least for now)
runOn2:
	mpiexec -np 2 -machinefile  mf  -map-by  node  ./mpiCudaOpemMP


