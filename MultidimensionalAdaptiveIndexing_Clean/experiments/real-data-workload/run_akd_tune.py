'''Runs the uniform data and workload experiment
'''
import os
import inspect
import sys
import json

# print(len(sys.argv))
if len(sys.argv) != 9:
    print("Usage: python3 run.py [datafile] [queryfile] [#queries] [#repetitions] [#datasize] [#dimensions] [output_folder_path] [partition size]")
    sys.exit(1)

CURRENT_DIR = os.getcwd()

DATA_FILE = f"{CURRENT_DIR}"+"/"+sys.argv[1]
WORKLOAD_FILE = f"{CURRENT_DIR}"+"/"+sys.argv[2]
NUMBER_OF_QUERIES = sys.argv[3]
REPETITIONS = sys.argv[4]
ROWS = sys.argv[5]
NUMDIMS = sys.argv[6]
OUTPUT_FOLDER = f"{CURRENT_DIR}"+"/"+sys.argv[7]
PARTITION_SIZE = sys.argv[8]

# script directory
SCRIPT_PATH = os.path.dirname(
        os.path.abspath(
            inspect.getfile(inspect.currentframe())
            )
        )
os.chdir(SCRIPT_PATH)


# magic to be able to import benchmark from parent dir
sys.path.append(os.getcwd() + '/..')

from benchmark import Benchmark

# FEATURES_FILE = f"{CURRENT_DIR}/../../data/mdrq-analysis/1000genomes_import/chr22_feature.vectors"
# GENOMES_FILE = f"{CURRENT_DIR}/../../data/mdrq-analysis/1000genomes_import/genes.txt"
# POWER_FILE = f"{CURRENT_DIR}/../../data/mdrq-analysis/power_import/DEBS2012-ChallengeData.txt"
# SKYSERVER_DATA_FILE = f"{CURRENT_DIR}/../../data/skyserver_2.csv"
# SKYSERVER_QUERY_FILE = f"{CURRENT_DIR}/../../data/skyserver2_query.csv"

# Read configuration
# with open('config.json') as json_file:
#     f = json.load(json_file)
#     NUMBER_OF_QUERIES = f['number_of_queries']
#     REPETITIONS = f['repetitions']
#     ROWS = f['rows']
#     PARTITION_SIZE = f['partition_size']
#     PROGRESSIVE_INDEX_DELTAS = f['deltas']


EXPERIMENTS = []

# # Setup for Genomics
# for i in [8]:
#     exp = {
#         "name": f"genomics_query_{i}",
#         "data": f"{CURRENT_DIR}/data/genomics_query_{i}data",
#         "workload": f"{CURRENT_DIR}/data/genomics_query_{i}queries",
#         "query_type": f'{i}',
#         "number_of_rows": ROWS,
#         'number_of_queries': "100",
#         "repetitions": REPETITIONS,
#         }
#     exp['exp_id'] = f"{exp['name']}-{exp['number_of_rows']}-{exp['number_of_queries']}-0.0"
#     command = [
#         "./genome_generator",
#         "-r", exp['number_of_rows'],
#         "-q", exp['number_of_queries'],
#         "-t", exp['query_type'],
#         "-e", FEATURES_FILE,
#         "-g", GENOMES_FILE,
#         "-f", exp['data'],
#         "-w", exp['workload']
#         ]
#     exp['command'] = ' '.join(command)
#     EXPERIMENTS.append(exp)

# # Setup for Power
# power_exp = {
#     "name": f"power",
#     "data": f"{CURRENT_DIR}/data/power_data",
#     "workload": f"{CURRENT_DIR}/data/power_queries",
#     "number_of_rows": ROWS,
#     'number_of_queries': "3000",
#     "repetitions": REPETITIONS,
# }

# power_exp['exp_id'] = f"{power_exp['name']}-{power_exp['number_of_rows']}-{power_exp['number_of_queries']}-0.0"
# command = [
#         "./power_generator",
#         "-r", power_exp['number_of_rows'],
#         "-q", power_exp['number_of_queries'],
#         "-p", POWER_FILE,
#         "-f", power_exp['data'],
#         "-w", power_exp['workload']
#         ]
# power_exp['command'] = ' '.join(command)
# EXPERIMENTS.append(power_exp)

# # Setup for SkyServer
# skyserver_exp = {
#         "name": "skyserver",
#         "data": f"{CURRENT_DIR}/data/skyserver_data",
#         "workload": f"{CURRENT_DIR}/data/skyserver_queries",
#         "repetitions": REPETITIONS
#         }

# skyserver_exp["exp_id"] = f"{skyserver_exp['name']}-0-0-0.0"
# command = [
#         "./skyserver_generator",
#         "-p", SKYSERVER_DATA_FILE,
#         "-q", SKYSERVER_QUERY_FILE,
#         "-f", skyserver_exp['data'],
#         "-w", skyserver_exp['workload']
#         ]
# skyserver_exp["command"] = ' '.join(command)
# EXPERIMENTS.append(skyserver_exp)

