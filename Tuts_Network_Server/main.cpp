
/******************************************************************************
Class: Net1_Client
Implements:
Author: Pieran Marris <p.marris@newcastle.ac.uk> and YOU!
Description:

:README:
- In order to run this demo, we also need to run "Tuts_Network_Client" at the same time.
- To do this:-
	1. right click on the entire solution (top of the solution exporerer) and go to properties
	2. Go to Common Properties -> Statup Project
	3. Select Multiple Statup Projects
	4. Select 'Start with Debugging' for both "Tuts_Network_Client" and "Tuts_Network_Server"

- Now when you run the program it will build and run both projects at the same time. =]
- You can also optionally spawn more instances by right clicking on the specific project
and going to Debug->Start New Instance.




FOR MORE NETWORKING INFORMATION SEE "Tuts_Network_Client -> Net1_Client.h"



		(\_/)
		( '_')
	 /""""""""""""\=========     -----D
	/"""""""""""""""""""""""\
....\_@____@____@____@____@_/

*//////////////////////////////////////////////////////////////////////////////

#pragma once

#include <enet\enet.h>
#include <nclgl\GameTimer.h>
#include <nclgl\Vector3.h>
#include <nclgl\common.h>
#include <ncltech\NetworkBase.h>
#include <iostream>
#include <string>
#include "../ncltech/MazeGenerator.h"
#include "../ncltech/MazeRenderer.h"
#include "../ncltech/SearchAStar.h"

//Needed to get computer adapter IPv4 addresses via windows
#include <iphlpapi.h>
#pragma comment(lib, "IPHLPAPI.lib")


#define SERVER_PORT 1234
#define UPDATE_TIMESTEP (1.0f / 30.0f) //send 30 position updates per second

NetworkBase server;
GameTimer timer;
float accum_time = 0.0f;
float rotation = 0.0f;
float mazeDensity = 0.5f;
int mazeSize = 10;

MazeGenerator*	generator = new MazeGenerator();
Mesh* wallmesh;
SearchAStar* search_as = new SearchAStar();


void Win32_PrintAllAdapterIPAddresses();

enum packetType { mazePack, mazeSizePack, helloPack , densityPack, sePack, pathPack
};

struct mazePacket {
	packetType t = mazePack;
	uint size;
	uint startId;
	uint endId;
	uint allEdges[1024];
};

struct gridSizePacket {
	packetType t = mazeSizePack;
	uint size;
};

struct mazeDensityPacket {
	packetType t = densityPack;
	float density;
};


struct startendPacket {
	packetType t = sePack;
	uint startId;
	uint endId;
};


struct pathPacket {
	packetType t = pathPack;
	uint pathLength;
	uint nodes[1024];
};

int onExit(int exitcode)
{
	server.Release();
	system("pause");
	exit(exitcode);
}

