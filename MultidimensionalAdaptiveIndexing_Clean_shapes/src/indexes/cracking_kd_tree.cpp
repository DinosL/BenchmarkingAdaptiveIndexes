#include "cracking_kd_tree.hpp"
#include "kd_node.hpp"
#include "full_scan.hpp"
#include <algorithm> // to check if all elements of a vector are true
#include <limits>
#include <full_scan_candidate_list.hpp>

using namespace std;

CrackingKDTree::CrackingKDTree(std::map<std::string, std::string> config){

    if(config.find("minimum_partition_size") == config.end())
        minimum_partition_size = 100;
    else
        minimum_partition_size = std::stoi(config["minimum_partition_size"]);
}
CrackingKDTree::~CrackingKDTree(){}

void CrackingKDTree::initialize(Table *table_to_copy){
        //! Check partition size, we change it to guarantee it always partitions all dimensions at least once
    while (minimum_partition_size > table_to_copy->row_count()/pow(2,table_to_copy->col_count())){
        minimum_partition_size /=2;
    }
    if (minimum_partition_size < 100){
        minimum_partition_size = 100;
    }
    // ******************
    auto start = measurements->time();

    // Copy the entire table
    table = make_unique<Table>(table_to_copy);

    index = make_unique<KDTree>(table->row_count());

    auto end = measurements->time();

    measurements->append(
            "initialization_time",
            std::to_string(Measurements::difference(end, start))
            );
    // ******************
}

void CrackingKDTree::adapt_index(Query &query) {
    // Before adapting calculate the scan overhead to measure how much the previous
    // queries helped this one
    auto search_results= index->search(query);
    auto partitions = search_results.first;
    n_tuples_scanned_before_adapting = 0;
    for(auto &partition : partitions)
        n_tuples_scanned_before_adapting += partition.second - partition.first;

    if(should_adapt){
        // If the index has converged then we stop adaptation
        if(index->has_converged(table.get(), minimum_partition_size)){
            should_adapt = false;
            // Add 0 to adaptation_time
            auto start = measurements->time();
            auto end = start;
            measurements->append(
                    "adaptation_time",
                    std::to_string(Measurements::difference(end, start))
                    );
        }else{
            // Adapt the KDTree 
            auto start = measurements->time();
            adapt(query);
            auto end = measurements->time();
            // ******************
            measurements->append(
                    "adaptation_time",
                    std::to_string(Measurements::difference(end, start))
                    );
        }
    }else{
        // Add 0 to adaptation_time
        auto start = measurements->time();
        auto end = start;
        measurements->append(
            "adaptation_time",
            std::to_string(Measurements::difference(end, start))
            );
    }
}

unique_ptr<Table> CrackingKDTree::range_query(Query &query, Query &original_query) {
    // ******************
    auto start = measurements->time();

    // Search on the index the correct partitions
    auto search_results= index->search(query);
    auto partitions = search_results.first;
    auto partition_skip = search_results.second;

    auto end = measurements->time();
    measurements->append(
            "index_search_time",
            std::to_string(Measurements::difference(end, start))
            );

    start = measurements->time();
    // Scan the table and returns the row ids 
    auto result = FullScanCandidateList::scan_partition_with_refinement(table.get(), query, partitions, partition_skip, original_query);

    // SHAPES
    // int trueResult = FullScanCandidateList::refine_window_query_extension(table.get(), table_max.get(), original_query, partitions);
    // SHAPES

    end = measurements->time();
    // ******************
    measurements->append(
        "scan_time",
        std::to_string(Measurements::difference(end, start))
    );

    int64_t n_tuples_scanned = 0;
    for(auto &partition : partitions)
        n_tuples_scanned += partition.second - partition.first;

    // std::cout << "--------------------- RESULTS " << result.second << std::endl;        
    // std::cerr << result.second << std::endl;        

    measurements->append("result", std::to_string(result.second));

    // std::cout << index->get_node_count() << std::endl;

    // Before returning the result, update the statistics.
    measurements->append("number_of_nodes", std::to_string(index->get_node_count()));
    measurements->append("max_height", std::to_string(index->get_max_height()));
    measurements->append("min_height", std::to_string(index->get_min_height()));
    measurements->append("memory_footprint", std::to_string(index->get_node_count() * sizeof(KDNode)));
    measurements->append("tuples_scanned", std::to_string(n_tuples_scanned));
    measurements->append("partitions_scanned", std::to_string(partitions.size()));
    measurements->total_tuples_scanned += n_tuples_scanned;
    // std::cout << n_tuples_scanned << std::endl;
    

    auto skips = 0;
    for(size_t i = 0; i < partition_skip.size(); ++i){
        if(all_of(partition_skip.at(i).begin(), partition_skip.at(i).end(), [](bool v) { return v; })){
            skips += 1;
        }
    }
    measurements->append("partitions_skipped", std::to_string(skips));


    measurements->append(
        "scan_overhead_before_adapt",
        std::to_string(
            n_tuples_scanned_before_adapting/static_cast<float>(result.second)
        )
    );

    measurements->append(
        "scan_overhead_after_adapt",
        std::to_string(
            n_tuples_scanned/static_cast<float>(result.second)
        )
    );

    auto t = make_unique<Table>(2);
    float row[2] = {static_cast<float>(result.first), static_cast<float>(result.second)};
    t->append(row,0);
    return t;
}

