#include "km.h"
#include <sstream>
using namespace std;

// float* weights;
// float* flagX;
// float* flagY;
// char* usedX;
// char* usedY;
// int* matchX;
// int* matchY;
static PyMemberDef KM_DataMembers[] =
{
	   {(char*)("max_w"),   T_FLOAT, offsetof(KM, max_w),   0, (char*)("Max weight")},
	   {(char*)("N"),    T_INT,  offsetof(KM, N),    0, (char*)("Max number of m and n")},
	   {(char*)("weights"), T_OBJECT,  offsetof(KM, weights), 0, (char*)("Weights of each node pair")},
	   {(char*)("flagX"),   T_OBJECT,  offsetof(KM, flagX),  0, (char*)("Flags of X nodes")},
	   {(char*)("flagY"), T_OBJECT, offsetof(KM, flagY),  0, (char*)("Flags of Y nodes") },
       {(char*)("usedX"), T_OBJECT, offsetof(KM, usedX),  0, (char*)("Use of X nodes")},
	   {(char*)("usedY"), T_OBJECT, offsetof(KM, usedY),  0, (char*)("Use of Y nodes") },
	   {(char*)("matchX"), T_OBJECT, offsetof(KM, matchX),  0, (char*)("Match of X nodes") },
	   {(char*)("matchY"), T_OBJECT, offsetof(KM, matchY),  0, (char*)("Match of Y nodes")},
	   {NULL, NULL, NULL, 0, NULL}
};


void KM::init(float* data, int m, int n){
	N = max(m, n);
	weights = new float[N * N];
	flagX = new float[N];
	flagY = new float[N];
	usedX = new char[N];
	usedY = new char[N];
	matchX = new int[N];
	matchY = new int[N];
	max_w = 0.0;

	constructMatrix(data, m, n);
	km();
}

KM::~KM() {
	
}
void KM::constructMatrix(float* data, int m, int n) {
	for (int i = 0; i < m; ++i) {
		for (int j = 0; j < n; ++j) {
			weights[i*N + j] = data[i*m + j];
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

void KM::km() {
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
		data[i] = static_cast<float>(PyFloat_AsDouble(PyList_GetItem(pyData, i)));
	}
	self->init(data, m, n);
	Py_DECREF(pyData);
	delete[] data;
}

static void KM_Destruct(KM* self) {
	delete[] self->weights;
	delete[] self->flagX;
	delete[] self->flagY;
	delete[] self->usedX;
	delete[] self->usedY;
	delete[] self->matchX;
	delete[] self->matchY;

	Py_TYPE(self)->tp_free((PyObject*)self);
}

static PyObject* KM_Str(KM* self)
{
	stringstream ostr;
	ostr << "Kuhn Munkres algorithm for "<<self->N<< "X nodes and "<<self->N<<" Y nodes"<< endl;
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

static PyObject* KM_Repr(KM* self)            //����repr���ú���ʱ�Զ�����.
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
	for (int i = 0; i < self->N; ++i) {
		if (xy == 0) {
			PyList_Append(list, PyLong_FromLong(self->matchX[i]));
		}
		else {
			PyList_Append(list, PyLong_FromLong(self->matchY[i]));
		}
	}
	return list;
}

static PyMethodDef KM_Methods[] = {
	{"getMatch", (PyCFunction)KM_getMatch, METH_VARARGS, "Get the result of Kuhn-Munkres"},
    {"maxWeights", (PyCFunction)maxWeights, METH_VARARGS, "Get the max matching weights"},
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
	   PyVarObject_HEAD_INIT(NULL, 0)"Module.MyCppClass",                 //����ͨ��__class__�������ַ���. CPP��������.__name__��ȡ.
	   sizeof(KM),0,(destructor)KM_Destruct,    //�����������.
	   0,0,0,0,
	   (reprfunc)KM_Repr,          //repr ���ú������á�
	   0,0,0,0,0,
	   (reprfunc)KM_Str,          //Str/print���ú�������.
	   0,0,0,
	   Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,                 //���û���ṩ�����Ļ���ΪPy_TPFLAGS_DEFAULE
	   "MyCppClass Objects---Extensioned by C++!",                   //__doc__,��/�ṹ��DocString.
	   0,0,0,0,0,0,
	   KM_Methods,        //������з�������.
	   KM_DataMembers,          //����������ݳ�Ա����.
	   0,0,0,0,0,0,
	   (initproc)KM_init,      //��Ĺ��캯��.
	   0,
};



PyMODINIT_FUNC           // == __decslpec(dllexport) PyObject*, ���嵼������.
PyInit_KM(void)       //ģ���ⲿ����Ϊ--CppClass
{
	PyObject* pReturn = 0;
	KM_ClassInfo.tp_new = PyType_GenericNew;       //�����new���ú�������������.

	if (PyType_Ready(&KM_ClassInfo) < 0)
		return NULL;

	pReturn = PyModule_Create(&ModuleInfo);          //����ģ����Ϣ����ģ��.
	if (pReturn == 0)
		return NULL;

	Py_INCREF(&ModuleInfo);
	PyModule_AddObject(pReturn, "KM", (PyObject*)&KM_ClassInfo); //���������뵽ģ���Dictionary��.
	return pReturn;
}