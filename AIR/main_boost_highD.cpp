//  g++ -I ./boost_1_61_0/ main.cpp -std=c++14 -o folan && ./folan
#include <boost/geometry.hpp>
#include <boost/geometry/geometries/point.hpp>
#include <boost/geometry/geometries/box.hpp>

#include <boost/geometry/index/rtree.hpp>

// to store queries results
#include <vector>

// just for output
#include <iostream>
#include <boost/foreach.hpp>

// FATEMEHi
#include <stdlib.h>
#include <sys/time.h>
#include <stdio.h>
#include <fstream>
#include <string>
#include <sstream>
using namespace std;
//#define NUMDIMS 8


double timing(){
    static struct timeval t1, t2;
    gettimeofday(&t2,NULL);
    double ret = t2.tv_sec - t1.tv_sec + (t2.tv_usec - t1.tv_usec) * 1e-6;
    t1 = t2;
    return ret;
}
// FATEMEH OUT

#ifdef DIM2
    #define NUMDIMS 2
#elif DIM3
    #define NUMDIMS 3
#elif DIM4
    #define NUMDIMS 4
#elif DIM5
    #define NUMDIMS 5
#elif DIM6
    #define NUMDIMS 6
#elif DIM7
    #define NUMDIMS 7
#endif

#ifdef datacount19M
    #define DATA_COUNT 19000000
#elif datacount60M
    #define DATA_COUNT 60000000
#elif datacount20M
    #define DATA_COUNT 20000000
#elif datacount1M
    #define DATA_COUNT 1000000
#elif datacount1
    #define DATA_COUNT 2
#elif datacount40M
    #define DATA_COUNT 4000000
#elif datacount70M
    #define DATA_COUNT 70000000
#endif



namespace bg = boost::geometry;
namespace bgi = boost::geometry::index;

int main(int argc, char **argv)
{
    cout << "Before  parameters\n";
    typedef bg::model::point<float, 2, bg::cs::cartesian> point;
    typedef bg::model::box<point> box;
    typedef std::pair<box, unsigned> value;


    string data_file_name = argv[1];
    int query_size = stoi(argv[2]);
    string query_file_name = argv[3];
    string time_file_name = argv[4];
    // int data_size = stoi(argv[5]);

    cout << "Read parameters\n";
    
    std::ifstream data_file(data_file_name.c_str());
    std::ifstream query_file(query_file_name.c_str());

    int query_count = std::count(std::istreambuf_iterator<char>(query_file),
                                 std::istreambuf_iterator<char>(), '\n');
    int data_count = std::count(std::istreambuf_iterator<char>(data_file),
                                 std::istreambuf_iterator<char>(), '\n');

    if(query_count < query_size) {cout<<"NOT ENOUGH QUERIES IN THE FILE\n"; return 1;}
    if(data_count < DATA_COUNT) {cout<<"NOT ENOUGH DATA IN THE FILE\n"; return 1;}

    data_file.clear();
    data_file.seekg(0, ios::beg);
    query_file.clear();
    query_file.seekg(0, ios::beg);

    cout << "Reading data\n";

    std::vector<value> all_data;
    // float min_x, min_y, min_z, max_x, max_y, max_z;
    float dataMins[DATA_COUNT][NUMDIMS], dataMaxes[DATA_COUNT][NUMDIMS];
    for(int i = 0; i < DATA_COUNT; i++){
        for(int j = 0; j < NUMDIMS; j++){
            data_file >> dataMins[i][j];
        }
        for(int j = 0; j < NUMDIMS; j++){
            data_file >> dataMaxes[i][j];
        }
    	box b(point(dataMins[i][0], dataMins[i][1]), point(dataMaxes[i][0], dataMaxes[i][1]));
    	all_data.push_back(std::make_pair(b, i));
    }

    ofstream times_file;
    times_file.open(time_file_name.c_str());

    timing();
    bgi::rtree< value, bgi::rstar<16> > rtree(all_data);
    double indexing_time = timing();
    std::vector<box> queries;
    float queryMins[10000][NUMDIMS];
    float queryMaxes[10000][NUMDIMS];

    cout << "Reading queries\n";

    for(int i = 0; i < query_size; i++){
        // query_file >> min_x >> min_y >> min_z >> max_x >> max_y >> max_z;
        for(int j = 0; j < NUMDIMS; j++){
            query_file >> queryMins[i][j];
        }
        for(int j = 0; j < NUMDIMS; j++){
            query_file >> queryMaxes[i][j];
        }
        box b(point(queryMins[i][0], queryMins[i][1]), point(queryMaxes[i][0], queryMaxes[i][1]));
	   queries.push_back(b);
    }

    // run all the queries and time them
    int last_id = DATA_COUNT -1;
    int this_id;
    float min[3]; float max[3];
    clock_t q_time = 0;
    std::vector<value> result_s;
    box b;
    clock_t insert_time, start_insert_time;
    clock_t start_query_time;
    // timing();
    for(int i = 0; i < query_size; i++){
        insert_time = 0.0;
        result_s.clear();
        start_query_time = clock();
        rtree.query(bgi::intersects(queries[i]), std::back_inserter(result_s));
        q_time = clock() - start_query_time;
        times_file << (double)insert_time/CLOCKS_PER_SEC << " " << (double)q_time/CLOCKS_PER_SEC << " " << indexing_time << "\n";
	    cout << "QUERY " << i << " " << result_s.size() << endl;
    }

    query_file.close();
    data_file.close();
    times_file.close();

    cout << "DONE!!\n";

    // FATEMEH OUT


    
    return 0;
}
