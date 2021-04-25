import math
import matplotlib.pyplot as plt
import statistics

lis = [float(x) for x in input("Enter numbers:").split()]
print(statistics.median(lis))
print(statistics.mean(lis))
print(min(lis))
print(max(lis))
plt.hist(lis, bins=400, rwidth=2)
plt.ylabel('Frequency')
plt.xlabel('Latency in ms')
plt.title('$ping -p ff00 81.27.240.126')
plt.show()