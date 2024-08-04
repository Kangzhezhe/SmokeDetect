import matplotlib.pyplot as plt
import numpy as np
import os
os.chdir(os.path.dirname(__file__))
def read_data(path):
    with open(path,'r') as f:
        lines = f.readlines()
    line = [l.strip().split(',') for l in lines ]
    data = np.array(line)
    data = data[:,1:].astype(np.float32)
    return data

data = read_data('output/output.txt')
data = data[:,:-1]
# plt.plot(data)
# plt.xlabel('time')
# plt.ylabel('data')
# plt.legend(['$C_v$', '$C_s$', '$d_{32}$'])
# plt.show()

plt.rcParams['font.sans-serif']=['SimHei'] #用来正常显示中文标签
plt.rcParams['axes.unicode_minus']=False #用来正常显示负号 #有中文出现的情况，需要u'内容
normal = data[0:55,:]       #cv
smoke = data[110:165,:]      #vs
water = data[390:445,:]      #d32
# import ipdb;ipdb.set_trace()
# cv = np.hstack((normal[:,0].reshape(-1,1), smoke[:,0].reshape(-1,1), water[:,0].reshape(-1,1)))
# plt.plot(cv)
# plt.title('不同情况下体积浓度增量$C_v$随时间变化')
# plt.xlabel('time')
# plt.ylabel(u'体积浓度$C_v$')
# plt.legend(['normal', 'smoke', 'water'])
# plt.show()

# cs = np.hstack((normal[:,1].reshape(-1,1), smoke[:,1].reshape(-1,1), water[:,1].reshape(-1,1)))
# plt.plot(cs)
# plt.title('不同情况下表面积浓度增量$C_s$随时间变化')
# plt.xlabel('time')
# plt.ylabel(u'体积浓度$C_s$')
# plt.legend(['normal', 'smoke', 'water'])
# plt.show()

d32 = np.hstack((normal[:,2].reshape(-1,1), smoke[:,2].reshape(-1,1), water[:,2].reshape(-1,1)))
plt.plot(d32)
plt.title('不同情况下Sauter平均粒径$d_{32}$随时间变化')
plt.xlabel('time')
plt.ylabel(u'体积浓度$C_s$')
plt.legend(['normal', 'smoke', 'water'])
plt.show()