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


threshold=2048
python3 ../MultidimensionalAdaptiveIndexing_Clean/experiments/real-data-workload/run_akd_quasii.py $DATA_FILE $QUERY_FILE $QUERYLENGTH $REPEAT $DATASIZE $NUMDIMS "../src/results/"$DATASETNAME $threshold

echo "-------------- done with AKD, QUASII --------------"

threshold=4096

rm ../AIR/air
g++ -O3 -D fo16 -D maxt4096 -D mint2048 -D DIM$NUMDIMS -D datacount$DATACOUNT  -o ../AIR/air ../AIR/general_testing_air.cpp

for i in $(seq 1 $REPEAT)
do
    ../AIR/air "../src/"$AIR_DATA_FILE $QUERYLENGTH "../src/"$QUERY_FILE "../src/results/"$DATASETNAME"/times_air_thresh"$threshold"_"$i".txt"
done

echo "-------------- done with AIR --------------"


rm akd_heuristic
g++ -D datacount$DATACOUNT -D thresh1024 -D STOCHASTIC -mcmodel=large -D DIM$NUMDIMS -O3 -o akd_heuristic  main_akd_heuristic.cpp

for i in $(seq 1 $REPEAT)
do
    ./akd_heuristic $DATA_FILE $QUERYLENGTH $QUERY_FILE "results/"$DATASETNAME"/times_AAKD_thresh1024_stochastic_"$i".txt" 10 
done

echo "-------------- done with AAKD -------------- "

gridSize=200
rm nonAdaptiveGrid
g++ -O3 -D datacount$DATACOUNT -D thresh1024 -mcmodel=large -D DIM$NUMDIMS -o nonAdaptiveGrid main_nonAdaptiveGrid.cpp 

for i in $(seq 1 $REPEAT)
do
    ./nonAdaptiveGrid $DATA_FILE $QUERYLENGTH $QUERY_FILE "results/"$DATASETNAME"/times_grid_"$gridSize"_"$i".txt" $gridSize 
done
echo "Done with "$gridSize

echo "-------------- done with non adaptive grid --------------"


gridSize=200
rm grid_aakd
g++ -O3 -D datacount$DATACOUNT -D thresh1024 -mcmodel=large -D STOCHASTIC -D DIM$NUMDIMS -o grid_aakd main_grid_heuristic.cpp

for i in $(seq 1 $REPEAT)
do
    ./grid_aakd $DATA_FILE $QUERYLENGTH $QUERY_FILE "results/"$DATASETNAME"/times_gridAAKD_"$gridSize"_"$i".txt" $gridSize 
done

echo "-------------- done with grid + AAKD --------------"







