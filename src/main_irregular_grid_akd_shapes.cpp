#include "basics.h"
// #include "AIR.h"
// #include "AKD.h"
// #include "RTile.h"
#include "AKD_grid_shapes_heuristic.h"



int partition(int *mapping, int low, int high)
{
    // Choosing the pivot
    int folan;
    int pivot = mapping[high];
 
    // Index of smaller element and indicates
    // the right position of pivot found so far
    int i = (low - 1);
 
    for (int j = low; j <= high - 1; j++) {
 
        // If current element is smaller than the pivot
        if (mapping[j] < pivot) {
 
            // Increment index of smaller element
            i++;
            for(folan = 0; folan< NUMDIMS; folan++){
                swap(m_data_arr[i][folan], m_data_arr[j][folan]);
            }
            swap(m_data_ids[i], m_data_ids[j]);
            swap(mapping[i],mapping[j]);
        }
    }
    for(folan = 0; folan< NUMDIMS; folan++){
        swap(m_data_arr[i + 1][folan], m_data_arr[high][folan]);
    }
    swap(m_data_ids[i + 1], m_data_ids[high]);
    swap(mapping[i + 1], mapping[high]);
    return (i + 1);
}


void quickSort(int *mapping, int low, int high)
{
    if (low < high) {
 
        // pi is partitioning index, arr[p]
        // is now at right place
        int pi = partition(mapping, low, high);
 
        // Separately sort elements before
        // partition and after partition
        quickSort(mapping, low, pi - 1);
        quickSort(mapping, pi + 1, high);
    }
}

int *dataToLeafId;
int compare (const void * a, const void * b) {
    double diff = dataToLeafId[*(int*)a] - dataToLeafId[*(int*)b];
    return  (0 < diff) - (diff < 0);
}

int binarySearchFirstBigger(ELEMTYPE *arr, int size, ELEMTYPE key)
{
    int start = 0;
    int end = size-1;

    if(start >= end)
        return 0;
    
    int low = start;
    int high = end;
    
    int ans = end;
    bool flag = false;
    while (low <= high) {
        
        int mid = (high+low)/2;
        double midVal = arr[mid];
        
        if (midVal < key) {
            low = mid + 1;
        }
        else if (midVal >= key) {
            ans = mid;
            high = mid - 1;
        }
    }
    if(low > end)
        return end+1;
    
    return ans;
}

int getCellId(ELEMTYPE x, ELEMTYPE y, int numCellsPerDimension) 
{
    return (y * numCellsPerDimension + x);
}

