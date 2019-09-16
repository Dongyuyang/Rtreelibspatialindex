#pragma once

#include <SpatialIndex.h>
using namespace SpatialIndex;
using namespace std;

class nnVisitor : public IVisitor
{
public:
	uint32_t Id;
	uint32_t m_indexIO;
	uint32_t m_leafIO;
	Point nn;

public:
	nnVisitor(): Id(0), m_indexIO(0), m_leafIO(0) {}

	uint32_t getIdentifierId(){
		return this->Id;
	}

	void visitNode(const INode& n){
		if(n.isLeaf()) m_leafIO++;
		else m_indexIO++;
	}

	void visitData(const IData& d){
		//cout << d.getIdentifier() << endl;
		this->Id = d.getIdentifier();

		IShape* pS;
		d.getShape(&pS);
		const Point *p = dynamic_cast<const Point *>(pS);
		if(p != NULL) this->nn = *p;
		else {
			const Region *pr = dynamic_cast<const Region *>(pS);
			if(pr != NULL){
				Point p2(pr->m_pLow, pr->m_dimension);
				this->nn = p2;
			}
		}
		delete pS;
	}

	void visitData(std::vector<const IData*>& v){
	}
};

class MyVisitor:public IVisitor
{
public:
	uint32_t m_indexIO;
	uint32_t m_leafIO;
	vector <Point> m_kNNs;  

public:
	MyVisitor() : m_indexIO(0), m_leafIO(0) {}

	void visitNode(const INode& n) {
		if (n.isLeaf()) m_leafIO++;
		else m_indexIO++;
	}

	void visitData(const IData& d) {
		IShape* pS;
		d.getShape(&pS);
		const Point *p = dynamic_cast<const Point *>(pS);
		if(p != NULL){
			m_kNNs.push_back(*p);
		}
		else{
			const Region *pr = dynamic_cast<const Region *>(pS);
			if (pr != NULL){
	 			//uint32_t dim = pr->m_dimension;
	 			//double * pCoords = pr->m_pLow;
	 			Point p2(pr->m_pLow, pr->m_dimension);
		 		m_kNNs.push_back(p2);
			}
		}	
		delete pS;
	}

	void visitData(std::vector<const IData*>& v) {
		//cout << v[0]->getIdentifier() << " " << v[1]->getIdentifier() << endl;
	}
};
