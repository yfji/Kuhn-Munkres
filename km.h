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
	KM() {};
	~KM();

	int N;
	float max_w;

	float* weights;
	float* flagX;
	float* flagY;
	char* usedX;
	char* usedY;
	int* matchX;
	int* matchY;

	void init(float* data, int m, int n);
	void constructMatrix(float* data, int m, int n);
	bool dfs(int v);
	void km();
};

static PyObject* KM_getMatch(KM* self, PyObject* pArgs);
static PyObject* maxWeights(KM* self, PyObject* pArgs) {
	return Py_BuildValue("f", self->max_w);
}