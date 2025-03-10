#!/bin/bash


# create folders

if [ ! -d "data/40m/" ]
then
    mkdir -p "data/40m"
fi

if [ ! -d "data/60m/" ]
then
    mkdir -p "data/60m"
fi

if [ ! -d "data/80m/" ]
then
    mkdir -p "data/80m"
fi


# create data

python3 generateData.py

# compile AKD/GPKD/PKD/QUASII

cd ../MultidimensionalAdaptiveIndexing_Clean/


rm -r build
rm -r bin
rm cmake_install.cmake
rm CMakeCache.txt
rm -r CMakeFiles

cmake .
make -j

cd ../MultidimensionalAdaptiveIndexing_Clean_shapes/

rm -r build
rm -r bin
rm cmake_install.cmake
rm CMakeCache.txt
rm -r CMakeFiles

cmake .
make -j


# download data
cd ..
wget https://www.dropbox.com/scl/fi/2f3642p3bh6ev19yt6fyf/data.tar.gz?rlkey=ke9iosy6k2ksk4tpxt9zrugzq
cp data.tar.gz\?rlkey\=ke9iosy6k2ksk4tpxt9zrugzq data.tar.gz
rm data.tar.gz\?rlkey\=ke9iosy6k2ksk4tpxt9zrugzq
tar -xzvf data.tar.gz
mv data/roads* src/data/20m/
mv data/synthetic_uniformly_* src/data
mv data/yellow* src/data
rm -r data
rm data.tar.gz


cd ../src
chmod 777 *.sh


# ====================================== Cleanup unsused files to save space ======================================











# ====================================== POINTS EXPERIMENTS ======================================

echo "====================================== POINTS EXPERIMENTS ======================================"
./motherscript_points_tuning.sh
echo "----Done with tuning ----"
./motherscript_points_experiments.sh
echo "----Done with experiments ----"
motherscript_points_highD.sh
echo "----Done with high dimensional ----"


echo "====================================== SHAPES EXPERIMENTS ======================================"
./motherscript_shapes_tune.sh
echo "----Done with tuning ----"
./motherscript_shapes.sh
echo "----Done with experiments ----"


# Make plots

