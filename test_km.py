import KM
import numpy as np

w=np.array([
    [3,5,5,4,1],
    [2,2,0,2,2],
    [2,4,4,1,0],
    [0,1,1,0,0],
    [1,2,1,3,3]]).ravel().tolist()

km=KM.KM(w,5,5)
print(km)

match=km.getMatch(0)
print('Kuhn Munkres match:')
print(match)
mw=km.maxWeights()
print('Max matching weights: {}'.format(mw))