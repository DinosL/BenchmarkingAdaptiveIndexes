#include "basics.h"
#include "AKD_grid_heuristic.h"

int getStartOfcell(ELEMTYPE start, ELEMTYPE extent)
{
    return (int)(start / extent + EPS);
} 


int getCellId(ELEMTYPE x, ELEMTYPE y, int numCellsPerDimension) 
{
    return (y * numCellsPerDimension + x);
}

void partition2DGrid(int partitionsPerDimension, ELEMTYPE** index, size_t *indexSize)
{
    int xCell, yCell, cellId;
    float partitionExtent = 1.0/partitionsPerDimension;
    int totalPartitions = partitionsPerDimension*partitionsPerDimension;

    memset(indexSize, 0, totalPartitions*sizeof(size_t));

    // cout << "In partition\n";

    #ifdef DEBUG
        cout << "extent is " << partitionExtent << " partitionsPerDim " << partitionsPerDimension << endl;
    #endif

    clock_t start = 0, endTime = 0;

    start = clock();
    for (size_t i = 0; i < DATA_COUNT; i++){
        
        // Find in which cell the data point belongs
        xCell = getStartOfcell(m_data_arr[i][0] + EPS, partitionExtent);
        yCell = getStartOfcell(m_data_arr[i][1] + EPS, partitionExtent);

        if(xCell == partitionsPerDimension)
            xCell = partitionsPerDimension - 1;
        if(yCell == partitionsPerDimension)
            yCell = partitionsPerDimension - 1;

        cellId = getCellId(xCell, yCell, partitionsPerDimension);
        // cout << "Item " << m_data_arr[i][0] << " " << m_data_arr[i][1] << " ";
        // cout << "cell " << cellId << " out of " << totalPartitions << endl;

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
        index[i] = (ELEMTYPE*)malloc(indexSize[i]*NUMDIMS*sizeof(ELEMTYPE));
    }
    int cellFilledPointer[totalPartitions];
    memset(cellFilledPointer, 0, totalPartitions*sizeof(int));
    endTime = clock() - start;

    // cout << "Allocation time " << (double)endTime/CLOCKS_PER_SEC << endl;

    // cout << "After allocating" << endl;

    start = clock();
    for (size_t i = 0; i < DATA_COUNT; i++){
        
        // Find in which cell the data point belongs
        xCell = getStartOfcell(m_data_arr[i][0] + EPS, partitionExtent);
        yCell = getStartOfcell(m_data_arr[i][1] + EPS, partitionExtent);
        if(xCell == partitionsPerDimension)
            xCell = partitionsPerDimension - 1;
        if(yCell == partitionsPerDimension)
            yCell = partitionsPerDimension - 1;
        cellId = getCellId(xCell, yCell, partitionsPerDimension);
        // if(cellFilledPointer[cellId] == 0)
        // {
        //     index[cellId] = (ELEMTYPE*)malloc(indexSize[cellId]*NUMDIMS*sizeof(ELEMTYPE));
        // }
        for(size_t j = 0; j < NUMDIMS; j++)
        {
            index[cellId][cellFilledPointer[cellId]*NUMDIMS+j] = m_data_arr[i][j];
        }
        cellFilledPointer[cellId]++;
    }
    endTime = clock() - start;
    // cout << "after assinging\n";

    // cout << "Assigning time " << (double)endTime/CLOCKS_PER_SEC << endl;
}


