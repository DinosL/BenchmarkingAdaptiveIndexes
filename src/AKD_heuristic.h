// #include "basics.h"

// #ifndef AKD_H_
// #define AKD_H_

// #include "RTile.h"

struct KDNode{
    KDNode(){
        // queryIDs = new vector<int>();
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
    // vector<int> *queryIDs; // this is for the initial partitioning. We keep track of the queries that fall overlap this leaf so we can check their predicates and count their results when we are batch partitioning.

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
        AKDTree();
        AKDTree(vector<pair<int, ELEMTYPE>> cuts);


        // methods
        void AdaptiveBatchQuery(vector<Rect> *thisBatchQueries, ELEMTYPE* minValue, ELEMTYPE *maxValue, vector<int> *queryResults);
        int AdaptiveQuery(ELEMTYPE *queryMins, ELEMTYPE *queryMaxes, Rect cover);
        int NonAdaptiveQuery(ELEMTYPE *queryMins, ELEMTYPE *queryMaxes);
        bool SplitNodeInPlace(KDNode *node, ELEMTYPE pivot, int pivotAxis);
        void SplitNodeStructurally(KDNode* thisNode, ELEMTYPE pivot, int pivotAxis);
        void SplitNodeStructurally(KDNode *node, ELEMTYPE pivot);
        KDNode* getRoot(){return &m_root;}
        void printTree(KDNode *node);
        int traverseTreeGetLeafID(KDNode *node, ELEMTYPE *dataPoint);
        KDNode* traverseTreeGetLeaf(KDNode *node, ELEMTYPE *dataPoint);
        int countNodes();
        int countLeaves();
        void printLeafSizesToFile(string file_name);
        void cerrLeafSizesToFile();
        int deepestLeaf();

        void DFSAndSetLsAndRAndEvaluateQueries(KDNode *thisNode, map<int,int> leafIDsToIndexInArray, map<int,int> *queryResults, vector<pair<int, Rect>> queries);

        void buildTreeBatch(vector<ELEMTYPE> *xbounds, vector<ELEMTYPE> *ybounds, KDNode* a_node, int crackAxi, int startXBound, int endXBound, int startYBound, int endYBound);
        void buildTreeBatchOldMediocre(vector<ELEMTYPE> *xbounds, vector<ELEMTYPE> *ybounds, KDNode* a_node, int crackAxis, int startXBound, int endXBound, int startYBound, int endYBound);
        void buildTreeBatchOldMediocreStructurally(vector<ELEMTYPE> *xbounds, vector<ELEMTYPE> *ybounds, KDNode* a_node, int crackAxis, int startXBound, int endXBound, int startYBound, int endYBound);
        void preBuildTree(int howManyLevels, ELEMTYPE minx, ELEMTYPE miny, ELEMTYPE maxx, ELEMTYPE maxy, vector<Point> *data);
        void preBuildTreeHashed(int howManyLevels, ELEMTYPE minx, ELEMTYPE miny, ELEMTYPE maxx, ELEMTYPE maxy, vector<Point> *data);
        void preBuildTreeHashed_v2(int howManyLevels, ELEMTYPE minx, ELEMTYPE miny, ELEMTYPE maxx, ELEMTYPE maxy, vector<Point> *data);
    private:
        // attributes
        KDNode m_root;


        // methods
        void cut(KDNode* a_node, ELEMTYPE pivotValue);
        int partition2D(int start, int end, int axis, ELEMTYPE pivot);

        void AdaptiveQuery(KDNode* a_node, ELEMTYPE *queryMins, ELEMTYPE *queryMaxes, int &foundCount, Rect cover);
        void AdaptiveQueryHeuristic(KDNode* a_node, ELEMTYPE *queryMins, ELEMTYPE *queryMaxes, int &foundCount, int cutAxis);
        void NonAdaptiveQuery(KDNode* a_node, ELEMTYPE *queryMins, ELEMTYPE *queryMaxes, int &foundCount);
        int ChooseAxisLargestSideMid(Rect node_cover, Rect query_cover);

        void AdaptiveBatchQuery(KDNode* a_node, vector<ELEMTYPE> *boundsVec);
        
        void countNodes(KDNode *thisNode, int &count);
        void countLeaves(KDNode *thisNode, int &count);
        void printLeafSizesToFile(KDNode *thisNode, ofstream &file);
        void cerrLeafSizesToFile(KDNode *thisNode);
        void deepestLeaf(KDNode *thisNode, int &count);

        void buildTreeOnRangeRec(KDNode* thisNode, ELEMTYPE* minVals, ELEMTYPE* maxVals, int axis, int howManyLevelsLeft, int* minGrid, int* maxGrid, map<pair<int, int>, KDNode*> *gridToLeafMap);
        void buildTreeOnRangeRec(KDNode* thisNode, ELEMTYPE* minVals, ELEMTYPE* maxVals, int axis, int howManyLevelsLeft, int* minGrid, int* maxGrid, unordered_map<int, KDNode*> *gridToLeafMap, int xLen);
};

AKDTree::AKDTree(){
    m_root = KDNode();

    m_root.axis = 0;
    m_root.level = 0;
    m_root.leafness = true;
    m_root.dataArrayLeftPointer = 0;
    m_root.dataArrayRightPointer = DATA_COUNT;
    m_root.leftChild = NULL;
    m_root.rightChild = NULL;
    totalNodes++;

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
    #ifdef DEBUG
    cout << "Trying to crack piece " << thisNode->dataArrayLeftPointer << " " << thisNode->dataArrayRightPointer << " on " << pivot << " ";
    #endif
    #ifdef TIMEBREAKDOWN
        auto startPartitionTime = clock();
        cerr << "Level is " << thisNode->level << " swaps ";
    #endif
    int crackIndex = partition2D(thisNode->dataArrayLeftPointer, thisNode->dataArrayRightPointer, pivotAxis, pivot);
    #ifdef TIMEBREAKDOWN
        partitionTime += clock() - startPartitionTime;
        traversals += thisNode->dataArrayRightPointer - thisNode->dataArrayLeftPointer;
    #endif
    // #ifdef DEBUG
    // cout << "in splitNode, after partition" << endl;
    // #endif

    // cout << "cracking for pivot " << pivot << " on " << crackIndex << endl;
    if(crackIndex == thisNode->dataArrayLeftPointer || crackIndex == thisNode->dataArrayRightPointer){
        #ifdef DEBUG
        // cout << "in splitNode, didn't crack " << pivot << " " << pivotAxis << " L: " << thisNode->dataArrayLeftPointer << " R: " << thisNode->dataArrayRightPointer << " crackIndex: " << crackIndex << endl;
        cout << " ----- > DIDN'T crack\n";
        #endif
        #ifdef TIMEBREAKDOWN
            failedCracks++;
        #endif
        return false;
    }

    #ifdef DEBUG
    cout << "in splitNode, after if" << endl;
    #endif

    // chnage the axis to the one it was cracked on
    thisNode->axis = pivotAxis;

    thisNode->leftChild = new KDNode;
    thisNode->rightChild = new KDNode;

    // cout << "SIZE " << sizeof(KDNode) << endl;

    #ifdef DEBUG
    cout << "in splitNode, after children" << endl;
    #endif

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
    totalNodes+=2;
    // then pass thisNode.leftChild and rightChild to each of the recursions
    // a few asserrts: make sure thisNode is leaf -> then make it inner node
    // assert axis of thisNode and axis in function match
    // fix axis of children
    // fix vars for thisNode
    // set uo vars for hte children
    return true;


}

void AKDTree::SplitNodeStructurally(KDNode* thisNode, ELEMTYPE pivot, int pivotAxis)
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
    // #ifdef DEBUG
    // cout << "in splitNode, before partition" << endl;
    // #endif
    // #ifdef TIMEBREAKDOWN
    //     auto startPartitionTime = clock();
    // #endif
    // int crackIndex = partition2D(thisNode->dataArrayLeftPointer, thisNode->dataArrayRightPointer, pivotAxis, pivot);
    // #ifdef TIMEBREAKDOWN
    //     partitionTime += clock() - startPartitionTime;
    // #endif
    // #ifdef DEBUG
    // cout << "in splitNode, after partition" << endl;
    // #endif

    // if(crackIndex == thisNode->dataArrayLeftPointer || crackIndex == thisNode->dataArrayRightPointer){
    //     #ifdef DEBUG
    //     cout << "in splitNode, didn't crack " << pivot << " " << pivotAxis << " L: " << thisNode->dataArrayLeftPointer << " R: " << thisNode->dataArrayRightPointer << " crackIndex: " << crackIndex << endl;
    //     #endif
    //     #ifdef TIMEBREAKDOWN
    //         failedCracks++;
    //     #endif
    //     return;
    // }

    #ifdef DEBUG
    cout << "in splitNode, after if" << endl;
    #endif

    // chnage the axis to the one it was cracked on
    thisNode->axis = pivotAxis;

    thisNode->leftChild = new KDNode;
    thisNode->rightChild = new KDNode;

    #ifdef DEBUG
    cout << "in splitNode, after children" << endl;
    #endif

    thisNode->pivotValue = pivot;
    thisNode->leafness = false;

    // cout << "cutting " << thisNode->axis << " on pivot  " << pivot << endl;

