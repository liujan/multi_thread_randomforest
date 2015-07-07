#include "RandomForest.h"
#include "fstream"
#include "Node.h"
#include "Tree.h"
#include "memory.h"
#include "pthread.h"
using namespace std;

extern const int numOfFeatures;
extern const int numOfTypes;
extern const int numOfTest;
extern const int numOfThread;
int result[numOfThread][numOfTest][numOfTypes + 1];

struct M{
	RandomForest *rf;
	int i;
};

RandomForest::RandomForest(){
	memset(result, 0, sizeof(result));
}


//训练一棵决策树，然后预测结果
void* build(void *arg){
	M t1 = (M)(*((M*)arg));
	RandomForest *rf = t1.rf;
	int t = t1.i;
	if (rf->test_samples.empty()){
		cout << "[Error] test sample is empty" << endl;
		return NULL;
	}

	Tree *tree = new Tree();
	if (tree->readSample(rf->all_samples)){
		cout << t  << "  ";
		tree->buildTree();
		for (int j = 0; j < rf->test_samples.size(); j++){
			int id = rf->test_samples[j].first;
			int type = tree->predict(rf->test_samples[j].second);
			result[t][id][type]++;
		}
	}
	delete tree;
	
	return NULL;
}

//训练num棵决策树，并预测结果
bool RandomForest::buildTree(int num){
	if (all_samples.empty()){
		cout << "[Error] all sample is empty" << endl;
		return false;
	}

	memset(result, 0, sizeof(result));
	memset(r, 0, sizeof(r));

	for (int i = 0; i < num; i+=numOfThread){
		pthread_t thread[numOfThread];
		M *m = new M[numOfThread];
		//memset(flag, false, sizeof(flag));
		for (int j = 0; j < numOfThread; j++){
			m[j].rf = this;
			m[j].i = j;
			pthread_create(&thread[j], NULL, build, &(m[j]));
		}
		for (int j = 0; j < numOfThread; j++)
			pthread_join(thread[j], NULL);
		
		delete[] m;
	}
	

	for (int i = 0; i < numOfTest; i++){
		for (int j = 0; j < numOfTypes + 1; j++){
			for (int k = 0; k < numOfThread; k++){
				r[i][j] += result[k][i][j];
			}
		}
	}
	
	return true;
}


//将预测结果归总，并写入到文件中
void RandomForest::predict(){
	if (this->test_samples.empty()){
		cout << "[Error] test sample is empty" << endl;
		return;
	}
	ofstream out("result.csv");
	if (!out.is_open()){
		cout << "[Error] failed to open file result.csv" << endl;
		return;
	}
	cout << "predicting ..." << endl;
	out << "Id,label" << "\n";

	for (int i = 0; i < numOfTest; i++){
		int max_value = -1000, index = 0;
		for (int j = 1; j < numOfTypes + 1; j++){
			if (r[i][j] > max_value){
				max_value = r[i][j];
				index = j;
			}
		}
		out << i << "," << index << "\n";
	}

	out.close();
	cout << "predicting completed" << endl;
}


bool RandomForest::readTrainSample(string file){
	ifstream train("train.csv");
	if (!train.is_open()){
		cout << "[Error] : Failed to open file " << file << endl;
		return false;
	}
	cout << "reading sample ..." << endl;

	string s;
	getline(train, s); //过滤掉第一行标签

	double value;
	char dot;
	int id;
	int label;
	while (true) {
		train >> id;
		if (train.eof())
			break;
		
		
		vector<double> features;
		for (int i = 0; i < numOfFeatures; i++){
			train >> dot;
			train >> value;
			features.push_back(value);
		}
		train >> dot;
		train >> label;
		all_samples.push_back(make_pair(label, features));

		if (train.peek() == '\n')
			train.get();
	}

	train.close();
	cout << "reading sample successful" << endl;
	return true;
}

bool RandomForest::readTestSample(string file){
	ifstream test("test.csv");
	if (!test.is_open()){
		cout << "[Error] : Failed to open file " << file << endl;
		return false;
	}
	cout << "reading test sample ..." << endl;
	test_samples.clear();
	string s;
	getline(test, s); //过滤掉第一行标签

	double value;
	char dot;
	int id;

	while (true) {
		test >> id;
		if (test.eof())
			break;

		vector<double> features;
		for (int i = 0; i < numOfFeatures; i++){
			test >> dot;
			test >> value;
			features.push_back(value);
		}

		test_samples.push_back(make_pair(id, features));

		if (test.peek() == '\n')
			test.get();
	}

	test.close();
	cout << "reading test sample successful" << endl;
	return true;
}

RandomForest::~RandomForest(){
}
