#!/bin/bash
#SBATCH -J gauss2proc
#SBATCH -o output_2.txt
#SBATCH -e error_2.txt
#SBATCH -n 2
#SBATCH -t 00:10:00
#SBATCH -p scit5

mpirun -np 2 ./parallelGauss