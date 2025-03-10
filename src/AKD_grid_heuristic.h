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
        // constructors
        AKDTree(){}
        AKDTree(vector<pair<int, ELEMTYPE>> cuts);
        AKDTree(ELEMTYPE *data, int size);
        AKDTree(ELEMTYPE *data, int start, int end);


        // methods
        void AdaptiveBatchQuery(vector<Rect> *thisBatchQueries, ELEMTYPE* minValue, ELEMTYPE *maxValue, vector<int> *queryResults);
        int AdaptiveQuery(ELEMTYPE *queryMins, ELEMTYPE *queryMaxes, Rect cover);
        int NonAdaptiveQuery(ELEMTYPE *queryMins, ELEMTYPE *queryMaxes);
        bool SplitNodeInPlace(KDNode *node, ELEMTYPE pivot, int pivotAxis);
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
        ELEMTYPE *dataArr;
        // vector<int> *ids;


        // methods
        void cut(KDNode* a_node, ELEMTYPE pivotValue);
        int partition2D(int start, int end, int axis, ELEMTYPE pivot);

        void AdaptiveQuery(KDNode* a_node, ELEMTYPE *queryMins, ELEMTYPE *queryMaxes, int &foundCount, Rect cover);
        int ChooseAxisLargestSideMid(Rect node_cover, Rect query_cover);
        
        void countNodes(KDNode *thisNode, int &count);
        void countLeaves(KDNode *thisNode, int &count);
        void printLeafSizesToFile(KDNode *thisNode, ofstream &file);
        void cerrLeafSizesToFile(KDNode *thisNode);
        void deepestLeaf(KDNode *thisNode, int &count);
};

AKDTree::AKDTree(ELEMTYPE *data, int size){
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

    last_id = 0;
    m_root.id = last_id;
}

AKDTree::AKDTree(ELEMTYPE *data, int start, int end){
    m_root = KDNode();

    m_root.axis = 0;
    m_root.level = 0;
    m_root.leafness = true;
    m_root.dataArrayLeftPointer = start;
    m_root.dataArrayRightPointer = end;
    m_root.leftChild = NULL;
    m_root.rightChild = NULL;

    dataArr = data;
    // ids = dataIds;

    last_id = 0;
    m_root.id = last_id;
}

bool AKDTree::SplitNodeInPlace(KDNode* thisNode, ELEMTYPE pivot, int pivotAxis)
{
    #ifdef TIMEBREAKDOWN
        numOfSplits++;
    #endif
    if(thisNode == NULL){
        cout << "thisNode iS null!!!!" << endl;
    }
    // cout << "in split ----------------------" << endl;
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
        return false;
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

    return true;


}


