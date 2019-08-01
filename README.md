# Kuhn Munkres algorithm for weighted bipartite graph matching
This is a C++ implement of the Kuhn Munkres (KM) algorithm. Besides, we provide a Python interface named KM.pyd compiled using VS2017 and Python3.6.

## Usage
* C++<br>
Construct the `KM` object using the weight array of float type.
```cpp
    float weights[] = { 3,5,5,4,1,2,2,0,2,2,2,4,4,1,0,1,2,1,3,3 };
    KM km(weights, 4, 5);   //KM(weight, rows, cols)
    km.compute();
    vector<int> match = km.getMatch(0);
    //If True, return the matched col nodes, else the matched row nodes
```
    
* Python<br>
Construct the `KM` object using the weight list, which is a 1-d view of the 2-d weight matrix. 
```python
w=np.array([
    [3,5,5,4,1],
    [2,2,0,2,2],
    [2,4,4,1,0],
    [1,2,1,3,3]], np.float32).ravel().tolist()

km=KM.KM(w,4,5) #KM(weight, rows, cols)
km.compute()
matches=km.getMatch(True)   
#If True, return the matched col nodes, else the matched row nodes
```

## Compile with Visual Studio
* Build a DLL project in VS
* Copy the `km.h` and `km.cpp` into your source folder
* Add the Python include and library path to the peoject. The `python36.lib` is used.
* Modify the project property. Set the target file name to `KM` and the target extension to `pyd`
* Build, then you get `xx.lib` and `KM.pyd`