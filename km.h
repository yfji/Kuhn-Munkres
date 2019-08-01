#pragma once

#pragma once
#include <iostream>
#include <vector>
#include <algorithm>
#include <Python.h>
#include <pycapsule.h>
#include <structmember.h>
using namespace std;

class KM {
	PyObject_HEAD
public:
	//	KM(float* data, int m, int n);
	KM(float* data, int m, int n) {
		init(data, m, n);
	}

	~KM();

	int N;
	int front;
	int back;
	int* matchX;
	int* matchY;
	float* weights;

	void init(float* data, int m, int n);
	void del();
	void compute();
	float maxWeight() {
		return max_w;
	}
	vector<int> getMatch(bool front2back = true);

private:
	
	float max_w;	
	float* flagX;
	float* flagY;
	char* usedX;
	char* usedY;
	
	void constructMatrix(float* data, int m, int n);
	bool dfs(int v);
};
