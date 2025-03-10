#!/bin/bash


if [ "$#" -ne 10 ]; then
    echo "Usage: sh runme_experimental_points.sh data_file query_file air_data_file date output_folder repeats querysize datasize data_count(format: 1M, 19M, etc) NUMDIMS"
    exit 2
fi

# samples=(1000)
# batchlengths=(10 100)
# gridSizes=(30 50 100 200 500 1000)
# gridSizes=(100 500)
# QUERYLENGTH=10000
# DATACOUNT=67M # same as datasize but for akd/grid/etc

DATA_FILE=$1
QUERY_FILE=$2
AIR_DATA_FILE=$3
DATE=$4
DATASETNAME=$DATE"/"$5"/"
REPEAT=$6
QUERYLENGTH=$7
DATASIZE=$8
DATACOUNT=$9
NUMDIMS=${10}

if [ ! -d "results/"$DATASETNAME"/" ]
then
    mkdir -p "results/"$DATASETNAME"/"
fi


# rm linear
# g++ -D datacount$DATACOUNT -D thresh512 -D DIM$NUMDIMS -O3 -o linear main_linearScan.cpp

# for i in $(seq 1 $REPEAT)
# do
#     # echo "akd $DATA_FILE $QUERYLENGTH $QUERY_FILE "results/"$DATASETNAME"/times_our_akd_"$i".txt" 10 "
#     ./linear $DATA_FILE $QUERYLENGTH $QUERY_FILE "results/"$DATASETNAME"/times_linearScan_"$i".txt" 10 
# done

# echo "-------------- done with linear scan -------------- "


if [ $NUMDIMS -eq 3 ]; then

    rm grid_3D
    g++ -O3 -D datacount$DATACOUNT -D thresh512 -D DIM$NUMDIMS -o grid_3D main_grid_3D.cpp

    gridSize=200

    for i in $(seq 1 $REPEAT)
    do
        ./grid_3D $DATA_FILE $QUERYLENGTH $QUERY_FILE "results/"$DATASETNAME"/times_gridAKD_"$gridSize"_"$i".txt" $gridSize 
    done
    echo "-------------- done with grid --------------"

    rm gridAAKD_3D
    g++ -O3 -D datacount$DATACOUNT -D thresh1024 -D STOCHASTIC -D DIM$NUMDIMS -o gridAAKD_3D main_gridAAKD_3D.cpp
    gridSizes=(70 100 200)

    for gridSize in ${gridSizes[@]}; do
        for i in $(seq 1 $REPEAT)
        do
            ./gridAAKD_3D $DATA_FILE $QUERYLENGTH $QUERY_FILE "results/"$DATASETNAME"/times_gridAAKD_"$gridSize"_"$i".txt" $gridSize 
        done
    done
    echo "-------------- done with grid AAKD --------------"


    rm nonAdaptiveGrid_3D
    g++ -O3 -D datacount$DATACOUNT -D thresh512 -D DIM$NUMDIMS -o nonAdaptiveGrid_3D main_nonAdaptiveGrid_3D.cpp 

    # gridSize=200

    for gridSize in ${gridSizes[@]}; do
        for i in $(seq 1 $REPEAT)
        do
            ./nonAdaptiveGrid_3D $DATA_FILE $QUERYLENGTH $QUERY_FILE "results/"$DATASETNAME"/times_grid_"$gridSize"_"$i".txt" $gridSize 
        done
    done

    echo "-------------- done with non adaptive grid --------------"

    nodeSize=2048

    rm ../AIR/boost3D
    g++ -O3 -D nodeSize$nodeSize -I ../boost_1_86_0/ ../AIR/main_boost_3d.cpp -o ../AIR/boost3D
    for i in $(seq 1 $REPEAT)
    do
        ../AIR/boost3D "../src/"$AIR_DATA_FILE $QUERYLENGTH "../src/"$QUERY_FILE "../src/results/"$DATASETNAME"/times_boost_nodeSize"$nodeSize"_"$i".txt" $DATASIZE
    done

    echo "-------------- done with RTREE tune --------------"


fi


rm ../AIR/air
threshold=4096

g++ -O3 -D fo16 -D maxt4096 -D mint2048 -D DIM$NUMDIMS -D datacount$DATACOUNT  -o ../AIR/air ../AIR/general_testing_air.cpp

for i in $(seq 1 $REPEAT)
do
    ../AIR/air "../src/"$AIR_DATA_FILE $QUERYLENGTH "../src/"$QUERY_FILE "../src/results/"$DATASETNAME"/times_air_thresh"$threshold"_"$i".txt"
done

echo "-------------- done with AIR --------------"

threshold=2048
python3 ../MultidimensionalAdaptiveIndexing_Clean/experiments/real-data-workload/run_akd_quasii.py $DATA_FILE $QUERY_FILE $QUERYLENGTH $REPEAT $DATASIZE $NUMDIMS "../src/results/"$DATASETNAME $threshold

echo "-------------- done with AKD, QUASII --------------"

rm akd_heuristic
g++ -D datacount$DATACOUNT -D thresh1024 -D STOCHASTIC -D DIM$NUMDIMS -O3 -o akd_heuristic  main_akd_heuristic.cpp

for i in $(seq 1 $REPEAT)
do
    ./akd_heuristic $DATA_FILE $QUERYLENGTH $QUERY_FILE "results/"$DATASETNAME"/times_AAKD_thesh1024_stochastic_"$i".txt" 10 
done

echo "-------------- done with AAKD -------------- "

# first convert the data to a format readable by AV
python3 ../AVtree/convertDataToAVFormat.py $DATA_FILE $QUERY_FILE $DATASIZE $QUERYLENGTH $NUMDIMS "../AVtree/datasets/"

make clean -C ../AVtree
make main_range distFunc=LMAX dims=$NUMDIMS datacount=$9 -C ../AVtree
dataset=$(basename $DATA_FILE)
queryset=$(basename $QUERY_FILE)
thersholds=(256)

for i in $(seq 1 $REPEAT)
do
    for thresh in ${thersholds[@]}; do
        ../AVtree/range -m -t $thresh "../AVtree/datasets/"$dataset "../AVtree/datasets/"$queryset > "results/"$DATASETNAME"/times_avtree_thresh"$thresh"_"$i".txt"
    done
done

echo "-------------- done with AVTree Lmax -------------- "

