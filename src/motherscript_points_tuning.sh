#!/bin/bash


# "Usage: sh runme_experimental_points.sh data_file query_file air_data_file date output_folder repeats"

REPEATS=1
QUERYSIZE=10000
DATASIZE=20000000
FOLDER=05032025_points_tune
SIZE=20M

echo "------------------ UNIFORM ------------------"
datafile="data/20m/synthetic_uniform_2D_20m_norm.txt"
queryfile="data/20m/synthetic_uniform_2D_queries_10K_norm.txt"
airfile="data/20m/synthetic_uniform_2D_20m_norm_air.txt"
./runme_experimental_points_tune.sh $datafile $queryfile $airfile $FOLDER Uniform $REPEATS $QUERYSIZE $DATASIZE $SIZE
echo "------------------ CLUSTERED ------------------"
datafile="data/20m/synthetic_clustered_2D_20m_norm.txt"
queryfile="data/20m/synthetic_clustered_2D_queries_10K_norm.txt"
airfile="data/20m/synthetic_clustered_2D_20m_norm_air.txt"
./runme_experimental_points_tune.sh $datafile $queryfile $airfile $FOLDER Clustered $REPEATS $QUERYSIZE $DATASIZE $SIZE
echo "------------------ SKEWED ------------------"
datafile="data/20m/synthetic_skewNormal_2D_20m_norm.txt"
queryfile="data/20m/synthetic_skewNormal_2D_queries_10K_norm.txt"
airfile="data/20m/synthetic_skewNormal_2D_20m_norm_air.txt"
./runme_experimental_points_tune.sh $datafile $queryfile $airfile $FOLDER Skewed $REPEATS $QUERYSIZE $DATASIZE $SIZE
echo "------------------ ROADS SHUFFLED------------------"
datafile="data/20m/roads_norm_shuffled.txt"
queryfile="data/20m/roads_queries_norm.txt"
airfile="data/20m/roads_norm_shuffled_air.txt"
./runme_experimental_points_tune.sh $datafile $queryfile $airfile $FOLDER Roads $REPEATS $QUERYSIZE 19000000 $SIZE
