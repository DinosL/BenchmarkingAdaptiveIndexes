#include "basics.h"
#include "AKD_grid_shapes.h"

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
    #ifdef SPACE_COMPLEXITY
        size_t totalSize = totalPartitions * sizeof(Rect*);
    #endif
    for (int i = 0; i < totalPartitions; i++){
    //     // index[i].reserve(indexSize[i]);
    //     indexMins[i] = (ELEMTYPE*)malloc(indexSize[i]*NUMDIMS*sizeof(ELEMTYPE));
    //     indexMaxes[i] = (ELEMTYPE*)malloc(indexSize[i]*NUMDIMS*sizeof(ELEMTYPE));
        // cout << "Partition " << i << " has " << indexSize[i] << " data " << endl;
        // indexData[i] = new vector<Rect>(indexSize[i]);
        indexData[i] = (Rect*)malloc(indexSize[i]*sizeof(Rect));
        #ifdef SPACE_COMPLEXITY
            totalSize += indexSize[i] * sizeof(Rect);
        #endif
    }
    #ifdef SPACE_COMPLEXITY
        cout << "Total Size " << totalSize << endl;
    #endif
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

bool pointInPolygon(float polygonX, float polygonY, Rect query)
{
    // cout << "Comparing " << polygonX << " < " <<  query.m_min[0] << " || " << polygonX  << " > " <<  query.m_max[0] << " || " <<  polygonY << " < " <<  query.m_min[1] <<  " || " << polygonY << " > " <<  query.m_max[1] << " ----- > "; 
    if(polygonX < query.m_min[0] || polygonX > query.m_max[0] || polygonY < query.m_min[1] || polygonY > query.m_max[1])
    {
        // cout << "FALSE\n";
        return false;
    }
    //  cout << "TRUE\n";
    return true;
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

    std::ifstream query_file(query_file_name.c_str());

    Rect *queries = (Rect *)malloc(query_size*sizeof(Rect));

    query_file.clear();
    query_file.seekg(0, ios::beg);

    for(int i = 0; i < query_size; i++){
        queries[i] = Rect();
        for(int j = 0; j < NUMDIMS; j++) query_file >> queries[i].m_min[j];
        for(int j = 0; j < NUMDIMS; j++) query_file >> queries[i].m_max[j];
    }

    std::ifstream data_file(data_file_name.c_str());
    for(int i = 0; i < DATA_COUNT; i++){
        m_data_arr_rects[i] = Rect();
        for(int j = 0; j < NUMDIMS; j++) data_file >> m_data_arr_rects[i].m_min[j];
        for(int j = 0; j < NUMDIMS; j++) data_file >> m_data_arr_rects[i].m_max[j];
    }  

    ofstream times_file;
    times_file.open(time_file_name.c_str());

    size_t *indexSize = (size_t *)malloc(totalPartitions * sizeof(size_t));
    Rect **cellData = (Rect**)malloc(totalPartitions*sizeof(Rect*));

    startPartitionTime = clock();
    partition2DGrid2(partitionsPerDimension, cellData, indexSize);
    // int totalSize = 0;
    // for(int i = 0; i < totalPartitions;i++)
    // {
    //     cout << "Partition\t" << i << "\t" << indexSize[i] << "\n";
    //     totalSize += indexSize[i];
    // }

    // cout << "Total size " << totalSize << endl;
    endPartitionTime = clock() - startPartitionTime;

    int bottomLeft, topLeft, bottomRight, topRight;
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

        // cout << " Query " << queries[i].m_min[0] << " " << queries[i].m_min[1] << " " << queries[i].m_max[0] << " " << queries[i].m_max[1] << endl;
        for(int xIndex = xStartCell; xIndex <= xEndCell; xIndex++)
        {
            for(int yIndex = yStartCell; yIndex <= yEndCell; yIndex++)
            {
                thisCellId = getCellId(xIndex, yIndex, partitionsPerDimension);
                
                if(xIndex == xStartCell || xIndex == xEndCell || yIndex == yStartCell || yIndex == yEndCell)
                {
                    for(int ii = 0; ii < indexSize[thisCellId]; ii++)
                    {
                        // cout << "----------------------- " << cellData[thisCellId][ii].m_min[0] << endl;
                        // if(cellData[thisCellId][ii*NUMDIMS] < queries[i].m_min[0] || cellData[thisCellId][ii*NUMDIMS] > queries[i].m_max[0] || cellData[thisCellId][ii*NUMDIMS+1] < queries[i].m_min[1] || cellData[thisCellId][ii*NUMDIMS+1] > queries[i].m_max[1])
                        // if(pointInPolygon(cellData[thisCellId][ii].m_min[0], cellData[thisCellId][ii].m_min[1], queries[i]) || pointInPolygon(cellData[thisCellId][ii].m_min[0], cellData[thisCellId][ii].m_max[1], queries[i]) || pointInPolygon(cellData[thisCellId][ii].m_max[0], cellData[thisCellId][ii].m_min[1], queries[i]) || pointInPolygon(cellData[thisCellId][ii].m_max[0], cellData[thisCellId][ii].m_max[1], queries[i]))
                        if(Overlap(&(queries)[i], &(cellData[thisCellId][ii])))
                        {
                            auto x = max(cellData[thisCellId][ii].m_min[0], queries[i].m_min[0]);
                            auto y = min(cellData[thisCellId][ii].m_max[1], queries[i].m_max[1]);
                            auto pid_ref = findReferenceCell(x, y, partitionExtent, partitionsPerDimension);
                            if(pid_ref == thisCellId) 
                            {
                                thisQueryResult++;
                            }
                        }
                    }
                        // if(cellData[thisCellId][ii*NUMDIMS] >= queries[i].m_min[0] && cellData[thisCellId][ii*NUMDIMS] <= queries[i].m_max[0] && cellData[thisCellId][ii*NUMDIMS+1] >= queries[i].m_min[1] && cellData[thisCellId][ii*NUMDIMS+1] <= queries[i].m_max[1])
                }
                else
                {
                    for(int ii = 0; ii < indexSize[thisCellId]; ii++)
                    {                    
                        auto x = max(cellData[thisCellId][ii].m_min[0], queries[i].m_min[0]);
                        auto y = min(cellData[thisCellId][ii].m_max[1], queries[i].m_max[1]);
                        auto pid_ref = findReferenceCell(x, y, partitionExtent, partitionsPerDimension);
                        if(pid_ref == thisCellId) 
                        {
                            thisQueryResult++;
                        }
                    }
                    // thisQueryResult += indexSize[thisCellId];
                }
            }
        }
        endTime = clock() - beginTime;
        times_file << thisQueryResult << "\t" << (double)endTime/CLOCKS_PER_SEC << "\t" << (double)endPartitionTime/CLOCKS_PER_SEC << endl;
        // cout << "Results = " << thisQueryResult << " " << tmp_Res << endl;
        #ifdef linearcheck
            linCount = 0;
            for(int ii =0; ii < DATA_COUNT; ii++){
                 if(Overlap(&(queries[i]), &(m_data_arr_rects[ii]))){
                    linCount++;
                }
            }
            //  cout << "Linear check " << thisQueryResult << " " << linCount << endl;
            // if(linCount != thisQueryResult){
            if(abs(linCount - thisQueryResult) > 1){
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



    // cout << "Partitioning time " << (double)endPartitionTime/CLOCKS_PER_SEC << endl;
    return 0;
}