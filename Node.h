#ifndef __RandomForest__Node__
#define __RandomForest__Node__

#include <stdio.h>
#include "memory.h"
#include "vector"
using namespace std;

const int numOfTypes = 26; //类别数
const int numOfFeatures = 617;
const int numOfTest=1559;
const int numOfThread = 8;
const int maxDepth = 12;

struct Node{
	int attribute; //µ±Ç°œÚµãÑ¡È¡ÄÄžöÊôÐÔÀŽœøÐÐ·ÖÁÑ
	double value; //µ±Ç°œÚµãÑ¡È¡ÊôÐÔµÄÄÄžöÖµÀŽœøÐÐ·ÖÁÑ
	Node *left; //×ó×ÓœÚµã
	Node *right;  //ÓÒ×ÓœÚµã
	int sum; //µ±Ç°œÚµãµÄÑù±Ÿ×ÜÊý
	int num[numOfTypes]; //µ±Ç°œÚµãÃ¿ÀàµÄÑù±ŸÊý
	int depth;
	vector<pair<int, vector<double> > > samples; //<label, features>
	Node();

	//¿œ±Ž¹¹Ôìº¯Êý
	Node(const Node &other);

};

#endif /* defined(__RandomForest__Node__) */
