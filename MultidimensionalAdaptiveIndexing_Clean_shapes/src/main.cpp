#include "index_factory.hpp"
#include <string>
#include <iostream>
#include <map>

// For the command line parsing
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>

#include "table.hpp"
#include "workload.hpp"

int main(int argc, char** argv){
    string original_workload_path = "queries";
    string extended_workload_path = "queries";
    string data_path = "data";
    string index_algorithm = "Full Scan";
    string results_file;
    size_t algorithm_id = 0;
    int number_of_repetitions = 3;
    map<string, string> config;
    int dimensions, dataSize;
    int workload_size;

    int c;
    while ((c = getopt (argc, argv, "w:d:i:r:s:p:a:t:cz:o:y:x:k:b:l:")) != -1)
        switch (c)
        {
        case 'x':
            dimensions = atoi(optarg);
            break;
        case 'k':
            dataSize = atoi(optarg);
            break;
        case 'w':
            original_workload_path = optarg;
            break;
        case 'b':
            extended_workload_path = optarg;
            break;
        case 'l':
            workload_size = atoi(optarg);
            break;
        case 'd':
            data_path = optarg;
            break;
        case 'i':
            algorithm_id = atoi(optarg);
            break;
        case 'r':
            number_of_repetitions = atoi(optarg);
            break;
        case 's':
            results_file = optarg;
            break;
        case 'p':
            config["minimum_partition_size"] = optarg;
            break;
        case 'a':
            config["delta"] = optarg;
            break;
        case 'c':
            config["interactivity_threshold"] = '1';
            break;
        case 'z':
            config["interactivity_threshold_is_time"] = '1';
            config["interactivity_threshold"] = optarg;
            break;
        case 'o':
            config["interactivity_threshold_is_time"] = '1';
            config["num_queries_over"] = optarg;
            break;
        case 'y':
            config["is_delta_fixed"] = "1";
            config["interactivity_threshold"] = optarg;
            break;
        default:
            cout << "Usage:\n";
            cout << "-w <workload_path>\n";
            cout << "-d <data_path>\n";
            cout << "-i <algorithm id>\n";
            cout << "-r <number_of_repetitions>\n";
            cout << "-s <file_to_save_results>\n";
            cout << "-p <minimum_partition_size>\n";
            cout << "-a <delta>\n";
            cout << "-c <flag for progressive index to use costmodels>\n";
            cout << "-z <flag for progressive index interactive threshold time>\n";
            cout << "-o <flag for progressive index number of queries over the interactive thrshold>\n";
            return -1;
        }

    for(auto repetition = 0; repetition < number_of_repetitions; repetition++){
        auto index = IndexFactory::getIndex(algorithm_id, config);

        cout << endl;
        cout << index->name() << endl;
        cout << "Repetition: " << repetition << endl;
        cout << "Workload: " << original_workload_path << endl;
        cout << "Data: " << data_path << endl;
        cout << "Dimensions: " << dimensions << endl;
        cout << "Datasize: " << dataSize << endl;
        cout << "Configurations:" << endl;

        // for(aIn quicksort

        auto table_max = make_unique<Table>(dimensions, dataSize);
        auto table = Table::read_file_dinos(data_path, dataSize, dimensions, table_max);
        auto extended_workload = Workload::read_file_dinos(extended_workload_path, dimensions);
        // ========================================= SHAPES =========================================

        auto original_workload = Workload::read_file_dinos(original_workload_path, dimensions);
        // auto data_max_points = Table::read_file(data_path + "_max");

        // ========================================= END SHAPES =========================================


        index->initialize(table.get());
        index->initialize_max_table(table_max.get());

        for(size_t i = 0; i < workload_size; ++i){
            index->adapt_index(extended_workload.queries.at(i));
            // cout << "After adaptation\n";
            // index->range_query(workload.queries.at(i));

            // cout << extended_workload.queries.at(i).predicates[0].low << " " << extended_workload.queries.at(i).predicates[1].low << " " << extended_workload.queries.at(i).predicates[0].high << " " << extended_workload.queries.at(i).predicates[1].high << endl;
            // cout << original_workload.queries.at(i).predicates[0].low << " " << original_workload.queries.at(i).predicates[1].low << " " << original_workload.queries.at(i).predicates[0].high << " " << original_workload.queries.at(i).predicates[1].high << endl;

            index->range_query(extended_workload.queries.at(i), original_workload.queries.at(i));
            // cout << "after range " << i << endl; 
            //index->draw_index(results_path + '/' + index->name() + ".dot");
            // cerr << i << "/" << workload.query_count() << endl;
        }

        index->measurements->save(
                results_file,
                repetition
                );
        cout << "AVG data scanned per query " << index->measurements->total_tuples_scanned/10000.0 << endl;

    }
    return 0;
}