    // cout << "DATA:::::::::::::::::::" << endl;
    // for(int i = 0; i < DATA_COUNT; ++i) cout << m_data_arr[i][0] << " " << m_data_arr[i][1] << "\n";
    // cout << "\nDATA:::::::::::::::::::" << endl;

    // and put the crackIndex to the children
    (thisNode->leftChild)->dataArrayLeftPointer = -1;
    (thisNode->leftChild)->dataArrayRightPointer = -1;

    (thisNode->rightChild)->dataArrayLeftPointer = -1;
    (thisNode->rightChild)->dataArrayRightPointer = -1;

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

void AKDTree::SplitNodeStructurally(KDNode* thisNode, ELEMTYPE pivot){
        // cout << "in split ----------------------" << endl;
    if(!thisNode->leafness){
        cout << "this node in split was not leaf, panic" << endl;
        exit(2);
    }
    thisNode->leftChild = new KDNode;
    thisNode->rightChild = new KDNode;
    // cutting on the boundsSet[axis][ans] on axis
    thisNode->pivotValue = pivot;
    thisNode->leafness = false;

    // cout << "cutting " << thisNode->axis << " on pivot  " << pivot << endl;
    // partition the data in Left and Right of the array
    // partition(m_data_arr.begin() + thisNode->dataArrayLeftPointer, m_data_arr.begin() + thisNode->dataArrayRightPointer, [pivot,thisNode](const auto& em){return em[thisNode->axis] < pivot;}); //  // cout << em << endl;
    // int crackIndex = partition2D(thisNode->dataArrayLeftPointer, thisNode->dataArrayRightPointer, thisNode->axis, pivot);


    // cout << "DATA:::::::::::::::::::" << endl;
    // for(int i = 0; i < DATA_COUNT; ++i) cout << m_data_arr[i][0] << " " << m_data_arr[i][1] << "\n";
    // cout << "\nDATA:::::::::::::::::::" << endl;

    // and put the crackIndex to the children

    (thisNode->leftChild)->dataArrayLeftPointer = -1;
    (thisNode->leftChild)->dataArrayRightPointer = -1;

    (thisNode->rightChild)->dataArrayLeftPointer = -1;
    (thisNode->rightChild)->dataArrayRightPointer = -1;

    (thisNode->leftChild)->level = thisNode->level + 1;
    (thisNode->rightChild)->level = thisNode->level + 1;

    (thisNode->leftChild)->axis = (thisNode->axis + 1) % NUMDIMS;
    (thisNode->rightChild)->axis = (thisNode->axis + 1) % NUMDIMS;

    // (thisNode->leftChild)->axis = -2;
    // (thisNode->rightChild)->axis = -2;

    (thisNode->leftChild)->leafness = true;
    (thisNode->rightChild)->leafness = true;
    // then pass thisNode.leftChild and rightChild to each of the recursions
    // a few asserrts: make sure thisNode is leaf -> then make it inner node
    // assert axis of thisNode and axis in function match
    // fix axis of children
    // fix vars for thisNode
    // set uo vars for hte children
}


int AKDTree::partition2D(int start, int end, int axis, ELEMTYPE pivot){


    #ifdef TIMEBREAKDOWN
        int localSwaps = 0;
    #endif
    int i = start, j = end - 1;
    // cout << "in partition " << i << " " << j << endl;
    while(i <= j && j > 0){
        // cout << "Is " << m_data_arr[i][axis] << " <= " << pivot << endl;
        if(m_data_arr[i][axis] < pivot){
            i++;
        }
        else{
            // cout << "Is " << m_data_arr[i][axis] << " > " << pivot << endl;
            while(j > 0 && j >= i && m_data_arr[j][axis] >= pivot){      // TODO can remove j > 0, i think
                j--;
            }
            // cout << "I,J " << i << " " << j << endl; 
            if(i < j){
                // cout << "swapping" << endl;
                #ifdef TIMEBREAKDOWN
                    clock_t startTime = clock();
                #endif
                // cout << "SWAP!!!!\n";
                std::swap(m_data_arr[i], m_data_arr[j]);
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


int AKDTree::NonAdaptiveQuery(ELEMTYPE *queryMins, ELEMTYPE *queryMaxes){
    int foundCount = 0;

    // first traverse the tree to find the leaves that hold the data in them
    NonAdaptiveQuery(&m_root, queryMins, queryMaxes, foundCount);
    return foundCount;

    
}

void AKDTree::DFSAndSetLsAndRAndEvaluateQueries(KDNode *thisNode, map<int,int> leafIDsToIndexInArray, map<int,int> *queryResults, vector<pair<int, Rect>> queries)
{

    // if(thisNode->leafness)
    // {
    //     auto it = leafIDsToIndexInArray.find(thisNode->id);
    //     thisNode->dataArrayLeftPointer = it->second;
    //     thisNode->dataArrayRightPointer = (++it)->second;

    //     for(int i = thisNode->dataArrayLeftPointer; i < thisNode->dataArrayRightPointer; ++i)
    //     {
    //         for(int j = 0; j < thisNode->queryIDs->size(); ++j)
    //         {
    //             if(m_data_arr[i][0] >= queries[j].second.m_min[0] && m_data_arr[i][0] < queries[j].second.m_max[0]
    //             &&
    //             m_data_arr[i][1] >= queries[j].second.m_min[1] && m_data_arr[i][1] < queries[j].second.m_max[1]
    //             )
    //             {
    //                 // cout << "overlaps! " << thisNode->queryIDs->at(iii) << " " << thisNode->queryIDs->at(iii) % batchSize << endl;
    //                 (*queryResults)[queries[j].first]++;
    //             }
    //         }
    //     }
    // }
    // else 
    // {
    //     DFSAndSetLsAndRAndEvaluateQueries(thisNode->leftChild, leafIDsToIndexInArray, queryResults, queries);
    //     DFSAndSetLsAndRAndEvaluateQueries(thisNode->rightChild, leafIDsToIndexInArray, queryResults, queries);
    // }

}


void AKDTree::NonAdaptiveQuery(KDNode* a_node, ELEMTYPE *queryMins, ELEMTYPE *queryMaxes, int &foundCount){
    if(a_node->leafness){
        // is a leaf
        for(int ii = a_node->dataArrayLeftPointer; ii < a_node->dataArrayRightPointer; ++ii){
            if(Overlap(queryMins, queryMaxes, m_data_arr[ii])){
                foundCount++;
            }
        }
        
    }
    else{
        // is internal
        // should recurse to children that overlap
        if(queryMins[a_node->axis] < a_node->pivotValue){
            NonAdaptiveQuery(a_node->leftChild, queryMins, queryMaxes, foundCount);
        }
        if(queryMaxes[a_node->axis] >= a_node->pivotValue){
            NonAdaptiveQuery(a_node->rightChild, queryMins, queryMaxes, foundCount);
        }
    }
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



int AKDTree::AdaptiveQuery(ELEMTYPE *queryMins, ELEMTYPE *queryMaxes, Rect cover){
    int foundCount = 0;

    // first traverse the tree to find the leaves that hold the data in them
    AdaptiveQuery(&m_root, queryMins, queryMaxes, foundCount, cover);
    return foundCount;

    
}


void AKDTree::AdaptiveQuery(KDNode* a_node, ELEMTYPE *queryMins, ELEMTYPE *queryMaxes, int &foundCount, Rect cover){
    if(a_node->leafness){
        // is a leaf
        // should crack on the bounds
        // also add to the foundcount
        bool stopCracking=false;
        int choice, mm, sum_of_mms = 0, chosen_axis, sum_of_choices = 0;
        
        KDNode* thisNode = a_node;
        if(a_node->dataArrayRightPointer - a_node->dataArrayLeftPointer > CRACKTHRESHOLD){
            // then for each of them crack them 2D times
            #ifdef STOCHASTIC
                vector<KDNode*> createdLeaves;
            #endif
            for(int i =0; i < 2*NUMDIMS; i++)
            {

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
                        // if(i == 0)
                        // {
        
                        //     if((thisNode->leftChild)->dataArrayRightPointer - (thisNode->leftChild)->dataArrayLeftPointer > CRACKTHRESHOLD)
                        //     {
                        //         inStoch++;
                        //         int chosenStochasticAxis = 0;
                        //         for(int ii = 1; ii < NUMDIMS; ++ii)
                        //         {
                        //             if(cover.m_max[ii] - cover.m_min[ii] > cover.m_max[chosenStochasticAxis] - cover.m_min[chosenStochasticAxis])
                        //                 chosenStochasticAxis = ii;
                        //         }

                        //         vector<int> rndValues;
                        //         rndValues.push_back((thisNode->leftChild)->dataArrayLeftPointer + ( rand() % ((thisNode->leftChild)->dataArrayRightPointer - (thisNode->leftChild)->dataArrayLeftPointer + 1 )));
                        //         rndValues.push_back((thisNode->leftChild)->dataArrayLeftPointer + ( rand() % ((thisNode->leftChild)->dataArrayRightPointer - (thisNode->leftChild)->dataArrayLeftPointer + 1 )));
                        //         rndValues.push_back((thisNode->leftChild)->dataArrayLeftPointer + ( rand() % ((thisNode->leftChild)->dataArrayRightPointer - (thisNode->leftChild)->dataArrayLeftPointer + 1 )));

                        //         // float m = m_data_arr[rndValues[0]][0];
                        //         float median = max(min(m_data_arr[rndValues[0]][chosenStochasticAxis],m_data_arr[rndValues[1]][chosenStochasticAxis]), min(max(m_data_arr[rndValues[0]][chosenStochasticAxis],m_data_arr[rndValues[1]][chosenStochasticAxis]),m_data_arr[rndValues[2]][chosenStochasticAxis]));
                        //         bool flag = SplitNodeInPlace((thisNode->leftChild), median, chosenStochasticAxis);
                        //         if(flag) {
                        //             actuallyCracked++;
                        //             sizeOfStochCrackedPiece += (thisNode->leftChild)->dataArrayRightPointer - (thisNode->leftChild)->dataArrayLeftPointer;
                        //         }
                        //     }

                        // }
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
                        // if(i == 0)
                        // {
        
                        //     if((thisNode->rightChild)->dataArrayRightPointer - (thisNode->rightChild)->dataArrayLeftPointer > CRACKTHRESHOLD)
                        //     {
                        //         inStoch++;
                        //         int chosenStochasticAxis = 0;
                        //         for(int ii = 1; ii < NUMDIMS; ++ii)
                        //         {
                        //             if(cover.m_max[ii] - cover.m_min[ii] > cover.m_max[chosenStochasticAxis] - cover.m_min[chosenStochasticAxis])
                        //                 chosenStochasticAxis = ii;
                        //         }

                        //         vector<int> rndValues;
                        //         rndValues.push_back((thisNode->rightChild)->dataArrayLeftPointer + ( rand() % ((thisNode->rightChild)->dataArrayRightPointer - (thisNode->rightChild)->dataArrayLeftPointer + 1 )));
                        //         rndValues.push_back((thisNode->rightChild)->dataArrayLeftPointer + ( rand() % ((thisNode->rightChild)->dataArrayRightPointer - (thisNode->rightChild)->dataArrayLeftPointer + 1 )));
                        //         rndValues.push_back((thisNode->rightChild)->dataArrayLeftPointer + ( rand() % ((thisNode->rightChild)->dataArrayRightPointer - (thisNode->rightChild)->dataArrayLeftPointer + 1 )));

                        //         // float m = m_data_arr[rndValues[0]][0];
                        //         float median = max(min(m_data_arr[rndValues[0]][chosenStochasticAxis],m_data_arr[rndValues[1]][chosenStochasticAxis]), min(max(m_data_arr[rndValues[0]][chosenStochasticAxis],m_data_arr[rndValues[1]][chosenStochasticAxis]),m_data_arr[rndValues[2]][chosenStochasticAxis]));
                        //         bool flag = SplitNodeInPlace((thisNode->rightChild), median, chosenStochasticAxis);
                        //         if(flag) 
                        //         {
                        //             actuallyCracked++;
                        //             sizeOfStochCrackedPiece += (thisNode->rightChild)->dataArrayRightPointer - (thisNode->rightChild)->dataArrayLeftPointer;
                        //         }
                        //     }

                        // }
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
                        float median = max(min(m_data_arr[rndValues[0]][chosenStochasticAxis],m_data_arr[rndValues[1]][chosenStochasticAxis]), min(max(m_data_arr[rndValues[0]][chosenStochasticAxis],m_data_arr[rndValues[1]][chosenStochasticAxis]),m_data_arr[rndValues[2]][chosenStochasticAxis]));
                        bool flag = SplitNodeInPlace(nodeToBeCracked, median, chosenStochasticAxis);
                        // if(flag) 
                        // {
                        //     actuallyCracked++;
                        //     sizeOfStochCrackedPiece += nodeToBeCracked->dataArrayRightPointer - nodeToBeCracked->dataArrayLeftPointer;
                        // }
                    }
                }
            #endif

            #ifdef TIMEBREAKDOWN
                clock_t start = clock();
            #endif

            // KDNode* thisNode = a_node;
            // for(int i = 0; i < NUMDIMS; ++i){
            //     // perform crack on min values
            //     SplitNodeInPlace(thisNode, queryMins[i], i);
            //     // printTree(a_node);
                
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
            //         //  printTree(a_node);
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
                if(Overlap(queryMins, queryMaxes, m_data_arr[ii])){
                    foundCount++;
                }
            }
            #ifdef TIMEBREAKDOWN
                adaptiveCountingResultsTime += clock() - start;
            #endif
        }
        else{
            #ifdef TIMEBREAKDOWN
                clock_t start = clock();
            #endif
            for(int ii = a_node->dataArrayLeftPointer; ii < a_node->dataArrayRightPointer; ++ii){
                if(Overlap(queryMins, queryMaxes, m_data_arr[ii])){
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

// void AKDTree::AdaptiveQueryHeuristic(KDNode* a_node, ELEMTYPE *queryMins, ELEMTYPE *queryMaxes, int &foundCount, int cutAxis){
//     if(a_node->leafness){
//         // is a leaf
//         // should crack on the bounds
//         // also add to the foundcount
//         bool stopCracking=false;
        
//         if(a_node->dataArrayRightPointer - a_node->dataArrayLeftPointer > CRACKTHRESHOLD){
//             // then for each of them crack them 2D times

//             #ifdef TIMEBREAKDOWN
//                 clock_t start = clock();
//             #endif

//             KDNode* thisNode = a_node;
//             for(int i = 0; i < NUMDIMS; ++i){
//                 // perform crack on min values
//                 SplitNodeInPlace(thisNode, queryMins[i], i);
//                 // printTree(a_node);
                
//                 // if(thisNode->leafness){
//                 //     // did not crack so we should stop
//                 //     stopCracking=true;
//                 //     break;
//                 // }
//                 if(!thisNode->leafness) thisNode = thisNode->rightChild;
//                 if(thisNode->dataArrayRightPointer - thisNode->dataArrayLeftPointer <= CRACKTHRESHOLD) break;
//             }
//             if(!stopCracking && thisNode->dataArrayRightPointer - thisNode->dataArrayLeftPointer > CRACKTHRESHOLD){
//                 for(int i = 0; i < NUMDIMS; ++i){
//                     // perform crack on max values
//                     SplitNodeInPlace(thisNode, queryMaxes[i], i);
//                     //  printTree(a_node);
//                     // if(thisNode->leafness){
//                     // // did not crack so we should stop
//                     //     stopCracking=true;
//                     //     break;
//                     // }
//                     if(!thisNode->leafness) thisNode = thisNode->leftChild;
//                     if(thisNode->dataArrayRightPointer - thisNode->dataArrayLeftPointer <= CRACKTHRESHOLD) break;
//                 }
//             }
//             #ifdef TIMEBREAKDOWN
//                 adaptiveSplitTime += clock() - start;
//             #endif

//             #ifdef TIMEBREAKDOWN
//                 start = clock();
//             #endif
//             // foundCount += (thisNode->dataArrayRightPointer - thisNode->dataArrayLeftPointer);
//             for(int ii = thisNode->dataArrayLeftPointer; ii < thisNode->dataArrayRightPointer; ++ii){
//                 if(Overlap(queryMins, queryMaxes, m_data_arr[ii])){
//                     foundCount++;
//                 }
//             }
//             #ifdef TIMEBREAKDOWN
//                 adaptiveCountingResultsTime += clock() - start;
//             #endif
            
//         }
//         else{
//             #ifdef TIMEBREAKDOWN
//                 clock_t start = clock();
//             #endif
//             for(int ii = a_node->dataArrayLeftPointer; ii < a_node->dataArrayRightPointer; ++ii){
//                 if(Overlap(queryMins, queryMaxes, m_data_arr[ii])){
//                     foundCount++;
//                 }
//             }
//             #ifdef TIMEBREAKDOWN
//                 adaptiveCountingResultsTime += clock() - start;
//             #endif

//         }
        
//     }
//     else{
//         // is internal
//         // should recurse to children that overlap
//         if(queryMins[a_node->axis] < a_node->pivotValue){
//             AdaptiveQueryHeuristic(a_node->leftChild, queryMins, queryMaxes, foundCount);
//         }
//         if(queryMaxes[a_node->axis] >= a_node->pivotValue){
//             AdaptiveQueryHeuristic(a_node->rightChild, queryMins, queryMaxes, foundCount);
//         }
//     }
// }



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


int AKDTree::traverseTreeGetLeafID(KDNode *node, ELEMTYPE *dataPoint){
    if(node->leafness){
        return node->id;
    }
    if(dataPoint[node->axis] < node->pivotValue){
        return traverseTreeGetLeafID(node->leftChild, dataPoint);
    }
    else{
        return traverseTreeGetLeafID(node->rightChild, dataPoint);
    }
}


KDNode* AKDTree::traverseTreeGetLeaf(KDNode *node, ELEMTYPE *dataPoint){
    if(node->leafness){
        return node;
    }
    if(dataPoint[node->axis] < node->pivotValue){
        return traverseTreeGetLeaf(node->leftChild, dataPoint);
    }
    else{
        return traverseTreeGetLeaf(node->rightChild, dataPoint);
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


// void AKDTree::printLeafSizesToFile(string file_name){
//     ofstream f(file_name.c_str());
//     printLeafSizesToFile(&m_root, f);
//     f.close();
// }

// void AKDTree::printLeafSizesToFile(KDNode* thisNode, ofstream &f){
    
//     if(!thisNode->leafness){
//         printLeafSizesToFile(thisNode->leftChild, f);
//         printLeafSizesToFile(thisNode->rightChild, f);
//     }
//     else{
//         f << thisNode->dataArrayRightPointer - thisNode->dataArrayLeftPointer << endl;
//     }
// }

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

void AKDTree::buildTreeOnRangeRec(KDNode* thisNode, ELEMTYPE* minVals, ELEMTYPE* maxVals, int axis, int howManyLevelsLeft, int* minGrid, int* maxGrid, map<pair<int, int>, KDNode*> *gridToLeafMap){

    if(howManyLevelsLeft == 0){
        // stop
        // we also need to fill the map wth the pointers to this leaf and its corresponding grid cell
        // first assert that the mins and max grids only have one different
        if(minGrid[0] + 1 != maxGrid[0] || minGrid[1] + 1 != maxGrid[1]){
            cout << "did not gop as planned..., tragedy,death,war, all that." << endl;
            cout << minGrid[0] << " " << minGrid[1] << " " << maxGrid[0] << " " << maxGrid[1] << endl;
            exit(5);
        }
        (*gridToLeafMap)[make_pair(minGrid[0], minGrid[1])] = thisNode;
        return;
    }

    thisNode->axis = axis;
    SplitNodeStructurally(thisNode, (minVals[axis] + maxVals[axis]) / 2);

    ELEMTYPE* rightMinVals = (ELEMTYPE*) malloc(NUMDIMS * sizeof(ELEMTYPE));
    ELEMTYPE* rightMaxVals = (ELEMTYPE*) malloc(NUMDIMS * sizeof(ELEMTYPE));
    ELEMTYPE* leftMinVals = (ELEMTYPE*) malloc(NUMDIMS * sizeof(ELEMTYPE));
    ELEMTYPE* leftMaxVals = (ELEMTYPE*) malloc(NUMDIMS * sizeof(ELEMTYPE));

    rightMinVals[axis] =  (minVals[axis] + maxVals[axis]) / 2;
    rightMinVals[1 - axis] = minVals[1 - axis];
    rightMaxVals[axis] =  maxVals[axis];
    rightMaxVals[1 - axis] = maxVals[1 - axis];

    leftMaxVals[axis] =  (minVals[axis] + maxVals[axis]) / 2;
    leftMaxVals[1 - axis] = maxVals[1 - axis];
    leftMinVals[axis] =  minVals[axis];
    leftMinVals[1 - axis] = minVals[1 - axis];


    int* rightMinGrid = (int*) malloc(NUMDIMS * sizeof(int));
    int* rightMaxGrid = (int*) malloc(NUMDIMS * sizeof(int));
    int* leftMinGrid = (int*) malloc(NUMDIMS * sizeof(int));
    int* leftMaxGrid = (int*) malloc(NUMDIMS * sizeof(int));

    rightMinGrid[axis] =  (minGrid[axis] + maxGrid[axis]) / 2;
    rightMinGrid[1 - axis] = minGrid[1 - axis];
    rightMaxGrid[axis] =  maxGrid[axis];
    rightMaxGrid[1 - axis] = maxGrid[1 - axis];

    leftMaxGrid[axis] =  (minGrid[axis] + maxGrid[axis]) / 2;
    leftMaxGrid[1 - axis] = maxGrid[1 - axis];
    leftMinGrid[axis] =  minGrid[axis];
    leftMinGrid[1 - axis] = minGrid[1 - axis];


    buildTreeOnRangeRec(thisNode->rightChild, rightMinVals, rightMaxVals, 1 - axis, howManyLevelsLeft - 1, rightMinGrid, rightMaxGrid, gridToLeafMap);
    buildTreeOnRangeRec(thisNode->leftChild, leftMinVals, leftMaxVals, 1 - axis, howManyLevelsLeft - 1, leftMinGrid, leftMaxGrid, gridToLeafMap);

}


void AKDTree::preBuildTree(int howManyLevels, ELEMTYPE minx, ELEMTYPE miny, ELEMTYPE maxx, ELEMTYPE maxy, vector<Point> *data){
    // !!!! assume that howManylevels is always divisible by NUMDIMS
    // !! ONLY FOR 2d


    // first we will build the tree structurally
    // while doing that we will also keep a hash map of (i, j) pairs to the KDNode of the leaves
    // besically mapping the grid to the KD-tree


    // then we will pass through the data and with 2 divisions find the (i, j) it belongs to and count the population of each leaf
    // then another scan throgh the data to put them in the correct order in the array

    // the partitions in each dim will be range/(2^hml)
    double partition_steps[2] = {(maxx - minx) / (pow(2, howManyLevels / 2)), (maxy - miny) / (pow(2, howManyLevels / 2))};
    #ifdef DEBUG
    cout << "partition steps: " << partition_steps[0] << " " << partition_steps[1] << endl;
    #endif

    ELEMTYPE* minVals = (ELEMTYPE*) malloc(NUMDIMS * sizeof(ELEMTYPE));
    ELEMTYPE* maxVals = (ELEMTYPE*) malloc(NUMDIMS * sizeof(ELEMTYPE));
    minVals[0] = minx; minVals[1] = miny;
    maxVals[0] = maxx; maxVals[1] = maxy;


    int* minGrid = (int*) malloc(NUMDIMS * sizeof(int));
    int* maxGrid = (int*) malloc(NUMDIMS * sizeof(int));
    minGrid[0] = 0; minGrid[1] = 0;
    maxGrid[0] = pow(2, howManyLevels/2); maxGrid[1] = pow(2, howManyLevels / 2);

    map<pair<int, int>, KDNode*> gridToLeafMap;
    #ifdef DEBUG
    cout << "before buildTree" << endl;
    #endif

    // build the tree structure
    buildTreeOnRangeRec(&m_root, minVals, maxVals, 0, howManyLevels, minGrid, maxGrid, &gridToLeafMap);

    #ifdef DEBUG
    // sanity check
    for(int i = 0; i < maxGrid[0]; ++i){
        for(int j = 0; j < maxGrid[1]; ++j){
            if(gridToLeafMap[make_pair(i, j)] == NULL){
                cout << "FOUND A NULLLLLLL!!!!!!!!!!!!!!!!!!!!!!!!!!" << endl;
                exit(3);
            }
        }
    }
    #endif

    #ifdef DEBUG
    cout << "after buildTree" << endl;
    #endif

    map<KDNode*, int> leafCounts;
    int thisI, thisJ;
    pair<int, int> thisPosition;
    for(int i = 0; i < DATA_COUNT; ++i){
        // find where it belongs
        thisPosition.first = (int) ((data->at(i).m_values[0] - minVals[0]) / partition_steps[0]);
        thisPosition.second = (int) ((data->at(i).m_values[1] - minVals[1]) / partition_steps[1]);
        // now we get the kdNpode from the grid map
        // then ++ the count of thst node
        // TODO: how do pairs work? is it with pointers? are they not going to be equal now??
        #ifdef DEBUG
        cout << data->at(i).m_values[0] << " " << partition_steps[0] << " " << (int) ((data->at(i).m_values[0] - minVals[0]) / partition_steps[0]) << " " << maxGrid[0] << endl;
        cout << data->at(i).m_values[1] << " " << partition_steps[1] << " " << (int) ((data->at(i).m_values[1] - minVals[1]) / partition_steps[1]) << " " << maxGrid[1] << endl;
        cout << "leafCounts of " << thisPosition.first << " " << thisPosition.second << " = " << leafCounts[gridToLeafMap[thisPosition]] << endl;
        // cout << "leafCounts of " << gridToLeafMap[thisPosition]->id << endl;
        // cout << "coutning for leaf " << thisPosition.first << " " << thisPosition.second << " " <<  gridToLeafMap[thisPosition]->id << endl;
        #endif

        thisPosition.first = (thisPosition.first == maxGrid[0]) ? thisPosition.first -1 : thisPosition.first;
        thisPosition.second = (thisPosition.second == maxGrid[1]) ? thisPosition.second -1 : thisPosition.second;

        leafCounts[gridToLeafMap[thisPosition]]++;
        
    }

    #ifdef DEBUG
    cout << "after counting" << endl;
    #endif

    // thwen we have to make those acc array pointer positions thinggs to the start of each of theser leaves in the array
    // it's just that they are already in m_data_arr...
    // what do i do now?...TODO

    map<KDNode*, int> leafPointers;
    int tmp = 0;

    #ifdef DEBUG
    cout << leafCounts.size() << endl;
    #endif
    
    for(auto it = leafCounts.begin(); it != leafCounts.end(); it++){
        #ifdef DEBUG
        cout << "leaf "  << endl << it->second << endl;
        if(it->first == NULL) cout << "first is NULL" << endl;
        #endif
        leafPointers[it->first] = tmp;
        (it->first)->dataArrayLeftPointer = tmp;
        (it->first)->dataArrayRightPointer = tmp + it->second;
        tmp += it->second;
    }

    #ifdef DEBUG
    cout << "after p[ointers]" << endl;
    #endif

    int this_pointer;
    for(int i = 0; i < DATA_COUNT; ++i){
        // find where it belongs
        thisPosition.first = (int) ((data->at(i).m_values[0] - minVals[0]) / partition_steps[0]);
        thisPosition.second = (int) ((data->at(i).m_values[1] - minVals[1]) / partition_steps[1]);

        thisPosition.first = (thisPosition.first == maxGrid[0]) ? thisPosition.first -1 : thisPosition.first;
        thisPosition.second = (thisPosition.second == maxGrid[1]) ? thisPosition.second -1 : thisPosition.second;

        this_pointer = leafPointers[gridToLeafMap[thisPosition]];
        m_data_arr[this_pointer][0] =  data->at(i).m_values[0];
        m_data_arr[this_pointer][1] =  data->at(i).m_values[1];

        leafPointers[gridToLeafMap[thisPosition]]++;
    }


    #ifdef DEBUG
    cout << "after copying" << endl;
    #endif

      

}

void AKDTree::buildTreeBatch(vector<ELEMTYPE> *xbounds, vector<ELEMTYPE> *ybounds, KDNode* a_node, int crackAxis, int startXBound, int endXBound, int startYBound, int endYBound){

    int numOfSamples = 5;
    vector<ELEMTYPE> rndXBound, rndYBound;
    // vector<int> rndXBound, rndYBound;
    float mediocre = 0.0;

    // cout << "LEVEL--------------------- " << a_node->level << " " << crackAxis << " " << a_node->dataArrayLeftPointer << " " << a_node->dataArrayRightPointer << endl;

    if(crackAxis == 0)
    {
        // cout << "axis is 0" << endl;
        // cout << "startXBound = " << startXBound << " endXBound = " << endXBound << " xbounds size " << xbounds->size() << endl;
        #ifdef TIMEBREAKDOWN
            clock_t mediocreStartTime = clock();
        #endif
        for(int i = 0; i < numOfSamples; ++i)
        {
            rndXBound.push_back(xbounds->at(startXBound + ( std::rand() % ( endXBound - startXBound +1  ) )));
            // rndXBound.push_back(startXBound + ( std::rand() % ( endXBound - startXBound +1  ) ));
        }
        // cout << " after for \n";
        size_t n = numOfSamples / 2;
        nth_element(rndXBound.begin(), rndXBound.begin()+n, rndXBound.end());
        ELEMTYPE midItem = rndXBound[n];
        // cout << "after nth \n";
        auto middle1 = std::partition(xbounds->begin() + startXBound, xbounds->begin() + endXBound, [midItem](const auto& em)
        {
            return em < midItem;
        });

        #ifdef TIMEBREAKDOWN
            mediocreTime += clock() - mediocreStartTime;
            clock_t splitStartTime = clock();
        #endif
        SplitNodeInPlace(a_node, midItem, crackAxis);
        #ifdef TIMEBREAKDOWN
            splitTime += clock() - splitStartTime;
        #endif
        // SplitNodeInPlace(a_node, xbounds->at(rndXBound[n]), crackAxis);

        // int midPoint = (startXBound + endXBound ) / 2;
        if(!a_node->leafness)
        {
            // buildTreeBatch(xbounds, ybounds, a_node->rightChild, (crackAxis+1)%NUMDIMS, midPoint, endXBound, startYBound, endYBound);
            // buildTreeBatch(xbounds, ybounds, a_node->leftChild, (crackAxis+1)%NUMDIMS, startXBound, midPoint, startYBound, endYBound);
            int middle = (middle1 - xbounds->begin());
            // cout << "after middle " << middle << " " << middle1 - xbounds->begin() << endl;
            buildTreeBatch(xbounds, ybounds, a_node->rightChild, (crackAxis+1)%NUMDIMS, middle, endXBound, startYBound, endYBound);
            buildTreeBatch(xbounds, ybounds, a_node->leftChild, (crackAxis+1)%NUMDIMS, startXBound, middle, startYBound, endYBound);
        }
    }
    else
    {
        // cout << "axis is NOT 0" << endl;
        #ifdef TIMEBREAKDOWN
            clock_t mediocreStartTime = clock();
        #endif
        for(int i = 0; i < numOfSamples; ++i)
        {
            rndYBound.push_back(ybounds->at(startYBound + ( std::rand() % ( endYBound - startYBound +1  ) )));
            //  rndYBound.push_back(startYBound + ( std::rand() % ( endYBound - startYBound +1  ) ));
        }
        size_t n = numOfSamples / 2;
        nth_element(rndYBound.begin(), rndYBound.begin()+n, rndYBound.end());
        ELEMTYPE midItem = rndYBound[n];
        auto middle1 = std::partition(ybounds->begin() + startYBound, ybounds->begin() + endYBound, [midItem](const auto& em)
        {
            return em < midItem;
        });

        #ifdef TIMEBREAKDOWN
            mediocreTime += clock() - mediocreStartTime;
            clock_t splitStartTime = clock();
        #endif
        SplitNodeInPlace(a_node, midItem, crackAxis);
        // SplitNodeInPlace(a_node, ybounds->at(rndYBound[n]), crackAxis);
        #ifdef TIMEBREAKDOWN
            splitTime += clock() - splitStartTime;
        #endif

        int midPoint = (startYBound + endYBound ) / 2;

        if(!a_node->leafness)
        {
            // buildTreeBatch(xbounds, ybounds, a_node->rightChild, (crackAxis+1)%NUMDIMS, startXBound, endXBound, midPoint, endYBound);
            // buildTreeBatch(xbounds, ybounds, a_node->leftChild, (crackAxis+1)%NUMDIMS, startXBound, endXBound, startYBound, midPoint);
            int middle = (middle1 - ybounds->begin());
            buildTreeBatch(xbounds, ybounds, a_node->rightChild, (crackAxis+1)%NUMDIMS, startXBound, endXBound, middle, endYBound);
            buildTreeBatch(xbounds, ybounds, a_node->leftChild, (crackAxis+1)%NUMDIMS, startXBound, endXBound, startYBound, middle);
        }

        // cout << "is Leaf " << a_node->leafness << endl;
    }
}

void AKDTree::buildTreeBatchOldMediocre(vector<ELEMTYPE> *xbounds, vector<ELEMTYPE> *ybounds, KDNode* a_node, int crackAxis, int startXBound, int endXBound, int startYBound, int endYBound){

    int numOfSamples = 5;
    vector<ELEMTYPE> rndXBound, rndYBound;
    // vector<int> rndXBound, rndYBound;
    float mediocre = 0.0;

    // cout << "LEVEL--------------------- " << a_node->level << " " << crackAxis << " " << a_node->dataArrayLeftPointer << " " << a_node->dataArrayRightPointer << endl;

    if(crackAxis == 0)
    {
        // cout << "axis is 0" << endl;
        // cout << "startXBound = " << startXBound << " endXBound = " << endXBound << " xbounds size " << xbounds->size() << endl;
        // for(int i = 0; i < numOfSamples; ++i)
        // {
        //     rndXBound.push_back(xbounds->at(startXBound + ( std::rand() % ( endXBound - startXBound +1  ) )));
        //     // rndXBound.push_back(startXBound + ( std::rand() % ( endXBound - startXBound +1  ) ));
        // }
        // // cout << " after for \n";
        // size_t n = numOfSamples / 2;
        // nth_element(rndXBound.begin(), rndXBound.begin()+n, rndXBound.end());

        #ifdef TIMEBREAKDOWN
            clock_t mediocreStartTime = clock();
        #endif

        rndXBound.push_back(xbounds->at(startXBound + ( std::rand() % ( endXBound - startXBound +1  ) )));
        rndXBound.push_back(xbounds->at(startXBound + ( std::rand() % ( endXBound - startXBound +1  ) )));
        rndXBound.push_back(xbounds->at(startXBound + ( std::rand() % ( endXBound - startXBound +1  ) )));


        ELEMTYPE midItem = max(min(rndXBound[0],rndXBound[1]), min(max(rndXBound[0],rndXBound[1]),rndXBound[2]));
                

        // ELEMTYPE midItem = rndXBound[n];
        // cout << "after nth \n";
        auto middle1 = std::partition(xbounds->begin() + startXBound, xbounds->begin() + endXBound, [midItem](const auto& em)
        {
            return em < midItem;
        });

        #ifdef TIMEBREAKDOWN
            mediocreTime += clock() - mediocreStartTime;
            clock_t splitStartTime = clock();
        #endif
        SplitNodeInPlace(a_node, midItem, crackAxis);
        #ifdef TIMEBREAKDOWN
            splitTime += clock() - splitStartTime;
        #endif
        // SplitNodeInPlace(a_node, xbounds->at(rndXBound[n]), crackAxis);

        // int midPoint = (startXBound + endXBound ) / 2;
        if(!a_node->leafness)
        {
            // buildTreeBatch(xbounds, ybounds, a_node->rightChild, (crackAxis+1)%NUMDIMS, midPoint, endXBound, startYBound, endYBound);
            // buildTreeBatch(xbounds, ybounds, a_node->leftChild, (crackAxis+1)%NUMDIMS, startXBound, midPoint, startYBound, endYBound);
            int middle = (middle1 - xbounds->begin());
            // cout << "after middle " << middle << " " << middle1 - xbounds->begin() << endl;
            // cout << "middle is " << middle << " " << midItem << endl;
            buildTreeBatchOldMediocre(xbounds, ybounds, a_node->rightChild, (crackAxis+1)%NUMDIMS, middle, endXBound, startYBound, endYBound);
            buildTreeBatchOldMediocre(xbounds, ybounds, a_node->leftChild, (crackAxis+1)%NUMDIMS, startXBound, middle, startYBound, endYBound);
        }
    }
    else
    {
        // cout << "axis is NOT 0" << endl;
        // for(int i = 0; i < numOfSamples; ++i)
        // {
        //     rndYBound.push_back(ybounds->at(startYBound + ( std::rand() % ( endYBound - startYBound +1  ) )));
        //     //  rndYBound.push_back(startYBound + ( std::rand() % ( endYBound - startYBound +1  ) ));
        // }
        // size_t n = numOfSamples / 2;
        // nth_element(rndYBound.begin(), rndYBound.begin()+n, rndYBound.end());

        #ifdef TIMEBREAKDOWN
            clock_t mediocreStartTime = clock();
        #endif
        
        rndYBound.push_back(ybounds->at(startYBound + ( std::rand() % ( endYBound - startYBound +1  ) )));
        rndYBound.push_back(ybounds->at(startYBound + ( std::rand() % ( endYBound - startYBound +1  ) )));
        rndYBound.push_back(ybounds->at(startYBound + ( std::rand() % ( endYBound - startYBound +1  ) )));


        ELEMTYPE midItem = max(min(rndYBound[0],rndYBound[1]), min(max(rndYBound[0],rndYBound[1]),rndYBound[2]));

        // ELEMTYPE midItem = rndYBound[n];
        auto middle1 = std::partition(ybounds->begin() + startYBound, ybounds->begin() + endYBound, [midItem](const auto& em)
        {
            return em < midItem;
        });
        // cout << "Splitting y on " << ybounds->at(rndYBound[n]) << endl;
        #ifdef TIMEBREAKDOWN
            mediocreTime += clock() - mediocreStartTime;
            clock_t splitStartTime = clock();
        #endif
        SplitNodeInPlace(a_node, midItem, crackAxis);
        // SplitNodeInPlace(a_node, ybounds->at(rndYBound[n]), crackAxis);
        #ifdef TIMEBREAKDOWN
            splitTime += clock() - splitStartTime;
        #endif

        int midPoint = (startYBound + endYBound ) / 2;

        if(!a_node->leafness)
        {
            // buildTreeBatch(xbounds, ybounds, a_node->rightChild, (crackAxis+1)%NUMDIMS, startXBound, endXBound, midPoint, endYBound);
            // buildTreeBatch(xbounds, ybounds, a_node->leftChild, (crackAxis+1)%NUMDIMS, startXBound, endXBound, startYBound, midPoint);
            int middle = (middle1 - ybounds->begin());
            buildTreeBatchOldMediocre(xbounds, ybounds, a_node->rightChild, (crackAxis+1)%NUMDIMS, startXBound, endXBound, middle, endYBound);
            buildTreeBatchOldMediocre(xbounds, ybounds, a_node->leftChild, (crackAxis+1)%NUMDIMS, startXBound, endXBound, startYBound, middle);
        }

        // cout << "is Leaf " << a_node->leafness << endl;
    }
}

void AKDTree::buildTreeBatchOldMediocreStructurally(vector<ELEMTYPE> *xbounds, vector<ELEMTYPE> *ybounds, KDNode* a_node, int crackAxis, int startXBound, int endXBound, int startYBound, int endYBound){

    int numOfSamples = 5;
    vector<ELEMTYPE> rndXBound, rndYBound;
    // vector<int> rndXBound, rndYBound;
    float mediocre = 0.0;

    // Need a terminating condition. Will use levels for now
    if(a_node->level > 5)
        return;

    cout << "LEVEL--------------------- " << a_node->level << " " << crackAxis << " " << a_node->dataArrayLeftPointer << " " << a_node->dataArrayRightPointer << endl;

    if(crackAxis == 0)
    {
        // cout << "axis is 0" << endl;
        // cout << "startXBound = " << startXBound << " endXBound = " << endXBound << " xbounds size " << xbounds->size() << endl;
        // for(int i = 0; i < numOfSamples; ++i)
        // {
        //     rndXBound.push_back(xbounds->at(startXBound + ( std::rand() % ( endXBound - startXBound +1  ) )));
        //     // rndXBound.push_back(startXBound + ( std::rand() % ( endXBound - startXBound +1  ) ));
        // }
        // // cout << " after for \n";
        // size_t n = numOfSamples / 2;
        // nth_element(rndXBound.begin(), rndXBound.begin()+n, rndXBound.end());

        #ifdef TIMEBREAKDOWN
            clock_t mediocreStartTime = clock();
        #endif

        rndXBound.push_back(xbounds->at(startXBound + ( std::rand() % ( endXBound - startXBound +1  ) )));
        rndXBound.push_back(xbounds->at(startXBound + ( std::rand() % ( endXBound - startXBound +1  ) )));
        rndXBound.push_back(xbounds->at(startXBound + ( std::rand() % ( endXBound - startXBound +1  ) )));


        ELEMTYPE midItem = max(min(rndXBound[0],rndXBound[1]), min(max(rndXBound[0],rndXBound[1]),rndXBound[2]));

        // cout << "middle item " << midItem << endl;
                

        // ELEMTYPE midItem = rndXBound[n];
        // cout << "after nth \n";
        auto middle1 = std::partition(xbounds->begin() + startXBound, xbounds->begin() + endXBound, [midItem](const auto& em)
        {
            return em < midItem;
        });

        #ifdef TIMEBREAKDOWN
            mediocreTime += clock() - mediocreStartTime;
            clock_t splitStartTime = clock();
        #endif
        SplitNodeStructurally(a_node, midItem, crackAxis);
        #ifdef TIMEBREAKDOWN
            splitTime += clock() - splitStartTime;
        #endif
        // SplitNodeInPlace(a_node, xbounds->at(rndXBound[n]), crackAxis);

        // int midPoint = (startXBound + endXBound ) / 2;
        if(!a_node->leafness)
        {
            // buildTreeBatch(xbounds, ybounds, a_node->rightChild, (crackAxis+1)%NUMDIMS, midPoint, endXBound, startYBound, endYBound);
            // buildTreeBatch(xbounds, ybounds, a_node->leftChild, (crackAxis+1)%NUMDIMS, startXBound, midPoint, startYBound, endYBound);
            int middle = (middle1 - xbounds->begin());
            // cout << "after middle " << middle << " " << middle1 - xbounds->begin() << endl;
            buildTreeBatchOldMediocreStructurally(xbounds, ybounds, a_node->rightChild, (crackAxis+1)%NUMDIMS, middle, endXBound, startYBound, endYBound);
            buildTreeBatchOldMediocreStructurally(xbounds, ybounds, a_node->leftChild, (crackAxis+1)%NUMDIMS, startXBound, middle, startYBound, endYBound);
        }
    }
    else
    {
        // cout << "axis is NOT 0" << endl;
        // for(int i = 0; i < numOfSamples; ++i)
        // {
        //     rndYBound.push_back(ybounds->at(startYBound + ( std::rand() % ( endYBound - startYBound +1  ) )));
        //     //  rndYBound.push_back(startYBound + ( std::rand() % ( endYBound - startYBound +1  ) ));
        // }
        // size_t n = numOfSamples / 2;
        // nth_element(rndYBound.begin(), rndYBound.begin()+n, rndYBound.end());

        #ifdef TIMEBREAKDOWN
            clock_t mediocreStartTime = clock();
        #endif
        
        rndYBound.push_back(ybounds->at(startYBound + ( std::rand() % ( endYBound - startYBound +1  ) )));
        rndYBound.push_back(ybounds->at(startYBound + ( std::rand() % ( endYBound - startYBound +1  ) )));
        rndYBound.push_back(ybounds->at(startYBound + ( std::rand() % ( endYBound - startYBound +1  ) )));


        ELEMTYPE midItem = max(min(rndYBound[0],rndYBound[1]), min(max(rndYBound[0],rndYBound[1]),rndYBound[2]));

        // ELEMTYPE midItem = rndYBound[n];
        auto middle1 = std::partition(ybounds->begin() + startYBound, ybounds->begin() + endYBound, [midItem](const auto& em)
        {
            return em < midItem;
        });
        // cout << "Splitting y on " << ybounds->at(rndYBound[n]) << endl;
        #ifdef TIMEBREAKDOWN
            mediocreTime += clock() - mediocreStartTime;
            clock_t splitStartTime = clock();
        #endif
        SplitNodeStructurally(a_node, midItem, crackAxis);
        // SplitNodeInPlace(a_node, ybounds->at(rndYBound[n]), crackAxis);
        #ifdef TIMEBREAKDOWN
            splitTime += clock() - splitStartTime;
        #endif

        int midPoint = (startYBound + endYBound ) / 2;

        if(!a_node->leafness)
        {
            // buildTreeBatch(xbounds, ybounds, a_node->rightChild, (crackAxis+1)%NUMDIMS, startXBound, endXBound, midPoint, endYBound);
            // buildTreeBatch(xbounds, ybounds, a_node->leftChild, (crackAxis+1)%NUMDIMS, startXBound, endXBound, startYBound, midPoint);
            int middle = (middle1 - ybounds->begin());
            buildTreeBatchOldMediocreStructurally(xbounds, ybounds, a_node->rightChild, (crackAxis+1)%NUMDIMS, startXBound, endXBound, middle, endYBound);
            buildTreeBatchOldMediocreStructurally(xbounds, ybounds, a_node->leftChild, (crackAxis+1)%NUMDIMS, startXBound, endXBound, startYBound, middle);
        }

        // cout << "is Leaf " << a_node->leafness << endl;
    }
}


void AKDTree::buildTreeOnRangeRec(KDNode* thisNode, ELEMTYPE* minVals, ELEMTYPE* maxVals, int axis, int howManyLevelsLeft, int* minGrid, int* maxGrid, unordered_map<int, KDNode*> *gridToLeafMap, int xLen){

    if(howManyLevelsLeft == 0){
        // stop
        // we also need to fill the map wth the pointers to this leaf and its corresponding grid cell
        // first assert that the mins and max grids only have one different
        if(minGrid[0] + 1 != maxGrid[0] || minGrid[1] + 1 != maxGrid[1]){
            cout << "did not gop as planned..., tragedy,death,war, all that." << endl;
            cout << minGrid[0] << " " << minGrid[1] << " " << maxGrid[0] << " " << maxGrid[1] << endl;
            exit(5);
        }
        // (*gridToLeafMap)[make_pair(minGrid[0], minGrid[1])] = thisNode;
        
        (*gridToLeafMap)[minGrid[0] * xLen + minGrid[1]] = thisNode;
        return;
    }

    thisNode->axis = axis;
    SplitNodeStructurally(thisNode, (minVals[axis] + maxVals[axis]) / 2);

    ELEMTYPE* rightMinVals = (ELEMTYPE*) malloc(NUMDIMS * sizeof(ELEMTYPE));
    ELEMTYPE* rightMaxVals = (ELEMTYPE*) malloc(NUMDIMS * sizeof(ELEMTYPE));
    ELEMTYPE* leftMinVals = (ELEMTYPE*) malloc(NUMDIMS * sizeof(ELEMTYPE));
    ELEMTYPE* leftMaxVals = (ELEMTYPE*) malloc(NUMDIMS * sizeof(ELEMTYPE));

    rightMinVals[axis] =  (minVals[axis] + maxVals[axis]) / 2;
    rightMinVals[1 - axis] = minVals[1 - axis];
    rightMaxVals[axis] =  maxVals[axis];
    rightMaxVals[1 - axis] = maxVals[1 - axis];

    leftMaxVals[axis] =  (minVals[axis] + maxVals[axis]) / 2;
    leftMaxVals[1 - axis] = maxVals[1 - axis];
    leftMinVals[axis] =  minVals[axis];
    leftMinVals[1 - axis] = minVals[1 - axis];


    int* rightMinGrid = (int*) malloc(NUMDIMS * sizeof(int));
    int* rightMaxGrid = (int*) malloc(NUMDIMS * sizeof(int));
    int* leftMinGrid = (int*) malloc(NUMDIMS * sizeof(int));
    int* leftMaxGrid = (int*) malloc(NUMDIMS * sizeof(int));

    rightMinGrid[axis] =  (minGrid[axis] + maxGrid[axis]) / 2;
    rightMinGrid[1 - axis] = minGrid[1 - axis];
    rightMaxGrid[axis] =  maxGrid[axis];
    rightMaxGrid[1 - axis] = maxGrid[1 - axis];

    leftMaxGrid[axis] =  (minGrid[axis] + maxGrid[axis]) / 2;
    leftMaxGrid[1 - axis] = maxGrid[1 - axis];
    leftMinGrid[axis] =  minGrid[axis];
    leftMinGrid[1 - axis] = minGrid[1 - axis];


    buildTreeOnRangeRec(thisNode->rightChild, rightMinVals, rightMaxVals, 1 - axis, howManyLevelsLeft - 1, rightMinGrid, rightMaxGrid, gridToLeafMap, xLen);
    buildTreeOnRangeRec(thisNode->leftChild, leftMinVals, leftMaxVals, 1 - axis, howManyLevelsLeft - 1, leftMinGrid, leftMaxGrid, gridToLeafMap, xLen);

}


void AKDTree::preBuildTreeHashed(int howManyLevels, ELEMTYPE minx, ELEMTYPE miny, ELEMTYPE maxx, ELEMTYPE maxy, vector<Point> *data){
    // !!!! assume that howManylevels is always divisible by NUMDIMS
    // !! ONLY FOR 2d


    // first we will build the tree structurally
    // while doing that we will also keep a hash map of (i, j) pairs to the KDNode of the leaves
    // besically mapping the grid to the KD-tree


    // then we will pass through the data and with 2 divisions find the (i, j) it belongs to and count the population of each leaf
    // then another scan throgh the data to put them in the correct order in the array

    // the partitions in each dim will be range/(2^hml)
    double partition_steps[2] = {(maxx - minx) / (pow(2, howManyLevels / 2)), (maxy - miny) / (pow(2, howManyLevels / 2))};
    #ifdef DEBUG
    cout << "partition steps: " << partition_steps[0] << " " << partition_steps[1] << endl;
    #endif

    ELEMTYPE* minVals = (ELEMTYPE*) malloc(NUMDIMS * sizeof(ELEMTYPE));
    ELEMTYPE* maxVals = (ELEMTYPE*) malloc(NUMDIMS * sizeof(ELEMTYPE));
    minVals[0] = minx; minVals[1] = miny;
    maxVals[0] = maxx; maxVals[1] = maxy;


    int* minGrid = (int*) malloc(NUMDIMS * sizeof(int));
    int* maxGrid = (int*) malloc(NUMDIMS * sizeof(int));
    minGrid[0] = 0; minGrid[1] = 0;
    maxGrid[0] = pow(2, howManyLevels/2); maxGrid[1] = pow(2, howManyLevels / 2);

    unordered_map<int, KDNode*> gridToLeafMap;

    #ifdef DEBUG
    cout << "before buildTree" << endl;
    #endif

    #ifdef TIMEBREAKDOWN
    clock_t begin = clock();
    #endif

    // build the tree structure
    buildTreeOnRangeRec(&m_root, minVals, maxVals, 0, howManyLevels, minGrid, maxGrid, &gridToLeafMap, maxGrid[0]);

    #ifdef TIMEBREAKDOWN
    clock_t end = clock() - begin;
    cerr << "buildTreeStucture " << (double)end/CLOCKS_PER_SEC << endl;
    #endif


    #ifdef DEBUG
    // sanity check
    for(int i = 0; i < maxGrid[0]; ++i){
        for(int j = 0; j < maxGrid[1]; ++j){
            if(gridToLeafMap[i * maxGrid[0] + j] == NULL){
                cout << "FOUND A NULLLLLLL!!!!!!!!!!!!!!!!!!!!!!!!!! " << i << " " << j << endl;
                exit(3);
            }
        }
    }
    #endif

    #ifdef DEBUG
    cout << "after buildTree" << endl;
    #endif

    #ifdef TIMEBREAKDOWN
    begin = clock();
    #endif

    unordered_map<KDNode*, int, KDNodeHashFunction> leafCounts;
    int thisI, thisJ;
    pair<int, int> thisPosition;
    for(int i = 0; i < DATA_COUNT; ++i){
        // find where it belongs
        thisI = (int) ((data->at(i).m_values[0] - minVals[0]) / partition_steps[0]);
        thisJ = (int) ((data->at(i).m_values[1] - minVals[1]) / partition_steps[1]);
        // now we get the kdNpode from the grid map
        // then ++ the count of thst node
        // TODO: how do pairs work? is it with pointers? are they not going to be equal now??
        // #ifdef DEBUG
        // cout << data->at(i).m_values[0] << " " << partition_steps[0] << " " << (int) (data->at(i).m_values[0] / partition_steps[0]) << endl;
        // cout << data->at(i).m_values[1] << " " << partition_steps[1] << " " << (int) (data->at(i).m_values[1] / partition_steps[1]) << endl;
        // cout << "leafCounts of " << thisPosition.first << " " << thisPosition.second << " = " << leafCounts[gridToLeafMap[thisPosition]] << endl;
        // cout << "leafCounts of " << gridToLeafMap[thisPosition]->id << endl;
        // // cout << "coutning for leaf " << thisPosition.first << " " << thisPosition.second << " " <<  gridToLeafMap[thisPosition]->id << endl;
        // #endif

        thisI = (thisI == maxGrid[0]) ? thisI -1 : thisI;
        thisJ = (thisJ == maxGrid[1]) ? thisJ -1 : thisJ;

        // leafCounts[gridToLeafMap[thisPosition]]++;
        leafCounts[gridToLeafMap[thisI * maxGrid[0] + thisJ]]++;
        
    }

    #ifdef TIMEBREAKDOWN
    end = clock() - begin;
    cerr << "CountingForLeaves " << (double)end/CLOCKS_PER_SEC << endl;
    #endif

    #ifdef DEBUG
    cout << "after counting" << endl;
    #endif

    // thwen we have to make those acc array pointer positions thinggs to the start of each of theser leaves in the array
    // it's just that they are already in m_data_arr...
    // what do i do now?...TODO

    #ifdef TIMEBREAKDOWN
    begin = clock();
    #endif

    unordered_map<KDNode*, int, KDNodeHashFunction> leafPointers;
    int tmp = 0;

    #ifdef DEBUG
    cout << leafCounts.size() << endl;
    #endif
    
    for(auto it = leafCounts.begin(); it != leafCounts.end(); it++){
        #ifdef DEBUG
        cout << "leaf "  << endl << it->second << endl;
        if(it->first == NULL) cout << "first is NULL" << endl;
        #endif
        leafPointers[it->first] = tmp;
        (it->first)->dataArrayLeftPointer = tmp;
        (it->first)->dataArrayRightPointer = tmp + it->second;
        tmp += it->second;
    }

    #ifdef TIMEBREAKDOWN
    end = clock() - begin;
    cerr << "LeafPointers " << (double)end/CLOCKS_PER_SEC << endl;
    #endif

    #ifdef DEBUG
    cout << "after p[ointers]" << endl;
    #endif

    #ifdef TIMEBREAKDOWN
    begin = clock();
    #endif

    int this_pointer;
    for(int i = 0; i < DATA_COUNT; ++i){
        // find where it belongs
        thisI = (int) ((data->at(i).m_values[0] - minVals[0]) / partition_steps[0]);
        thisJ = (int) ((data->at(i).m_values[1] - minVals[1]) / partition_steps[1]);

        thisI = (thisI == maxGrid[0]) ? thisI -1 : thisI;
        thisJ = (thisJ == maxGrid[1]) ? thisJ -1 : thisJ;

        this_pointer = leafPointers[gridToLeafMap[thisI * maxGrid[0] + thisJ]];
        m_data_arr[this_pointer][0] =  data->at(i).m_values[0];
        m_data_arr[this_pointer][1] =  data->at(i).m_values[1];

        leafPointers[gridToLeafMap[thisI * maxGrid[0] + thisJ]]++;
    }

    #ifdef TIMEBREAKDOWN
    end = clock() - begin;
    cerr << "copyData " << (double)end/CLOCKS_PER_SEC << endl;
    #endif


    #ifdef DEBUG
    cout << "after copying" << endl;
    #endif

      

}

void AKDTree::preBuildTreeHashed_v2(int howManyLevels, ELEMTYPE minx, ELEMTYPE miny, ELEMTYPE maxx, ELEMTYPE maxy, vector<Point> *data){
    // !!!! assume that howManylevels is always divisible by NUMDIMS
    // !! ONLY FOR 2d


    // first we will build the tree structurally
    // while doing that we will also keep a hash map of (i, j) pairs to the KDNode of the leaves
    // besically mapping the grid to the KD-tree


    // then we will pass through the data and with 2 divisions find the (i, j) it belongs to and count the population of each leaf
    // then another scan throgh the data to put them in the correct order in the array

    // the partitions in each dim will be range/(2^hml)
    double partition_steps[2] = {(maxx - minx) / (pow(2, howManyLevels / 2)), (maxy - miny) / (pow(2, howManyLevels / 2))};
    #ifdef DEBUG
    cout << "partition steps: " << partition_steps[0] << " " << partition_steps[1] << endl;
    #endif

    ELEMTYPE* minVals = (ELEMTYPE*) malloc(NUMDIMS * sizeof(ELEMTYPE));
    ELEMTYPE* maxVals = (ELEMTYPE*) malloc(NUMDIMS * sizeof(ELEMTYPE));
    minVals[0] = minx; minVals[1] = miny;
    maxVals[0] = maxx; maxVals[1] = maxy;


    int* minGrid = (int*) malloc(NUMDIMS * sizeof(int));
    int* maxGrid = (int*) malloc(NUMDIMS * sizeof(int));
    minGrid[0] = 0; minGrid[1] = 0;
    maxGrid[0] = pow(2, howManyLevels/2); maxGrid[1] = pow(2, howManyLevels / 2);

    unordered_map<int, KDNode*> gridToLeafMap;

    #ifdef DEBUG
    cout << "before buildTree" << endl;
    #endif

    #ifdef TIMEBREAKDOWN
    clock_t begin = clock();
    #endif

    // build the tree structure
    buildTreeOnRangeRec(&m_root, minVals, maxVals, 0, howManyLevels, minGrid, maxGrid, &gridToLeafMap, maxGrid[0]);

    #ifdef TIMEBREAKDOWN
    clock_t end = clock() - begin;
    cerr << "buildTreeStucture " << (double)end/CLOCKS_PER_SEC << endl;
    #endif


    #ifdef DEBUG
    // sanity check
    for(int i = 0; i < maxGrid[0]; ++i){
        for(int j = 0; j < maxGrid[1]; ++j){
            if(gridToLeafMap[i * maxGrid[0] + j] == NULL){
                cout << "FOUND A NULLLLLLL!!!!!!!!!!!!!!!!!!!!!!!!!! " << i << " " << j << endl;
                exit(3);
            }
        }
    }
    #endif

    #ifdef DEBUG
    cout << "after buildTree" << endl;
    #endif

    #ifdef TIMEBREAKDOWN
    begin = clock();
    #endif

    // unordered_map<KDNode*, int, KDNodeHashFunction> leafCounts;
    unordered_map<int, int> leafCounts;
    int thisI, thisJ;
    pair<int, int> thisPosition;
    for(int i = 0; i < DATA_COUNT; ++i){
        // find where it belongs
        thisI = (int) ((data->at(i).m_values[0] - minVals[0]) / partition_steps[0]);
        thisJ = (int) ((data->at(i).m_values[1] - minVals[1]) / partition_steps[1]);
        // now we get the kdNpode from the grid map
        // then ++ the count of thst node
        // TODO: how do pairs work? is it with pointers? are they not going to be equal now??
        // #ifdef DEBUG
        // cout << data->at(i).m_values[0] << " " << partition_steps[0] << " " << (int) (data->at(i).m_values[0] / partition_steps[0]) << endl;
        // cout << data->at(i).m_values[1] << " " << partition_steps[1] << " " << (int) (data->at(i).m_values[1] / partition_steps[1]) << endl;
        // cout << "leafCounts of " << thisPosition.first << " " << thisPosition.second << " = " << leafCounts[gridToLeafMap[thisPosition]] << endl;
        // cout << "leafCounts of " << gridToLeafMap[thisPosition]->id << endl;
        // // cout << "coutning for leaf " << thisPosition.first << " " << thisPosition.second << " " <<  gridToLeafMap[thisPosition]->id << endl;
        // #endif

        thisI = (thisI == maxGrid[0]) ? thisI -1 : thisI;
        thisJ = (thisJ == maxGrid[1]) ? thisJ -1 : thisJ;

        // leafCounts[gridToLeafMap[thisPosition]]++;
        // leafCounts[gridToLeafMap[thisI * maxGrid[0] + thisJ]]++;
        leafCounts[thisI * maxGrid[0] + thisJ]++;
        
    }

    #ifdef TIMEBREAKDOWN
    end = clock() - begin;
    cerr << "CountingForLeaves " << (double)end/CLOCKS_PER_SEC << endl;
    #endif

    #ifdef DEBUG
    cout << "after counting" << endl;
    #endif

    // thwen we have to make those acc array pointer positions thinggs to the start of each of theser leaves in the array
    // it's just that they are already in m_data_arr...
    // what do i do now?...TODO

    #ifdef TIMEBREAKDOWN
    begin = clock();
    #endif

    // unordered_map<KDNode*, int, KDNodeHashFunction> leafPointers;
    unordered_map<int, int> leafPointers;
    int tmp = 0;

    #ifdef DEBUG
    cout << leafCounts.size() << endl;
    #endif
    
    for(auto it = leafCounts.begin(); it != leafCounts.end(); it++){
        #ifdef DEBUG
        cout << "leaf "  << endl << it->second << endl;
        // if(it->first == NULL) cout << "first is NULL" << endl;
        #endif
        leafPointers[it->first] = tmp;
        gridToLeafMap[it->first]->dataArrayLeftPointer = tmp;
        gridToLeafMap[it->first]->dataArrayRightPointer = tmp + it->second;
        tmp += it->second;
    }

    #ifdef TIMEBREAKDOWN
    end = clock() - begin;
    cerr << "LeafPointers " << (double)end/CLOCKS_PER_SEC << endl;
    #endif

    #ifdef DEBUG
    cout << "after p[ointers]" << endl;
    #endif

    #ifdef TIMEBREAKDOWN
    begin = clock();
    #endif

    int this_pointer;
    for(int i = 0; i < DATA_COUNT; ++i){
        // find where it belongs
        thisI = (int) ((data->at(i).m_values[0] - minVals[0]) / partition_steps[0]);
        thisJ = (int) ((data->at(i).m_values[1] - minVals[1]) / partition_steps[1]);

        thisI = (thisI == maxGrid[0]) ? thisI -1 : thisI;
        thisJ = (thisJ == maxGrid[1]) ? thisJ -1 : thisJ;

        // this_pointer = leafPointers[gridToLeafMap[thisI * maxGrid[0] + thisJ]];
        this_pointer = leafPointers[thisI * maxGrid[0] + thisJ];
        m_data_arr[this_pointer][0] =  data->at(i).m_values[0];
        m_data_arr[this_pointer][1] =  data->at(i).m_values[1];

        leafPointers[thisI * maxGrid[0] + thisJ]++;
    }

    #ifdef TIMEBREAKDOWN
    end = clock() - begin;
    cerr << "copyData " << (double)end/CLOCKS_PER_SEC << endl;
    #endif


    #ifdef DEBUG
    cout << "after copying" << endl;
    #endif

      

}
