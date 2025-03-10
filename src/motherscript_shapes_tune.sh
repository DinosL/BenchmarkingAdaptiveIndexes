#!/bin/bash


# "Usage: sh runme_experimental_points.sh data_file query_file air_data_file date output_folder repeats"

REPEATS=1
QUERYSIZE=10000
DATASIZE=20000000
FOLDER=03052025_shapes_tune
DATACOUNT=20M


echo "------------------ UNIFORM ------------------"
datafile="data/20m/synthetic_uniform_shapes_2D_20m_norm.txt"
queryfile="data/20m/synthetic_uniform_shapes_2D_queries_10K_norm.txt"
extendedQueryFile="data/20m/synthetic_uniform_shapes_2D_queries_10K_norm_extended.txt"
./runme_experimental_shapes_bestMethods.sh $datafile $queryfile $datafile $FOLDER Uniform $REPEATS $QUERYSIZE $DATASIZE $extendedQueryFile $DATACOUNT


#echo "------------------ CLUSTERED ------------------"
#datafile="data/20m/synthetic_clustered_shapes_2D_20m_norm.txt"
#queryfile="data/20m/synthetic_clustered_shapes_2D_queries_10K_norm.txt"
#extendedQueryFile="data/20m/synthetic_clustered_shapes_2D_queries_10K_norm_extended.txt"
#./runme_experimental_shapes_bestMethods.sh $datafile $queryfile $datafile $FOLDER synthetic_clustered $REPEATS $QUERYSIZE $DATASIZE $extendedQueryFile 20M

#echo "------------------ SKEWNORMAL ------------------"
#datafile="data/20m/synthetic_skewNormal_shapes_2D_20m_norm.txt"
#queryfile="data/20m/synthetic_skewNormal_shapes_2D_queries_10K_norm.txt"
#extendedQueryFile="data/20m/synthetic_skewNormal_shapes_2D_queries_10K_norm_extended.txt"
#./runme_experimental_shapes_bestMethods.sh $datafile $queryfile $datafile $FOLDER synthetic_skewNormal $REPEATS $QUERYSIZE $DATASIZE $extendedQueryFile 20M


#echo "------------------ ROADS Shuffled ------------------"
#datafile="data/1m/roads_mbrs_norm_shuffled.txt"
#queryfile="data/1m/roads_queries_norm.txt"
#extendedQueryFile="data/1m/roads_queries_norm_extended.txt"
#./runme_experimental_shapes_bestMethods.sh $datafile $queryfile $datafile $FOLDER roads_shuffled $REPEATS $QUERYSIZE 19000000 $extendedQueryFile 19M

