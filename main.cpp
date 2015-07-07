#include "iostream"
#include "fstream"
#include "math.h"
#include "stdio.h"
#include "string"
#include "RandomForest.h"
#include "stdlib.h"
#include "RandomForest.h"
#include "ctime"
using namespace std;

void fun(){
	ifstream in("train.csv");
	ofstream out("train1.csv");

	string s;
	getline(in, s);
	out << s << endl;
	while (1){
		if (in.eof())
			break;
		getline(in, s);
		double p = rand() * 1.0 / RAND_MAX;
		if (p <= 0.1)
			out << s << endl;
		if (in.peek() == '\n')
			in.get();
	}
	in.close();
	out.close();
}

int main(int argc,  char * argv[]) {
	// insert code here...
	
	clock_t start, end;
	start = clock();

	RandomForest forest;
	forest.readTrainSample("train.csv");
	forest.readTestSample("test.csv");
	forest.buildTree(120);
	forest.predict();
	end = clock();
	cout << "runint time is " << (end - start) / CLOCKS_PER_SEC << " second" <<  endl;
	return 0;
}
