// #include "basics.h"

// #ifndef AKD_H_
// #define AKD_H_

// #include "RTile.h"

struct KDNode{
    KDNode(){
        queryIDs = new vector<int>();
        last_id++;    
        id = last_id;
    }

    bool operator==(const KDNode& a) const{
        return a.id == id;
    }

    int axis; // which axis is being split
    int level; // lower is higher, the root is 0
    ELEMTYPE pivotValue = -1; // if it is leaf, then this is meaningless
    bool leafness = true; // is leaf or not
    int dataArrayLeftPointer; // if leaf, this points to the start of the ocntinguous range of data in this leaf, [), inlcuding the left item
    int dataArrayRightPointer; //if leaf, this points to the end of the ocntinguous range of data in this leaf, [), not including the right item
    KDNode* leftChild = NULL; // if inner node, pointer to left child, if leaf, NULL
    KDNode* rightChild = NULL; // if inner node, pointer to right child, if leaf NULL

    // BAD CODE
    // DON'T KNOW HOW TO FIX FOR NOW
    // TODO
    vector<int> *queryIDs; // this is for the initial partitioning. We keep track of the queries that fall overlap this leaf so we can check their predicates and count their results when we are batch partitioning.

    // MORE BAD CODE
    int id;
};


class KDNodeHashFunction{
    public:
        size_t operator()(const KDNode *a) const{
            return a->id;
        }
};

class AKDTree{
    public:
        Rect* dataArr;
        // constructors
        AKDTree(){}
        AKDTree(vector<pair<int, ELEMTYPE>> cuts);
        AKDTree(ELEMTYPE *dataMins, ELEMTYPE *dataMaxes, int size);
        AKDTree(ELEMTYPE *data, int start, int end);
        AKDTree(Rect* data, int size);



        // methods
        void AdaptiveBatchQuery(vector<Rect> *thisBatchQueries, ELEMTYPE* minValue, ELEMTYPE *maxValue, vector<int> *queryResults);
        int AdaptiveQuery(Rect *extendedQuery, Rect* originalQuery, float partitionExtent, int partitionsPerDimensio, int thisCellId);
        int AdaptiveQueryDuplicates(Rect *extendedQuery, Rect* originalQuery, float partitionExtent, int partitionsPerDimensio, int thisCellId);
        int NonAdaptiveQuery(ELEMTYPE *queryMins, ELEMTYPE *queryMaxes);
        void SplitNodeInPlace(KDNode *node, ELEMTYPE pivot, int pivotAxis);
        KDNode* getRoot(){return &m_root;}
        void printTree(KDNode *node);
        int countNodes();
        int countLeaves();
        void printLeafSizesToFile(string file_name);
        void cerrLeafSizesToFile();
        int deepestLeaf();

    private:
        // attributes
        KDNode m_root;
        
        // vector<int> *ids;

        // methods
        void cut(KDNode* a_node, ELEMTYPE pivotValue);
        int partition2D(int start, int end, int axis, ELEMTYPE pivot);

        void AdaptiveQuery(KDNode* a_node, Rect *query, int &foundCount, vector<Rect> &candidates);
        
        void countNodes(KDNode *thisNode, int &count);
        void countLeaves(KDNode *thisNode, int &count);
        void printLeafSizesToFile(KDNode *thisNode, ofstream &file);
        void cerrLeafSizesToFile(KDNode *thisNode);
        void deepestLeaf(KDNode *thisNode, int &count);
};

// AKDTree::AKDTree(ELEMTYPE *dataMins, ELEMTYPE *dataMaxes, int size){
//     m_root = KDNode();

//     m_root.axis = 0;
//     m_root.level = 0;
//     m_root.leafness = true;
//     m_root.dataArrayLeftPointer = 0;
//     m_root.dataArrayRightPointer = size;
//     m_root.leftChild = NULL;
//     m_root.rightChild = NULL;

//     dataArrMins = dataMins;
//     dataArrMaxes = dataMaxes;
//     // ids = dataIds;

//     last_id = 0;
//     m_root.id = last_id;
// }

AKDTree::AKDTree(Rect* data, int size){
    m_root = KDNode();

    m_root.axis = 0;
    m_root.level = 0;
    m_root.leafness = true;
    m_root.dataArrayLeftPointer = 0;
    m_root.dataArrayRightPointer = size;
    m_root.leftChild = NULL;
    m_root.rightChild = NULL;

    dataArr = data;
    // ids = dataIds;
    // for(int i = 0; i < size; i++)
    // {
    //     // dataArr[i] = data[i];
    //     cout << data->m_min[0] << " " << data->m_min[1] << endl;
    // }
    // cout << "Data is " << data[0].m_min[0] << " and arr is " << dataArr[0].m_min[0] << endl;

    last_id = 0;
    m_root.id = last_id;
}

