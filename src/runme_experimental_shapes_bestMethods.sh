#!/bin/bash


if [ "$#" -ne 10 ]; then
    echo "Usage: sh runme_experimental_points.sh data_file query_file air_data_file date output_folder repeats querysize datasize extended_query_file data_count(format: 1M, 19M, etc)"
    exit 2
fi

NUMDIMS=2

DATA_FILE=$1
QUERY_FILE=$2
AIR_DATA_FILE=$3
DATE=$4
DATASETNAME=$DATE"/"$5"/"
REPEAT=$6
QUERYLENGTH=$7
DATASIZE=$8
EXTENDED_QUERY_FILE=$9
DATACOUNT=${10}


if [ ! -d "results/"$DATASETNAME"/" ]
then
    mkdir -p "results/"$DATASETNAME"/"
fi

echo "====================================== STATIC METHODS ======================================"



# nodeSizes=(256 512 1024 2048 4096 8192)
nodeSizes=(4096)
for ns in ${nodeSizes[@]}; do
    rm ../AIR/boost 
    g++ -O3 -I ../boost_1_86_0/ -D nodeSize$nodeSizes ../AIR/main_boost.cpp -o ../AIR/boost 

    for i in $(seq 1 $REPEAT)
    do
        ../AIR/boost "../src/"$DATA_FILE $QUERYLENGTH "../src/"$QUERY_FILE "../src/results/"$DATASETNAME"/times_boost_nodeSize"$ns"_"$i".txt" $DATASIZE
    done
done

echo "-------------- done with RTREE --------------"



make clean -C ../QuadTree
make quadTreePoints -C ../QuadTree

# capacities=(100 500 1000 5000 10000)
# hs=(8 10 12 14)


capacities=(10000)
hs=(10)

for capacity in ${capacities[@]}; do
    for height in ${hs[@]}; do
        for i in $(seq 1 $REPEAT)
        do
            ../QuadTree/qt_points -c $capacity -h $height -w $DATA_FILE $QUERY_FILE > "../src/results/"$DATASETNAME"/times_quadTree_capacity"$capacity"_height"$height"_"$i".txt"
        done
    done
done

echo "-------------- done with quad tree -------------- "


# gridSizes=(10 15 30 50 100 200 500 1000)
gridSizes=(10)
rm nonAdaptiveGrid_shapes
g++ -O3 -D datacount$DATACOUNT -D DIM2 -o nonAdaptiveGrid_shapes main_nonAdaptiveGrid_shapes.cpp 

for gridSize in ${gridSizes[@]}; do
    for i in $(seq 1 $REPEAT)
    do
        ./nonAdaptiveGrid_shapes $DATA_FILE $QUERYLENGTH $QUERY_FILE "results/"$DATASETNAME"/times_grid"$gridSize"_"$i".txt" $gridSize 
    done
done

echo "-------------- done with grid --------------"



echo "====================================== GRID METHODS ======================================"

gridSizes=(10 15 30 50 100 200 500 1000)
thresholds=(256 512 1024 2048 4096 8192)

gridSizes=(200)
thresholds=(4096)

for thresh in ${thresholds[@]}; do
    rm grid_aakd_shapes
    g++ -O3 -D datacount$DATACOUNT -D thresh$thresh -D STOCHASTIC -D DIM$NUMDIMS -o grid_aakd_shapes main_grid_shapes_heuristic.cpp
    for gridSize in ${gridSizes[@]}; do
        for i in $(seq 1 $REPEAT)
        do
            ./grid_aakd_shapes $DATA_FILE $QUERYLENGTH $QUERY_FILE "results/"$DATASETNAME"/times_gridAAKD_"$gridSize"_thresh"$thresh"_"$i".txt" $gridSize $EXTENDED_QUERY_FILE
        done
    done
done

echo "-------------- done with grid + AAKD --------------"


rm irregular_grid
echo "g++ -D datacount$DATACOUNT -D thresh1024 -D DIM$NUMDIMS -O3 -o irregular_grid main_irregular_grid_akd_shapes.cpp"

# SAMPLESIZE=(10 50 100 500 1000 5000 10000)
# gridSizes=(10 50 100 500 1000)

SAMPLESIZE=(10000)
gridSizes=(100)

for sample in ${SAMPLESIZE[@]}; do
    for gridSize in ${gridSizes[@]}; do
        for i in $(seq 1 $REPEAT)
        do
            echo "./irregular_grid $DATA_FILE $QUERYLENGTH $QUERY_FILE "results/"$DATASETNAME"/times_irregular_grid"$gridSize"_sample"$sample"_"$i".txt" $sample $gridSize" $EXTENDED_QUERY_FILE
        done
    done
done

echo "-------------- done with irregular grid -------------- "

echo "====================================== AKD METHODS ======================================"

# thresholds=(128 256 512 1024 2048 4096 8192)

thresholds=(4096)

for thr in ${thresholds[@]}; do
    python3 ../MultidimensionalAdaptiveIndexing_Clean_shapes/experiments/real-data-workload/run.py $DATA_FILE $QUERY_FILE $QUERYLENGTH $REPEAT $DATASIZE $NUMDIMS "../src/results/"$DATASETNAME $thr $EXTENDED_QUERY_FILE
done
echo "-------------- done with AKD and QUASII --------------"

# thresholds=(256 512 1024 2048 4096 8192)
thresholds=(4096)

for thresh in ${thresholds[@]}; do
    rm akd_heuristic_shapes
    g++ -D datacount$DATACOUNT -D thresh$thresh -D STOCHASTIC -O3 -D DIM2 -o akd_heuristic_shapes main_akd_heuristic_shapes.cpp

    for i in $(seq 1 $REPEAT)
    do
        ./akd_heuristic_shapes $DATA_FILE $QUERYLENGTH $QUERY_FILE "results/"$DATASETNAME"/times_AAKD_thresh"$thresh"_stochastic_"$i".txt" 10 $EXTENDED_QUERY_FILE
    done
done

echo "-------------- done with AAKD --------------"

# thresholds=(256 512 1024 2048 4096 8192)
# gridSizes=(10 15 30 50 100 200 500 1000)

thresholds= (4096)
gridSizes=(200)

for thresh in ${thresholds[@]}; do
    rm grid_air_shapes_qwe
    g++ -O3 -D datacount$DATACOUNT -D thresh$thresh -D DIM2 -o grid_air_shapes_qwe main_grid_shapes_air_qwe.cpp
    for gridSize in ${gridSizes[@]}; do
        for i in $(seq 1 $REPEAT)
        do
            ./grid_air_shapes_qwe $DATA_FILE $QUERYLENGTH $QUERY_FILE "results/"$DATASETNAME"/times_gridQWEAIR_"$gridSize"_thresh"$thresh"_"$i".txt" $gridSize $EXTENDED_QUERY_FILE
        done
    done
done

echo "-------------- done with grid + AIR QWE --------------"