dinos_exp = {
        "name": "dinos",
        "data": DATA_FILE,
        "workload": WORKLOAD_FILE,
        "number_of_rows": ROWS,
        'number_of_queries': NUMBER_OF_QUERIES,
        "repetitions": "1",
        "dimensions": NUMDIMS,
        }

dinos_exp["exp_id"] = f"{dinos_exp['name']}-{dinos_exp['number_of_rows']}-{dinos_exp['number_of_queries']}-0.0"
# command = [
#         "./skyserver_generator",
#         "-p", MY_FILE,
#         "-q", MY_WORKLOAD,
#         "-f", dinos_exp['data'],
#         "-w", dinos_exp['workload']
#         ]
# dinos_exp["command"] = ' '.join(command)
EXPERIMENTS.append(dinos_exp)

# generate_workload(MY_WORKLOAD, dinos_exp['workload'], int(dinos_exp['number_of_queries']))

RUNS = [
    # {
    #     "algorithm_id": "111",
    #     "name": "full_scan_cl",
    #     "result": f"{CURRENT_DIR}/results/full_scan_cl-{0.0}-{0}"
    # },
     {
         "algorithm_id": "2",
         "partitions_size": str(PARTITION_SIZE),
         "name": "cracking_kd_tree",
         "result": f"{OUTPUT_FOLDER}times_original_akd_partition{PARTITION_SIZE}_",
         "run_times" : sys.argv[4],
    },
    # {
    #     "algorithm_id": "3",
    #     "partitions_size": str(PARTITION_SIZE),
    #     "name": "cracking_kd_tree_pd",
    #     "result": f"{CURRENT_DIR}/results/cracking_kd_tree_pd-{0.0}-{PARTITION_SIZE}"
    # },
    # {
    #     "algorithm_id": "4",
    #     "partitions_size": str(PARTITION_SIZE),
    #     "name": "average_kd_tree",
    #     "result": f"{CURRENT_DIR}/results/average_kd_tree-{0.0}-{PARTITION_SIZE}"
    # },
    # {
    #     "algorithm_id": "5",
    #     "partitions_size": str(PARTITION_SIZE),
    #     "name": "median_kd_tree",
    #     "result": f"{CURRENT_DIR}/results/median_kd_tree-{0.0}-{PARTITION_SIZE}"
    # },
    {
         "algorithm_id": "6",
         "partitions_size": str(PARTITION_SIZE),
         "name": "quasii",
         "result": f"{OUTPUT_FOLDER}times_quasii_partition{PARTITION_SIZE}_",
         "run_times" : sys.argv[4],
    }
]

# for delta in PROGRESSIVE_INDEX_DELTAS:
#RUNS.append(
#     {
#         "algorithm_id": "7",
#         "partitions_size": str(PARTITION_SIZE),
#         "name": "progressive_index",
#         "delta": "0.2",
#         "result": f"{OUTPUT_FOLDER}times_PKD_",
#         "run_times" : sys.argv[4]
 #    }
 #)
#RUNS.append(
#    {
#        "algorithm_id": "7",
#        "partitions_size": str(PARTITION_SIZE),
#        "name": "progressive_index_cm",
#        "delta": "0.2",
#        "extra_flags": "-c",
#        "result": f"{OUTPUT_FOLDER}times_GPKD_0.2delta_",
#        "run_times" : sys.argv[4]
#    }
#)

#RUNS.append(
#    {
#        "algorithm_id": "7",
#        "partitions_size": str(PARTITION_SIZE),
#        "name": "progressive_index_cm",
#        "delta": "0.4",
#        "extra_flags": "-c",
#        "result": f"{OUTPUT_FOLDER}times_GPKD_0.4delta_",
#        "run_times" : sys.argv[4]
#    }
#)

#RUNS.append(
#    {
#        "algorithm_id": "7",
#        "partitions_size": str(PARTITION_SIZE),
#        "name": "progressive_index_cm",
#        "delta": "0.6",
#        "extra_flags": "-c",
#        "result": f"{OUTPUT_FOLDER}times_GPKD_0.6delta_",
#        "run_times" : sys.argv[4]
#    }
#)

#RUNS.append(
#    {
#        "algorithm_id": "7",
#        "partitions_size": str(PARTITION_SIZE),
#        "name": "progressive_index_cm",
#        "delta": "0.8",
#        "extra_flags": "-c",
#        "result": f"{OUTPUT_FOLDER}times_GPKD_0.8delta_",
#        "run_times" : sys.argv[4]
#    }
#)

#RUNS.append(
#    {
#        "algorithm_id": "7",
#        "partitions_size": str(PARTITION_SIZE),
#        "name": "progressive_index_cm",
#        "delta": "1",
#        "extra_flags": "-c",
#        "result": f"{OUTPUT_FOLDER}times_GPKD_1delta_",
#        "run_times" : sys.argv[4]
#    }
#)

def main():
    ''' Main method
    '''
    build_dir = "../../build/"
    bin_dir = "../../bin"

    benchmark = Benchmark(EXPERIMENTS, RUNS, build_dir, bin_dir)
    # benchmark.generate()
    benchmark.run()
    # benchmark.clean(build_dir)


if __name__ == "__main__":
    main()
