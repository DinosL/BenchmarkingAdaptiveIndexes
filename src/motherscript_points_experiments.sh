#!/bin/bash


# "Usage: sh runme_experimental_points.sh data_file query_file air_data_file date output_folder repeats"

REPEATS=1
QUERYSIZE=10000
DATASIZE=20000000
FOLDER=05032025
SIZE=20M

echo "------------------ UNIFORM ------------------"
datafile="data/20m/synthetic_uniform_2D_20m_norm.txt"
queryfile="data/20m/synthetic_uniform_2D_queries_10K_norm.txt"
airfile="data/20m/synthetic_uniform_2D_20m_norm_air.txt"
./runme_experimental_points_experiments.sh $datafile $queryfile $airfile $FOLDER Uniform $REPEATS $QUERYSIZE $DATASIZE $SIZE
echo "------------------ UNIFORM SEQUENTIAL ------------------"
datafile="data/20m/synthetic_uniform_2D_20m_norm.txt"
queryfile="data/20m/synthetic_uniform_sequential_queries.txt"
airfile="data/20m/synthetic_uniform_2D_20m_norm_air.txt"
./runme_experimental_points_experiments.sh $datafile $queryfile $airfile $FOLDER Uniform_Sequential $REPEATS 99 $DATASIZE $SIZE
echo "------------------ UNIFORM ZOOM IN ------------------"
datafile="data/20m/synthetic_uniform_2D_20m_norm.txt"
queryfile="data/20m/synthetic_uniform_zoomin_queries.txt"
airfile="data/20m/synthetic_uniform_2D_20m_norm_air.txt"
./runme_experimental_points_experiments.sh $datafile $queryfile $airfile $FOLDER Uniform_ZoomIn $REPEATS 1000 $DATASIZE $SIZE
echo "------------------ CLUSTERED ------------------"
datafile="data/20m/synthetic_clustered_2D_20m_norm.txt"
queryfile="data/20m/synthetic_clustered_2D_queries_10K_norm.txt"
airfile="data/20m/synthetic_clustered_2D_20m_norm_air.txt"
./runme_experimental_points_experiments.sh $datafile $queryfile $airfile $FOLDER Clustered $REPEATS $QUERYSIZE $DATASIZE $SIZE
echo "------------------ SKEWED ------------------"
datafile="data/20m/synthetic_skewNormal_2D_20m_norm.txt"
queryfile="data/20m/synthetic_skewNormal_2D_queries_10K_norm.txt"
airfile="data/20m/synthetic_skewNormal_2D_20m_norm_air.txt"
./runme_experimental_points_experiments.sh $datafile $queryfile $airfile $FOLDER Skewed $REPEATS $QUERYSIZE $DATASIZE $SIZE
echo "------------------ ROADS SHUFFLED------------------"
datafile="data/20m/roads_norm_shuffled.txt"
queryfile="data/20m/roads_queries_norm.txt"
airfile="data/20m/roads_norm_shuffled_air.txt"
./runme_experimental_points_experiments.sh $datafile $queryfile $airfile $FOLDER Roads $REPEATS $QUERYSIZE 19000000 $SIZE

echo "------------------ UNIFORM SELECTIVITY 0.1% ------------------"
datafile="data/20m/synthetic_uniform_2D_20m_norm.txt"
queryfile="data/20m/selectivities/synthetic_uniform_2D_queries_selectivity01_10K_norm.txt"
airfile="data/20m/synthetic_uniform_2D_20m_norm_air.txt"
./runme_experimental_points_experiments.sh $datafile $queryfile $airfile $FOLDER Uniform_selectivity01 $REPEATS $QUERYSIZE $DATASIZE $SIZE

echo "------------------ UNIFORM SELECTIVITY 0.01% ------------------"
datafile="data/20m/synthetic_uniform_2D_20m_norm.txt"
queryfile="data/20m/selectivities/synthetic_uniform_2D_queries_selectivity001_10K_norm.txt"
airfile="data/20m/synthetic_uniform_2D_20m_norm_air.txt"
./runme_experimental_points_experiments.sh $datafile $queryfile $airfile $FOLDER Uniform_selectivity001 $REPEATS $QUERYSIZE $DATASIZE $SIZE

