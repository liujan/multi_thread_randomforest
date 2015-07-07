#ifndef __RandomForest__RandomForest__
#define __RandomForest__RandomForest__


#include "iostream"
#include "vector"
#include "string"
#include "Node.h"
#include "map"
using namespace std;
class Tree;
extern const int numOfTypes;
extern const int numOfTest;

class RandomForest{
private:
	vector<pair<int, vector<double> > > all_samples; //<label ,features> ,训练样本
	vector<pair<int, vector<double> > > test_samples; //<id, features>，预测样本
	int r[numOfTest][numOfTypes+1];
public:
	bool readTrainSample(string file);
	bool buildTree(int num);
	~RandomForest();
	bool readTestSample(string test);
	void predict();
	RandomForest();
	friend void* build(void *arg);

};

#endif /* defined(__RandomForest__RandomForest__) */