int main(int arcg, char** argv)
{
	if (enet_initialize() != 0)
	{
		fprintf(stderr, "An error occurred while initializing ENet.\n");
		return EXIT_FAILURE;
	}

	//Initialize Server on Port 1234, with a possible 32 clients connected at any time
	if (!server.Initialize(SERVER_PORT, 32))
	{
		fprintf(stderr, "An error occurred while trying to create an ENet server host.\n");
		onExit(EXIT_FAILURE);
	}

	printf("Server Initiated\n");

	search_as->SetWeightings(1.0f, 1.0f);

	Win32_PrintAllAdapterIPAddresses();

	timer.GetTimedMS();
	while (true)
	{
		float dt = timer.GetTimedMS() * 0.001f;
		accum_time += dt;
		rotation += 0.5f * PI * dt;

		//Handle All Incoming Packets and Send any enqued packets
		server.ServiceNetwork(dt, [&](const ENetEvent& evnt)
		{
			switch (evnt.type)
			{
			case ENET_EVENT_TYPE_CONNECT:
				printf("- New Client Connected\n");
				break;

			case ENET_EVENT_TYPE_RECEIVE:
				//switch what to do depending on packet type
				if (evnt.packet->data[0] == 's') {
					//print is string
					printf("\t Client %d says: %s\n", evnt.peer->incomingPeerID, evnt.packet->data);
				}
				if (evnt.packet->dataLength == sizeof(gridSizePacket) && evnt.packet->data[0] == mazeSizePack) {
					//update if integer
					
				    memcpy(&mazeSize, evnt.packet->data + sizeof(packetType), sizeof(uint));
					cout << "grid size recived from clients.(server) \n";

					//generate the maze
					generator->Generate(mazeSize, mazeDensity);
					MazeRenderer* mazeRenderer = new MazeRenderer(generator, wallmesh);
					//send maze to client
					mazePacket mazeInfo;
					mazeInfo.size = generator->GetSize();
					mazeInfo.startId = generator->idxS;
					mazeInfo.endId = generator->idxE;

					for (int i = 0;(uint)i < generator->base_offset * 2;++i) {
						GraphEdge* tempEdge = &generator->allEdges[i];
						if (tempEdge->_iswall) {
							mazeInfo.allEdges[i] = 1;
						}
						else {
							mazeInfo.allEdges[i] = 0;
						}	
					}
				
					//create and send packet ot client
					ENetPacket* mazeInfoPacket = enet_packet_create(&mazeInfo, sizeof(mazeInfo), 0);
					enet_host_broadcast(server.m_pNetwork, 0, mazeInfoPacket);
					cout << "maze sent to clients.(server) \n";

				}
				if (evnt.packet->dataLength == sizeof(mazeDensityPacket) && evnt.packet->data[0] == densityPack) {
					//update if integer
					memcpy(&mazeDensity, evnt.packet->data + sizeof(packetType), sizeof(float));

					cout << "density recived from clients.(server) \n";
					//generate the maze
					generator->Generate(mazeSize, mazeDensity);
					MazeRenderer* mazeRenderer = new MazeRenderer(generator, wallmesh);
					//send maze to client
					mazePacket mazeInfo;
					mazeInfo.size = generator->GetSize();
					mazeInfo.startId = generator->idxS;
					mazeInfo.endId = generator->idxE;

					for (int i = 0;(uint)i < generator->base_offset * 2;++i) {
						GraphEdge* tempEdge = &generator->allEdges[i];
						if (tempEdge->_iswall) {
							mazeInfo.allEdges[i] = 1;
						}
						else {
							mazeInfo.allEdges[i] = 0;
						}
					}

					//create and send packet ot client
					ENetPacket* mazeInfoPacket = enet_packet_create(&mazeInfo, sizeof(mazeInfo), 0);
					enet_host_broadcast(server.m_pNetwork, 0, mazeInfoPacket);

					cout << "maze sent to clients.(server) \n";

				}
				//calculate the route from start and end recived from client and send best route packet back
				if (evnt.packet->dataLength == sizeof(startendPacket) && evnt.packet->data[0] == sePack) {
					startendPacket se;
					memcpy(&se, evnt.packet->data, sizeof(startendPacket));
					generator->setEndNode(se.endId);
					generator->setStartNode(se.startId);
					GraphNode* start = generator->GetStartNode();
					GraphNode* end = generator->GetGoalNode();

					cout << "start and end nodes recived from clients.(server) \n";

					search_as->FindBestPath(start, end);
					//populate a path packet with the best path
					pathPacket pathP;
					pathP.pathLength = search_as->GetFinalPath().size();
					int k = 0;
					for (auto const& it : search_as->GetFinalPath()) {
						for (int j = 0; j < (int)(generator->size*generator->size); ++j) {
							if (generator->allNodes[j]._pos ==  it->_pos) {
								pathP.nodes[k] = j;
								++k;
							}
						}
      				}
					//send path packet to client
					
					ENetPacket* pathInfoPacket = enet_packet_create(&pathP, sizeof(pathP), 0);
					enet_host_broadcast(server.m_pNetwork, 0, pathInfoPacket);
				
					cout << "path sent to clients.(server) \n";
				}
				enet_packet_destroy(evnt.packet);
				break;

			case ENET_EVENT_TYPE_DISCONNECT:
				printf("- Client %d has disconnected.\n", evnt.peer->incomingPeerID);
				break;
			}
		});
		
		//Broadcast update packet to all connected clients at a rate of UPDATE_TIMESTEP updates per second
		if (accum_time >= UPDATE_TIMESTEP)
		{

			//Packet data
			// - At the moment this is just a position update that rotates around the origin of the world
			//   though this can be any variable, structure or class you wish. Just remember that everything 
			//   you send takes up valuable network bandwidth so no sending every PhysicsObject struct each frame ;)
			accum_time = 0.0f;
			Vector3 pos = Vector3(
				cos(rotation) * 2.0f,
				1.5f,
				sin(rotation) * 2.0f);


			//Create the packet and broadcast it (unreliable transport) to all clients
			ENetPacket* position_update = enet_packet_create(&pos, sizeof(Vector3), 0);
			enet_host_broadcast(server.m_pNetwork, 0, position_update);

		}

		Sleep(0);
	}

	system("pause");
	server.Release();
}




//Yay Win32 code >.>
//  - Grabs a list of all network adapters on the computer and prints out all IPv4 addresses associated with them.
void Win32_PrintAllAdapterIPAddresses()
{
	//Initially allocate 5KB of memory to store all adapter info
	ULONG outBufLen = 5000;
	

	IP_ADAPTER_INFO* pAdapters = NULL;
	DWORD status = ERROR_BUFFER_OVERFLOW;

	//Keep attempting to fit all adapter info inside our buffer, allocating more memory if needed
	// Note: Will exit after 5 failed attempts, or not enough memory. Lets pray it never comes to this!
	for (int i = 0; i < 5 && (status == ERROR_BUFFER_OVERFLOW); i++)
	{
		pAdapters = (IP_ADAPTER_INFO *)malloc(outBufLen);
		if (pAdapters != NULL) {

			//Get Network Adapter Info
			status = GetAdaptersInfo(pAdapters, &outBufLen);

			// Increase memory pool if needed
			if (status == ERROR_BUFFER_OVERFLOW) {
				free(pAdapters);
				pAdapters = NULL;
			}
			else {
				break;
			}
		}
	}

	
	if (pAdapters != NULL)
	{
		//Iterate through all Network Adapters, and print all IPv4 addresses associated with them to the console
		// - Adapters here are stored as a linked list termenated with a NULL next-pointer
		IP_ADAPTER_INFO* cAdapter = &pAdapters[0];
		while (cAdapter != NULL)
		{
			IP_ADDR_STRING* cIpAddress = &cAdapter->IpAddressList;
			while (cIpAddress != NULL)
			{
				printf("\t - Listening for connections on %s:%u\n", cIpAddress->IpAddress.String, SERVER_PORT);
				cIpAddress = cIpAddress->Next;
			}
			cAdapter = cAdapter->Next;
		}

		free(pAdapters);
	}
	
}