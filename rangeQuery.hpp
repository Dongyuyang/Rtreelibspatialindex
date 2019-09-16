#ifndef _RANGEQUERY_HPP_
#define _RANGEQUERY_HPP_
#include "common.h"

class NNEntry
{
public:
    id_type m_id;
    double m_minDist;

    NNEntry(id_type id, double amindist) : m_id(id), m_minDist(amindist) {}
    ~NNEntry() {}
    struct ascending : public std::binary_function<NNEntry*, NNEntry*, bool>{
	bool operator()(const NNEntry* __x, const NNEntry* __y) const
	{return __x->m_minDist > __y->m_minDist; }
    };
};

class MyRangeQueryStrategy : public SpatialIndex::IQueryStrategy
{
private:
    std::priority_queue<NNEntry*, std::vector<NNEntry*>, NNEntry::ascending> m_queue;
    Point m_q;
    double m_r;

public:
    int indexIO = 0;
    int leafIO = 0;
    bool hit = false;
    std::vector<Point> result;

    MyRangeQueryStrategy(const Point &q, double r) : m_q(q), m_r(r) {}

    void getNextEntry(const IEntry &entry, id_type& nextEntry, bool& hasNext)
    {
	const INode *node = dynamic_cast<const INode*>(&entry);

	if(node->isIndex()){ // internal node
	    indexIO++;
	    //foreach entry of this node
	    for(uint32_t cChild = 0; cChild < node->getChildrenCount(); cChild++){		
		IShape* pS;
		node->getChildShape(cChild, &pS);
		const Region *pr = dynamic_cast<const Region *>(pS);
		if(pr != NULL){
		    double currentdist = pr->getMinimumDistance(m_q);
		    if(currentdist <= m_r)
			m_queue.push(new NNEntry(node->getChildIdentifier(cChild), currentdist)); 
		}
		delete pS;
	    }
	} else { // leaf node
	    leafIO++;
	    //foreach point in leaf node
	    for(uint32_t cChild = 0; cChild < node->getChildrenCount(); cChild++){
		IShape* pS;
		node->getChildShape(cChild, &pS);
		const Region *pr = dynamic_cast<const Region *>(pS);
		if(pr != NULL){
		    uint32_t dim = pr->m_dimension;
		    double * pCoords = pr->m_pLow;
		    Point p = Point(pCoords, dim);		    
		    double currentdist = pr->getMinimumDistance(m_q);
		    if(currentdist <= m_r){
			//std::cout << "dist: " << currentdist << std::endl; 
			if(!hit)
			    hit = true;
			delete pS;
			break;
			//result.push_back(p);
			//displayCoordinates(p);
			
		    }
		}
		delete pS;
	    } 
	} // if internal node else leaf node

	//traversal tree
	if (! m_queue.empty() && !hit){
	    // get next entry N_j from list]
	    NNEntry* pFirst = m_queue.top();
	    m_queue.pop();
	    nextEntry =  pFirst->m_id;
	    hasNext = true;
	    double currentdist = pFirst->m_minDist; 
	    if (currentdist < m_r){
		// Recursion
		//break;
	    } else { //out of m_r
		hasNext = false;
		//break;
	    }
	} else {	//if(m_queue.empty())
	    hasNext = false;
	}
	
    } //getNextEntry
};



#endif /* _RANGEQUERY_HPP_ */