int AKDTree::partition2D(int start, int end, int axis, ELEMTYPE pivot){


    #ifdef TIMEBREAKDOWN
        int localSwaps = 0;
    #endif
    int i = start, j = end - 1;
    // cout << "in partition " << i << " " << j << endl;
    while(i <= j && j > 0){
        // cout << "Is " << m_data_arr[i][axis] << " <= " << pivot << endl;
        if(dataArr[i*NUMDIMS + axis] < pivot){
            i++;
        }
        else{
            // cout << "Is " << m_data_arr[i][axis] << " > " << pivot << endl;
            while(j > 0 && j >= i && dataArr[j * NUMDIMS + axis] >= pivot){      // TODO can remove j > 0, i think
                j--;
            }
            // cout << "I,J " << i << " " << j << endl; 
            if(i < j){
                // cout << "swapping" << endl;
                #ifdef TIMEBREAKDOWN
                    clock_t startTime = clock();
                #endif
                // cout << "SWAP!!!!\n";
                for(int jj = 0; jj < NUMDIMS; jj++)
                {
                    std::swap(dataArr[i*NUMDIMS+jj], dataArr[j*NUMDIMS+jj]);
                    // std::swap(m_data_ids[i], m_data_ids[j]);
                }
                // std::swap(m_data_ids[i], m_data_ids[j]);
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
        // cerr << localSwaps << " for " << end-start << " data objects" << endl;
    #endif

    // cout << "Return " << i << endl;
    // if (i > 0) i++;
    return i;

}

int AKDTree::AdaptiveQuery(ELEMTYPE *queryMins, ELEMTYPE *queryMaxes, Rect cover){
    int foundCount = 0;

    // first traverse the tree to find the leaves that hold the data in them
    AdaptiveQuery(&m_root, queryMins, queryMaxes, foundCount, cover);
    return foundCount;

    
}


int AKDTree::ChooseAxisLargestSideMid(Rect node_cover, Rect query_cover) {

    float this_ms;
    float current_max_m = std::abs(node_cover.m_max[0] - node_cover.m_min[0]);
    //    cout << "in choose, first " << current_max_m << endl;
    int current_max_index = 0;
    for(int i =1; i < NUMDIMS; i++){
        this_ms = std::abs(node_cover.m_max[i] - node_cover.m_min[i]);
        //        cout << "in choose, next " << this_ms << endl;
        if(this_ms > current_max_m){
            current_max_m = this_ms;
            current_max_index = i;
        }
    }
    //    cout << "in choose, last " << current_max_m << endl;

    // now we have to figure out if q_min[current_max_index] ids closer to mid of node_cover
    float mid_of_node_cover_axis = (float) ((node_cover.m_max[current_max_index] + node_cover.m_min[current_max_index]) / 2);
    //    cout << "mid of node cover axis " << mid_of_node_cover_axis << endl;
    //    Rect query_cover_in_node;
    //    for(int i = 0; i < NUMDIMS; i++){
    //        query_cover_in_node.m_min[i] = std::max(query_cover.m_min[i], node_cover.m_min[i]);
    //        query_cover_in_node.m_max[i] = std::min(query_cover.m_max[i], node_cover.m_max[i]);
    ////        cout << "query cover in node, axis " << i << " min " << query_cover_in_node.m_min[i] << " max " << query_cover_in_node.m_max[i] << endl;
    //    }

    //    if(std::abs(query_cover_in_node.m_min[current_max_index] - mid_of_node_cover_axis) <
    //       std::abs(query_cover_in_node.m_max[current_max_index] - mid_of_node_cover_axis)){
    ////        cout << "chose min\n";
    //        return (2 * current_max_index);
    //    } else{
    ////        cout << "chose max\n";
    //        return (2 * current_max_index + 1);
    //    }
    if(std::abs(query_cover.m_min[current_max_index] - mid_of_node_cover_axis) <
    std::abs(query_cover.m_max[current_max_index] - mid_of_node_cover_axis)){
//                cout << "chose min and axis " << current_max_index << endl;
        return (2 * current_max_index);
    } else{
//                cout << "chose max and axis " << current_max_index << endl;
        return (2 * current_max_index + 1);
    }
}

void AKDTree::AdaptiveQuery(KDNode* a_node, ELEMTYPE *queryMins, ELEMTYPE *queryMaxes, int &foundCount, Rect cover){
    if(a_node->leafness){
        // is a leaf
        // should crack on the bounds
        // also add to the foundcount
        bool stopCracking=false;
        int choice, mm, sum_of_mms = 0, chosen_axis, sum_of_choices = 0;
        

        
        if(a_node->dataArrayRightPointer - a_node->dataArrayLeftPointer > CRACKTHRESHOLD){
            // then for each of them crack them 2D times
            #ifdef STOCHASTIC
                vector<KDNode*> createdLeaves;
            #endif
            KDNode* thisNode = a_node;
            for(int i =0; i < 2*NUMDIMS; i++){

                if(i == ((2 * NUMDIMS) - 1)){
                    choice = (((2*NUMDIMS) - 1) * NUMDIMS) - sum_of_choices;
                }
                else {
                    Rect q = Rect(queryMins, queryMaxes);
                    choice = ChooseAxisLargestSideMid(cover, q);
                }
                if(choice < 0) break;
                if(sum_of_mms >= NUMDIMS) mm = 1;
                else if (sum_of_mms <= (-1*NUMDIMS)) mm = 0;
                else {
                    mm = choice % 2;
                    if(mm == 0) sum_of_mms++;
                    else sum_of_mms--;
                }
                chosen_axis = choice / 2;

                if(chosen_axis >= NUMDIMS || chosen_axis < 0) break;

                sum_of_choices += choice;

                if(mm == 0)
                {
                    SplitNodeInPlace(thisNode, queryMins[chosen_axis], chosen_axis);
                    if(!thisNode->leafness) 
                    {
                        #ifdef STOCHASTIC
                            createdLeaves.push_back(thisNode->leftChild);
                        #endif
                        thisNode = thisNode->rightChild;
                    }
                    if(thisNode->dataArrayRightPointer - thisNode->dataArrayLeftPointer <= CRACKTHRESHOLD) 
                    {
                        #ifdef STOCHASTIC
                            createdLeaves.push_back(thisNode);
                        #endif
                        break;
                    }
                }
                else
                {
                    SplitNodeInPlace(thisNode, queryMaxes[chosen_axis], chosen_axis);
                    if(!thisNode->leafness)
                    {
                        #ifdef STOCHASTIC
                            createdLeaves.push_back(thisNode->rightChild);
                        #endif
                        thisNode = thisNode->leftChild;
                    }
                    if(thisNode->dataArrayRightPointer - thisNode->dataArrayLeftPointer <= CRACKTHRESHOLD) 
                    {
                        #ifdef STOCHASTIC
                            createdLeaves.push_back(thisNode);
                        #endif

                        break;
                    }
                }
            }
            #ifdef STOCHASTIC
                if(createdLeaves.size() > 0)
                {
                    KDNode *nodeToBeCracked = createdLeaves[0];
                    for(int ii = 1; ii < createdLeaves.size(); ++ii)
                    {
                        if(createdLeaves[ii]->dataArrayRightPointer - createdLeaves[ii]->dataArrayLeftPointer > nodeToBeCracked->dataArrayRightPointer - nodeToBeCracked->dataArrayLeftPointer)
                            nodeToBeCracked = createdLeaves[ii];
                    }
                    if(nodeToBeCracked->dataArrayRightPointer - nodeToBeCracked->dataArrayLeftPointer > CRACKTHRESHOLD)
                    {
                        inStoch++;
                        int chosenStochasticAxis = 0;
                        for(int ii = 1; ii < NUMDIMS; ++ii)
                        {
                            if(cover.m_max[ii] - cover.m_min[ii] > cover.m_max[chosenStochasticAxis] - cover.m_min[chosenStochasticAxis])
                                chosenStochasticAxis = ii;
                        }

                        vector<int> rndValues;
                        rndValues.push_back(nodeToBeCracked->dataArrayLeftPointer + ( rand() % (nodeToBeCracked->dataArrayRightPointer - nodeToBeCracked->dataArrayLeftPointer + 1 )));
                        rndValues.push_back(nodeToBeCracked->dataArrayLeftPointer + ( rand() % (nodeToBeCracked->dataArrayRightPointer - nodeToBeCracked->dataArrayLeftPointer + 1 )));
                        rndValues.push_back(nodeToBeCracked->dataArrayLeftPointer + ( rand() % (nodeToBeCracked->dataArrayRightPointer - nodeToBeCracked->dataArrayLeftPointer + 1 )));

                        // float m = m_data_arr[rndValues[0]][0];
                        float median = max(min(dataArr[rndValues[0]*NUMDIMS + chosenStochasticAxis],dataArr[rndValues[1]*NUMDIMS + chosenStochasticAxis]), min(max(dataArr[rndValues[0]*NUMDIMS + chosenStochasticAxis],dataArr[rndValues[1]*NUMDIMS + chosenStochasticAxis]),dataArr[rndValues[2]*NUMDIMS + chosenStochasticAxis]));
                        bool flag = SplitNodeInPlace(nodeToBeCracked, median, chosenStochasticAxis);
                        if(flag)
                        {
                            actuallyCracked++;
                        }
                    }
                }
            #endif

            #ifdef TIMEBREAKDOWN
                clock_t start = clock();
            #endif


            // for(int i = 0; i < NUMDIMS; ++i){
            //     // perform crack on min values
            //     SplitNodeInPlace(thisNode, queryMins[i], i);
            //     // if(thisNode->leafness){
            //     //     // did not crack so we should stop
            //     //     stopCracking=true;
            //     //     break;
            //     // }
            //     if(!thisNode->leafness) thisNode = thisNode->rightChild;
            //     if(thisNode->dataArrayRightPointer - thisNode->dataArrayLeftPointer <= CRACKTHRESHOLD) break;
            // }
            // if(!stopCracking && thisNode->dataArrayRightPointer - thisNode->dataArrayLeftPointer > CRACKTHRESHOLD){
            //     for(int i = 0; i < NUMDIMS; ++i){
            //         // perform crack on max values
            //         SplitNodeInPlace(thisNode, queryMaxes[i], i);
            //         // if(thisNode->leafness){
            //         // // did not crack so we should stop
            //         //     stopCracking=true;
            //         //     break;
            //         // }
            //         if(!thisNode->leafness) thisNode = thisNode->leftChild;
            //         if(thisNode->dataArrayRightPointer - thisNode->dataArrayLeftPointer <= CRACKTHRESHOLD) break;
            //     }
            // }
            #ifdef TIMEBREAKDOWN
                adaptiveSplitTime += clock() - start;
            #endif

            #ifdef TIMEBREAKDOWN
                start = clock();
            #endif
            // foundCount += (thisNode->dataArrayRightPointer - thisNode->dataArrayLeftPointer);
            for(int ii = thisNode->dataArrayLeftPointer; ii < thisNode->dataArrayRightPointer; ++ii){
                // if(Overlap(queryMins, queryMaxes, &dataArr[ii*NUMDIMS])){
                if(Overlap(queryMins, queryMaxes, dataArr[ii*NUMDIMS], dataArr[ii*NUMDIMS+1])){
                    // cout << dataArr[ii*NUMDIMS] << " " << dataArr[ii*NUMDIMS+1] << " result!\n";
                    foundCount++;
                }
            }
            #ifdef TIMEBREAKDOWN
                adaptiveCountingResultsTime += clock() - start;
            #endif

            #ifdef STOCHASTIC
                // cout << "STOCHASTIC\n";
                // if(thisNode->dataArrayRightPointer - thisNode->dataArrayLeftPointer > CRACKTHRESHOLD)
                // {
                //     int chosenStochasticAxis = 0;
                //     for(int ii = 1; ii < NUMDIMS; ++ii)
                //     {
                //         if(cover.m_max[ii] - cover.m_min[ii] > cover.m_max[chosenStochasticAxis] - cover.m_min[chosenStochasticAxis])
                //             chosenStochasticAxis = ii;
                //     }

                //     vector<int> rndValues;
                //     rndValues.push_back(thisNode->dataArrayLeftPointer + ( rand() % (thisNode->dataArrayRightPointer - thisNode->dataArrayLeftPointer + 1 )));
                //     rndValues.push_back(thisNode->dataArrayLeftPointer + ( rand() % (thisNode->dataArrayRightPointer - thisNode->dataArrayLeftPointer + 1 )));
                //     rndValues.push_back(thisNode->dataArrayLeftPointer + ( rand() % (thisNode->dataArrayRightPointer - thisNode->dataArrayLeftPointer + 1 )));

                //     // float m = m_data_arr[rndValues[0]][0];
                //     float median = max(min(dataArr[rndValues[0] * NUMDIMS + chosenStochasticAxis],dataArr[rndValues[1] * NUMDIMS + chosenStochasticAxis]), min(max(dataArr[rndValues[0] * NUMDIMS + chosenStochasticAxis],dataArr[rndValues[1] * NUMDIMS + chosenStochasticAxis]),dataArr[rndValues[2] * NUMDIMS + chosenStochasticAxis]));
                //     // cout << "Median is " << median << endl;
                //     SplitNodeInPlace(thisNode, median, chosenStochasticAxis);
                // }

                createdLeaves.push_back(thisNode->leftChild);
            #endif
            
        }
        else{
            #ifdef TIMEBREAKDOWN
                clock_t start = clock();
            #endif
            for(int ii = a_node->dataArrayLeftPointer; ii < a_node->dataArrayRightPointer; ++ii){
            // cout << "Checking 2 " << dataArr[ii*NUMDIMS] << " " << dataArr[ii*NUMDIMS+1] << endl;
                // if(Overlap(queryMins, queryMaxes, m_data_arr[ii])){
                if(Overlap(queryMins, queryMaxes, dataArr[ii*NUMDIMS], dataArr[ii*NUMDIMS+1])){
                    // cout << dataArr[ii*NUMDIMS] << " " << dataArr[ii*NUMDIMS+1] << " result!\n";
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
        Rect leftCover = Rect();
        Rect rightCover = Rect();
        for(int i = 0; i < NUMDIMS; ++i)
        {
            if(i == a_node->axis)
            {
                leftCover.m_min[i] = cover.m_min[i];
                leftCover.m_max[i] = a_node->pivotValue;

                rightCover.m_min[i] = a_node->pivotValue;
                rightCover.m_max[i] = cover.m_max[i];
            }
            else
            {
                leftCover.m_min[i] = cover.m_min[i];
                leftCover.m_max[i] = cover.m_max[i];

                rightCover.m_min[i] = cover.m_min[i];
                rightCover.m_max[i] = cover.m_max[i];
            }
        }
        if(queryMins[a_node->axis] < a_node->pivotValue){
            AdaptiveQuery(a_node->leftChild, queryMins, queryMaxes, foundCount, leftCover);
        }
        if(queryMaxes[a_node->axis] >= a_node->pivotValue){
            AdaptiveQuery(a_node->rightChild, queryMins, queryMaxes, foundCount, rightCover);
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
