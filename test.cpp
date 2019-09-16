#include <SpatialIndex.h>
#include "common.h"
#include "rangeQuery.hpp"

#define CAPACITY 10
#define FACTOR 0.5
#define DIM 2
#define LOOPNUM 1
#define PNUM 10000
#define RANGE 0.1

int main()
{
    auto P = genPoints(DIM,PNUM,1,2);
    auto Q = genPoints(DIM,LOOPNUM,1,1);
    displayPset(Q);
    int N = P.size();
    //CATCH RTR, NAI;

    try {
		IStorageManager* memfile = StorageManager::createNewMemoryStorageManager();
		StorageManager::IBuffer* file = StorageManager::createNewRandomEvictionsBuffer(*memfile, 10, false);
		id_type indexIdentifier;
		ISpatialIndex* tree = RTree::createNewRTree(*file, FACTOR, CAPACITY, CAPACITY, DIM, SpatialIndex::RTree::RV_RSTAR, indexIdentifier);
		id_type id = 0;
		for(uint32_t i = 0; i < N; ++i){
                std::ostringstream os;
                os << P[i];
                std::string data = os.str();
                tree->insertData(data.size() + 1,
                                 reinterpret_cast<const byte*>(data.c_str()), P[i], id);
                id++;
        }


        /* Run!!! */

	//RTR.catch_time();
        for(size_t loop = 0; loop < LOOPNUM; loop++){
            auto nnID = nearestNeighbor(tree, Q[loop]);
            displayCoordinates(P[nnID]);
	    //MyRangeQueryStrategy mrqs(Q[loop],RANGE);
	    //tree->queryStrategy(mrqs);
		
        }
        //RTR.catch_time();
        //std::cout << "RTR time: " << RTR.get_cost(2) << " millsecond(s)" << std::endl;

        //NAI.catch_time();
        for(size_t loop = 0; loop < LOOPNUM; loop++){
            auto nnid = brute_NN(Q[loop], P);
	    //brute_range(Q[loop], P, RANGE);
            displayCoordinates(P[nnid]);
        }
        //NAI.catch_time();
        //std::cout << "NAI time: " << NAI.get_cost(2) << " millsecond(s)" << std::endl;

        delete tree;
        delete file;
        delete memfile;

    } catch(Tools::Exception& e) {
		cerr << "*********ERROR**********" << endl;
		std::string s = e.what();
		cerr << s << endl;
		return -1;
	} catch(...) {
		cerr << "**********ERROR********" << endl;
		return -1;
	}

    return 1;
}


