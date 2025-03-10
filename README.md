# BenchmarkingAdaptiveIndexes

This repository contains all the files needed to recreate the findings of our paper Benchmarking Adaptive Multidimensional Indices.

The folders contain the following:
- src: contains all the code we implemented for our own hybrid indices
- AIR: contains the code of AIR taken from [here](https://gitlab.com/adaptivertree/artree)
- MultidimensionalAdaptiveIndexing_Clean(_shapes): contains the code for AKD, QUASII, PKD and GPKD taken from [here](https://github.com/pdet/MultidimensionalAdaptiveIndexing), (extended to work for shapes).
- QuadTree: contains the code for the quad tree implementation used in our experiments.

# How to run (easy way)

Since the project consists of multiple files and has a complicated structure we provide a script that (hopefully) will make running the code a smooth experience. All you have to do is run the `run.sh` script and that should create the necessary folders, generate and download data, and run experiments.


# How to run (the hard way)

## Getting the data

You can download the real data used in our experiments [here](https://www.dropbox.com/scl/fi/6zxh51crmvl7eppjw642l/data.tar.gz?rlkey=g307x72o41r89cb9pkstx78fa&dl=0)

Then generate the synthethic data using `python3 generateData.py`

Work in progress...