// AKDTree::AKDTree(ELEMTYPE *data, int start, int end){
//     m_root = KDNode();

//     m_root.axis = 0;
//     m_root.level = 0;
//     m_root.leafness = true;
//     m_root.dataArrayLeftPointer = start;
//     m_root.dataArrayRightPointer = end;
//     m_root.leftChild = NULL;
//     m_root.rightChild = NULL;

//     dataArr = data;
//     // ids = dataIds;

//     last_id = 0;
//     m_root.id = last_id;
// }

void AKDTree::SplitNodeInPlace(KDNode* thisNode, ELEMTYPE pivot, int pivotAxis)
{
    #ifdef TIMEBREAKDOWN
        numOfSplits++;
    #endif
    if(thisNode == NULL){
        cout << "thisNode iS null!!!!" << endl;
    }
    // cout << "in split ---------------------- " << thisNode->dataArrayLeftPointer << " " << thisNode->dataArrayRightPointer << endl;
    if(!thisNode->leafness){
        cout << "this node in split was not leaf, panic" << endl;
        exit(2);
    }
    // cutting on the boundsSet[axis][ans] on axis
    // partition the data in Left and Right of the array
    // partition(m_data_arr.begin() + thisNode->dataArrayLeftPointer, m_data_arr.begin() + thisNode->dataArrayRightPointer, [pivot,thisNode](const auto& em){return em[thisNode->axis] < pivot;}); //  // cout << em << endl;
    // int crackIndex = partition2D(thisNode->dataArrayLeftPointer, thisNode->dataArrayRightPointer, thisNode->axis, pivot);
    #ifdef TIMEBREAKDOWN
        auto startPartitionTime = clock();
        cerr << "Level is " << thisNode->level << " swaps ";
    #endif
    int crackIndex = partition2D(thisNode->dataArrayLeftPointer, thisNode->dataArrayRightPointer, pivotAxis, pivot);
    #ifdef TIMEBREAKDOWN
        partitionTime += clock() - startPartitionTime;
        traversals += thisNode->dataArrayRightPointer - thisNode->dataArrayLeftPointer;
    #endif

    if(crackIndex == thisNode->dataArrayLeftPointer || crackIndex == thisNode->dataArrayRightPointer){
        #ifdef TIMEBREAKDOWN
            failedCracks++;
        #endif
        return;
    }


    // chnage the axis to the one it was cracked on
    thisNode->axis = pivotAxis;

    thisNode->leftChild = new KDNode;
    thisNode->rightChild = new KDNode;


    thisNode->pivotValue = pivot;
    thisNode->leafness = false;

    // cout << "cutting " << thisNode->axis << " on pivot  " << pivot << endl;

    // cout << "DATA:::::::::::::::::::" << endl;
    // for(int i = 0; i < DATA_COUNT; ++i) cout << m_data_arr[i][0] << " " << m_data_arr[i][1] << "\n";
    // cout << "\nDATA:::::::::::::::::::" << endl;

    // and put the crackIndex to the children
    (thisNode->leftChild)->dataArrayLeftPointer = thisNode->dataArrayLeftPointer;
    (thisNode->leftChild)->dataArrayRightPointer = crackIndex;

    (thisNode->rightChild)->dataArrayLeftPointer = crackIndex;
    (thisNode->rightChild)->dataArrayRightPointer = thisNode->dataArrayRightPointer;

    (thisNode->leftChild)->level = thisNode->level + 1;
    (thisNode->rightChild)->level = thisNode->level + 1;

    // maybe they don't need initialising at all TODO
    (thisNode->leftChild)->axis = (thisNode->axis + 1) % NUMDIMS;
    (thisNode->rightChild)->axis = (thisNode->axis + 1) % NUMDIMS;

    (thisNode->leftChild)->leafness = true;
    (thisNode->rightChild)->leafness = true;
    // then pass thisNode.leftChild and rightChild to each of the recursions
    // a few asserrts: make sure thisNode is leaf -> then make it inner node
    // assert axis of thisNode and axis in function match
    // fix axis of children
    // fix vars for thisNode
    // set uo vars for hte children


}

