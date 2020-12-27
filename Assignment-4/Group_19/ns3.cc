#include <iostream>
#include <fstream>
#include <string>
#include "ns3/internet-module.h"
#include "ns3/core-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/tcp-header.h"
#include "ns3/udp-header.h"
#include "ns3/applications-module.h"
#include "ns3/network-module.h"
#include "ns3/packet-sink.h"
#include "ns3/error-model.h"
#include "ns3/enum.h"
#include "ns3/event-id.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "ns3/flow-monitor-helper.h"
#include "ns3/traffic-control-module.h"
#include "ns3/flow-monitor-module.h"

using namespace ns3;
using namespace std;

AsciiTraceHelper ascii;
Ptr<PacketSink> cbr[5], tcpSink;
int total_bytes_transfer = 0;
int total_packet_drops = 0;
bool first_drop = 1;
bool first = 1;

NS_LOG_COMPONENT_DEFINE ("Networks Lab Group 19");

// Function to record Congestion Window values (storing old congestion and new congestion window values)
// Can be used to find the slow start, congestion avoidance
static void CwndChange(Ptr<OutputStreamWrapper> stream, uint32_t oldCwnd, uint32_t newCwnd)
{
    *stream->GetStream () << Simulator::Now ().GetSeconds () << " ," << oldCwnd << " ," << newCwnd << ",\n";
}

// Function to record packet drops
// total_packet_drops is incremented each time the packet is dropped
static void RxDrop(Ptr<OutputStreamWrapper> stream, Ptr<const Packet> p)
{
    // first_drop variable is used to check if the packet drop is first or not
    if(first_drop){
        first_drop = 0;
        *stream->GetStream() << 0 << " ," << 0 << ",\n";
        total_packet_drops += 1;
    }
    else {
        total_packet_drops += 1;
        *stream->GetStream() << Simulator::Now().GetSeconds() << " ," << total_packet_drops << ",\n";
    }
}

// Function to find the total cumulative transfered bytes
// total_bytes_transfer is incremented each time the byte is transfered
static void TotalRx(Ptr<OutputStreamWrapper> stream)
{
    total_bytes_transfer += tcpSink->GetTotalRx();
    for(int i = 0; i < 5; i++){
        total_bytes_transfer += cbr[i]->GetTotalRx();
    }
    // first variable is used to check if the byte transfer is first or not
    if(first){
    	first = 0;
    }
    else {
    	*stream->GetStream() << Simulator::Now().GetSeconds() << " ," << total_bytes_transfer << ",\n";
    }
    Simulator::Schedule(Seconds(0.001),&TotalRx, stream);
}

// Trace Congestion window length
// Function to call CwndChange function with appropriate stream to write the value in
static void TraceCwnd(Ptr<OutputStreamWrapper> stream)
{
    //Trace changes to the congestion window
    Config::ConnectWithoutContext("/NodeList/0/$ns3::TcpL4Protocol/SocketList/0/CongestionWindow", MakeBoundCallback(&CwndChange,stream));
}

