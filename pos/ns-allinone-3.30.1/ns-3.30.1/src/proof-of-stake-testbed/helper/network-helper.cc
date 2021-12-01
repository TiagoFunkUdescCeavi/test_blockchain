#include "network-helper.h"
#include "../model/constants.h"
#include <math.h>

namespace ns3 {
    NS_LOG_COMPONENT_DEFINE ("NetworkHelper");

    std::vector <Ipv4InterfaceContainer *> NetworkHelper::CreateBusNetwork(NodeContainer nodes) {
        std::vector <Ipv4InterfaceContainer *> returnInterfaces;

        CsmaHelper lanNet;
        lanNet.SetChannelAttribute("DataRate", StringValue("100Mbps"));
        lanNet.SetChannelAttribute("Delay", StringValue("2ms"));

        NetDeviceContainer netDevices;
        netDevices = lanNet.Install(nodes);

        InternetStackHelper stack;
        stack.Install(nodes);

        Ipv4AddressHelper address;
        address.SetBase("192.168.1.0", "255.255.255.0");
        Ipv4InterfaceContainer netInterfaces;
        netInterfaces = address.Assign(netDevices);

        returnInterfaces.push_back(&netInterfaces);
        return returnInterfaces;
    }

    std::vector <Ipv4InterfaceContainer *> NetworkHelper::CreateDecentralizedNetwork(NodeContainer nodes) {
        std::vector <Ipv4InterfaceContainer *> returnInterfaces;

        int sizePerOneMesh = constants.networkDecentralizedSizePerLocal;
        int countOfMeshs = ceil((double)constants.numberOfNodes / sizePerOneMesh);

        if(sizePerOneMesh > 254 ){
            NS_FATAL_ERROR("Mesh network is not support more than 254 local nodes");
        }
        if(countOfMeshs > 127 ){
            NS_FATAL_ERROR("Mesh network is not support more than 128 local networks");
        }

        // craete localNetworks
        int i = 0;
        for (i = 0; i < countOfMeshs; i++) {
            NodeContainer lanNodes;
            for(int j = 0; j < sizePerOneMesh; j++){
                int index = (i*sizePerOneMesh) + j;
                if(index > ((int)constants.numberOfNodes - 1)){
                    break;
                }
                lanNodes.Add(nodes.Get(index));
            }

            CsmaHelper lanNet;
            lanNet.SetChannelAttribute("DataRate", StringValue("100Mbps"));
            lanNet.SetChannelAttribute("Delay", StringValue("2ms"));

            NetDeviceContainer netDevices;
            netDevices = lanNet.Install(lanNodes);

            InternetStackHelper stack;
            stack.Install(lanNodes);

            std::string localipv4address = "192.168." + std::to_string(i) + ".0";
            Ipv4AddressHelper address;
            address.SetBase(localipv4address.c_str(), "255.255.255.0");
            Ipv4InterfaceContainer netInterfaces;
            netInterfaces = address.Assign(netDevices);

            returnInterfaces.push_back(&netInterfaces);
        }

        //mesh connection
        int lastIPv4address = i;
        for (i = 0; i < (countOfMeshs-1); i++) {
            NodeContainer p2pNodes;
            //TODO check miminla size of loclal network
            p2pNodes.Add(nodes.Get(i*sizePerOneMesh));
            p2pNodes.Add(nodes.Get(((i+1)*sizePerOneMesh)));

            PointToPointHelper pointToPoint;
            pointToPoint.SetDeviceAttribute ("DataRate", StringValue("100Mbps"));
            pointToPoint.SetChannelAttribute ("Delay", StringValue ("2ms"));

            NetDeviceContainer p2pDevices;
            p2pDevices = pointToPoint.Install(p2pNodes);

            std::string localipv4address = "192.168." + std::to_string(i+lastIPv4address) + ".0";
            Ipv4AddressHelper address;
            address.SetBase(localipv4address.c_str(), "255.255.255.0");
            Ipv4InterfaceContainer p2pInterfaces;
            p2pInterfaces = address.Assign (p2pDevices);

            returnInterfaces.push_back(&p2pInterfaces);
        }

        return returnInterfaces;
    }

    std::vector<Ipv4InterfaceContainer *> NetworkHelper::CreateDistributedNetwork(NodeContainer nodes) {
        std::vector <Ipv4InterfaceContainer *> returnInterfaces;
        int oneNodeConnections = constants.networkDistributedCountOfConnections;
        if(oneNodeConnections > 16 ){
            NS_FATAL_ERROR("Disctibuted network is not support more than 16 connection per node");
        }
        if(constants.numberOfNodes < oneNodeConnections ){
            NS_FATAL_ERROR("Disctibuted network - count of nodes is less then connections peer node");
        }
        if(oneNodeConnections*constants.numberOfNodes >  65000){
            NS_FATAL_ERROR("Distributed network support only max 65 000 connection networks. Change number of ones or number of connection");
        }

        int networkCounter = 0;

        InternetStackHelper stack;
        stack.Install(nodes);

        for (int j=0;j < (int)constants.numberOfNodes;j++){
            for(int i=1; i <= (int)oneNodeConnections; i++){
                int connectedNode = j + i;
                if(connectedNode >= (int)constants.numberOfNodes){
                    break;
                }
                networkCounter++;
                int lastIpPart = networkCounter % 255;
                int firstIpPart = networkCounter / 255;

                NodeContainer p2pNodes;
                p2pNodes.Add(nodes.Get(j));
                p2pNodes.Add(nodes.Get(connectedNode));

                PointToPointHelper pointToPoint;
                pointToPoint.SetDeviceAttribute ("DataRate", StringValue("100Mbps"));
                pointToPoint.SetChannelAttribute ("Delay", StringValue ("2ms"));

                NetDeviceContainer p2pDevices;
                p2pDevices = pointToPoint.Install(p2pNodes);

                std::string localIPv4address = "192." + std::to_string(firstIpPart) + "." + std::to_string(lastIpPart) + ".0";
                Ipv4AddressHelper address;
                address.SetBase(localIPv4address.c_str(), "255.255.255.0");
                Ipv4InterfaceContainer p2pInterfaces;
                p2pInterfaces = address.Assign (p2pDevices);

                returnInterfaces.push_back(&p2pInterfaces);
            }
        }

        return returnInterfaces;
    }
}