void partition2DGrid(int partitionsPerDimension, Rect** index, size_t *indexSize, ELEMTYPE *xPartitionValues, ELEMTYPE *yPartitionValues)
{
    int xCell, yCell, cellId;
    float partitionExtent = 1.0/partitionsPerDimension;
    int totalPartitions = partitionsPerDimension*partitionsPerDimension;

    memset(indexSize, 0, totalPartitions*sizeof(size_t));

    #ifdef DEBUG
        cout << "extent is " << partitionExtent << " partitionsPerDim " << partitionsPerDimension << endl;
    #endif

    clock_t start = 0, endTime = 0;

    start = clock();
    for (size_t i = 0; i < DATA_COUNT; i++){
        
        // Find in which cell the data point belongs
        // xCell = getStartOfcell(m_data_arr[i][0] + EPS, partitionExtent);
        // yCell = getStartOfcell(m_data_arr[i][1] + EPS, partitionExtent);

        xCell = binarySearchFirstBigger(xPartitionValues, partitionsPerDimension-1, m_data_arr_rects[i].m_min[0]);
        yCell = binarySearchFirstBigger(yPartitionValues, partitionsPerDimension-1, m_data_arr_rects[i].m_min[1]);

        if(xCell == partitionsPerDimension)
            xCell = partitionsPerDimension - 1;
        if(yCell == partitionsPerDimension)
            yCell = partitionsPerDimension - 1;

        cellId = getCellId(xCell, yCell, partitionsPerDimension);
        // cout << "Item " << m_data_arr[i][0] << " " << m_data_arr[i][1] << " xCell, yCell " << xCell << " " << yCell << " ";
        // cout << "cell " << cellId << endl;

        #ifdef DEBUG
            if(cellId > totalPartitions)
            {
                cout << "In build, cellID more than total number of cells " << cellId << " data coords " << m_data_arr[i][0] << ", " << m_data_arr[i][1]  << endl;
                cout << "xCell, yCell " << xCell << " ," << yCell << " max is " << totalPartitions << endl;
                cout << yCell * partitionsPerDimension + xCell << endl;
                exit(0);
            }
        #endif

        indexSize[cellId]++;
    }
    endTime = clock() - start;
    // cout << "Counting time " << (double)endTime/CLOCKS_PER_SEC << endl;
    // cout << "Afte counting\n";

    start = clock();
    for (int i = 0; i < totalPartitions; i++){
        // index[i].reserve(indexSize[i]);
        index[i] = (Rect*)malloc(indexSize[i]*sizeof(Rect));
    }
    int cellFilledPointer[totalPartitions];
    memset(cellFilledPointer, 0, totalPartitions*sizeof(int));
    endTime = clock() - start;

    // cout << "Allocation time " << (double)endTime/CLOCKS_PER_SEC << endl;

    // cout << "After allocating" << endl;

    start = clock();
    for (size_t i = 0; i < DATA_COUNT; i++){
        
        // Find in which cell the data point belongs
        // xCell = getStartOfcell(m_data_arr[i][0] + EPS, partitionExtent);
        // yCell = getStartOfcell(m_data_arr[i][1] + EPS, partitionExtent);
        xCell = binarySearchFirstBigger(xPartitionValues, partitionsPerDimension-1, m_data_arr_rects[i].m_min[0]);
        yCell = binarySearchFirstBigger(yPartitionValues, partitionsPerDimension-1, m_data_arr_rects[i].m_min[1]);
        if(xCell == partitionsPerDimension)
            xCell = partitionsPerDimension - 1;
        if(yCell == partitionsPerDimension)
            yCell = partitionsPerDimension - 1;
        cellId = getCellId(xCell, yCell, partitionsPerDimension);
        // if(cellFilledPointer[cellId] == 0)
        // {
        //     index[cellId] = (ELEMTYPE*)malloc(indexSize[cellId]*NUMDIMS*sizeof(ELEMTYPE));
        // }
        // for(size_t j = 0; j < NUMDIMS; j++)
        // {
        //     index[cellId][cellFilledPointer[cellId]*NUMDIMS+j] = m_data_arr[i][j];
        // }
        Rect r = Rect(m_data_arr_rects[i].m_min,m_data_arr_rects[i].m_max);
        index[cellId][cellFilledPointer[cellId]] = r;
        cellFilledPointer[cellId]++;
    }
    endTime = clock() - start;
    // cout << "after assinging\n";

    // cout << "Assigning time " << (double)endTime/CLOCKS_PER_SEC << endl;
}


