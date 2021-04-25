Westwood=[0,1,0.803845 ,2,
0.985957 ,3,
0.990677 ,4,
0.995397 ,5,
1.41292 ,6]
Hybla=[0,1,0.601008 ,2,
0.775696 ,3,
0.884128 ,4,
0.888848 ,5,
1.29894 ,6]

Vegas=[0,1,0.701008 ,2,
0.775486 ,3,
0.880416 ,4,
0.985136 ,5,
1.30266 ,6]

Scalable=[0,1,0.701008 ,2,
0.875696 ,3,
0.880416 ,4,
0.885136 ,5]

NewReno=[0,1,0.797296 ,2,
0.809408 ,3,
0.854128 ,4,
0.888848 ,5,
1.36947 ,6]

hyb=5
ren=5
scal=4
west=5

W=[]
H=[]
V=[]
S=[]
R=[]
W1=[]
H1=[]
V1=[]
S1=[]
R1=[]

import matplotlib.pyplot as plt

for i in range(6):
  W.append(Westwood[2*i])
  H.append(Hybla[2*i])
  R.append(NewReno[2*i])
  V.append(Vegas[2*i])
  W1.append(Westwood[2*i+1]-1)
  H1.append(Hybla[2*i+1]-1)
  R1.append(NewReno[2*i+1]-1)
  V1.append(Vegas[2*i+1]-1)

for i in range(5):
  S.append(Scalable[2*i])
  S1.append(Scalable[2*i+1]-1)

plt.plot(W, W1, label='Westwood')
#plt.xlabel('Time in seconds')
#plt.ylabel('No. of packets lost in the specific time interval')
#plt.title('TCP Westwood')


plt.plot(R, R1, label='NewReno')
#plt.xlabel('Time in seconds')
#plt.ylabel('No. of packets lost in the specific time interval')
#plt.title('TCP NewReno')


plt.plot(H,H1 ,label='Hybla')
#plt.xlabel('Time in seconds')
#plt.ylabel('No. of packets lost in the specific time interval')
#plt.title('TCP Hybla')


plt.plot(S,S1, label='Scalable')
#plt.xlabel('Time in seconds')
#plt.ylabel('No. of packets lost in the specific time interval')
#plt.title('TCP Scalable')


plt.plot(V, V1, label='Vegas')
plt.xlabel('Time in seconds')
plt.ylabel('No. of packets/segments lost')
plt.title('Cumulative segments lost over time')
plt.legend(loc='upper left')
plt.show()