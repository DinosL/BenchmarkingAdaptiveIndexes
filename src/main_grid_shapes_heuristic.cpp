#include "basics.h"
#include "AKD_grid_shapes_heuristic.h"

int getStartOfcell(ELEMTYPE start, ELEMTYPE extent)
{
    return (int)(start / extent + EPS);
} 


int getCellId(ELEMTYPE x, ELEMTYPE y, int numCellsPerDimension) 
{
    return (y * numCellsPerDimension + x);
}

double myRemainder(double numer, double denom, int q) 
{
    double rem = double(numer - q*denom);

    return ((abs(rem) < EPS) ? 0: rem);
}

void partition2DGrid(int partitionsPerDimension, Rect** index, size_t *indexSize)
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
        xCell = getStartOfcell(m_data_arr_rects[i].m_min[0] + EPS, partitionExtent);
        yCell = getStartOfcell(m_data_arr_rects[i].m_min[1] + EPS, partitionExtent);

        if(xCell == partitionsPerDimension)
            xCell = partitionsPerDimension - 1;
        if(yCell == partitionsPerDimension)
            yCell = partitionsPerDimension - 1;

        cellId = getCellId(xCell, yCell, partitionsPerDimension);

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

    start = clock();
    for (int i = 0; i < totalPartitions; i++){
        // index[i].reserve(indexSize[i]);
        index[i] = (Rect*)malloc(indexSize[i]*sizeof(Rect));
    }
    int cellFilledPointer[totalPartitions];
    memset(cellFilledPointer, 0, totalPartitions*sizeof(int));
    endTime = clock() - start;

    // cout << "Allocation time " << (double)endTime/CLOCKS_PER_SEC << endl;

    start = clock();
    for (size_t i = 0; i < DATA_COUNT; i++){
        
        // Find in which cell the data point belongs
        xCell = getStartOfcell(m_data_arr_rects[i].m_min[0] + EPS, partitionExtent);
        yCell = getStartOfcell(m_data_arr_rects[i].m_min[1] + EPS, partitionExtent);
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

    // cout << "Assigning time " << (double)endTime/CLOCKS_PER_SEC << endl;
}


