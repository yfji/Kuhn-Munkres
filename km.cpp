#include "km.h"

#include <sstream>
using namespace std;

//float* weights;
//float* flagX;
//float* flagY;
//char* usedX;
//char* usedY;
//int* matchX;
//int* matchY;
static PyMemberDef KM_DataMembers[] =
{
	   {(char*)("N"),    T_INT,  offsetof(KM, N),    0, (char*)("Max number of m and n")},
	   {(char*)("weights"), T_OBJECT,  offsetof(KM, weights), 0, (char*)("Weights of each node pair")},
	   {(char*)("matchX"), T_OBJECT, offsetof(KM, matchX),  0, (char*)("Match of X nodes") },
	   {(char*)("matchY"), T_OBJECT, offsetof(KM, matchY),  0, (char*)("Match of Y nodes")},
	   {NULL, NULL, NULL, 0, NULL}
};


void KM::init(float* data, int m, int n) {
	N = max(m, n);
	front = m;
	back = n;
	weights = new float[N * N];
	flagX = new float[N];
	flagY = new float[N];
	usedX = new char[N];
	usedY = new char[N];
	matchX = new int[N];
	matchY = new int[N];
	max_w = 0.0;

	constructMatrix(data, m, n);
}

void KM::del() {
	delete[] weights;
	delete[] flagX;
	delete[] flagY;
	delete[] usedX;
	delete[] usedY;
	delete[] matchX;
	delete[] matchY;
}

KM::~KM() {
	// The destruction will not be called in tp_free(Python)
	del();
	//cout << "Destruct KM" << endl;
}

void KM::constructMatrix(float* data, int m, int n) {
	for (int i = 0; i < m; ++i) {
		for (int j = 0; j < n; ++j) {
			weights[i*N + j] = data[i*n + j];
		}
		for (int j = n; j < N; ++j) {
			weights[i*N + j] = 0.0;
		}
	}
	for (int i = m; i < N; ++i) {
		for (int j = 0; j < N; ++j) {
			weights[i*N + j] = 0.0;
		}
	}

	for (int i = 0; i < N; ++i) {
		flagX[i] = -1e7;
		flagY[i] = 0;
		for (int j = 0; j < N; ++j) {
			flagX[i] = max(flagX[i], weights[i*N + j]);
		}
		usedX[i] = 0;
		usedY[i] = 0;
		matchX[i] = -1;
		matchY[i] = -1;
	}
}

bool KM::dfs(int v) {
	usedX[v] = 1;
	float* data = weights + v * N;
	for (int i = 0; i < N; ++i) {
		if (!usedY[i] && flagX[v] + flagY[i] == data[i]) {
			usedY[i] = 1;
			if (matchY[i] == -1 || dfs(matchY[i])) {
				matchY[i] = v;
				matchX[v] = i;
				return true;
			}
		}
	}
	return false;
}

void KM::compute() {
	for (int i = 0; i < N; ++i) {
		while (true) {
			for (int i = 0; i < N; ++i) {
				usedX[i] = 0;
				usedY[i] = 0;
			}
			if (dfs(i))
				break;

			float d = 1e7;
			for (int j = 0; j < N; ++j) {
				if (!usedX[j])
					continue;
				for (int k = 0; k < N; ++k) {
					if (!usedY[k]) {
						d = min(d, flagX[j] + flagY[k] - weights[j*N + k]);
					}
				}
			}
			if (d == 0) {
				max_w = -1;
				cout << "No max weight match" << endl;
				return;
			}
			for (int j = 0; j < N; ++j) {
				if (usedX[j])
					flagX[j] -= d;
				if (usedY[j])
					flagY[j] += d;
			}
		}
	}
	for (int i = 0; i < N; ++i) {
		int j = matchX[i];
		if (j >= 0) {
			max_w += weights[i*N + j];
		}
	}
}

vector<int> KM::getMatch(bool front2back) {
	vector<int> matches;
	if (front2back) {
		for (int i = 0; i < front; ++i)
			matches.push_back(matchX[i]);
	}
	else {
		for (int i = 0; i < back; ++i)
			matches.push_back(matchY[i]);
	}
	return matches;
}

/*
*Python functions
*/

