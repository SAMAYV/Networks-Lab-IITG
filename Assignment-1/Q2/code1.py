import matplotlib.pyplot as plt
from pylab import MaxNLocator


# x axis values
x = [64,256,512,768,1024,1280,1536,1792,2048]
# corresponding y axis values
y = [189.173,180.710,174.433,198.399,198.420,212.349,320.631,353.784,350.795]
y1 = [202.694,209.226,194.802,193.756,180.637,201.340,349.374,389.181,379.531]
y2 = [215.989,213.496,199.902,175.759,184.200,190.697,277.521,346.711,338.367]

# plotting the points
plt.plot(x, y, color='green',linewidth = 3,
         marker='o', markerfacecolor='black', markersize=6, label='2am')
plt.plot(x, y1, color='orange',linewidth = 3,
         marker='o', markerfacecolor='black', markersize=6, label='8pm')
plt.plot(x, y2, color='brown',linewidth = 3,
         marker='o', markerfacecolor='black', markersize=6, label='2pm')

#plt.xlim(0,24)
#plt.ylim(26,31)

plt.xlabel('Packet Size in bytes',color="red")
plt.ylabel('Average RTT in ms',color="red")
plt.legend() 
# giving a title to my graph
plt.title('Average RTT VS Packet Size for vidyabharatischool.org',color="magenta")
 
# function to show the plot
plt.show()