void partition2DGrid2(int partitionsPerDimension, Rect** indexData, size_t *indexSize)
{
    int xStartCell, yStartCell, xEndCell, yEndCell, firstCellId, lastCellId;
    float partitionExtent = 1.0/partitionsPerDimension;
    int totalPartitions = partitionsPerDimension*partitionsPerDimension;

    memset(indexSize, 0, totalPartitions*sizeof(size_t));

    #ifdef DEBUG
        cout << "extent is " << partitionExtent << " partitionsPerDim " << partitionsPerDimension << endl;
    #endif

    clock_t start = 0, endTime = 0;

    start = clock();
    for (size_t i = 0; i < DATA_COUNT; i++){
        
        // Find in which cell the data rectangle belongs
        xStartCell = getStartOfcell(m_data_arr_rects[i].m_min[0] + EPS, partitionExtent);
        yStartCell = getStartOfcell(m_data_arr_rects[i].m_min[1] + EPS, partitionExtent);

        if(xStartCell == partitionsPerDimension)
            xStartCell = partitionsPerDimension - 1;
        if(yStartCell == partitionsPerDimension)
            yStartCell = partitionsPerDimension - 1;

        firstCellId = getCellId(xStartCell, yStartCell, partitionsPerDimension);

        xEndCell = myRemainder(m_data_arr_rects[i].m_max[0], partitionExtent, int(getStartOfcell(m_data_arr_rects[i].m_max[0] + EPS, partitionExtent)));
        yEndCell = myRemainder(m_data_arr_rects[i].m_max[1], partitionExtent, int(getStartOfcell(m_data_arr_rects[i].m_max[1] + EPS, partitionExtent)));

        if (m_data_arr_rects[i].m_max[0] + EPS >= 1) {
            xEndCell = partitionsPerDimension - 1;
        }
        else {
            xEndCell = getStartOfcell(m_data_arr_rects[i].m_max[0] + EPS, partitionExtent);
        }

        if (m_data_arr_rects[i].m_max[1] + EPS >= 1) {
            yEndCell = partitionsPerDimension - 1;
        }
        else {
            yEndCell = getStartOfcell(m_data_arr_rects[i].m_max[1] + EPS, partitionExtent);
        }

        lastCellId = getCellId(xEndCell, yEndCell, partitionsPerDimension);

        // cout << "Xstartcell " << xStartCell <<" " << xEndCell << " " << yStartCell << "  " << yEndCell << endl;

        if(firstCellId == lastCellId)
        {
            indexSize[firstCellId]++;
        }
        else
        {
            // cout << "+1 for first" << endl;
            indexSize[firstCellId]++;
            int cellNumber;
            for(int x = xStartCell; x <= xEndCell; ++x)
            {
                if(x != xStartCell)
                {
                    cellNumber = getCellId(x, yStartCell, partitionsPerDimension);
                    // cout << "+1 for " << cellNumber << endl;
                    indexSize[cellNumber]++;
                }
                for(int y = yStartCell + 1; y <= yEndCell; y++)
                {
                    if(y != yStartCell)
                    {
                        cellNumber = getCellId(x, y, partitionsPerDimension);
                        // cout << "++1 for " << cellNumber << " " << i << " " << j << endl;
                        indexSize[cellNumber]++;
                    }
                }
                
            }
        }
    }
    endTime = clock() - start;
    // cout << "Counting time " << (double)endTime/CLOCKS_PER_SEC << endl;

    // start = clock();
    for (int i = 0; i < totalPartitions; i++){
    //     // index[i].reserve(indexSize[i]);
    //     indexMins[i] = (ELEMTYPE*)malloc(indexSize[i]*NUMDIMS*sizeof(ELEMTYPE));
    //     indexMaxes[i] = (ELEMTYPE*)malloc(indexSize[i]*NUMDIMS*sizeof(ELEMTYPE));
        // cout << "Partition " << i << " has " << indexSize[i] << " data " << endl;
        // indexData[i] = new vector<Rect>(indexSize[i]);
        indexData[i] = (Rect*)malloc(indexSize[i]*sizeof(Rect));
    }
    int cellFilledPointer[totalPartitions];
    memset(cellFilledPointer, 0, totalPartitions*sizeof(int));
    // endTime = clock() - start;

    // cout << "Allocation time " << (double)endTime/CLOCKS_PER_SEC << endl;

    start = clock();
    for (size_t i = 0; i < DATA_COUNT; i++){
        // Do the same but now copy data to partitions
        xStartCell = getStartOfcell(m_data_arr_rects[i].m_min[0] + EPS, partitionExtent);
        yStartCell = getStartOfcell(m_data_arr_rects[i].m_min[1] + EPS, partitionExtent);

        if(xStartCell == partitionsPerDimension)
            xStartCell = partitionsPerDimension - 1;
        if(yStartCell == partitionsPerDimension)
            yStartCell = partitionsPerDimension - 1;

        firstCellId = getCellId(xStartCell, yStartCell, partitionsPerDimension);

        xEndCell = myRemainder(m_data_arr_rects[i].m_max[0], partitionExtent, int(getStartOfcell(m_data_arr_rects[i].m_max[0] + EPS, partitionExtent)));
        yEndCell = myRemainder(m_data_arr_rects[i].m_max[1], partitionExtent, int(getStartOfcell(m_data_arr_rects[i].m_max[1] + EPS, partitionExtent)));

        if (m_data_arr_rects[i].m_max[0] + EPS >= 1) {
            xEndCell = partitionsPerDimension - 1;
        }
        else {
            xEndCell = getStartOfcell(m_data_arr_rects[i].m_max[0] + EPS, partitionExtent);
        }


        if (m_data_arr_rects[i].m_max[1] + EPS >= 1) {
            yEndCell = partitionsPerDimension - 1;
        }
        else {
            yEndCell = getStartOfcell(m_data_arr_rects[i].m_max[1] + EPS, partitionExtent);
        }

        lastCellId = getCellId(xEndCell, yEndCell, partitionsPerDimension);


        if(firstCellId == lastCellId)
        {
            // for(size_t j = 0; j < NUMDIMS; j++)
            // {
            //     indexMins[firstCellId][cellFilledPointer[firstCellId]*NUMDIMS+j] = m_data_arr_mins[i][j];
            //     indexMaxes[firstCellId][cellFilledPointer[firstCellId]*NUMDIMS+j] = m_data_arr_maxes[i][j];
            // }
            // cellFilledPointer[firstCellId]++;
            // cout << "trying to push back 1" << endl;
            // indexData[firstCellId]->push_back(m_data_arr_rects[i]);
            indexData[firstCellId][cellFilledPointer[firstCellId]] = m_data_arr_rects[i];
            // cout << "Pushed 1 " <<m_data_arr_rects[i].m_min[0] << " to " << firstCellId << " " << cellFilledPointer[firstCellId] << endl;
            cellFilledPointer[firstCellId]++;
        }
        else
        {
            // for(size_t j = 0; j < NUMDIMS; j++)
            // {
            //     indexMins[firstCellId][cellFilledPointer[firstCellId]*NUMDIMS+j] = m_data_arr_mins[i][j];
            //     indexMaxes[firstCellId][cellFilledPointer[firstCellId]*NUMDIMS+j] = m_data_arr_maxes[i][j];
            // }
            // cellFilledPointer[firstCellId]++;
            // cout << "trying to push back 2" << endl;
            // indexData[firstCellId]->push_back(m_data_arr_rects[i]);
            indexData[firstCellId][cellFilledPointer[firstCellId]] = m_data_arr_rects[i];
            // cout << "Pushed 2 " <<m_data_arr_rects[i].m_min[0] << " to " << firstCellId << " " << cellFilledPointer[firstCellId] << endl;
            cellFilledPointer[firstCellId]++;
            

            // for(int o = 0; o < cellFilledPointer[firstCellId]; o++)
            // {
            //     // cout << " -- " << indexData[firstCellId]->at(0).m_min[0] << " " << indexData[firstCellId]->at(0).m_min[1] << " " << indexData[firstCellId]->at(0).m_max[0] << " " << indexData[firstCellId]->at(0).m_max[1] << endl;
            //     cout << " -- " << indexData[firstCellId][o].m_min[0] << " " << indexData[firstCellId][o].m_min[1] << " " << indexData[firstCellId][o].m_max[0] << " " << indexData[firstCellId][o].m_max[1] << endl;
            // }
            // cout << "Pushed " << indexData[firstCellId]->m_min[0] << " " << indexData[firstCellId]->data()->m_min[1] << " " << indexData[firstCellId]->data()->m_max[0] << " " << indexData[firstCellId]->data()->m_min[1] << endl;
            // cout << "Pushed " << m_data_arr_rects[i].m_min[0] << " " << m_data_arr_rects[i].m_min[1] << " " << m_data_arr_rects[i].m_max[0] << " " << m_data_arr_rects[i].m_max[1] << endl;


            int cellNumber;
            for(int x = xStartCell; x <= xEndCell; ++x)
            {
                if(x != xStartCell)
                {
                    cellNumber = getCellId(x, yStartCell, partitionsPerDimension);
                    // indexSize[cellNumber]++;
                    // for(size_t j = 0; j < NUMDIMS; j++)
                    // {
                    //     indexMins[cellNumber][cellFilledPointer[cellNumber]*NUMDIMS+j] = m_data_arr_mins[i][j];
                    //     indexMaxes[cellNumber][cellFilledPointer[cellNumber]*NUMDIMS+j] = m_data_arr_maxes[i][j];
                    // }
                    // cellFilledPointer[cellNumber]++;
                    // cout << "trying to push back 3" << endl;
                    // indexData[cellNumber]->push_back(m_data_arr_rects[i]);
                    indexData[cellNumber][cellFilledPointer[cellNumber]] = m_data_arr_rects[i];
                    // cout << "Pushed 3 " <<m_data_arr_rects[i].m_min[0] << " to " << cellNumber << " " << cellFilledPointer[cellNumber] << endl;
                    cellFilledPointer[cellNumber]++;
                }
                for(int y = yStartCell + 1; y <= yEndCell; y++)
                {
                    cellNumber = getCellId(x, y, partitionsPerDimension);
                    // for(size_t j = 0; j < NUMDIMS; j++)
                    // {
                    //     indexMins[cellNumber][cellFilledPointer[cellNumber]*NUMDIMS+j] = m_data_arr_mins[i][j];
                    //     indexMaxes[cellNumber][cellFilledPointer[cellNumber]*NUMDIMS+j] = m_data_arr_maxes[i][j];
                    // }
                    // cellFilledPointer[cellNumber]++;
                    // cout << "trying to push back 4" << endl;
                    // indexData[cellNumber]->push_back(m_data_arr_rects[i]);
                    indexData[cellNumber][cellFilledPointer[cellNumber]] = m_data_arr_rects[i];
                    // cout << "Pushed 4 " <<m_data_arr_rects[i].m_min[0] << " to " << cellNumber << " " << cellFilledPointer[cellNumber] << endl;
                    cellFilledPointer[cellNumber]++;
                    
                }
                
            }
        }
    }

    endTime = clock() - start;

    // cout << "Assigning time " << (double)endTime/CLOCKS_PER_SEC << endl;
}

