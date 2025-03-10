#!/bin/bash


if [ "$#" -ne 9 ]; then
    echo "Usage: sh runme_experimental_points.sh data_file query_file air_data_file date output_folder repeats querysize datasize data_count(format: 1M, 19M, etc)"
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
DATACOUNT=$9

if [ ! -d "results/"$DATASETNAME"/" ]
then
    mkdir -p "results/"$DATASETNAME"/"
fi


# gridSizes=(10 15 30 50 100 200 500 1000)

# gridSizes=(200)

# rm nonAdaptiveGrid
# g++ -O3 -D datacount$DATACOUNT -D thresh512 -D DIM$NUMDIMS -o nonAdaptiveGrid main_nonAdaptiveGrid.cpp 

# for gridSize in ${gridSizes[@]}; do
#     for i in $(seq 1 $REPEAT)
#     do
#         ./nonAdaptiveGrid $DATA_FILE $QUERYLENGTH $QUERY_FILE "results/"$DATASETNAME"/times_grid_"$gridSize"_"$i".txt" $gridSize 
#     done
#     echo "Done with "$gridSize
# done

# echo "-------------- done with non adaptive grid --------------"

# rm irregular_grid
# g++ -D datacount$DATACOUNT -D thresh1024 -D STOCHASTIC -D DIM$NUMDIMS -O3 -o irregular_grid main_irregular_grid_akd_heuristic.cpp

# # SAMPLESIZE=(10 50 100 500 1000 5000 10000)
# # gridSizes=(10 50 100 500 1000)

# SAMPLESIZE=(5000)
# gridSizes=(100)

# for i in $(seq 1 $REPEAT)
# do
#     for bs in ${SAMPLESIZE[@]}; do
#         for grid in ${gridSizes[@]}; do
#             ./irregular_grid $DATA_FILE $QUERYLENGTH $QUERY_FILE "results/"$DATASETNAME"/times_irregular_grid"$grid"_sample"$bs"_stochastic_"$i".txt" $bs $grid
#         done
#     done
# done

# echo "-------------- done with irregular grid -------------- "

# make clean -C ../QuadTree
# make quadTreePoints -C ../QuadTree

# # capacities=(100 500 1000 5000 10000)
# # hs=(8 10 12 14)

# capacities=(10000)
# hs=(10)

# for i in $(seq 1 $REPEAT)
# do
#     for capacity in ${capacities[@]}; do
#         for height in ${hs[@]}; do
#             ../QuadTree/qt_points -c $capacity -h $height -w $DATA_FILE $QUERY_FILE > "../src/results/"$DATASETNAME"/times_quadTree_capacity"$capacity"_height"$height"_"$i".txt"
#         done
#     done
# done

# echo "-------------- done with quad tree -------------- "


# # thresholds=(128 256 512 1024 2048 4096)

# thresholds=(2048)
# for thr in ${thresholds[@]}; do
#     python3 ../MultidimensionalAdaptiveIndexing_Clean/experiments/real-data-workload/run_akd_tune.py $DATA_FILE $QUERY_FILE $QUERYLENGTH $REPEAT $DATASIZE $NUMDIMS "../src/results/"$DATASETNAME $thr
# done

# echo "-------------- done with AKD and QUASII tune --------------"

# python3 ../MultidimensionalAdaptiveIndexing_Clean/experiments/real-data-workload/run_gpkd_tune.py $DATA_FILE $QUERY_FILE $QUERYLENGTH $REPEAT $DATASIZE $NUMDIMS "../src/results/"$DATASETNAME 2048

# echo "-------------- done with GPKD --------------"

# # nodeSizes=(256 512 1024 2048 4096 8192)

# nodeSizes=(2048)

# for ns in ${nodeSizes[@]}; do
#     rm ../AIR/boost 
#     g++ -O3 -D nodeSize$ns -I ../boost_1_86_0/ ../AIR/main_boost.cpp -o ../AIR/boost 
#     for i in $(seq 1 $REPEAT)
#     do
#         ../AIR/boost "../src/"$AIR_DATA_FILE $QUERYLENGTH "../src/"$QUERY_FILE "../src/results/"$DATASETNAME"/times_boost_nodeSize"$ns"_"$i".txt" $DATASIZE
#     done
# done

# echo "-------------- done with RTREE tune --------------"

# first convert the data to a format readable by AV
if [ ! -d "../AVtree/datasets/" ]
then
    mkdir -p "../AVtree/datasets/"
fi
python3 ../AVtree/convertDataToAVFormat.py $DATA_FILE $QUERY_FILE $DATASIZE $QUERYLENGTH $NUMDIMS "../AVtree/datasets/"

make clean -C ../AVtree
make main_range distFunc=LMAX dims=$NUMDIMS datacount=$9 -C ../AVtree
dataset=$(basename $DATA_FILE)
queryset=$(basename $QUERY_FILE)
# thersholds=(128 256 512 1024 2048)

thersholds=(512)

for i in $(seq 1 $REPEAT)
do
    for thresh in ${thersholds[@]}; do
        ../AVtree/range -m -t $thresh "../AVtree/datasets/"$dataset "../AVtree/datasets/"$queryset > "results/"$DATASETNAME"/times_avtree_thresh"$thresh"_"$i".txt"
    done
done

echo "-------------- done with AVTree Lmax -------------- "


# thresholds=(256 512 1024 2048 4096 8192)

# thresholds=(4096)
# rm ../AIR/air


# for thresh in ${thresholds[@]}; do

#     minThresh=$((thresh/2))
#     g++ -O3 -D fo16 -D maxt$thresh -D mint$minThresh -D DIM$NUMDIMS -D datacount$DATACOUNT  -o ../AIR/air ../AIR/general_testing_air.cpp
    
#     for i in $(seq 1 $REPEAT)
#     do
#         ../AIR/air "../src/"$AIR_DATA_FILE $QUERYLENGTH "../src/"$QUERY_FILE "../src/results/"$DATASETNAME"/times_air_thresh"$thresh"_"$i".txt"
#     done
# done

# echo "-------------- done with AIR --------------"



# # thresholds=(256 512 1024 2048 4096 8192)


# thresholds=(4096)

# for thresh in ${thresholds[@]}; do
#     rm akd_heuristic
#     g++ -D datacount$DATACOUNT -D thresh$thresh -D STOCHASTIC -D DIM$NUMDIMS -O3 -o akd_heuristic  main_akd_heuristic.cpp

#     for i in $(seq 1 $REPEAT)
#     do
#         ./akd_heuristic $DATA_FILE $QUERYLENGTH $QUERY_FILE "results/"$DATASETNAME"/times_AAKD_stochastic_thresh"$thresh"_"$i".txt" 10 
#     done
# done

# echo "-------------- done with AAKD new Heuristic -------------- "

# # gridSizes=(10 30 50 100 200 500)
# # thresholds=(32 64 128 256 512 1024)

# gridSizes=(200)
# thresholds=(1024)

# for thresh in ${thresholds[@]}; do
#     rm grid_aakd
#     g++ -O3 -D datacount$DATACOUNT -D thresh$thresh -D STOCHASTIC -D DIM$NUMDIMS -o grid_aakd main_grid_heuristic.cpp

#     for gridSize in ${gridSizes[@]}; do
#         for i in $(seq 1 $REPEAT)
#         do
#             ./grid_aakd $DATA_FILE $QUERYLENGTH $QUERY_FILE "results/"$DATASETNAME"/times_gridAAKD_newStochastic_thresh"$thresh"_"$gridSize"_"$i".txt" $gridSize 
#         done
#     done
# done

# echo "-------------- done with grid + AAKD --------------"