echo "------------------ UNIFORM SELECTIVITY 0.001% ------------------"
datafile="data/20m/synthetic_uniform_2D_20m_norm.txt"
queryfile="data/20m/selectivities/synthetic_uniform_2D_queries_selectivity0001_10K_norm.txt"
airfile="data/20m/synthetic_uniform_2D_20m_norm_air.txt"
./runme_experimental_points_experiments.sh $datafile $queryfile $airfile $FOLDER Uniform_selectivity0001 $REPEATS $QUERYSIZE $DATASIZE $SIZE

echo "------------------ UNIFORM SELECTIVITY 0.0001% ------------------"
datafile="data/20m/synthetic_uniform_2D_20m_norm.txt"
queryfile="data/20m/selectivities/synthetic_uniform_2D_queries_selectivity00001_10K_norm.txt"
airfile="data/20m/synthetic_uniform_2D_20m_norm_air.txt"
./runme_experimental_points_experiments.sh $datafile $queryfile $airfile $FOLDER Uniform_selectivity00001 $REPEATS $QUERYSIZE $DATASIZE $SIZE



DATASIZE=40000000
SIZE=40M

echo "------------------ UNIFORM 40M ------------------"
datafile="data/40m/synthetic_uniform_2D_40m_norm.txt"
queryfile="data/40m/synthetic_uniform_2D_queries_norm.txt"
airfile="data/40m/synthetic_uniform_2D_40m_norm_air.txt"
./runme_experimental_points_experiments.sh $datafile $queryfile $airfile $FOLDER Uniform_40M $REPEATS $QUERYSIZE $DATASIZE $SIZE

echo "------------------ CLUSTERED 40M ------------------"
datafile="data/40m/synthetic_clustered_2D_40m_norm.txt"
queryfile="data/40m/synthetic_clustered_2D_queries_10K_norm.txt"
airfile="data/40m/synthetic_clustered_2D_40m_norm_air.txt"
./runme_experimental_points_experiments.sh $datafile $queryfile $airfile $FOLDER Clustered_40M $REPEATS $QUERYSIZE $DATASIZE $SIZE

DATASIZE=60000000
SIZE=60M

echo "------------------ UNIFORM 60M ------------------"
datafile="data/60m/synthetic_uniform_2D_60m_norm.txt"
queryfile="data/60m/synthetic_uniform_2D_queries_norm.txt"
airfile="data/60m/synthetic_uniform_2D_60m_norm_air.txt"
./runme_experimental_points_experiments.sh $datafile $queryfile $airfile $FOLDER Uniform_60M $REPEATS $QUERYSIZE $DATASIZE $SIZE

echo "------------------ CLUSTERED 60M ------------------"
datafile="data/60m/synthetic_clustered_2D_60m_norm.txt"
queryfile="data/60m/synthetic_clustered_2D_queries_10K_norm.txt"
airfile="data/60m/synthetic_clustered_2D_60m_norm_air.txt"
./runme_experimental_points_experiments.sh $datafile $queryfile $airfile $FOLDER Clustered_60M $REPEATS $QUERYSIZE $DATASIZE $SIZE

DATASIZE=80000000
SIZE=80M

echo "------------------ UNIFORM 80M ------------------"
datafile="data/80m/synthetic_uniform_2D_80m_norm.txt"
queryfile="data/80m/synthetic_uniform_2D_queries_norm.txt"
airfile="data/80m/synthetic_uniform_2D_80m_norm_air.txt"
./runme_experimental_points_experiments.sh $datafile $queryfile $airfile $FOLDER Uniform_80M $REPEATS $QUERYSIZE $DATASIZE $SIZE

echo "------------------ CLUSTERED 80M ------------------"
datafile="data/80m/synthetic_clustered_2D_80m_norm.txt"
queryfile="data/80m/synthetic_clustered_2D_queries_10K_norm.txt"
airfile="data/80m/synthetic_clustered_2D_80m_norm_air.txt"
./runme_experimental_points_experiments.sh $datafile $queryfile $airfile $FOLDER Clustered_80M $REPEATS $QUERYSIZE $DATASIZE $SIZE