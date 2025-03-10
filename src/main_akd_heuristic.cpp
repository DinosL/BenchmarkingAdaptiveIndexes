#include "basics.h"
// #include "AIR.h"
#include "AKD_heuristic.h"
// #include "RTile.h"

int main(int argc, char **argv){


    // int* matrix = (int*)malloc(9*sizeof(int));
    // matrix[0] = 1; matrix[1] = 2; matrix[2] = 9; matrix[3] = 4; matrix[4] = 5; matrix[5] = 6; matrix[6] = 7; matrix[7] = 2; matrix[8] = 3;
    // RTile(100, 34, matrix, 3, 3, NULL);
    // return 2;
    string data_file_name = argv[1];
    int query_size = stoi(argv[2]);
    string query_file_name = argv[3];
    string time_file_name = argv[4];
    int batchSize = stoi(argv[5]);


     auto seed = time(NULL); // 1695893019
    // auto seed = 0;
//    auto seed = 1697617670;
    // cout << seed << endl;
    // auto seed = 1692015686; // crashes at 81199
    srand(seed);

    std::ifstream query_file(query_file_name.c_str());
    if(!query_file.is_open())
    {
        cout << "Cannot read query file\n";
        exit(1);
    }
    // if(query_file.is_open()){cout << "it is open:)\n";}

    ofstream times_file;
    times_file.open(time_file_name.c_str());

    // we also make empty tree to fill up
    AKDTree akd = AKDTree();

    ELEMTYPE min[NUMDIMS]; ELEMTYPE max[NUMDIMS];
    int this_id;

    // cout << "BUILDING COMPLETE\n";

    query_file.clear();
    query_file.seekg(0, ios::beg);
    // cout << "query size " << query_size << endl;
    cout << "Threshold " << CRACKTHRESHOLD << endl;
    // Rect queries[query_size];
    Rect *queries = (Rect *)malloc(query_size*sizeof(Rect));
    // cout << "created rect array\n";

    vector<Point> data(DATA_COUNT);
    std::ifstream data_file(data_file_name.c_str());
    if(!data_file.is_open())
    {
        cout << "Cannot read data file\n";
        exit(1);
    }

    ELEMTYPE minValue[NUMDIMS], maxValue[NUMDIMS];
    for(int j =0; j < NUMDIMS; ++j){
        minValue[j] = std::numeric_limits<float>::max();
        maxValue[j] = std::numeric_limits<float>::min();
    }

    for(int i = 0; i < DATA_COUNT; i++){
        data[i] = Point();
        for(int j = 0; j < NUMDIMS; j++) data_file >> data[i].m_values[j];
        // for(int j = 0; j < NUMDIMS; j++) data_file >> data[i].m_max[j];
        
        for(int j = 0; j < NUMDIMS; j++) 
        {
            minValue[j] = std::min(data[i].m_values[j], minValue[j]);
            maxValue[j] = std::max(data[i].m_values[j], maxValue[j]);
        }
    }    

    for(int i = 0; i < query_size; i++){
        queries[i] = Rect();
        for(int j = 0; j < NUMDIMS; j++) query_file >> queries[i].m_min[j];
        for(int j = 0; j < NUMDIMS; j++) query_file >> queries[i].m_max[j];
    }
    // cout << "LOADED QUERIES\n";
    int found_count; int sum_ids; int total_count;
    int lin_count;
    double q_time = 0;
    int rightestright = 0;
    this_id = DATA_COUNT -1;
    std::set<ELEMTYPE> boundsSet[NUMDIMS];
    vector<pair<int, Rect>> thisBatchQueries;
    bool init = true;
    clock_t begin_time, end_time;

    clock_t total_time = 0;


    #ifdef CGI
        begin_time = clock();
        int preBuildLevels = stoi(argv[6]);
        #ifdef DEBUG
        cout << "before prebuild" << endl;
        #endif
        akd.preBuildTree(preBuildLevels, minValue[0], minValue[1], maxValue[0], maxValue[1], &data);
    #ifdef DEBUG
        cout << "after prebuild" << endl;
        #endif
        end_time = clock() - begin_time;
        times_file << (double)end_time/CLOCKS_PER_SEC << endl;
        total_time += end_time;
    #elif CGIHASH
        begin_time = clock();
        int preBuildLevels = stoi(argv[6]);
        #ifdef DEBUG
        cout << "before prebuild" << endl;
        #endif
        akd.preBuildTreeHashed(preBuildLevels, minValue[0], minValue[1], maxValue[0], maxValue[1], &data);
        #ifdef DEBUG
        cout << "after prebuild" << endl;
    #endif
    end_time = clock() - begin_time;
    times_file << (double)end_time/CLOCKS_PER_SEC << endl;
    total_time += end_time;
    #elif CGIHASHv2
        begin_time = clock();
        int preBuildLevels = stoi(argv[6]);
        #ifdef DEBUG
        cout << "before prebuild" << endl;
    #endif
        akd.preBuildTreeHashed_v2(preBuildLevels, minValue[0], minValue[1], maxValue[0], maxValue[1], &data);
        #ifdef DEBUG
        cout << "after prebuild" << endl;
    #endif
        end_time = clock() - begin_time;
        times_file << (double)end_time/CLOCKS_PER_SEC << endl;
        total_time += end_time;
    #else
        begin_time = clock();
        // fill m_data_arr
        for(int i = 0; i < DATA_COUNT; i++){
            for(int j = 0; j < NUMDIMS; j++) m_data_arr[i][j] = data[i].m_values[j];
            m_data_ids[i] = i;
        }
        end_time = clock() - begin_time;
        // times_file << (double)end_time/CLOCKS_PER_SEC << endl;
        total_time += end_time;
    #endif

    Rect rootCover = Rect();
    rootCover.m_min[0] = 0;
    rootCover.m_max[0] = 1;
    rootCover.m_min[1] = 0;
    rootCover.m_max[1] = 1;
    for(int i = 0; i < query_size; i++){

        // timing();
        #ifdef TIMEBREAKDOWN
            adaptiveSplitTime = 0;
            adaptiveCountingResultsTime = 0;
            failedCracks = 0;
        #endif
        begin_time = clock();


        found_count = akd.AdaptiveQuery(queries[i].m_min, queries[i].m_max, rootCover);

        #ifdef SPACE_COMPLEXITY
            cout << akd.countNodes() << endl;
        #endif

        // q_time = timing()
        end_time = clock() - begin_time;
        // times_file << q_time << endl;
        // times_file << (double)end_time/CLOCKS_PER_SEC << endl;
        #ifdef TIMEBREAKDOWN
            times_file << (double)end_time/CLOCKS_PER_SEC << " " << (double)adaptiveSplitTime/CLOCKS_PER_SEC << " " << (double)adaptiveCountingResultsTime/CLOCKS_PER_SEC << " " << failedCracks << endl;
        #else
            times_file << found_count << "\t" << (double)end_time/CLOCKS_PER_SEC << endl;
        #endif


        total_time += end_time;

        #ifdef DEBUG
            cout << "Query " << i  << " = " << found_count << " time = " << (double)end_time/CLOCKS_PER_SEC << endl;
            cout << "Query bounds: " << queries[i].m_min[0] << " " << queries[i].m_min[1] << " " << queries[i].m_max[0] << " " << queries[i].m_max[1] << endl;
        #endif

        #ifdef TIMEBREAKDOWN
        // if(i == batchSize - 1){
            cerr << "Swaps after first batch " << swaps << endl;
            cerr << "PartitionTime " << (double)partitionTime/CLOCKS_PER_SEC << endl;
            cerr << "SwapTime " << (double)swapTime/CLOCKS_PER_SEC << endl;
            cerr << "TraversalTime " << (double)partitionTime/CLOCKS_PER_SEC - (double)swapTime/CLOCKS_PER_SEC << endl;
            cerr << "Traversals " << traversals << endl;
            cerr << "END OF BATCH" << endl; 
            cerr << akd.countNodes() << " " << akd.countLeaves() << " " << akd.deepestLeaf() << endl;
            cerr << "Total time  " << (double)end_time/CLOCKS_PER_SEC << endl; 
            akd.cerrLeafSizesToFile();
            partitionTime = 0;
            swaps = 0;
            traversals = 0;
            swapTime = 0;
        // }
        #endif

        #ifdef linearcheck

            // linear scan
            lin_count = 0;
            for(int ii =0; ii < DATA_COUNT; ii++){
                // if(Overlap(&(queries[i]), &(data[ii]))) {
                if(Overlap(queries[i].m_min, queries[i].m_max, data[ii].m_values)){
                    lin_count++;
                }
            }
            // cout << "WRONG ANSWER, TRAGEDY, DISASTER, ALL THAT..." << found_count << " " << lin_count << endl;

            if(lin_count != found_count){
            // if(abs(lin_count - found_count) > 1){
                cout << "WRONG ANSWER, TRAGEDY, DISASTER, ALL THAT..." << found_count << " " << lin_count << endl;
                cout << "Query " << i << endl;
                for(int ii = 0; ii < NUMDIMS; ii++)
                {
                    cout << queries[i].m_min[ii] << " " << queries[i].m_max[ii] << ",";
                }
                cout << "\n";
                // return 3;
            }

        #endif
        
        // cerr << "QUERY " << i << endl;
        // cout << "QUERY " << i << " found count: " << found_count << " truth: " << lin_count << endl;

    }
    // cout << "Instoch " << inStoch << " actual " << actuallyCracked << " totalNodes " << totalNodes << endl; 
    // cout << "AVG stochastically cracked piece size " << sizeOfStochCrackedPiece/actuallyCracked << endl;

    #ifdef TIMEBREAKDOWN
    cerr << "END OF WORKLOAD" << endl; 
    cerr << "Swaps " << swaps << endl;
    cerr << "PartitionTime " << (double)partitionTime/CLOCKS_PER_SEC << endl;
    cerr << "SwapTime " << (double)swapTime/CLOCKS_PER_SEC << endl;
    cerr << "TraversalTime " << (double)partitionTime/CLOCKS_PER_SEC - (double)swapTime/CLOCKS_PER_SEC << endl;
    cerr << "Traversals " << traversals << endl;
    cerr << akd.countNodes() << " " << akd.countLeaves() << " " << akd.deepestLeaf() << endl;
    cerr << "Total nodes " << totalNodes << endl;
    cerr << "Total time  " << (double)end_time/CLOCKS_PER_SEC << endl; 
    akd.cerrLeafSizesToFile();
    #endif
    // times_file << (double)total_time/CLOCKS_PER_SEC << endl;
    query_file.close();
    times_file.close();
    // cout << "DONE!!\n";
    return 0;

}