void swapRects(Rect *rectA, Rect *rectB)
{
    Rect tmp;
    // tmp = rectA;
    for(int j = 0; j < NUMDIMS; ++j)
    {
        tmp.m_min[j] = rectA->m_min[j];
        tmp.m_max[j] = rectA->m_max[j];

        rectA->m_min[j] = rectB->m_min[j];
        rectA->m_max[j] = rectB->m_max[j];

        rectB->m_min[j] = tmp.m_min[j];
        rectB->m_max[j] = tmp.m_max[j];
    }
}

int AKDTree::partition2D(int start, int end, int axis, ELEMTYPE pivot){


    #ifdef TIMEBREAKDOWN
        int localSwaps = 0;
    #endif
    int i = start, j = end - 1;
    // cout << "in partition " << i << " " << j << endl;
    while(i <= j && j > 0){
        // cout << "Is " << m_data_arr[i][axis] << " <= " << pivot << endl;
        if(dataArr[i].m_min[axis] < pivot){
            i++;
        }
        else{
            // cout << "Is " << m_data_arr[i][axis] << " > " << pivot << endl;
            while(j > 0 && j >= i && dataArr[j].m_min[axis] >= pivot){      // TODO can remove j > 0, i think
                j--;
            }
            // cout << "I,J " << i << " " << j << endl; 
            if(i < j){
                // cout << "swapping" << endl;
                #ifdef TIMEBREAKDOWN
                    clock_t startTime = clock();
                #endif
                // cout << "SWAP!!!!\n";
                std::swap(dataArr[i], dataArr[j]);
                std::swap(m_data_ids[i], m_data_ids[j]);
                #ifdef TIMEBREAKDOWN
                    swapTime += clock() - startTime;
                #endif
                #ifdef TIMEBREAKDOWN
                    swaps++;
                    localSwaps++;
                #endif
                j--; i++;
            }
        }
    }
    #ifdef TIMEBREAKDOWN
        cerr << localSwaps << " for " << end-start << " data objects" << endl;
    #endif

    // cout << "Return " << i << endl;
    // if (i > 0) i++;
    return i;

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

int AKDTree::AdaptiveQuery(Rect *extendedQuery, Rect* originalQuery, float partitionExtent, int partitionsPerDimension, int thisCellId){
    int foundCount = 0;

    // first traverse the tree to find the leaves that hold the data in them
    vector<Rect> candidates;
    AdaptiveQuery(&m_root, extendedQuery, foundCount, candidates);
    int realResults = 0;
    for(size_t i  = 0; i < candidates.size(); ++i)
    {
        // cout << "Checking " << m_data_arr_rects[i].m_max[0] <<  " >= " << originalQuery->m_min[0] << endl;
        // cout << "Checking " << m_data_arr_rects[i].m_min[0] <<  " <= " << originalQuery->m_max[0] << endl;
        // cout << "Checking " << m_data_arr_rects[i].m_max[1] <<  " <= " << originalQuery->m_min[1] << endl;
        // cout << "Checking " << m_data_arr_rects[i].m_min[1] <<  " <= " << originalQuery->m_max[1] << endl;
 
        // cout << m_data_arr_rects[candidate_ids[i]].m_min[0] << " " << m_data_arr_rects[candidate_ids[i]].m_min[1] << " " << m_data_arr_rects[candidate_ids[i]].m_max[0] << " " << m_data_arr_rects[candidate_ids[i]].m_max[1] << endl;
        // cout << candidates[i].m_min[0] << " " << candidates[i].m_min[1] << " " << candidates[i].m_max[0] << " " << candidates[i].m_max[1] << endl;
        // cout << originalQuery->m_min[0] << " " << originalQuery->m_min[1] << " " << originalQuery->m_max[0] << " " << originalQuery->m_max[1] << endl;

        // TODO--
        // if(m_data_arr_maxes[i][0] >= originalQueryMins[0] && m_data_arr_mins[i][0] <= originalQueryMaxes[0] && m_data_arr_maxes[i][1] >= originalQueryMins[1] && m_data_arr_mins[i][1] <= originalQueryMaxes[1])
        // if(candidates[i].m_max[0] >= originalQuery->m_min[0] && candidates[i].m_min[0] <= originalQuery->m_max[0] && candidates[i].m_max[1] >= originalQuery->m_min[1] && candidates[i].m_min[1] <= originalQuery->m_max[1])
        if(Overlap(originalQuery, &(candidates[i])))
        {
            // cout << "Candidate " << candidates[i].m_min[0] << " " << candidates[i].m_min[1] << " " << candidates[i].m_max[0] << " " << candidates[i].m_max[1] << " overlaps original query ";
            // auto x = max(candidates[i].m_min[0], originalQuery->m_min[0]);
            // auto y = min(candidates[i].m_max[1], originalQuery->m_max[1]);
            // cout << x << ", " << y << ", " << partitionExtent << ", " << partitionsPerDimension << endl;
            // auto pid_ref = findReferenceCell(x, y, partitionExtent, partitionsPerDimension);
            // if(pid_ref == thisCellId) 
            // {
                //  cout << "Candidate " << candidates[i].m_min[0] << " " << candidates[i].m_min[1] << " " << candidates[i].m_max[0] << " " << candidates[i].m_max[1] << " is a result \n";
                realResults++;
            // }
            // else cout << " but is not reference point " << pid_ref << " " << thisCellId << endl;
        }
        // cout << "\n";
    }
    // cout << "Size " << candidates.size() << " Found " << foundCount << " Actual " << realResults << endl;
    return realResults;
}

int AKDTree::AdaptiveQueryDuplicates(Rect *extendedQuery, Rect* originalQuery, float partitionExtent, int partitionsPerDimension, int thisCellId){
    int foundCount = 0;

    // first traverse the tree to find the leaves that hold the data in them
    vector<Rect> candidates;
    AdaptiveQuery(&m_root, extendedQuery, foundCount, candidates);
    int realResults = 0;
    for(size_t i  = 0; i < candidates.size(); ++i)
    {
        // cout << "Checking " << m_data_arr_rects[i].m_max[0] <<  " >= " << originalQuery->m_min[0] << endl;
        // cout << "Checking " << m_data_arr_rects[i].m_min[0] <<  " <= " << originalQuery->m_max[0] << endl;
        // cout << "Checking " << m_data_arr_rects[i].m_max[1] <<  " <= " << originalQuery->m_min[1] << endl;
        // cout << "Checking " << m_data_arr_rects[i].m_min[1] <<  " <= " << originalQuery->m_max[1] << endl;
 
        // cout << m_data_arr_rects[candidate_ids[i]].m_min[0] << " " << m_data_arr_rects[candidate_ids[i]].m_min[1] << " " << m_data_arr_rects[candidate_ids[i]].m_max[0] << " " << m_data_arr_rects[candidate_ids[i]].m_max[1] << endl;
        // cout << candidates[i].m_min[0] << " " << candidates[i].m_min[1] << " " << candidates[i].m_max[0] << " " << candidates[i].m_max[1] << endl;
        // cout << originalQuery->m_min[0] << " " << originalQuery->m_min[1] << " " << originalQuery->m_max[0] << " " << originalQuery->m_max[1] << endl;

        // TODO--
        // if(m_data_arr_maxes[i][0] >= originalQueryMins[0] && m_data_arr_mins[i][0] <= originalQueryMaxes[0] && m_data_arr_maxes[i][1] >= originalQueryMins[1] && m_data_arr_mins[i][1] <= originalQueryMaxes[1])
        // if(candidates[i].m_max[0] >= originalQuery->m_min[0] && candidates[i].m_min[0] <= originalQuery->m_max[0] && candidates[i].m_max[1] >= originalQuery->m_min[1] && candidates[i].m_min[1] <= originalQuery->m_max[1])
        if(Overlap(originalQuery, &(candidates[i])))
        {
            // cout << "Candidate " << candidates[i].m_min[0] << " " << candidates[i].m_min[1] << " " << candidates[i].m_max[0] << " " << candidates[i].m_max[1] << " overlaps original query ";
            auto x = max(candidates[i].m_min[0], originalQuery->m_min[0]);
            auto y = min(candidates[i].m_max[1], originalQuery->m_max[1]);
            // cout << x << ", " << y << ", " << partitionExtent << ", " << partitionsPerDimension << endl;
            auto pid_ref = findReferenceCell(x, y, partitionExtent, partitionsPerDimension);
            if(pid_ref == thisCellId) 
            {
                //  cout << "Candidate " << candidates[i].m_min[0] << " " << candidates[i].m_min[1] << " " << candidates[i].m_max[0] << " " << candidates[i].m_max[1] << " is a result \n";
                realResults++;
            }
            // else cout << " but is not reference point " << pid_ref << " " << thisCellId << endl;
        }
        // cout << "\n";
    }
    // cout << "Size " << candidates.size() << " Found " << foundCount << " Actual " << realResults << endl;
    return realResults;
}

void AKDTree::AdaptiveQuery(KDNode* a_node, Rect *query, int &foundCount, vector<Rect> &candidates){
    // cout << "IN ADAPTIVE\n";
    if(a_node->leafness)
    {
        // cout << "IS LEAF \n";
        // is a leaf
        // should crack on the bounds
        // also add to the foundcount
        bool stopCracking=false;

        
        if(a_node->dataArrayRightPointer - a_node->dataArrayLeftPointer > CRACKTHRESHOLD){
            // then for each of them crack them 2D times

            #ifdef TIMEBREAKDOWN
                clock_t start = clock();
            #endif

            KDNode* thisNode = a_node;
            // cout << "Cracking on min " << query->m_min[0] << " " << query->m_min[1] << endl;
            for(int i = 0; i < NUMDIMS; ++i){
                // perform crack on min values
                SplitNodeInPlace(thisNode, query->m_min[i], i);
                // if(thisNode->leafness){
                //     // did not crack so we should stop
                //     stopCracking=true;
                //     break;
                // }
                if(!thisNode->leafness) thisNode = thisNode->rightChild;
                if(thisNode->dataArrayRightPointer - thisNode->dataArrayLeftPointer <= CRACKTHRESHOLD) break;
            }
            if(!stopCracking && thisNode->dataArrayRightPointer - thisNode->dataArrayLeftPointer > CRACKTHRESHOLD){
                // cout << "Cracking on max " << query->m_max[0] << " " << query->m_max[1] << endl;
                for(int i = 0; i < NUMDIMS; ++i){
                    // perform crack on max values
                    SplitNodeInPlace(thisNode, query->m_max[i], i);
                    // if(thisNode->leafness){
                    // // did not crack so we should stop
                    //     stopCracking=true;
                    //     break;
                    // }
                    if(!thisNode->leafness) thisNode = thisNode->leftChild;
                    if(thisNode->dataArrayRightPointer - thisNode->dataArrayLeftPointer <= CRACKTHRESHOLD) break;
                }
            }
            #ifdef TIMEBREAKDOWN
                adaptiveSplitTime += clock() - start;
            #endif

            #ifdef TIMEBREAKDOWN
                start = clock();
            #endif
            // foundCount += (thisNode->dataArrayRightPointer - thisNode->dataArrayLeftPointer);
            // cout << "Pointers 1 " << thisNode->dataArrayLeftPointer << " " << thisNode->dataArrayRightPointer << endl;
            for(int ii = thisNode->dataArrayLeftPointer; ii < thisNode->dataArrayRightPointer; ++ii){
                // if(Overlap(queryMins, queryMaxes, &dataArr[ii*NUMDIMS])){
                // if(Overlap(queryMins, queryMaxes, dataArrMins[ii*NUMDIMS], dataArrMaxes[ii*NUMDIMS+1])){
                 if(Overlap(query, &(dataArr)[ii]))
                 {
                    // cout << dataArr[ii*NUMDIMS] << " " << dataArr[ii*NUMDIMS+1] << " result!\n";
                    // cout << dataArr[ii].m_min[0] << " is a result " << endl;
                    // Rect rec;
                    // rec.m_min = dataArrMins[ii];
                    // rec.m_max = dataArrMins[ii*+1]
                    candidates.push_back(dataArr[ii]);
                    foundCount++;
                }
            }
            #ifdef TIMEBREAKDOWN
                adaptiveCountingResultsTime += clock() - start;
            #endif

             #ifdef STOCHASTIC

                if(thisNode->dataArrayRightPointer - thisNode->dataArrayLeftPointer > CRACKTHRESHOLD )
                {
                    vector<int> rndValues;
                    rndValues.push_back(thisNode->dataArrayLeftPointer + ( rand() % (thisNode->dataArrayRightPointer - thisNode->dataArrayLeftPointer + 1 )));
                    rndValues.push_back(thisNode->dataArrayLeftPointer + ( rand() % (thisNode->dataArrayRightPointer - thisNode->dataArrayLeftPointer + 1 )));
                    rndValues.push_back(thisNode->dataArrayLeftPointer + ( rand() % (thisNode->dataArrayRightPointer - thisNode->dataArrayLeftPointer + 1 )));

                    // float m = m_data_arr[rndValues[0]][0];
                    // float median = max(min(m_data_arr[rndValues[0]][0],m_data_arr[rndValues[1]][0]), min(max(m_data_arr[rndValues[0]][0],m_data_arr[rndValues[1]][0]),m_data_arr[rndValues[2]][0]));
                    float median = max(min(dataArr[rndValues[0]].m_min[0],dataArr[rndValues[1]].m_min[0]), min(max(dataArr[rndValues[0]].m_min[0],dataArr[rndValues[1]].m_min[0]),dataArr[rndValues[2]].m_min[0]));
                    // cout << "Median is " << median << endl;
                    SplitNodeInPlace(thisNode, median, 0);
                }
            #endif
            
        }
        else{
            #ifdef TIMEBREAKDOWN
                clock_t start = clock();
            #endif
            // cout << "Pointers 2 " << a_node->dataArrayLeftPointer << " " << a_node->dataArrayRightPointer << endl;
            for(int ii = a_node->dataArrayLeftPointer; ii < a_node->dataArrayRightPointer; ++ii){
            // cout << "Checking 2 " << dataArr[ii*NUMDIMS] << " " << dataArr[ii*NUMDIMS+1] << endl;
                // if(Overlap(queryMins, queryMaxes, m_data_arr[ii])){
                 if(Overlap(query, &(dataArr)[ii]))
                 {
                    // cout << dataArr[ii].m_min[0] << " is a result " << endl;
                    // cout << dataArr[ii*NUMDIMS] << " " << dataArr[ii*NUMDIMS+1] << " result!\n";
                    // Rect rec;
                    // rec.m_min = dataArrMins[ii];
                    // rec.m_max = dataArrMins[ii+1]
                    // candidates.push_back(rec));
                    candidates.push_back(dataArr[ii]);
                    foundCount++;
                }
            }
            #ifdef TIMEBREAKDOWN
                adaptiveCountingResultsTime += clock() - start;
            #endif

        }
        
    }
    else{
        // is internal
        // should recurse to children that overlap
        // cout << "IS INTERNAL"  << endl;
        if(query->m_min[a_node->axis] < a_node->pivotValue){
            AdaptiveQuery(a_node->leftChild, query, foundCount, candidates);
        }
        if(query->m_max[a_node->axis] >= a_node->pivotValue){
            AdaptiveQuery(a_node->rightChild, query, foundCount, candidates);
        }
    }
}

void AKDTree::printTree(KDNode *node){
    if( node != NULL)
    {
        if(node->leafness){
            cout << node->level << " " << node->axis << " leaf" << endl;
        }
        else{
            cout << node->level << " " << node->axis << " " << node->pivotValue << endl;
        }
        printTree(node->leftChild);
        printTree(node->rightChild);
    }
}

int AKDTree::countNodes(){
    int count = 0;
    countNodes(&m_root, count);
    return count;
}

void AKDTree::countNodes(KDNode* thisNode, int &count){
    count++;
    if(!thisNode->leafness){
        countNodes(thisNode->leftChild, count);
        countNodes(thisNode->rightChild, count);
    }
}

int AKDTree::countLeaves(){
    int count = 0;
    countLeaves(&m_root, count);
    return count;
}

void AKDTree::countLeaves(KDNode* thisNode, int &count){
    if(!thisNode->leafness){
        countLeaves(thisNode->leftChild, count);
        countLeaves(thisNode->rightChild, count);
    }
    else{
        count++;
    }
}

void AKDTree::cerrLeafSizesToFile(){
    cerrLeafSizesToFile(&m_root);
}

void AKDTree::cerrLeafSizesToFile(KDNode* thisNode){
    
    if(!thisNode->leafness){
        cerrLeafSizesToFile(thisNode->leftChild);
        cerrLeafSizesToFile(thisNode->rightChild);
    }
    else{
        cerr << thisNode->dataArrayRightPointer - thisNode->dataArrayLeftPointer << " " << thisNode->level << endl;
    }
}

int AKDTree::deepestLeaf(){
    int level = 0;
    deepestLeaf(&m_root, level);
    return level;
}

void AKDTree::deepestLeaf(KDNode* thisNode, int &level){
    if(!thisNode->leafness){
        deepestLeaf(thisNode->leftChild, level);
        deepestLeaf(thisNode->rightChild, level);
    }
    else{
        if(level < thisNode->level) level = thisNode->level;
    }
}
