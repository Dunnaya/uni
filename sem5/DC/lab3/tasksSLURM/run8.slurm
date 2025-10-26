#!/bin/bash
#SBATCH -J gauss8proc
#SBATCH -o output_8.txt
#SBATCH -e error_8.txt
#SBATCH -n 8
#SBATCH -t 00:10:00
#SBATCH -p scit5

mpirun -np 8 ./parallelGauss