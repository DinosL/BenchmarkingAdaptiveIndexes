#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <time.h>
#include <stdio.h>
#include <sys/time.h>
// #include "RTree_723oct21s_L2.h"
// #include "RTree_723oct21s_stochastic_15June23.h"
#include "RTree_723oct21s_stochastic_14dec22.h"
// #include "RTree_723oct21s_stochastic_24July23.h"
// #include "RTree_723oct21s_stochastic_25July23.h"

using namespace std;


typedef int ValueType;
typedef float ELEMTYPE;
typedef double ELEMTYPEREAL;



#ifdef fo8
    const int fo = 8;
#elif fo16
    const int fo = 16;
#elif fo32
    const int fo = 32;
#endif


#ifdef maxt32
    const int maxt = 32;
#elif maxt64
    const int maxt = 64;
#elif maxt128
    const int maxt = 128;
#elif maxt256
    const int maxt = 256;
#elif maxt512
    const int maxt = 512;
#elif maxt1024
    const int maxt = 1024;
#elif maxt2048
    const int maxt = 2048;
#elif maxt4096
    const int maxt = 4096;
#endif


#ifdef mint16
    const int mint = 16;
#elif mint32
    const int mint = 32;
#elif mint64
    const int mint = 64;
#elif mint128
    const int mint = 128;
#elif mint256
    const int mint = 256;
#elif mint512
    const int mint = 512;
#elif mint1024
    const int mint = 1024;
#elif mint2048
    const int mint = 2048;
#endif


struct Rect
{
    Rect()  {}

    //Rect(ELEMTYPE a_minX, ELEMTYPE a_minY, ELEMTYPE a_maxX, ELEMTYPE a_maxY)
    //{
    //    m_min[0] = a_minX;
    //    m_min[1] = a_minY;
    //    m_max[0] = a_maxX;
    //    m_max[1] = a_maxY;
    //}

    Rect(ELEMTYPE a_min[NUMDIMS], ELEMTYPE a_max[NUMDIMS]){
        for(int i = 0; i < NUMDIMS; i++) m_min[i] = a_min[i];
        for(int i = 0; i < NUMDIMS; i++) m_max[i] = a_max[i];
    }


    ELEMTYPE m_min[NUMDIMS];
    ELEMTYPE m_max[NUMDIMS];
};


bool Overlap(Rect *a_rectA, Rect *a_rectB);

bool Overlap(Rect *a_rectA, Rect *a_rectB) {
            ASSERT(a_rectA && a_rectB);

    for (int index = 0; index < 2; ++index) {
        if (a_rectA->m_min[index] > a_rectB->m_max[index] || a_rectB->m_min[index] > a_rectA->m_max[index]) {
            return false;
        }
    }
    return true;
}


double timing(){
    static struct timeval t1, t2;
    gettimeofday(&t2,NULL);
    double ret = t2.tv_sec - t1.tv_sec + (t2.tv_usec - t1.tv_usec) * 1e-6;
    t1 = t2;
    return ret;
}