void CrackingKDTree::adapt(Query& query){ 

    if(index->root == nullptr){
        auto position = table->CrackTable(
                0, table->row_count(),
                query.predicates[0].low, 0 
                );
        index->root = index->create_node(0, query.predicates[0].low, position);
    }

    for(size_t dim = 0; dim < query.predicate_count(); ++dim){
        if(query.predicates[dim].low >= std::numeric_limits<float>::lowest())
            adapt_recursion(
                    index->root.get(),
                    query,
                    dim, query.predicates[dim].low,
                    0, table->row_count()
                    );
    }

    for(size_t dim = 0; dim < query.predicate_count(); ++dim){
        if(query.predicates[dim].high <= std::numeric_limits<float>::max())
            adapt_recursion(
                    index->root.get(),
                    query,
                    dim, query.predicates[dim].high,
                    0, table->row_count()
                    );
    }
}


void CrackingKDTree::adapt_recursion(
        KDNode *current,
        Query& query,
        size_t pivot_dim,
        float pivot,
        size_t lower_limit,
        size_t upper_limit
        ){

    // If the size of the partition is already too small then stop exploring it
    if(upper_limit - lower_limit + 1 < minimum_partition_size)
        return;

    // If that pivot has already been inserted then we don't need to
    // keep looking in this branch.
    if(current->column == pivot_dim && current->key == pivot){
        return;
    }

    // If the current node has the same column as the pivot dim
    if(current->column == pivot_dim){
        if(pivot < current->key){
            if(current->left_child == nullptr){
                auto position = table->CrackTable(
                        lower_limit, current->position,
                        pivot, pivot_dim
                        );
                current->left_child = index->create_node(
                        pivot_dim, pivot, position
                        );
            }else{
                adapt_recursion(
                        current->left_child.get(), query,
                        pivot_dim, pivot,
                        lower_limit, current->position
                        );
                return;
            }
        }else{
            if(current->right_child == nullptr){
                auto position = table->CrackTable(
                        current->position, upper_limit,
                        pivot, pivot_dim
                        );
                current->right_child = index->create_node(
                        pivot_dim, pivot, position
                        );
            }else{
                adapt_recursion(
                        current->right_child.get(), query,
                        pivot_dim, pivot,
                        current->position, upper_limit
                        );
                return;
            }
        }
    }else{
        switch(current->compare(query)){
            case -1:
                //                  Key
                // Data:  |----------!--------|
                // Query:            |-----|
                //                  low   high
                if(current->right_child == nullptr){
                    auto position = table->CrackTable(
                            current->position, upper_limit,
                            pivot, pivot_dim
                            );
                    current->right_child = index->create_node(
                            pivot_dim, pivot, position
                            );
                }else{
                    adapt_recursion(
                            current->right_child.get(), query,
                            pivot_dim, pivot,
                            current->position, upper_limit
                            );
                    return;
                }
                break;
            case +1:
                //                  Key
                // Data:  |----------!--------|
                // Query:      |-----|
                //            low   high
                if(current->left_child == nullptr){
                    auto position = table->CrackTable(
                            lower_limit, current->position,
                            pivot, pivot_dim
                            );
                    current->left_child = index->create_node(
                            pivot_dim, pivot, position
                            );
                }else{
                    adapt_recursion(
                            current->left_child.get(), query,
                            pivot_dim, pivot,
                            lower_limit, current->position
                            );
                    return;
                }
                break;
            case 0:
                if(current->left_child == nullptr){
                    auto position = table->CrackTable(
                            lower_limit, current->position,
                            pivot, pivot_dim
                            );
                    current->left_child = index->create_node(
                            pivot_dim, pivot, position
                            );
                }else{
                    adapt_recursion(
                            current->left_child.get(), query,
                            pivot_dim, pivot,
                            lower_limit, current->position
                            );
                }
                if(current->right_child == nullptr){
                    auto position = table->CrackTable(
                            current->position, upper_limit,
                            pivot, pivot_dim
                            );
                    current->right_child = index->create_node(
                            pivot_dim, pivot, position
                            );
                }else{
                    adapt_recursion(
                            current->right_child.get(), query,
                            pivot_dim, pivot,
                            current->position, upper_limit
                            );
                }
                break;
            default:
                assert(false);
        }
    }
}
