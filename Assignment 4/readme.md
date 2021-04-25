An ns3 simulation script to compare various TCP variants under congestion

Topology:
 * Topology consists of two nodes N0 and N1 connected by a link  of bandwidth 1Mbps and link delay 10ms. We are using a drop-tail queue at the link.
 * n0 ----------- n1

Simulation:
There is a TCP agent at N0 which creates FTP traffic destined for N1. There are 5 CBR traffic agents of rate 300 Kbps each at N0 destined for N1.
The timing of the flows is as follows:
 * FTP starts at 0 sec and continues till the end of simulation
 * CBR1 starts at 200 ms and continues till end
 * CBR2 starts at 400 ms and continues till end
 * CBR3 starts at 600 ms and stops at 1200 ms
 * CBR4 starts at 800 ms and stops at 1400 ms
 * CBR5 starts at 1000 ms and stops at 1600 ms
 * Simulation runs for 1800 ms

Installation Instructions:
Steps:
 * Install ns3 and put ns3.cc in the scratch directory.
 * Run the script using $```./waf --run "ns3"``` if want to use default tcp type which is equal to TcpWestwood in our ns3.cc code.
 * Run using $```./waf --run "ns3 --tcp_prot=tcp_type"``` where tcp_type is equal to one of the following values: TcpNewReno, TcpHybla, TcpVegas, TcpWestwood, TcpScalable and ns3 is name of our file and tcp_prot stores the tcp type in our ns3.cc code.
 * For other command line options and help run $```./waf --run "ns3 --PrintHelp"```

Plots: 
 * "Congestion Window vs Time" plot is stored in the file named "cwnd_stats.png" file
 * "Total Bytes vs Time" plot is stored in the file named "bytes_rcvd.png" file
 * "Packet Drops vs Time" plot is stored in the file named "dropped_packets.png" file

Note:
 * On running the code, the Flow Monitor Statistics of the corresponding TCP Type used (eg TcpNewReno, TcpHybla etc) in ns3.cc code get stored in the file named tcp_type.txt where tcp_type is equal to one of the following values: TcpNewReno, TcpHybla, TcpVegas, TcpWestwood, TcpScalable.