import numpy as np
from time import time
import KM

# w=np.array([
#     [3,5,5,4,1],
#     [2,2,0,2,2],
#     [2,4,4,1,0],
#     [1,2,1,3,3]], np.float32).ravel().tolist()
w=np.array([11.2472,0.229506,0.0382638,0.0381432,0.0397027,0.0313909,
0.224794,8.14484,0.0458661,0.0453376,0.0466399,0.0367264,
0.0299137,0.0354885,0.163514,0.149832,0.138921,0.0995749,
0.0395173,0.0463618,21.4613,0.963952,0.436966,0.255043,
0.041079,0.0473524,0.46014,0.818429,15.3917,0.8489,
0.0330598,0.0378731,0.264884,0.380127,0.944165,26.4972,
0.0391057,0.0455497,1.07156,9.93479,0.726293,0.35456,
0.0355863,0.0427534,0.317523,0.236992,0.185949,0.124624], np.float32).tolist()

tic=time()
km=KM.KM(w,8,6)
km.compute()
toc=time()

print('KM costs {}s'.format(toc-tic))
# print(km)   #print the weight matrix

match=km.getMatch(False)
print('Kuhn Munkres match:')
print(match)
mw=km.maxWeight()
print('Max matching weights: {}'.format(mw))