// int findReferenceCell(double x, double y, double cellExtent, int numCellsPerDimension) {
//     int xInt,yInt;

//     xInt = (x + EPS)/cellExtent;
//     yInt = (y + EPS)/cellExtent;
    
//     if (numCellsPerDimension == xInt){
//         xInt = numCellsPerDimension -1;
//     }

//     if (numCellsPerDimension == yInt){
//         yInt = numCellsPerDimension -1;
//     }

//     return (yInt * numCellsPerDimension + xInt);
// }



int main(int argc, char **argv){

    string data_file_name = argv[1];
    int query_size = stoi(argv[2]);
    string extended_query_file_name = argv[6];
    string time_file_name = argv[4];
    int partitionsPerDimension = stoi(argv[5]); // if we want a 30x30 grid the input should be 30
    string original_query_file_name = argv[3];

    int totalPartitions = partitionsPerDimension*partitionsPerDimension;
    int xStartCell, yStartCell, xEndCell, yEndCell, startCellId, endCellId, thisCellId, thisQueryResult;
    int linCount = 0;

    clock_t startPartitionTime, endPartitionTime, beginTime, endTime;
    float partitionExtent = 1.0/partitionsPerDimension;

    std::ifstream extended_query_file(extended_query_file_name.c_str());
    std::ifstream original_query_file(original_query_file_name.c_str());

    Rect *extended_queries = (Rect *)malloc(query_size*sizeof(Rect));
    Rect *original_queries = (Rect *)malloc(query_size*sizeof(Rect));

    extended_query_file.clear();
    extended_query_file.seekg(0, ios::beg);

    original_query_file.clear();
    original_query_file.seekg(0, ios::beg);

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

    std::ifstream data_file(data_file_name.c_str());
    for(int i = 0; i < DATA_COUNT; i++){
        m_data_arr_rects[i] = Rect();
        for(int j = 0; j < NUMDIMS; j++) data_file >> m_data_arr_rects[i].m_min[j];
        for(int j = 0; j < NUMDIMS; j++) data_file >> m_data_arr_rects[i].m_max[j];
    }  

    ofstream times_file;
    times_file.open(time_file_name.c_str());

    // Point *index = (Point *)malloc(partitionsPerDimension*sizeof(Point));
    // vector<Point> *cellData = new vector<Point>[totalPartitions];
    // ELEMTYPE **cellDataMins = (ELEMTYPE**)malloc(totalPartitions*sizeof(ELEMTYPE*));
    // ELEMTYPE **cellDataMaxes = (ELEMTYPE**)malloc(totalPartitions*sizeof(ELEMTYPE*));
    // ELEMTYPE *cellData = (ELEMTYPE*)malloc(NUMDIMS*DATA_COUNT*sizeof(ELEMTYPE*));
    size_t *indexSize = (size_t *)malloc(totalPartitions * sizeof(size_t));
    // vector<Rect> *cellData[totalPartitions];
    Rect **cellData = (Rect**)malloc(totalPartitions*sizeof(Rect*));

    startPartitionTime = clock();
    partition2DGrid(partitionsPerDimension, cellData, indexSize);
    AKDTree *cellIndexes = (AKDTree*)malloc(totalPartitions*sizeof(AKDTree));
    for(int i = 0; i < totalPartitions;i++)
    {
        cellIndexes[i] = AKDTree(cellData[i], indexSize[i]);
    }
    endPartitionTime = clock() - startPartitionTime;

    int bottomLeft, topLeft, bottomRight, topRight;
    Rect rootCover = Rect();
    rootCover.m_min[0] = 0;
    rootCover.m_max[0] = 1;
    rootCover.m_min[1] = 0;
    rootCover.m_max[1] = 1;
    for(int i = 0; i < query_size; i++)
    {
        // if(queries[i].m_max[0] > 1.0)
        //     queries[i].m_max[0] = 1.0;
        // if(queries[i].m_max[1] > 1.0)
        //     queries[i].m_max[1] = 1.0;

        beginTime = clock();
        
        thisQueryResult = 0;

        xStartCell = getStartOfcell(extended_queries[i].m_min[0] + EPS, partitionExtent);
        yStartCell = getStartOfcell(extended_queries[i].m_min[1] + EPS, partitionExtent);
        if(xStartCell == partitionsPerDimension)
            xStartCell = partitionsPerDimension - 1;
        if(yStartCell == partitionsPerDimension)
            yStartCell = partitionsPerDimension - 1;

        startCellId= getCellId(xStartCell, yStartCell, partitionsPerDimension);


        xEndCell = getStartOfcell(extended_queries[i].m_max[0] + EPS, partitionExtent);
        yEndCell = getStartOfcell(extended_queries[i].m_max[1] + EPS, partitionExtent);
        if(xEndCell == partitionsPerDimension)
            xEndCell = partitionsPerDimension - 1;
        if(yEndCell == partitionsPerDimension)
            yEndCell = partitionsPerDimension - 1;
        endCellId = getCellId(xEndCell, yEndCell, partitionsPerDimension);


        // cout << "Query covers cells " << xStartCell << " " << yStartCell << " " << xEndCell << " " << yEndCell << endl;
        // cout << "Sizes " << indexSize[xStartCell] << " " << indexSize[yStartCell] << " " << " " << indexSize[xEndCell] << " " << indexSize[yEndCell] << endl;

        // int sanityCheck = 0;
        // for(int oo = 0; oo < indexSize[xStartCell]; ++oo)
        // {
        //     if(cellIndexes[xStartCell].dataArr[oo].m_min[0] >= original_queries[i].m_min[0] && cellIndexes[xStartCell].dataArr[oo].m_max[0] <= original_queries[i].m_max[0] && cellIndexes[xStartCell].dataArr[oo].m_min[1] >= original_queries[i].m_min[1] && cellIndexes[xStartCell].dataArr[oo].m_max[1] <= original_queries[i].m_max[1])
        //         sanityCheck++;

        // }
        // cout << "SANITY CHECK " << sanityCheck << endl;

        for(int xIndex = xStartCell; xIndex <= xEndCell; xIndex++)
        {
            for(int yIndex = yStartCell; yIndex <= yEndCell; yIndex++)
            {
                thisCellId = getCellId(xIndex, yIndex, partitionsPerDimension);
                if(xIndex == xStartCell || xIndex == xEndCell || yIndex == yStartCell || yIndex == yEndCell)
                {
                    //adaptive query
                    #ifdef DEBUG
                        cout << "Query bounds " << original_queries[i].m_max[0] << " " << original_queries[i].m_max[1] << endl;
                        cout << "before query: cellID " << thisCellId << " xIndex, yIndex " << xIndex << ", " << yIndex  << endl;
                    #endif
                    if(indexSize[thisCellId] != 0)
                    {
                        // cout << "In for partition " << thisCellId << " with size " << indexSize[thisCellId] << " extent "  << partitionExtent << endl;
                        thisQueryResult += cellIndexes[thisCellId].AdaptiveQuery(&(extended_queries)[i], &(original_queries)[i], partitionsPerDimension, thisCellId, rootCover);
                    }
                    // thisQueryResult += cellIndexes[thisCellId].AdaptiveQuery(&(extended_queries)[i], &(original_queries[i]));
                    #ifdef DEBUG
                        cout << "after query" << endl;
                    #endif

                }
                else
                {
                //    cout << "Looking through " << indexSize[thisCellId] << " items " << endl;
                    for(int ii = 0; ii < indexSize[thisCellId]; ++ii)
                    {
                        // auto x = max(cellIndexes[thisCellId].dataArr[ii].m_min[0], original_queries->m_min[0]);
                        // auto y = min(cellIndexes[thisCellId].dataArr[ii].m_max[1], original_queries->m_max[1]);
                        // auto pid_ref = findReferenceCell(x, y, partitionExtent, partitionsPerDimension);
                        // if(pid_ref == thisCellId) 
                        if(Overlap(&(original_queries)[i], &(cellData[thisCellId][ii])))
                            thisQueryResult++;
                    }
                }
            }
        }
        endTime = clock() - beginTime;
        times_file << thisQueryResult << "\t" << (double)endTime/CLOCKS_PER_SEC << "\t" << (double)endPartitionTime/CLOCKS_PER_SEC << endl;
        // cout << "RESULTS = " << thisQueryResult << endl;

        #ifdef linearcheck
            linCount = 0;
            // cout << "Q " << original_queries->m_min[0] << " " << original_queries->m_min[1] << " " << original_queries->m_max[0] << " " << original_queries->m_max[1] << endl;
            for(int ii =0; ii < DATA_COUNT; ii++){
                if(Overlap(&(original_queries[i]), &(m_data_arr_rects[ii]))){
                    linCount++;
                }
            }
            if(linCount != thisQueryResult){
            // if(abs(linCount - thisQueryResult) > 1){
                cout << "WRONG ANSWER, TRAGEDY, DISASTER, ALL THAT..." << thisQueryResult << " " << linCount << endl;
                cout << "Query \n";
                for(int ii = 0; ii < NUMDIMS; ii++)
                {
                    cout << original_queries[i].m_min[ii] << " " << original_queries[i].m_max[ii] << ",";
                }
                cout << "\n";
                // exit(0);
            }
        #endif

        // cout << "Query " << i << " : " << thisQueryResult << endl;
        // cout << thisQueryResult << endl;
    }


    // cout << "Partitioning time " << (double)endPartitionTime/CLOCKS_PER_SEC << endl;
    return 0;
}