int main(int argc, char **argv){


    string data_file_name = argv[1];
    int query_size = stoi(argv[2]);
    string original_query_file_name = argv[3];
    string time_file_name = argv[4];
    int batchSize = stoi(argv[5]);
    int partitionsPerDimension = stoi(argv[6]);
    string extended_query_file_name = argv[7];
    int totalPartitions = partitionsPerDimension*partitionsPerDimension;
    int xStartCell, yStartCell, xEndCell, yEndCell, startCellId, endCellId, thisCellId, thisQueryResult;

    clock_t startPartitionTime, endPartitionTime, beginTime, endTime;

    clock_t endSamplesTime, endSortXTime, endSortYTime, endPartitionXValuesTime, endPartitionYValuesTime;

    cout << "Grid " << partitionsPerDimension << " batch " << batchSize << endl;

    auto seed = time(NULL); // 1695893019
    // auto seed = 0;
    seed = 1697617670;
    // cout << seed << endl;
    // auto seed = 1692015686; // crashes at 81199
    srand(seed);
    // cout << CRACKTHRESHOLD << endl;

    std::ifstream extended_query_file(extended_query_file_name.c_str());
    std::ifstream original_query_file(original_query_file_name.c_str());

    ofstream times_file;
    times_file.open(time_file_name.c_str());


    // we also make empty tree to fill up
    AKDTree akd = AKDTree();

    ELEMTYPE min[NUMDIMS]; ELEMTYPE max[NUMDIMS];
    int this_id;

    // cout << "BUILDING COMPLETE\n";

    extended_query_file.clear();
    extended_query_file.seekg(0, ios::beg);

    original_query_file.clear();
    original_query_file.seekg(0, ios::beg);

    Rect *extended_queries = (Rect *)malloc(query_size*sizeof(Rect));
    Rect *original_queries = (Rect *)malloc(query_size*sizeof(Rect));

    std::ifstream data_file(data_file_name.c_str());
    for(int i = 0; i < DATA_COUNT; i++){
        m_data_arr_rects[i] = Rect();
        for(int j = 0; j < NUMDIMS; j++) data_file >> m_data_arr_rects[i].m_min[j];
        for(int j = 0; j < NUMDIMS; j++) data_file >> m_data_arr_rects[i].m_max[j];
    }  

    for(int i = 0; i < query_size; i++){
        extended_queries[i] = Rect();
        for(int j = 0; j < NUMDIMS; j++) extended_query_file >> extended_queries[i].m_min[j];
        for(int j = 0; j < NUMDIMS; j++) extended_query_file >> extended_queries[i].m_max[j];
    }

    for(int i = 0; i < query_size; i++){
        original_queries[i] = Rect();
        for(int j = 0; j < NUMDIMS; j++) original_query_file >> original_queries[i].m_min[j];
        for(int j = 0; j < NUMDIMS; j++) original_query_file >> original_queries[i].m_max[j];
    }

    // for(int i = 0; i < query_size; i++){
    //     for(int j = 0; j < NUMDIMS; j++){
    //         cout << original_queries[i].m_min[j] << " " << original_queries[i].m_max[j];
    //     }
    //     cout << "\n";
    // }

    // for(int i = 0; i < query_size; i++){
    //     for(int j = 0; j < NUMDIMS; j++){
    //         cout << extended_queries[i].m_min[j] << " " << extended_queries[i].m_max[j];
    //     }
    //     cout << "\n";
    // }

    
    int found_count; int sum_ids; int total_count;
    int lin_count=0;
    double q_time = 0;
    int rightestright = 0;
    this_id = DATA_COUNT -1;
    std::set<ELEMTYPE> boundsSet[NUMDIMS];
    vector<pair<int, Rect>> thisBatchQueries;
    int i;

    bool init = true;
    #ifdef TIMEBREAKDOWN
        cerr << "Threshold : " << CRACKTHRESHOLD << endl;
    #endif

    // get batchSize random data objects as a sample to create irregular partitions

    #ifdef TIMEBREAKDOWN
        beginTime = clock();
    #endif
    vector<Rect> samples;
    for(i = 0; i < batchSize; ++i)
    {
        int rndNum = std::rand() % DATA_COUNT;
        samples.push_back(m_data_arr_rects[rndNum]);
    }

    #ifdef TIMEBREAKDOWN
        endSamplesTime = clock() - beginTime;
    #endif

    // sort on x


    #ifdef TIMEBREAKDOWN
        beginTime = clock();
    #endif
    sort(samples.begin(), samples.end(), [](const Rect& a, const Rect& b) {
        return a.m_min[0] < b.m_min[0];
    });

    #ifdef TIMEBREAKDOWN
        endSortXTime = clock() - beginTime;
    #endif

    size_t partitionSize = samples.size() / (partitionsPerDimension - 1);

    #ifdef TIMEBREAKDOWN
        beginTime = clock();
    #endif
    size_t start = 0;
    ELEMTYPE *xPartitionValues = (ELEMTYPE*)malloc(partitionsPerDimension*sizeof(ELEMTYPE));
    for(i = 0; i < partitionsPerDimension-1; ++i)
    {
        size_t end = start + partitionSize;
        if(end >= batchSize) end -= 1;
        xPartitionValues[i] = samples[end].m_min[0];
        start = end;
    }
    #ifdef TIMEBREAKDOWN
        endPartitionXValuesTime = clock() - beginTime;
    #endif

    // sort on y
    #ifdef TIMEBREAKDOWN
        beginTime = clock();
    #endif
    sort(samples.begin(), samples.end(), [](const Rect& a, const Rect& b) {
        return a.m_min[1] < b.m_min[1];
    });
    #ifdef TIMEBREAKDOWN
        endSortYTime = clock() - beginTime;
    #endif

    #ifdef TIMEBREAKDOWN
        beginTime = clock();
    #endif
    start = 0;
    ELEMTYPE *yPartitionValues = (ELEMTYPE*)malloc(partitionsPerDimension*sizeof(ELEMTYPE));
    for(i = 0; i < partitionsPerDimension-1; ++i)
    {
        size_t end = start + partitionSize;
        if(end >= batchSize) end -= 1;
        yPartitionValues[i] = samples[end].m_min[1];
        start = end;
    }

    #ifdef TIMEBREAKDOWN
        endPartitionYValuesTime = clock() - beginTime;
    #endif

    // Now partition the data based on the values found using the sample
    // To do that for each data point we do a binary search on x to find in which column it belongs
    // then binary search on y to find the exact cell

    int xValue, yValue;
    Rect **cellData = (Rect**)malloc(totalPartitions * sizeof(Rect*));
    size_t *indexSize = (size_t *)malloc(totalPartitions * sizeof(size_t));


    startPartitionTime = clock();
    partition2DGrid(partitionsPerDimension, cellData, indexSize, xPartitionValues, yPartitionValues);

    AKDTree *cellIndexes = (AKDTree*)malloc(totalPartitions*sizeof(AKDTree));
    for(int i = 0; i < totalPartitions;i++)
    {
        cellIndexes[i] = AKDTree(cellData[i], indexSize[i]);
        // cout << "Partition\t" << i << "\t" << indexSize[i] << endl;
    }
    // exit(0);
    endPartitionTime = clock() - startPartitionTime;

    #ifdef TIMEBREAKDOWN
        cout << "SamplingsTime\tsortXTime\tsortYTime\tpartitionXValues\tpartitionYValues\ttotalPartition\n";
        cout << (double)endSamplesTime/CLOCKS_PER_SEC << "\t" << (double)endSortXTime/CLOCKS_PER_SEC << "\t" << (double)endSortYTime/CLOCKS_PER_SEC << "\t" << (double)endPartitionXValuesTime/CLOCKS_PER_SEC << "\t" << (double)endPartitionYValuesTime/CLOCKS_PER_SEC << "\t" << (double)endPartitionTime/CLOCKS_PER_SEC << endl;
    #endif
    


    Rect rootCover = Rect();
    rootCover.m_min[0] = 0;
    rootCover.m_max[0] = 1;
    rootCover.m_min[1] = 0;
    rootCover.m_max[1] = 1;
    for(int i = 0; i < query_size; i++)
    {

        beginTime = clock();
        
        thisQueryResult = 0;
        
        // xStartCell = getStartOfcell(queries[i].m_min[0] + EPS, partitionExtent);
        // yStartCell = getStartOfcell(queries[i].m_min[1] + EPS, partitionExtent);

        xStartCell = binarySearchFirstBigger(xPartitionValues, partitionsPerDimension-1, extended_queries[i].m_min[0]);
        yStartCell = binarySearchFirstBigger(yPartitionValues, partitionsPerDimension-1, extended_queries[i].m_min[1]);

        if(xStartCell == partitionsPerDimension)
            xStartCell = partitionsPerDimension - 1;
        if(yStartCell == partitionsPerDimension)
            yStartCell = partitionsPerDimension - 1;

        startCellId = getCellId(xStartCell, yStartCell, partitionsPerDimension);


        // xEndCell = getStartOfcell(queries[i].m_max[0] + EPS, partitionExtent);
        // yEndCell = getStartOfcell(queries[i].m_max[1] + EPS, partitionExtent);
        xEndCell = binarySearchFirstBigger(xPartitionValues, partitionsPerDimension-1, extended_queries[i].m_max[0]);
        yEndCell = binarySearchFirstBigger(yPartitionValues, partitionsPerDimension-1, extended_queries[i].m_max[1]);
        if(xEndCell == partitionsPerDimension)
            xEndCell = partitionsPerDimension - 1;
        if(yEndCell == partitionsPerDimension)
            yEndCell = partitionsPerDimension - 1;
        // endCellId = getCellId(xEndCell, yEndCell, partitionsPerDimension);

        // cout << "Query covers " << xStartCell << " " << xEndCell << " " << yStartCell << " " << yEndCell << endl;

        for(int xIndex = xStartCell; xIndex <= xEndCell; xIndex++)
        {
            for(int yIndex = yStartCell; yIndex <= yEndCell; yIndex++)
            {
                thisCellId = getCellId(xIndex, yIndex, partitionsPerDimension);
                if(xIndex == xStartCell || xIndex == xEndCell || yIndex == yStartCell || yIndex == yEndCell)
                {
                    //adaptive query
                    #ifdef DEBUG
                        cout << "Query bounds " << queries[i].m_max[0] << " " << queries[i].m_max[1] << endl;
                        cout << "before query: cellID " << thisCellId << " xIndex, yIndex " << xIndex << ", " << yIndex  << endl;
                    #endif
                    // cout << "Evaluating query " << i << endl;
                    thisQueryResult += cellIndexes[thisCellId].AdaptiveQuery(&(extended_queries)[i], &(original_queries)[i], partitionsPerDimension, thisCellId, rootCover);
                    #ifdef DEBUG
                        cout << "after query" << endl;
                    #endif

                }
                else
                {
                    thisQueryResult += indexSize[thisCellId];
                }
            }
        }
        endTime = clock() - beginTime;
        times_file << thisQueryResult << "\t" << (double)endTime/CLOCKS_PER_SEC << "\t" << (double)endPartitionTime/CLOCKS_PER_SEC << endl;

        #ifdef linearcheck
            int linCount = 0;
            for(int ii =0; ii < DATA_COUNT; ii++){
                if(Overlap(&(original_queries[i]), &(m_data_arr_rects[ii]))){
                    linCount++;
                }
            }

            // cout << "Found, Linear: " << thisQueryResult << " " << linCount << endl;
            if(linCount != thisQueryResult){
            // if(abs(linCount - thisQueryResult) > 1){
                cout << "WRONG ANSWER, TRAGEDY, DISASTER, ALL THAT..." << thisQueryResult << " " << linCount << endl;
                // cout << "Query \n";
                // for(int ii = 0; ii < NUMDIMS; ii++)
                // {
                //     cout << queries[i].m_min[ii] << " " << queries[i].m_max[ii] << ",";
                // }
                // cout << "\n";
                // exit(0);
            }
        #endif

        // cout << "Query " << i << " : " << thisQueryResult << endl;
        // cout << thisQueryResult << endl;
    }


    original_query_file.close();
    extended_query_file.close();
    times_file.close();
    cout << "DONE!!\n";
    return 0;

}

