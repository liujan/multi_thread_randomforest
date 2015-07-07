#include <stdio.h>
#include "Tree.h"
#include "fstream"
#include "iostream"
#include "vector"
#include "memory"
#include "math.h"
#include "stdio.h"
#include "map"
#include "set"
#include "queue"
#include "string"
#include "stdlib.h"
using namespace std;

extern const int numOfFeatures;
extern const int numOfTypes;
extern const int maxDepth;

Tree::Tree(){
	root = new Node();
}

bool Tree::readSample(vector<pair<int, vector<double> > > all_samples){
	if (all_samples.empty()){
		cout << "[Error] all sample is empty!" << endl;
		return false;
	}

	if (root == NULL)
		root = new Node();

	double p = 0.0;
	for (int i = 0; i < all_samples.size(); i++){
		p = rand() * 1.0 / RAND_MAX;
		if (p <= 0.6) {  //每棵树用总样本的60%来训练
			root->samples.push_back(all_samples[i]);
			int type = all_samples[i].first;
			root->num[type - 1]++;
		}
	}
	root->sum = root->samples.size();

	return true;
}

bool Tree::buildTree(){

	if (root == NULL)
		return false;

	cout << "building tree ..." << endl;

	double p = 0.7;  //当一个节点里有p的样本为同一类型，则该节点不再进行分裂
	queue<Node *> q; //用于辅助非递归建树
	q.push(root);
	int sum = 0; //记录一共分裂了多少次
	while (!q.empty()) {
		Node * current = q.front();
		q.pop();
		if (current == NULL)
			continue;

		for (int i = 0; i < numOfTypes; i++){ //当一个节点里有p的样本为同一类型，则该节点不再进行分裂
			if ((current->num[i] * 1.0 / current->sum) > p)
				continue;
		}
		if (current->depth >= maxDepth) //当当前节点的高度大于5时，当前节点不再进行分裂
			continue;

		if (current->sum < 50) //当一个节点里样本数小于50时，该节点不再进行分裂
			continue;

		//cout <<"depth = " <<  current->depth << " bin = " << sum << endl;
		pair<Node *, Node *> child = this->selectBestPart(current);
		Node *left = child.first;
		Node *right = child.second;

		current->left = left;
		current->right = right;

		q.push(left);
		q.push(right);
		sum++;
	}

	cout << "building tree completed" << endl;
	return true;
}
int getType(Node *node){
	if (node == NULL)
		return 0;
	int max_value = -1000, index = 0;
	for (int i = 0; i < numOfTypes; i++){
		if (max_value <= node->num[i]){
			max_value = node->num[i];
			index = i + 1;
		}
	}
	return index;
}

int Tree::predict(vector<double> features){
	if (root == NULL){
		cout << "[Error] empty tree" << endl;
		return 0;
	}
	Node *head = root;
	while (true) {
		if (head->left == NULL && head->right == NULL){
			return getType(head);
		}
		int attr = head->attribute;
		double value = head->value;
		if (features[attr] < value){
			if (head->left == NULL)
				return getType(head);
			else
				head = head->left;
		}
		else {
			if (head->right == NULL)
				return getType(head);
			else
				head = head->right;
		}
	}

}


