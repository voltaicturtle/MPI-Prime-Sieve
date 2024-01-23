/*
  Filename   : mpiParPrimeSieve.cc
  Author     : Ian Martin
  Course     : CSCI 476
  Date       : 11/30/2023
  Assignment : Final
  Description: Performs a prime sieve
        using mpi and validates it serially.
*/

#include <cctype>
#include <cstdint>
#include <mpi.h>
#include <vector>
#include <iostream>
//#include <omp.h>

unsigned long
parSeive(unsigned long myRank, unsigned long commSz, unsigned long n)
{
    //setup for each process based on their rank
    unsigned long start = (myRank * n) / commSz;
    unsigned long end = ((myRank + 1) * n) / commSz;
    unsigned long length = end - start;
    std::vector<bool> v (length, true);

    unsigned long skip = 2;

    // if main process
    if (myRank == 0)
    {
        //sets 0 and 1 to be prime
        v[0] = false;
        v[1] = false;

        while (skip * skip < n) 
        {
            // enter if it finds a prime
            if (v[skip])
            {
                //tells all of the other processes
                for (unsigned i = 1; i < commSz; ++i) 
                {
                    MPI_Send(&skip, 1, MPI_UNSIGNED_LONG, i, 0, MPI_COMM_WORLD);
                }

                //marks off all the non primes
                for (unsigned long j = skip * skip; j < length; j += skip)
                    v[j] = false;
            }
            ++skip;
        }
        //tells the other processes that we found the end and break out
        //the program will hang without this
        for (unsigned i = 1; i < commSz; ++i) 
            MPI_Send(&skip, 1, MPI_UNSIGNED_LONG, i, 0, MPI_COMM_WORLD);
    }

    // if any other process
    else 
    {
        while (skip * skip < n) 
        {
            //waits to recieve what number to skip by
            MPI_Recv(&skip, 1, MPI_UNSIGNED_LONG, 0, 0, MPI_COMM_WORLD, nullptr);

            //breaks out of statement if the exit condition is met
            if (skip * skip > n)
                break;

            //calculates the starting point based on where our start point is
            auto startingPoint = (start % skip ? skip - (start%skip) : 0);

            //marks off all of the non prime numbers
            for (unsigned long j = startingPoint; j < length; j += skip)
                v[j] = false;
        }
    }

    //counts all of the primes and returns
    unsigned long count = 0;
    for (unsigned long i = 0; i < v.size(); ++i)
    {
        count += v[i];
    }
    return count;
}

unsigned long
seive(unsigned long n)
{
    std::vector<bool> primes(n, true);
    //sets 0 and 1 to false
    primes[0] = false;
    primes[1] = false;

    for (unsigned long i = 2; i * i < n; ++i) 
    {
        if (primes[i])
        //uncomment below block if you want to use open mp
        //#pragma omp parallel for num_threads(4)
            for (unsigned long j = i * i; j < n; j += i)
                primes[j] = false;
    }
    unsigned long count= 0;
    for (unsigned long i = 0; i < primes.size(); ++i)
        count += primes[i];
    return count;
}

int
main()
{
    //mpi settup
    MPI_Init(nullptr, nullptr);
    int myRank = 0;
    MPI_Comm_rank(MPI_COMM_WORLD, &myRank);
    int commSz = 0;
    MPI_Comm_size(MPI_COMM_WORLD, &commSz);

    unsigned long n;
    if (myRank == 0)
    {
        std::cout << "n ==> ";
        std::cin >> n;
        std::cout << "\n";
        if (commSz * commSz > n)
        {
            std::cout << "Please input a lower number of processes or higher n.\n";
            return 0;
        }
    }

    //Take start time
    MPI_Barrier(MPI_COMM_WORLD);
    double t1 = MPI_Wtime();
    MPI_Bcast(&n, 1, MPI_UNSIGNED_LONG, 0, MPI_COMM_WORLD);

    //do the sort and report count
    auto countPar = parSeive(myRank, commSz, n);
    
    //collects the counts of all of the processes
    unsigned long total = 0;
    MPI_Reduce(&countPar, &total, 1, MPI_UNSIGNED_LONG, MPI_SUM, 0, MPI_COMM_WORLD);
    
    double t2 = MPI_Wtime();

    // prints and calculates the serial times on source process
    if (myRank == 0)
    {
        std::cout << "Parallel Count: " << total << "\n"; 
        std::cout << "Parallel Time:  " << t2 - t1 << " Seconds\n\n";
        auto time = MPI_Wtime();
        auto verify = seive(n);
        time = MPI_Wtime() - time;
        std::cout << "Serial Count: " << verify << "\n";
        std::cout << "Serial Time:  " << time << " Seconds\n";    
    }
    MPI_Finalize();
}