import matplotlib.pyplot as plt
from pylab import MaxNLocator


# x axis values
x = [1,5,8,11,14,17,21]
# corresponding y axis values
y = [85,85,84,84,85,85,86]

# plotting the points
plt.plot(x, y, color='green',linewidth = 3,
         marker='o', markerfacecolor='blue', markersize=6)

#plt.xlim(0,24)
#plt.ylim(26,31)

plt.xlabel('Time - Different Hours in a Day (in Hours)',color="red")
plt.ylabel('Number of Online Hosts',color="red")
# giving a title to my graph
plt.title('Graph to find Hourly trends of Online Hosts',color="magenta")
 
# function to show the plot
plt.show()