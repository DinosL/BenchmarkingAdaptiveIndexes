#!/bin/bash


# "Usage: sh runme_experimental_points.sh data_file query_file air_data_file date output_folder repeats"

REPEATS=1
QUERYSIZE=10000
DATASIZE=20000000
# FOLDER=11112024_bestThresholds
FOLDER=05032025
SIZE=20M

echo "------------------ TAXI 3D  ------------------"
datafile="data/yellow_JanJuly24_data_clean_3D.txt"
queryfile="data/yellow_JanJuly24_queries_new_2_norm_3D.txt"
airfile="data/yellow_JanJuly24_data_clean_3D_air.txt"
#./runme_experimental_points_highD.sh $datafile $queryfile $airfile $FOLDER taxi_3D $REPEATS $QUERYSIZE $DATASIZE $SIZE 3

echo "------------------ TAXI 4D  ------------------"
datafile="data/yellow_JanJuly24_data_clean_4D.txt"
queryfile="data/yellow_JanJuly24_queries_new_2_norm_4D.txt"
airfile="data/yellow_JanJuly24_data_clean_4D_air.txt"
./runme_experimental_points_highD.sh $datafile $queryfile $airfile $FOLDER taxi_4D $REPEATS $QUERYSIZE $DATASIZE $SIZE 4

echo "------------------ TAXI 5D  ------------------"
datafile="data/yellow_JanJuly24_data_clean_5D.txt"
queryfile="data/yellow_JanJuly24_queries_new_2_norm_5D.txt"
airfile="data/yellow_JanJuly24_data_clean_5D_air.txt"
./runme_experimental_points_highD.sh $datafile $queryfile $airfile $FOLDER taxi_5D $REPEATS $QUERYSIZE $DATASIZE $SIZE 5

echo "------------------ TAXI 6D  ------------------"
datafile="data/yellow_JanJuly24_data_clean_6D.txt"
queryfile="data/yellow_JanJuly24_queries_new_2_norm_6D.txt"
airfile="data/yellow_JanJuly24_data_clean_5D_air.txt"
./runme_experimental_points_highD.sh $datafile $queryfile $airfile $FOLDER taxi_6D $REPEATS $QUERYSIZE $DATASIZE $SIZE 6
