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

namespace bg = boost::geometry;
namespace bgi = boost::geometry::index;

#ifdef nodeSize16
    #define nodeSize 16
#elif nodeSize32
    #define nodeSize 32
#elif nodeSize64
    #define nodeSize 64
#elif nodeSize128
    #define nodeSize 128
#elif nodeSize256
    #define nodeSize 256
#elif nodeSize512
    #define nodeSize 512
#elif nodeSize1024
    #define nodeSize 1024
#elif nodeSize2048
    #define nodeSize 2048
#elif nodeSize4096
    #define nodeSize 4096
#elif nodeSize8192
    #define nodeSize 8192
#endif




int main(int argc, char **argv)
{
    typedef bg::model::point<float, 2, bg::cs::cartesian> point;
    typedef bg::model::box<point> box;
    typedef std::pair<box, unsigned> value;

    string data_file_name = argv[1];
    int query_size = stoi(argv[2]);
    string query_file_name = argv[3];
    string time_file_name = argv[4];
    int data_size = stoi(argv[5]);
    
    std::ifstream data_file(data_file_name.c_str());
    std::ifstream query_file(query_file_name.c_str());

    int query_count = std::count(std::istreambuf_iterator<char>(query_file),
                                 std::istreambuf_iterator<char>(), '\n');
    int data_count = std::count(std::istreambuf_iterator<char>(data_file),
                                 std::istreambuf_iterator<char>(), '\n');
    
    if(query_count < query_size) {cout<<"NOT ENOUGH QUERIES IN THE FILE\n"; return 1;}
    if(data_count < data_size) {cout<<"NOT ENOUGH DATA IN THE FILE " << data_count << " " << data_size << "\n";  return 1;}

    cout << "NodeSize " << nodeSize << endl;

    data_file.clear();
    data_file.seekg(0, ios::beg);
    query_file.clear();
    query_file.seekg(0, ios::beg);

    // read all data and put into vector
    std::vector<value> all_data;
    float min_x, min_y, max_x, max_y;
    for(int i = 0; i < data_size; i++){
        data_file >> min_x >> min_y >> max_x >> max_y;
	    box b(point(min_x, min_y), point(max_x, max_y));
	    all_data.push_back(std::make_pair(b, i));
    }

    ofstream times_file;
    times_file.open(time_file_name.c_str());

    // create the rtree using default constructor
    timing();
    bgi::rtree< value, bgi::quadratic<nodeSize> > rtree(all_data);
    double indexing_time = timing();

    // FATEMEH
    // read all queries and put them in a vector
    // i do this because when I do it at the same time, it freaks out
    // not important enough to fix:))
    std::vector<box> queries;

    for(int i = 0; i < query_size; i++){
        query_file >> min_x >> min_y >> max_x >> max_y;
        box b(point(min_x, min_y), point(max_x, max_y));
	    queries.push_back(b);
    }

    // run all the queries and time them
    int last_id = data_size -1;
    int this_id;
    float min[2]; float max[2];
    clock_t q_time = 0;
    std::vector<value> result_s;
    box b;
    clock_t insert_time, start_insert_time;
    clock_t start_query_time;
    
    for(int i = 0; i < query_size; i++){
        result_s.clear();
        start_query_time = clock();
        rtree.query(bgi::intersects(queries[i]), std::back_inserter(result_s));
        q_time = clock() - start_query_time;
        times_file << result_s.size() << "\t" << "\t" << (double)q_time/CLOCKS_PER_SEC << "\t" << indexing_time << "\n";
    }

    query_file.close();
    data_file.close();
    times_file.close();

    return 0;
}