//选出当前节点向下分裂的最好的分裂属性及其取值
pair<Node*, Node*> Tree::selectBestPart(Node *current){
	if (current == NULL)
		return pair<Node *, Node *>(NULL, NULL);

	vector<int> features = this->randomSelect(); //随机选出所有属性的60%作为分裂的候选属性
	vector<pair<int, vector<double> > > samples = current->samples;
	map<int, set<double> > values; //保存每个候选属性所能取到的值
	map<int, pair<double, double> > max_min; //保存每个候选属性的最大取值和最小取值

	for (int j = 0; j < features.size(); j++){
		int index = features[j];
		max_min[index].first = -1000;
		max_min[index].second = 1000;
	}

	//拿到所有候选属性的最大取值和最小取值
	for (int i = 0; i < samples.size(); i++){
		for (int j = 0; j < features.size(); j++){
			int index = features[j];
			double value = samples[i].second[index];

			if (value < max_min[index].second)
				max_min[index].second = value;
			if (value > max_min[index].first)
				max_min[index].first = value;
		}
	}


	map<int, pair<double, double> >::iterator it_value = max_min.begin();
	int numOfPart = 20;
	for (; it_value != max_min.end(); it_value++){
		double max = it_value->second.first;
		double min = it_value->second.second;
		int index = it_value->first;
		double step = (max - min) / numOfPart;
		if ((max - min) < 1e-5) {
			values[index].insert(min);
			continue;
		}
		for (int i = 1; i < numOfPart; i++){
			values[index].insert(min + step * i);
		}
	}


	//选出最好分裂属性的最好取值
	map<pair<int, double>, double> gini;
	map<int, set<double> >::iterator it = values.begin();

	for (; it != values.end(); it++){
		int attr = it->first;
		set<double>::iterator  it1 = it->second.begin();

		for (; it1 != it->second.end(); it1++){
			double value = *it1;
			int small[numOfTypes + 1];
			int large[numOfTypes + 1];
			int small_sum = 0;
			int large_sum = 0;
			memset(small, 0, sizeof(small));
			memset(large, 0, sizeof(large));
			for (int i = 0; i < samples.size(); i++){
				int type = samples[i].first;
				if (samples[i].second[attr] < value) {
					small[type]++;
					small_sum++;
				}
				else {
					large[type]++;
					large_sum++;
				}
			}
			double sumgini_small = 0.0;
			double sumgini_large = 0.0;

			for (int i = 0; i < numOfTypes + 1; i++){
				sumgini_small += ((small[i] * 1.0 / small_sum) * (small[i] * 1.0 / small_sum));
				sumgini_large += ((large[i] * 1.0 / large_sum) * (large[i] * 1.0 / large_sum));
			}
			gini[make_pair(attr, value)] = small_sum * 1.0 / samples.size() * (1 - sumgini_small) + large_sum * 1.0 / samples.size() * (1 - sumgini_large);
		}
	}


	int attr = 0;
	double value = 0;
	double min_value = 10000;
	//出分裂后基尼系数最小的分裂属性的值
	map<pair<int, double>, double>::iterator it_gini = gini.begin();
	for (; it_gini != gini.end(); it_gini++){
		if (it_gini->second <= min_value){
			min_value = it_gini->second;
			attr = it_gini->first.first;
			value = it_gini->first.second;
		}
	}

	Node *small = new Node();
	Node *large = new Node();
	//根据上面找出的基尼系数最小的属性和值，分裂成两个节点
	for (int i = 0; i < samples.size(); i++){
		//cout << attr << " " << samples[i].second[attr] << " " << value << endl;
		if (samples[i].second[attr] <= value) {
			small->samples.push_back(samples[i]);
			int type = samples[i].first;
			small->num[type - 1]++;
			small->sum++;
		}
		else {
			large->samples.push_back(samples[i]);
			int type = samples[i].first;
			large->num[type - 1]++;
			large->sum++;
		}
	}

	current->attribute = attr;
	current->value = value;
	small->depth = current->depth + 1;
	large->depth = current->depth + 1;
	return pair<Node *, Node *>(small, large);
}

//随机选出所有属性的60%，并返回其下标
vector<int> Tree::randomSelect(){
	vector<int> v;
	for (int i = 0; i < numOfFeatures; i++){
		double p = rand() * 1.0 / RAND_MAX;
		if (p <= 0.05)
			v.push_back(i);
	}
	return v;
}

Tree::~Tree(){
	Node *head = root;
	queue<Node*>q;
	q.push(head);
	while (!q.empty()){
		head = q.front();
		q.pop();
		if (head == NULL)
			continue;
		Node *left = head->left;
		Node *right = head->right;
		delete head;
		q.push(left);
		q.push(right);
	}
}
