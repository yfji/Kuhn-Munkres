import KM
import numpy as np
from time import time

w=np.array([
    [3,5,5,4,1],
    [2,2,0,2,2],
    [2,4,4,1,0],
    [1,2,1,3,3]], np.float32).ravel().tolist()

tic=time()
km=KM.KM(w,4,5)
km.compute()
toc=time()

print('KM costs {}s'.format(toc-tic))
print(km)   #print the weight matrix

match=km.getMatch(True)
print('Kuhn Munkres match:')
print(match)
mw=km.maxWeight()
print('Max matching weights: {}'.format(mw))