int main(int argc, char **argv){



    string data_file_name = argv[1];
    int query_size = stoi(argv[2]);
    string query_file_name = argv[3];
    string time_file_name = argv[4];

    auto seed = time(NULL); // 1695893019
    srand(1733411143);
    // cout << "seed " << seed << endl;

    
    std::ifstream query_file(query_file_name.c_str());
    // if(query_file.is_open()){cout << "it is open:)\n";}

    #ifdef stats
        ofstream stats_file;
        string stats_file_name = argv[5];
        stats_file.open(stats_file_name.c_str());   
    #endif

    ofstream times_file;
    times_file.open(time_file_name.c_str());

    typedef RTree<ValueType, fo, fo/2, maxt, mint> MyTree;
    MyTree tree(data_file_name);

    ELEMTYPE min[NUMDIMS]; ELEMTYPE max[NUMDIMS];
    int this_id;

    // cout << "INSERTION COMPLETE\n";

    query_file.clear();
    query_file.seekg(0, ios::beg);
    // cout << "cleared q file\n";
    // cout << "query size " << query_size << endl;
    Rect queries[query_size];
    // cout << "created rect array\n";

    vector<Rect> data(DATA_COUNT);
    std::ifstream data_file(data_file_name.c_str());

    for(int i = 0; i < DATA_COUNT; i++){
        data[i] = Rect();
        for(int j = 0; j < NUMDIMS; j++) data_file >> data[i].m_min[j];
        for(int j = 0; j < NUMDIMS; j++) data_file >> data[i].m_max[j];
    }
    
    for(int i = 0; i < query_size; i++){
        queries[i] = Rect();
        for(int j = 0; j < NUMDIMS; j++) query_file >> queries[i].m_min[j];
        for(int j = 0; j < NUMDIMS; j++) query_file >> queries[i].m_max[j];
       
    }
    // cout << "LOADED QUERIES\n";
    clock_t startTime, endTime;
    int found_count; int sum_ids; int total_count;
    int lin_count;
    double q_time = 0;
    int rightestright = 0;
    this_id = DATA_COUNT -1;
    timing();
    // cout << "DATA COUNT: " << DATA_COUNT << " data.size() " << data.size() << endl;
    // exit(4);
    // cout << "I am air\n";
    for(int i = 0; i < query_size; i++){
        // cout << "QUERY " << i << "\n";

        // if(i % ratio == 0 && i != 0){
        //     // insert one
        //     for(int o = 0; o < insert_count; o++){
        //         for(int j = 0; j < NUMDIMS; j++) data_file >> min[j];
        //         for(int j = 0; j < NUMDIMS; j++) data_file >> max[j];
        //         // this_id = ++last_id;
        //         this_id++;
        //         // tree.Insert(min, max, this_id);
        //         // cout << "inserted " << this_id << endl;
        //         data.push_back(Rect(min, max));
        //     }
        // }

        // timing();
        startTime = clock();
        found_count = tree.QueryAdaptive_v13(queries[i].m_min, queries[i].m_max);
        #ifdef SPACE_COMPLEXITY
            cout << tree.CountNodes() << endl;
        #endif
        endTime = clock() - startTime;

        // cout << "found count in tree: " << found_count << endl;

        // cout << "This_id = " << this_id << " " << DATA_COUNT-1 << endl;
        // for(int ii = DATA_COUNT; ii < this_id + 1; ii++){
        //     if(Overlap(&(queries[i]), &(data[ii]))) {
        //         // cerr << ii << endl;
        //         // cerr << ii << " " << data[ii].m_min[0] << " " << data[ii].m_min[1] << " " << data[ii].m_max[0] << " " << data[ii].m_max[1] << endl;
        //         found_count++;
        //     }
        // }
        // q_time = timing();
        // times_file << found_count << " " << q_time <<  "\n";
        times_file << found_count << "\t" << (double)endTime/CLOCKS_PER_SEC <<  "\n";                    
        #ifdef stats
            // int rightestright = tree.getRightestRight();
            // int holes = tree.getCountOfHoles();
            int height = tree.TreeHeight();
            // int pending_count = tree.getPendingCount();
            // auto leaf_count = tree.CountLeaves();
            // int node_count = tree.CountNodes();
            // stats_file << node_count << " " << leaf_count.first << " " << leaf_count.second << " " << pending_count << " " << height << " " << rightestright << endl;
            // stats_file << rightestright << " " << holes << " " << push_back_count << " " << pushed_back_items_count << " " << fill_holes_count << " " << split_leaf_count << endl;
            // stats_file << "i " << rightestright << " " << holes << " " << count_total_holes  << endl;
            stats_file << "i " << " " << height << " " << count_internal_nodes << " " << count_regular_leaves << " " << count_irregular_leaves << endl;
            // if(count_irregular_leaves != leaf_count.second || count_regular_leaves != leaf_count.first)
            // {
            //     cout << "leaf count is wrong! " << count_regular_leaves << " " << leaf_count.first << " " << count_irregular_leaves << " " << leaf_count.second  << endl;
            //     exit(3);
            // }
            // if(count_internal_nodes + count_regular_leaves + count_irregular_leaves != node_count)
            // {
            //     cout << "node count is wrong " << count_internal_nodes << " " << " " << count_irregular_leaves << " " << count_regular_leaves << " " <<  node_count  << endl;
            //     exit(3);
            // }
            // if(holes != count_total_holes) {
            //     stats_file << "FREAK OUT!!!!!!!!!!!!!" << endl;
            //     exit(3);
            // }
            // push_back_count = 0;
            // fill_holes_count = 0;
            // split_leaf_count = 0;
            // pushed_back_items_count = 0;
            
        #endif

        #ifdef linearcheck

            // linear scan
            // timing();
            // // cout << "data.size() " << data.size() << endl;  
            lin_count = 0;
            for(int ii =0; ii < DATA_COUNT; ii++){
                // cout << queries[i].m_min[0] << " " << queries[i].m_min[1] << " " << queries[i].m_max[0] << " " << queries[i].m_max[1] << endl;
                if(Overlap(&(queries[i]), &(data[ii]))) {
            //         // cerr << ii << endl;
            //         // cerr << ii << " " << data[ii].m_min[0] << " " << data[ii].m_min[1] << " " << data[ii].m_max[0] << " " << data[ii].m_max[1] << endl;
                    lin_count++;
                }
            //     if(ii == DATA_COUNT - 1){
            //         // cout << "lin count in original data " << lin_count << endl;
            //     }
            }
            cout << "Lin , Found " << lin_count << " " << found_count << endl;
            // if(lin_count != found_count)
            // {
            //     cout << "Wrong answer " << lin_count << " " << found_count << endl;
            //     tree.findObject(15474964);
            //     exit(1);
            // }
            if(lin_count != found_count){
                cout << "WRONG ANSWER, TRAGEDY, DISASTER, ALL THAT..." << endl;
                cout << "Query \n";
                for(int ii = 0; ii < NUMDIMS; ii++)
                {
                    cout << queries[i].m_min[ii] << " " << queries[i].m_max[ii] << ",";
                }
                cout << "\n";
                // tree.findObject(15474964);

                return 3;
            }

        #endif
        // q_time = timing();
        // times_file << lin_count << " " << q_time <<  "\n";
        // cerr << "QUERY " << i << endl;
        // cout << "QUERY " << i << " found count: " << found_count << " truth: " << lin_count << endl;
        // if(i == 8) tree.getLeafArea("leafArea_AIR_structure_plot"+to_string(i)+"afterQuery.txt");
        // cerr << tree.getLeafAreaAverage() << endl;

        // if(i > 61630)
        //     tree.printLeafLinkedList();


        // total_count = tree.CountDataPoints();
        // cout << "COUNT " << total_count << " pending insertions: " << tree.getPendingCount() << "\n";

        // rightestright = tree.getRightestRight();
        // cout << "########## rightest right: " << rightestright << endl;
        // if(rightestright > 3*DATA_COUNT){return 3;}
        // tree.findAllLeavesWithDataInThem(506405);
        // if(lin_count != found_count){
        //     cout << "WRONG ANSWER, TRAGEDY, DISASTER, ALL THAT..." << endl;
        //     // cout << "Query \n";
        //     for(int ii = 0; ii < NUMDIMS; ii++)
        //     {
        //         cout << queries[i].m_min[ii] << " " << queries[i].m_max[ii] << ",";
        //     }
        //     cout << "\n";

        //     cout << "Data object 506405 \n";
        //     for(int ii = 0; ii < NUMDIMS; ii++)
        //     {
        //         cout << data[506405].m_min[ii] << " " << data[506405].m_max[ii] << ",";
        //     }
        //     cout << "\n";

        // //     // tree.printLeafBoundsOfData(70155);
        //     tree.findAllLeavesWithDataInThem(506405);

            // tree.printLeafLinkedList();
        //     return 3;
        // }

        //  #ifdef stats
        //     // int rightestright = tree.getRightestRight();
        //     int height = tree.TreeHeight();
        //     // int pending_count = tree.getPendingCount();
        //     // auto leaf_count = tree.CountLeaves();
        //     int node_count = tree.CountNodes();
        //     stats_file << node_count << " " << leaf_count.first << " " << leaf_count.second  << " " << height  << endl;
            
        // #endif

        


    }
    // tree.getLeafArea("leafArea_air_2m.txt");

    query_file.close();
    times_file.close();
    // cout << "DONE!!\n";
    return 0;
}

