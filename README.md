
# MPI-Prime-Sieve
<p>A Prime Sieve using distributed memory in C++ 
</p>
<p>Make sure you have MPI installed on your systems.
Make sure your "Hosts.txt" has all of your systme names each followed by a return
</p>
<p>If you would ever want change the serial function to use openmp<br>
have open mp on your system<br>
uncomment the include for openmp<br>
uncomment the #pragma line in the sieve method<br>
remember to add -fopenmp to the compile line<br>
</p>
<p>Command line to compile and run:
> mpicxx -Wall -Werror -std=c++23 -O3 mpiParPrimeSieve.cc -o mpi
and then to run:
> mpirun --oversubscribe -n (number of processes) -hostfile ./Hosts.txt ./mpi
</p>