int main(int argc, char **argv){

    string data_file_name = argv[1];
    int query_size = stoi(argv[2]);
    string query_file_name = argv[3];
    string time_file_name = argv[4];
    int partitionsPerDimension = stoi(argv[5]); // if we want a 30x30 grid the input should be 30
    int totalPartitions = partitionsPerDimension*partitionsPerDimension;
    int xStartCell, yStartCell, xEndCell, yEndCell, startCellId, endCellId, thisCellId, thisQueryResult;
    int linCount = 0;

    clock_t startPartitionTime, endPartitionTime, beginTime, endTime;
    float partitionExtent = 1.0/partitionsPerDimension;

    Rect *queries = (Rect *)malloc(query_size*sizeof(Rect));
    std::ifstream query_file(query_file_name.c_str());

    for(int i = 0; i < query_size; i++){
        queries[i] = Rect();
        for(int j = 0; j < NUMDIMS; j++) query_file >> queries[i].m_min[j];
        for(int j = 0; j < NUMDIMS; j++) query_file >> queries[i].m_max[j];
    }

    vector<Point> data(DATA_COUNT);
    std::ifstream data_file(data_file_name.c_str());
    for(int i = 0; i < DATA_COUNT; i++){
        for(int j = 0; j < NUMDIMS; j++) data_file >> m_data_arr[i][j];
        data[i].m_values[0] = m_data_arr[i][0];
        data[i].m_values[1] = m_data_arr[i][1];
        m_data_ids[i] = i;
    }

    ofstream times_file;
    times_file.open(time_file_name.c_str());

    // Point *index = (Point *)malloc(partitionsPerDimension*sizeof(Point));
    // vector<Point> *cellData = new vector<Point>[totalPartitions];
    ELEMTYPE **cellData = (ELEMTYPE**)malloc(totalPartitions*sizeof(ELEMTYPE*));
    // ELEMTYPE *cellData = (ELEMTYPE*)malloc(NUMDIMS*DATA_COUNT*sizeof(ELEMTYPE*));
    size_t *indexSize = (size_t *)malloc(totalPartitions * sizeof(size_t));


    startPartitionTime = clock();
    partition2DGrid(partitionsPerDimension, cellData, indexSize);
    // cout << "After partitioning\n";
    // partition2DGridOneArray(partitionsPerDimension, cellData, indexSize);
    

    // int partitionPointer[totalPartitions];
    // AKDTree *cellIndexes = (AKDTree*)malloc(totalPartitions*sizeof(AKDTree));
    // partitionPointer[0] = 0;
    // cellIndexes[0] = AKDTree(cellData, 0, indexSize[0]);
    // for (int i = 1; i < totalPartitions; i++){
    //     partitionPointer[i] = indexSize[i-1] + partitionPointer[i-1];
    //     cellIndexes[i] = AKDTree(cellData, partitionPointer[i], partitionPointer[i]+indexSize[i]);
    // }

    AKDTree *cellIndexes = (AKDTree*)malloc(totalPartitions*sizeof(AKDTree));
    for(int i = 0; i < totalPartitions;i++)
    {
        cellIndexes[i] = AKDTree(cellData[i], indexSize[i]);
        // cout << "Partition\t" << i << "\t" << indexSize[i] << endl;
    }
    // exit(0);
    endPartitionTime = clock() - startPartitionTime;
    // cout << "Partitioning time " << (double)endPartitionTime/CLOCKS_PER_SEC << endl;
    // cellIndexes[0] = AKDTree(cellData, 0, indexSize[0]);
    // for(int i = 0; i < totalPartitions;i++)
    // {
    //     // cellIndexes[i] = AKDTree(cellData[i], indexSize[i]);
       
    // }

    int bottomLeft, topLeft, bottomRight, topRight;
    Rect rootCover = Rect();
    rootCover.m_min[0] = 0;
    rootCover.m_max[0] = 1;
    rootCover.m_min[1] = 0;
    rootCover.m_max[1] = 1;
    for(int i = 0; i < query_size; i++)
    {
        if(queries[i].m_max[0] > 1.0)
            queries[i].m_max[0] = 1.0;
        if(queries[i].m_max[1] > 1.0)
            queries[i].m_max[1] = 1.0;

        beginTime = clock();
        
        thisQueryResult = 0;
        
        xStartCell = getStartOfcell(queries[i].m_min[0] + EPS, partitionExtent);
        yStartCell = getStartOfcell(queries[i].m_min[1] + EPS, partitionExtent);
        if(xStartCell == partitionsPerDimension)
            xStartCell = partitionsPerDimension - 1;
        if(yStartCell == partitionsPerDimension)
            yStartCell = partitionsPerDimension - 1;

        startCellId= getCellId(xStartCell, yStartCell, partitionsPerDimension);


        xEndCell = getStartOfcell(queries[i].m_max[0] + EPS, partitionExtent);
        yEndCell = getStartOfcell(queries[i].m_max[1] + EPS, partitionExtent);
        if(xEndCell == partitionsPerDimension)
            xEndCell = partitionsPerDimension - 1;
        if(yEndCell == partitionsPerDimension)
            yEndCell = partitionsPerDimension - 1;
        endCellId = getCellId(xEndCell, yEndCell, partitionsPerDimension);

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
                    thisQueryResult += cellIndexes[thisCellId].AdaptiveQuery(queries[i].m_min, queries[i].m_max, rootCover);
                    #ifdef SPACE_COMPLEXITY
                        cout << cellIndexes[thisCellId].countNodes() << endl;
                    #endif
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
            linCount = 0;
            for(int ii =0; ii < DATA_COUNT; ii++){
                if(Overlap(queries[i].m_min, queries[i].m_max, data[ii].m_values)){
                    linCount++;
                }
            }
            if(linCount != thisQueryResult){
            // if(abs(linCount - thisQueryResult) > 1){
                cout << "WRONG ANSWER, TRAGEDY, DISASTER, ALL THAT..." << thisQueryResult << " " << linCount << endl;
                cout << "Query \n";
                for(int ii = 0; ii < NUMDIMS; ii++)
                {
                    cout << queries[i].m_min[ii] << " " << queries[i].m_max[ii] << ",";
                }
                cout << "\n";
                // exit(0);
            }
        #endif

        // cout << "Query " << i << " : " << thisQueryResult << endl;
        // cout << thisQueryResult << endl;
    }
    cout << "Instoch " << inStoch << " actual " << actuallyCracked << endl; 

    #ifdef SPACE_COMPLEXITY
        int cnt = 0;
        for(int i = 0; i < totalPartitions; i++)
        {
            // cout << cellIndexes[i].countNodes() << endl;
            cnt += cellIndexes[i].countNodes();
        }
        cout << cnt*sizeof(AKDTree) << " " << sizeof(AKDTree)  << endl;
    #endif


    // cout << "Partitioning time " << (double)endPartitionTime/CLOCKS_PER_SEC << endl;
    return 0;
}