#include <SpatialIndex.h>
#include <ctime>
#include <cstdlib>
#include "common.h"
#include "rangeQuery.hpp"


#define CAPACITY 50
#define FACTOR 0.5
//#define DIM 6
double SETNUM=1000;
double SETSIZE=10000;
double DIM = 10;

int NUM = SETNUM * SETSIZE + 1;
double Tcell = 0.8;
double Tset = SETSIZE * 0.6;
double Tdist = std::sqrt(DIM) * (1 - Tcell);
	

void runRtree(const std::vector<std::vector<Point> > &P,
			  const std::vector<Point> &Q)
{
	int totalindexIO = 0;
	int totalleafIO = 0;
	double totaltime = 0.0;
	double totalindextime = 0.0;
	std::vector<int> joinablesetid;

	int match = 0;

	int pid = 0;
	for(auto &pset : P){
		try {
			/* build rtree index */
			CATCH INIT;
			INIT.catch_time();
			IStorageManager* memfile = StorageManager::createNewMemoryStorageManager();
			StorageManager::IBuffer* file = StorageManager::createNewRandomEvictionsBuffer(*memfile, 10, false);
			id_type indexIdentifier;
			ISpatialIndex* tree = RTree::createNewRTree(*file, FACTOR, CAPACITY, CAPACITY, DIM, SpatialIndex::RTree::RV_RSTAR, indexIdentifier);
			id_type id = 0;
			for(uint32_t i = 0; i < pset.size(); ++i){
				std::ostringstream os;
				os << pset[i];
				std::string data = os.str();
				tree->insertData(data.size() + 1,
								 reinterpret_cast<const byte*>(data.c_str()), pset[i], id);
				id++;
			}
			INIT.catch_time();
			totalindextime += INIT.get_cost(2);


			/* Run!!! */
			CATCH RTR;
			RTR.catch_time();

			for(int loop = 0; loop < 100; loop++){
				//std::cout << "loop: " << loop << std::endl;
				auto QQ = genPoints(DIM, SETSIZE, 1, rand());
				for(auto &q : QQ){				
					MyRangeQueryStrategy mrqs(q,Tdist);
					tree->queryStrategy(mrqs);
					totalindexIO += mrqs.indexIO;
					totalleafIO += mrqs.leafIO;

					if(!mrqs.result.empty())
						match++;
				
					if(match >= Tset){
						joinablesetid.push_back(pid);
						break;
					}
				}
			}//loop
	   
				
			pid++;
			RTR.catch_time();
			totaltime += RTR.get_cost(2);
			
			delete tree;
			delete file;
			delete memfile;
			
			//std::cout << "time: " << pid << std::endl;


		} catch(Tools::Exception& e) {
			cerr << "*********ERROR**********" << endl;
			std::string s = e.what();
			cerr << s << endl;
			//return -1;
		} catch(...) {
			cerr << "**********ERROR********" << endl;
			//return -1;
		}
	}// each p

	std::cout << "INIT time: " << totalindextime << " millsecond(s)" << std::endl;
	std::cout << "IndexACC#: " << double(totalindexIO) / 100
			  << "   LeafACC#: " << double(totalleafIO) / 100
			  << std::endl;
	std::cout << "RTR time: " << totaltime / 100 << " millsecond(s)" << std::endl;
}

std::vector<std::vector<Point> > splitPoints(const std::vector<Point> &P, int setnum, int setsize)
{
	std::vector<std::vector<Point> > result;

	int begin = 0;
	for(int i = 0; i < setnum; i++){
		std::vector<Point> pset;
		for( ; begin < setsize + i*setsize; begin++){
			pset.push_back(P[begin]);
		}
		result.push_back(pset);
	}

	return result;
}

int main()
{
	srand(time(0));

	std::cout << "###setting###" << std::endl;
	std::cout << "SETNUM: " << SETNUM << std::endl;
	std::cout << "SETSIZE: " << SETSIZE << std::endl;
	std::cout << "Tcell: " << Tcell
			  << "  Tset: " << (double)Tset/SETSIZE << std::endl;

	std::vector<int> dims = {32,64,128};
	
	for(int t = 0; t < 3; t++){
		DIM = dims[t];
		std::cout << "DIM: "<< DIM << std::endl;
		auto P = genPoints(DIM,NUM,1,rand());
		auto sets = splitPoints(P,SETNUM, SETSIZE);
		auto Q = genPoints(DIM,SETSIZE,1,rand());
		runRtree(sets,Q);
	}
	
    //displayPset(Q);

	/*Naive*/
    // CATCH NAI;
	// NAI.catch_time();
	// int naicount = 0;
	// for(size_t loop = 0; loop < LOOPNUM; loop++){
	// 	//auto nnid = brute_NN(Q[loop], P);
	// 	naicount += brute_range(Q[loop], P, RANGE);
	// 	//displayCoordinates(P[nnid]);
	// }
	// NAI.catch_time();
	// std::cout << "NAI #: " << naicount << std::endl;
	// std::cout << "NAI time: " << NAI.get_cost(2) << " millsecond(s)" << std::endl;

    return 1;
}


