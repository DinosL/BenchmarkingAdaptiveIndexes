// #include "basics.h"
#include "RTree_723oct21s_stochastic_14dec22.h"

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


int findReferenceCell(double x, double y, double cellExtent, int numCellsPerDimension) {
    int xInt,yInt;

    xInt = (x + EPS)/cellExtent;
    yInt = (y + EPS)/cellExtent;
    
    if (numCellsPerDimension == xInt){
        xInt = numCellsPerDimension -1;
    }

    if (numCellsPerDimension == yInt){
        yInt = numCellsPerDimension -1;
    }

    return (yInt * numCellsPerDimension + xInt);
}



const int fo = 16;
const int maxt = 512;
const int mint = 256;

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
    // partition2DGrid(partitionsPerDimension, cellDataMins, cellDataMaxes, indexSize);
    partition2DGrid(partitionsPerDimension, cellData, indexSize);
    // partition2DGridOneArray(partitionsPerDimension, cellData, indexSize);
    
    // int partitionPointer[totalPartitions];
    // AKDTree *cellIndexes = (AKDTree*)malloc(totalPartitions*sizeof(AKDTree));
    // partitionPointer[0] = 0;
    // cellIndexes[0] = AKDTree(cellData, 0, indexSize[0]);
    // for (int i = 1; i < totalPartitions; i++){
    //     partitionPointer[i] = indexSize[i-1] + partitionPointer[i-1];
    //     cellIndexes[i] = AKDTree(cellData, partitionPointer[i], partitionPointer[i]+indexSize[i]);
    // }


    typedef RTree<ValueType, fo, fo/2, maxt, mint> MyTree;
    MyTree *cellIndexes = (MyTree*)malloc(totalPartitions*sizeof(MyTree));
    // vector<MyTree> cellIndexes;
    for(int i = 0; i < totalPartitions;i++)
    {
        // MyTree tree(cellData[i], indexSize[i]);
        RTree<ValueType, fo, fo/2, maxt, mint>  *tree;
        tree = new MyTree(cellData[i], indexSize[i]);
        cellIndexes[i] = *tree;
        // cellIndexes.push_back(*tree);
        // cout << "Partition " << i << " has size " << indexSize[i] << endl;
    }
    endPartitionTime = clock() - startPartitionTime;

    // for(int i = 0; i < totalPartitions;i++)
    // {
    //     cout << "Cell " << i << " size " << cellIndexes[i].mySize << endl;
    // }
    // cout << "Partitioning time " << (double)endPartitionTime/CLOCKS_PER_SEC << endl;
    // cellIndexes[0] = AKDTree(cellData, 0, indexSize[0]);
    // for(int i = 0; i < totalPartitions;i++)
    // {
    //     cellIndexes[i] = AKDTree(cellData[i], indexSize[i]);
       
    // }
    

    int bottomLeft, topLeft, bottomRight, topRight;
    for(int i = 0; i < query_size; i++)
    {

        // cout << "--------------------------------------- QUERY " << i << endl;
        // if(queries[i].m_max[0] > 1.0)
        //     queries[i].m_max[0] = 1.0;
        // if(queries[i].m_max[1] > 1.0)
        //     queries[i].m_max[1] = 1.0;

        beginTime = clock();
        
        thisQueryResult = 0;

        // original_queries[i].m_min[0] = 0.903047;
        // original_queries[i].m_min[1] = 0.127021;
        // original_queries[i].m_max[0] = 0.912936;
        // original_queries[i].m_max[1] = 0.136907;

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
                        thisQueryResult += cellIndexes[thisCellId].QueryAdaptive_v13(&(extended_queries)[i], &(original_queries)[i]);
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
                        if(Overlap(&(original_queries)[i], &(cellData[thisCellId][ii])))
                        {
                            thisQueryResult++;
                        }
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

            // cout << "LINEAR " << linCount << endl;
            if(linCount != thisQueryResult){
            // if(abs(linCount - thisQueryResult) > 1){
                cout << "WRONG ANSWER, TRAGEDY, DISASTER, ALL THAT..." << thisQueryResult << " " << linCount << endl;
                cout << "Query " << i << endl;
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