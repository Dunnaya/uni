#!/bin/bash
#SBATCH -J mm9proc
#SBATCH -o output_9.txt
#SBATCH -e error_9.txt
#SBATCH -n 9
#SBATCH -t 00:10:00
#SBATCH -p scit5

mpirun -np 9 ./parallelMM