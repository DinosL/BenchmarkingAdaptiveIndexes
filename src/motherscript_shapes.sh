#!/bin/bash


# "Usage: sh runme_experimental_points.sh data_file query_file air_data_file date output_folder repeats"

REPEATS=1
QUERYSIZE=10000
DATASIZE=20000000
FOLDER=05032025_shapes
DATACOUNT=20M


echo "------------------ UNIFORM ------------------"
datafile="data/20m/synthetic_uniform_shapes_2D_20m_norm.txt"
queryfile="data/20m/synthetic_uniform_shapes_2D_queries_10K_norm.txt"
extendedQueryFile="data/20m/synthetic_uniform_shapes_2D_queries_10K_norm_extended.txt"
./runme_experimental_shapes.sh $datafile $queryfile $datafile $FOLDER Uniform $REPEATS $QUERYSIZE $DATASIZE $extendedQueryFile $DATACOUNT

echo "------------------ UNIFORM SEQUENTIAL ------------------"
datafile="data/20m/synthetic_uniform_shapes_2D_20m_norm.txt"
queryfile="data/20m/synthetic_uniform_sequential_queries.txt"
extendedQueryFile="data/20m/synthetic_uniform_sequential_queries_extended.txt"
./runme_experimental_shapes.sh $datafile $queryfile $datafile $FOLDER synthetic_uniform_sequential $REPEATS 99 $DATASIZE $extendedQueryFile 20M

echo "------------------ UNIFORM ZOOM IN ------------------"
datafile="data/20m/synthetic_uniform_shapes_2D_20m_norm.txt"
queryfile="data/20m/synthetic_uniform_zoomin_queries.txt"
extendedQueryFile="data/20m/synthetic_uniform_zoomin_queries_extended.txt"
./runme_experimental_shapes.sh $datafile $queryfile $datafile $FOLDER synthetic_uniform_zoomin $REPEATS 1000 $DATASIZE $extendedQueryFile 20M

echo "------------------ CLUSTERED ------------------"
datafile="data/20m/synthetic_clustered_shapes_2D_20m_norm.txt"
queryfile="data/20m/synthetic_clustered_shapes_2D_queries_10K_norm.txt"
extendedQueryFile="data/20m/synthetic_clustered_shapes_2D_queries_10K_norm_extended.txt"
./runme_experimental_shapes.sh $datafile $queryfile $datafile $FOLDER Clustered $REPEATS $QUERYSIZE $DATASIZE $extendedQueryFile 20M

echo "------------------ SKEWNORMAL ------------------"
datafile="data/20m/synthetic_skewNormal_shapes_2D_20m_norm.txt"
queryfile="data/20m/synthetic_skewNormal_shapes_2D_queries_10K_norm.txt"
extendedQueryFile="data/20m/synthetic_skewNormal_shapes_2D_queries_10K_norm_extended.txt"
./runme_experimental_shapes.sh $datafile $queryfile $datafile $FOLDER Skewed $REPEATS $QUERYSIZE $DATASIZE $extendedQueryFile 20M

echo "------------------ ROADS Shuffled ------------------"
datafile="data/20m/roads_mbrs_norm_shuffled.txt"
queryfile="data/20m/roads_queries_norm.txt"
extendedQueryFile="data/20m/roads_queries_norm_extended.txt"
./runme_experimental_shapes.sh $datafile $queryfile $datafile $FOLDER Roads $REPEATS $QUERYSIZE 19000000 $extendedQueryFile 19M



echo "------------------ EXTENSION EXPERIMENT ------------------"

echo "------------------ UNIFORM 0.01 ------------------"
datafile="data/20m/objectSize/synthetic_uniform_shapes_2D_20m_size001_norm.txt"
queryfile="data/20m/objectSize/synthetic_uniform_shapes_2D_queries_size001_10K_norm.txt"
extendedQueryFile="data/20m/objectSize/synthetic_uniform_shapes_2D_queries_size001_10K_norm_extended.txt"
./runme_experimental_shapes.sh $datafile $queryfile $datafile $FOLDER synthetic_uniform_001 $REPEATS $QUERYSIZE $DATASIZE $extendedQueryFile 20M


echo "------------------ UNIFORM 0.05 ------------------"
datafile="data/20m/objectSize/synthetic_uniform_shapes_2D_20m_size005_norm.txt"
queryfile="data/20m/objectSize/synthetic_uniform_shapes_2D_queries_size005_10K_norm.txt"
extendedQueryFile="data/20m/objectSize/synthetic_uniform_shapes_2D_queries_size005_10K_norm_extended.txt"
./runme_experimental_shapes.sh $datafile $queryfile $datafile $FOLDER synthetic_uniform_005 $REPEATS $QUERYSIZE $DATASIZE $extendedQueryFile 20M


echo "------------------ UNIFORM 0.005 ------------------"
datafile="data/20m/objectSize/synthetic_uniform_shapes_2D_20m_size0005_norm.txt"
queryfile="data/20m/objectSize/synthetic_uniform_shapes_2D_queries_size0005_10K_norm.txt"
extendedQueryFile="data/20m/objectSize/synthetic_uniform_shapes_2D_queries_size0005_10K_norm_extended.txt"
./runme_experimental_shapes.sh $datafile $queryfile $datafile $FOLDER synthetic_uniform_0005 $REPEATS $QUERYSIZE $DATASIZE $extendedQueryFile 20M

echo "------------------ EXPONENTIALY SIZED ------------------"
datafile="data/synthetic_uniformly_located_exponentialy_sized_data_norm.txt"
queryfile="data/synthetic_uniformly_located_uniformly_sized_workload_norm.txt"
extendedQueryFile="data/synthetic_uniformly_located_uniformly_sized_workload_norm_extended.txt"
./runme_experimental_shapes.sh $datafile $queryfile $datafile $FOLDER exponential $REPEATS $QUERYSIZE 20000000 $extendedQueryFile 20M