static void KM_init(KM* self, PyObject* pArgs) {
	PyObject* pyData = PyList_New(0);
	float* data;
	int m, n;
	int ok = PyArg_ParseTuple(pArgs, "Oii", &pyData, &m, &n);
	if (!ok) {
		cout << "PyObject parsing args error" << endl;
		return;
	}
	data = new float[m*n];
	for (int i = 0; i < m*n; ++i) {
		float x = static_cast<float>(PyFloat_AsDouble(PyList_GetItem(pyData, i)));
		data[i] = x;
	}
	self->init(data, m, n);
	Py_DECREF(pyData);
	delete[] data;
}

static void KM_Destruct(KM* self) {
	self->del();
	//cout << "Python destruct" << endl;
	Py_TYPE(self)->tp_free((PyObject*)self);
}

static PyObject* KM_Str(KM* self)
{
	stringstream ostr;
	ostr << "Kuhn Munkres algorithm for " << self->N << " X nodes and " << self->N << " Y nodes" << endl;
	for (int i = 0; i < self->N; ++i) {
		for (int j = 0; j < self->N; ++j) {
			ostr << self->weights[i*self->N + j];
			if (j < self->N - 1)
				ostr << ',';
			else
				ostr << endl;
		}
	}
	return Py_BuildValue("s", ostr.str().c_str());
}

static PyObject* KM_Repr(KM* self)            //调用repr内置函数时自动调用.
{
	return KM_Str(self);
}


static PyObject* KM_getMatch(KM* self, PyObject* pArgs) {
	vector<int> matches;
	PyObject* list = PyList_New(0);
	int xy;
	if (PyArg_ParseTuple(pArgs, "i", &xy) == 0) {
		return NULL;
	}
	if (xy == 1) {
		for (int i = 0; i < self->front; ++i)
			PyList_Append(list, PyLong_FromLong(self->matchX[i]));
	}
	else {
		for (int i = 0; i < self->back; ++i)
			PyList_Append(list, PyLong_FromLong(self->matchY[i]));
	}
	return list;
}

static PyObject* KM_maxWeight(KM* self, PyObject* pArgs) {
	return Py_BuildValue("f", self->maxWeight());
}

static void KM_compute(KM* self, PyObject* pArgs) {
	self->compute();
}

static PyMethodDef KM_Methods[] = {
	{"getMatch", (PyCFunction)KM_getMatch, METH_VARARGS, "Get the result of Kuhn-Munkres"},
	{"maxWeight", (PyCFunction)KM_maxWeight, METH_VARARGS, "Get the max matching weights"},
	{"compute", (PyCFunction)KM_compute, METH_VARARGS, "Compute the KM match" },
	{NULL, NULL}
};

static PyModuleDef ModuleInfo =
{
	PyModuleDef_HEAD_INIT,
	"Kuhn-Munkres C++ Class Module", //__name__
	"This Module Created By C++--extension a class to Python!",	//__doc__
	-1,
	NULL, NULL, NULL, NULL, NULL
};

static PyTypeObject KM_ClassInfo =
{
	   PyVarObject_HEAD_INIT(NULL, 0)"Module.MyCppClass",                 //可以通过__class__获得这个字符串. CPP可以用类.__name__获取.
	   sizeof(KM),0,(destructor)KM_Destruct,    //类的析构函数.
	   0,0,0,0,
	   (reprfunc)KM_Repr,          //repr 内置函数调用。
	   0,0,0,0,0,
	   (reprfunc)KM_Str,          //Str/print内置函数调用.
	   0,0,0,
	   Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,                 //如果没有提供方法的话，为Py_TPFLAGS_DEFAULE
	   "MyCppClass Objects---Extensioned by C++!",                   //__doc__,类/结构的DocString.
	   0,0,0,0,0,0,
	   KM_Methods,        //类的所有方法集合.
	   KM_DataMembers,          //类的所有数据成员集合.
	   0,0,0,0,0,0,
	   (initproc)KM_init,      //类的构造函数.
	   0,
};



PyMODINIT_FUNC           // == __decslpec(dllexport) PyObject*, 定义导出函数.
PyInit_KM(void)       //模块外部名称为--CppClass
{
	PyObject* pReturn = 0;
	KM_ClassInfo.tp_new = PyType_GenericNew;       //此类的new内置函数—建立对象.

	if (PyType_Ready(&KM_ClassInfo) < 0)
		return NULL;

	pReturn = PyModule_Create(&ModuleInfo);          //根据模块信息创建模块.
	if (pReturn == 0)
		return NULL;

	Py_INCREF(&ModuleInfo);
	PyModule_AddObject(pReturn, "KM", (PyObject*)&KM_ClassInfo); //将这个类加入到模块的Dictionary中.
	return pReturn;
}