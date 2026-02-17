#!/bin/bash
#SBATCH --job-name=graphcrawler
#SBATCH --partition=Centaurus
#SBATCH --time=00:10:00
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=1
#SBATCH --mem=10G

make
./crawler "Tom Hanks" 2