int main(int argc, char *argv[])
{
    CommandLine cmd;
    uint32_t maxBytes = 0;
    string tcp_prot = "TcpWestwood";

    // Start and End times for the 5 CBR traffic agents
    double start[5] = {0.2, 0.4, 0.6, 0.8, 1.0};
    double end[5]   = {1.8, 1.8, 1.2, 1.4, 1.6};

    // Allow the user to override any of the defaults values at run time, via command line arguments
    // Use ./waf --run "ns3 --maxBytes=value" for entering maxBytes where value is entered by user
    cmd.AddValue("maxBytes","Total number of bytes for application to send", maxBytes);

    // Use ./waf --run "ns3 --tcp_prot=specific_tcp_type" for running with specific tcp type where specific_tcp_type is the tcp type entered by user
    cmd.AddValue("tcp_prot", "TcpNewReno, TcpHybla, TcpVegas, TcpScalable, TcpWestwood", tcp_prot);
    cmd.Parse(argc, argv);

    // Selecting the TCP type 
    if(tcp_prot == "TcpNewReno"){
        Config::SetDefault("ns3::TcpL4Protocol::SocketType", TypeIdValue(TcpNewReno::GetTypeId()));
    }
    else if(tcp_prot == "TcpHybla"){
        Config::SetDefault("ns3::TcpL4Protocol::SocketType", TypeIdValue(TcpHybla::GetTypeId()));
    }
    else if(tcp_prot == "TcpVegas"){
        Config::SetDefault("ns3::TcpL4Protocol::SocketType", TypeIdValue(TcpVegas::GetTypeId()));
    }
    else if(tcp_prot == "TcpScalable"){
        Config::SetDefault ("ns3::TcpL4Protocol::SocketType", TypeIdValue(TcpScalable::GetTypeId()));
    }
    else if(tcp_prot == "TcpWestwood"){
        Config::SetDefault ("ns3::TcpL4Protocol::SocketType", TypeIdValue(TcpWestwood::GetTypeId()));
    }
    else {
        NS_LOG_DEBUG("Not Valid TCP Type");
        exit(1);
    }

    string str1 = tcp_prot + "_bytes_rcvd.dat";
    string str2 = tcp_prot + "_dropped_packets.dat";
    string str3 = tcp_prot + "_cwnd_stat.dat";

    // Create file streams for data storage
    Ptr<OutputStreamWrapper> bytes_rcvd = ascii.CreateFileStream(str1);
    Ptr<OutputStreamWrapper> dropped_packets = ascii.CreateFileStream(str2);
    Ptr<OutputStreamWrapper> cwnd_stat = ascii.CreateFileStream(str3);

    // Explicitly create the point-to-point link required by the topology (shown above).
    NS_LOG_INFO("Creating channels");

    // Creating point-to-point link with given Data rate 1Mbps and Delay 10ms
    PointToPointHelper pointToPoint;
    pointToPoint.SetDeviceAttribute("DataRate", StringValue("1Mbps"));
    pointToPoint.SetChannelAttribute("Delay", StringValue("10ms"));
    // Bandwidth * delay= 10^4 bits = 1250 Bytes and we have set packet size = 1250 Bytes
    pointToPoint.SetQueue("ns3::DropTailQueue","MaxSize", StringValue ("1p"));

    // Explicitly create the nodes required by the topology(shown above).
    NS_LOG_INFO("Creating nodes");
    NodeContainer nodes;
    nodes.Create(2);

    // Attaching nodes to the NetDeviceContainer
    NetDeviceContainer devices;
    devices = pointToPoint.Install(nodes);

    // Install the internet stack on the nodes
    InternetStackHelper internet;
    internet.Install(nodes);

    // Create error model
    Ptr<RateErrorModel> em = CreateObject<RateErrorModel> ();
    em->SetAttribute("ErrorRate", DoubleValue(0.00001));
    devices.Get(1)->SetAttribute("ReceiveErrorModel", PointerValue(em));

    // We've got the "hardware" in place. Now we need to add IP addresses.
    NS_LOG_INFO ("Assign IP Addresses.");
    Ipv4AddressHelper ipv4;
    ipv4.SetBase ("10.1.1.0", "255.255.255.0");
    Ipv4InterfaceContainer ipv4Container = ipv4.Assign(devices);

    // Create a BulkSendApplication and install it on node 0
    NS_LOG_INFO("Create Applications");
    uint16_t port = 12100;
    BulkSendHelper source("ns3::TcpSocketFactory",InetSocketAddress(ipv4Container.GetAddress(1), port));

    // Set the amount of data to send in bytes.  Zero is unlimited.
    source.SetAttribute("MaxBytes", UintegerValue(maxBytes));
    ApplicationContainer sourceApps = source.Install(nodes.Get (0));
    sourceApps.Start(Seconds(0.0));
    sourceApps.Stop(Seconds(1.80));

    // Create a PacketSinkApplication and install it on node 1
    PacketSinkHelper sink ("ns3::TcpSocketFactory", InetSocketAddress(Ipv4Address::GetAny(), port));
    ApplicationContainer sinkApps = sink.Install(nodes.Get(1));
    sinkApps.Start(Seconds(0.0));
    sinkApps.Stop(Seconds(1.80));

    tcpSink = DynamicCast<PacketSink>(sinkApps.Get(0));

    // creating a port
    uint16_t cbrPort = 12000;

    for(int i = 0; i < 5; i++){
        // Install applications: five CBR streams each saturating the channel
        ApplicationContainer cbrApps;
        ApplicationContainer cbrSinkApps;

        // cbrPort+i is used to get the different port numbers
        OnOffHelper onOffHelper("ns3::UdpSocketFactory", InetSocketAddress(ipv4Container.GetAddress(1), cbrPort+i));
        onOffHelper.SetAttribute("PacketSize", UintegerValue(1024));
        onOffHelper.SetAttribute("OnTime",  StringValue("ns3::ConstantRandomVariable[Constant=1]"));
        onOffHelper.SetAttribute("OffTime", StringValue("ns3::ConstantRandomVariable[Constant=0]"));

        onOffHelper.SetAttribute("DataRate", StringValue("300Kbps"));
        onOffHelper.SetAttribute("StartTime", TimeValue(Seconds(start[i])));
        onOffHelper.SetAttribute("StopTime", TimeValue(Seconds(end[i])));
        cbrApps.Add(onOffHelper.Install(nodes.Get(0)));
        // Packet sinks for each CBR agent

        PacketSinkHelper sink("ns3::UdpSocketFactory", InetSocketAddress(Ipv4Address::GetAny(), cbrPort+i));
        cbrSinkApps = sink.Install(nodes.Get(1));
        cbrSinkApps.Start(Seconds(0.0));
        cbrSinkApps.Stop(Seconds(1.8));
        cbr[i] = DynamicCast<PacketSink>(cbrSinkApps.Get(0));
    }

    // Keeping track of dropped packets using the RxDrop function
    devices.Get(1)->TraceConnectWithoutContext("PhyRxDrop", MakeBoundCallback(&RxDrop, dropped_packets));

    // Running the simulation
    NS_LOG_INFO("Run Simulation");
    Simulator::Schedule(Seconds(0.00001),&TotalRx,bytes_rcvd);
    Simulator::Schedule(Seconds(0.00001),&TraceCwnd,cwnd_stat);

    // Using Flow monitor for displaying various stats
    Ptr<FlowMonitor> flowMonitor;
    FlowMonitorHelper flowHelper;
    flowMonitor = flowHelper.InstallAll();

    Simulator::Stop(Seconds(1.80));
    Simulator::Run();

    Ptr<Ipv4FlowClassifier> classifier = DynamicCast<Ipv4FlowClassifier>(flowHelper.GetClassifier());
    map<FlowId, FlowMonitor::FlowStats> stats = flowMonitor->GetFlowStats();
    FlowMonitor::FlowStats tmp = stats[1];

    string p = tcp_prot + ".txt";
    cout << "\nUsing " << tcp_prot <<  " TCP congestion control algorithm\n";
    cout << "All the flow monitor statistics are written in file named " << p << "\n";

    // Writing the flow monitor statistics to the file named 'p' (same name as that of tcp type used)
    fstream f;
    f.open(p.c_str(), ios::out);
    f << "\nUsing " << tcp_prot <<  " TCP congestion control algorithm\n\n";
    f << "Flow monitor statistics\n";
    f << "Transfered Packets:   " << tmp.txPackets << "\n";
    f << "Transfered Bytes:     " << tmp.txBytes << "\n";
    f << "Offered Load: 		" << tmp.txBytes * 8.0 / (tmp.timeLastTxPacket.GetSeconds() - tmp.timeFirstTxPacket.GetSeconds()) / 1000000 << " Mbps" << "\n";
    f << "Received Packets:   	" << tmp.rxPackets << "\n";
    f << "Received Bytes:     	" << tmp.rxBytes<< "\n";
    f << "Throughput:   		" << tmp.rxBytes * 8.0 / (tmp.timeLastRxPacket.GetSeconds() - tmp.timeFirstRxPacket.GetSeconds()) / 1000000 << " Mbps" << "\n";
    f.close();
    
    flowMonitor->SerializeToXmlFile("stats.flowmon", true, true);

    Simulator::Destroy();
    return 0;
}
