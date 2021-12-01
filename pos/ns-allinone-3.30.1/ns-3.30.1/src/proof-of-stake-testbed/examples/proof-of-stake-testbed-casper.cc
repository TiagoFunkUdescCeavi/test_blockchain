/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/csma-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "../model/blockchain.h"
#include "../model/casper-node.h"
#include "../model/constants.h"
#include "../helper/network-helper.h"
#include <iostream>
#include <string>

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("ProofOfStakeTestbed");

int main(int argc, char *argv[]) {

    //parse cmd params
    CommandLine cmd;
    cmd.Parse(argc, argv);

    //start logging
    LogComponentEnable("BlockChain", LOG_LEVEL_INFO);
    LogComponentEnable("NodeHelper", LOG_LEVEL_INFO);
    LogComponentEnable("BlockChainNodeApp", LOG_LEVEL_INFO);
    LogComponentEnable("CasperNodeApp", LOG_LEVEL_INFO);
    LogComponentEnable("CasperHelper", LOG_LEVEL_INFO);
    LogComponentEnable("NetworkHelper", LOG_LEVEL_INFO);
    LogComponentEnable("ProofOfStakeTestbed", LOG_LEVEL_INFO);

    //create nodes
    NodeContainer nodes;
    nodes.Create(constants.numberOfNodes);

    //create network
    auto netInterfaces = NetworkHelper::CreateDistributedNetwork(nodes);
//    std::vector <Ipv4Address> allAddress;
//    for(unsigned int i=0;i<constants.numberOfNodes;i++) {
//        allAddress.push_back(netInterfaces.GetAddress (i));
//    }
    // routing in the network
    Ipv4GlobalRoutingHelper::PopulateRoutingTables();

    // create applications
    CasperHelper nodeHelper(
            constants.numberOfNodes,
            constants.totalStack);

    std::vector<Ptr <CasperNodeApp>> applications;
    for(unsigned int i=0;i<constants.numberOfNodes;i++) {
        Ptr <CasperNodeApp> app = CreateObject<CasperNodeApp>(&nodeHelper);
        applications.push_back(app);
        nodes.Get(i)->AddApplication(app);
        app->SetStartTime(Seconds(0.));
        app->SetStopTime(Seconds(constants.simulationTimeSeconds));
    }

    // run simulator
    NS_LOG_INFO ("Run Simulation.");
    Simulator::Run();
    Simulator::Destroy();
    nodeHelper.PrintProcessInfo();


    // print statistics
    int maxNumberOfHops = 0;
    double roundNumberOfHops = applications.front()->GetRoundNumberOfHops();
    for(Ptr <CasperNodeApp> app: applications) {
        if(app->GetHighestNumberOfHops() > maxNumberOfHops){
            maxNumberOfHops = app->GetHighestNumberOfHops();
        }
        roundNumberOfHops = (roundNumberOfHops + app->GetRoundNumberOfHops()) /2;
    }

    NS_LOG_INFO("");
    NS_LOG_INFO("----------------------------------------------------------------------------------------   ");
    NS_LOG_INFO(" Statistics ");
    NS_LOG_INFO("----------------------------------------------------------------------------------------   ");
    NS_LOG_INFO(" Highest count of hops (message)  |  Round count of hops (message)  | ");
    NS_LOG_INFO("                " << maxNumberOfHops << "                |                " << roundNumberOfHops << "               | ");
    NS_LOG_INFO("");

    return 0;
}
