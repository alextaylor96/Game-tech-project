/******************************************************************************
Class: Net1_Client
Implements:
Author: Pieran Marris <p.marris@newcastle.ac.uk> and YOU!
Description:

:README:
- In order to run this demo, we also need to run "Tuts_Network_Server" at the same time.
- To do this:-
	1. right click on the entire solution (top of the solution exporerer) and go to properties
	2. Go to Common Properties -> Statup Project
	3. Select Multiple Statup Projects
	4. Select 'Start with Debugging' for both "Tuts_Network_Client" and "Tuts_Network_Server"

- Now when you run the program it will build and run both projects at the same time. =]
- You can also optionally spawn more instances by right clicking on the specific project
  and going to Debug->Start New Instance.




This demo scene will demonstrate a very simple network example, with a single server
and multiple clients. The client will attempt to connect to the server, and say "Hellooo!" 
if it successfully connects. The server, will continually broadcast a packet containing a 
Vector3 position to all connected clients informing them where to place the server's player.

This designed as an example of how to setup networked communication between clients, it is
by no means the optimal way of handling a networked game (sending position updates at xhz).
If your interested in this sort of thing, I highly recommend finding a good book or an
online tutorial as there are many many different ways of handling networked game updates
all with varying pitfalls and benefits. In general, the problem always comes down to the
fact that sending updates for every game object 60+ frames a second is just not possible,
so sacrifices and approximations MUST be made. These approximations do result in a sub-optimal
solution however, so most work on networking (that I have found atleast) is on building
a network bespoke communication system that sends the minimal amount of data needed to
produce satisfactory results on the networked peers.


::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
::: IF YOUR BORED! :::
::::::::::::::::::::::
	1. Try setting up both the server and client within the same Scene (disabling collisions
	on the objects as they now share the same physics engine). This way we can clearly
	see the affect of packet-loss and latency on the network. There is a program called "Clumsy"
	which is found within the root directory of this framework that allows you to inject
	latency/packet loss etc on network. Try messing around with various latency/packet-loss
	values.

	2. Packet Loss
		This causes the object to jump in large (and VERY noticable) gaps from one position to 
		another.

	   A good place to start in compensating for this is to build a buffer and store the
	   last x update packets, now if we miss a packet it isn't too bad as the likelyhood is
	   that by the time we need that position update, we will already have the next position
	   packet which we can use to interpolate that missing data from. The number of packets we
	   will need to backup will be relative to the amount of expected packet loss. This method
	   will also insert additional 'buffer' latency to our system, so we better not make it wait
	   too long.

	3. Latency
	   There is no easy way of solving this, and will have all felt it's punishing effects
	   on all networked games. The way most games attempt to hide any latency is by actually
	   running different games on different machines, these will react instantly to your actions
	   such as shooting which the server will eventually process at some point and tell you if you
	   have hit anything. This makes it appear (client side) like have no latency at all as you
	   moving instantly when you hit forward and shoot when you hit shoot, though this is all smoke
	   and mirrors and the server is still doing all the hard stuff (except now it has to take into account
	   the fact that you shot at time - latency time).

	   This smoke and mirrors approach also leads into another major issue, which is what happens when
	   the instances are desyncrhonised. If player 1 shoots and and player 2 moves at the same time, does
	   player 1 hit player 2? On player 1's screen he/she does, but on player 2's screen he/she gets
	   hit. This leads to issues which the server has to decipher on it's own, this will also happen
	   alot with generic physical elements which will ocasional 'snap' back to it's actual position on 
	   the servers game simulation. This methodology is known as "Dead Reckoning".

::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::


*//////////////////////////////////////////////////////////////////////////////

#include "Net1_Client.h"
#include <ncltech\SceneManager.h>
#include <ncltech\PhysicsEngine.h>
#include <nclgl\NCLDebug.h>
#include <ncltech\DistanceConstraint.h>
#include <ncltech\CommonUtils.h>
#include <ncltech\MazeRenderer.h>

uint gridSize = 10;
float density = 0.5;

const Vector3 status_color3 = Vector3(1.0f, 0.6f, 0.6f);
const Vector4 status_color = Vector4(status_color3.x, status_color3.y, status_color3.z, 1.0f);


MazeGenerator*	generator = new MazeGenerator();
MazeRenderer* render;

vector<GraphNode*> navMeshNodes;

bool showPath = false;
vector<GraphNode*> pathNodes;
bool pathMade = false;
SearchHistory finalpath;

vector<bool> mazeInfoClient;

enum packetType{ mazePack, mazeSizePack, helloPack , densityPack ,sePack, pathPack, avaPack
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


struct avatarPacket{
	packetType t = avaPack;
	Vector3 pos;
};


Net1_Client::Net1_Client(const std::string& friendly_name)
	: Scene(friendly_name)
	, serverConnection(NULL)
	, box(NULL)
{
}

void Net1_Client::generateNewMaze() {
		////Send a maze parameter
		gridSizePacket p;
		p.size = gridSize;
		ENetPacket* gridSize = enet_packet_create(&p, sizeof(p), 0);
		enet_peer_send(serverConnection, 0, gridSize);
		cout << "grid size sent to server.(client) \n";
}

void Net1_Client::increaseGridSize() {
	if (gridSize < 20) {
		gridSize++;
		////Send a maze parameter
		gridSizePacket p;
		p.size = gridSize;
		ENetPacket* gridSize = enet_packet_create(&p, sizeof(p), 0);
		enet_peer_send(serverConnection, 0, gridSize);
		cout << "grid size sent to server.(client) \n";
	}
}

void Net1_Client::decreaseGridSize() {
	if (gridSize > 1) {
		gridSize--;
		////Send a maze parameter
		gridSizePacket p;
		p.size = gridSize;
		ENetPacket* gridSize = enet_packet_create(&p, sizeof(p), 0);
		enet_peer_send(serverConnection, 0, gridSize);

		cout << "grid size sent to server.(client) \n";
	}
}

void Net1_Client::increaseDensity() {
	if (density < 1) {
		density += 0.1f;
		////Send a maze parameter
		mazeDensityPacket d;
		d.density = density;
		ENetPacket* densityP = enet_packet_create(&d, sizeof(d), 0);
		enet_peer_send(serverConnection, 0, densityP);

		cout << "density sent to server.(client) \n";
	}
}

void Net1_Client::decreaseDensity() {
	if (density >= 0.1) {
		density -= 0.1f;
		////Send a maze parameter
		mazeDensityPacket d;
		d.density = density;
		ENetPacket* densityP = enet_packet_create(&d, sizeof(d), 0);
		enet_peer_send(serverConnection, 0, densityP);

		cout << "density sent to server.(client) \n";
	}
}

void Net1_Client::createAvatar() {
	//create an avatar packet to send to server to let it know to create avatar
	struct avatarPacket {
		packetType t = avaPack;
		Vector3 pos;
	};

	avatarPacket aP;

	aP.pos = Vector3(0,0,0);

	ENetPacket* aPack = enet_packet_create(&aP, sizeof(aP), 0);
	enet_peer_send(serverConnection, 0, aPack);
	this->AddGameObject(box);
}

void Net1_Client::OnInitializeScene()
{
	GraphicsPipeline::Instance()->GetCamera()->SetPosition(Vector3(10, 25, 14));
	GraphicsPipeline::Instance()->GetCamera()->SetPitch(-80);
	GraphicsPipeline::Instance()->GetCamera()->SetYaw(0);

	//Initialize Client Network
	if (network.Initialize(0))
	{
		NCLDebug::Log("Network: Initialized!");

		//Attempt to connect to the server on localhost:1234
		serverConnection = network.ConnectPeer(127, 0, 0, 1, 1234);
		NCLDebug::Log("Network: Attempting to connect to server.");
	}
	//There is always too much ground to cover...
	this->AddGameObject(CommonUtils::BuildCuboidObject(
		"Ground",
		Vector3(0.0f, -1.5f, 0.0f),
		Vector3(20.0f, 1.0f, 20.0f),
		false,
		0.0f,
		false,
		false,
		Vector4(0.2f, 0.5f, 1.0f, 1.0f)));
	//Generate Simple Scene with a box that can be updated upon recieving server packets
	box = CommonUtils::BuildCuboidObject(
		"Avatar",
		Vector3(0.0f, 1.0f, 0.0f),
		Vector3(0.05f, 3.05f, 0.05f),
		true,									//Physics Enabled here Purely to make setting position easier via Physics()->SetPosition()
		0.0f,
		false,
		false,
		Vector4(1.f, 0.f, 1.f, 1.0f));
	
	pathMade = false;
}

void Net1_Client::toggleShowPath() {
	showPath = !showPath;
}

void Net1_Client::OnCleanupScene()
{
	Scene::OnCleanupScene();
	box = NULL; // Deleted in above function

	//Send one final packet telling the server we are disconnecting
	// - We are not waiting to resend this, so if it fails to arrive
	//   the server will have to wait until we time out naturally
	enet_peer_disconnect_now(serverConnection, 0);

	//Release network and all associated data/peer connections
	network.Release();
	serverConnection = NULL;
}

void Net1_Client::OnUpdateScene(float dt)
{
	Scene::OnUpdateScene(dt);


	//Update Network
	auto callback = std::bind(
		&Net1_Client::ProcessNetworkEvent,	// Function to call
		this,								// Associated class instance
		std::placeholders::_1);				// Where to place the first parameter
	network.ServiceNetwork(dt, callback);



	//Add Debug Information to screen
	uint8_t ip1 = serverConnection->address.host & 0xFF;
	uint8_t ip2 = (serverConnection->address.host >> 8) & 0xFF;
	uint8_t ip3 = (serverConnection->address.host >> 16) & 0xFF;
	uint8_t ip4 = (serverConnection->address.host >> 24) & 0xFF;
	
	NCLDebug::AddStatusEntry(status_color, "Network Traffic");
	NCLDebug::AddStatusEntry(status_color, "    Incoming: %5.2fKbps", network.m_IncomingKb);
	NCLDebug::AddStatusEntry(status_color, "    Outgoing: %5.2fKbps", network.m_OutgoingKb);
	NCLDebug::AddStatusEntry(status_color, "Peer: %u.%u.%u.%u:%u", ip1, ip2, ip3, ip4, serverConnection->address.port);
	NCLDebug::AddStatusEntry(status_color, "G to create a new maze.");
	NCLDebug::AddStatusEntry(status_color, "Press 2 to decrease maze size, 1 to increase maze size");
	NCLDebug::AddStatusEntry(status_color, "    Maze grid size: %i", gridSize);

	NCLDebug::AddStatusEntry(status_color, "Press 0 to decrease maze size, 9 to increase maze size");
	NCLDebug::AddStatusEntry(status_color, "    Maze density: %0.2f", density);

	NCLDebug::AddStatusEntry(status_color, "I to toggle showing path.");

	NCLDebug::AddStatusEntry(status_color, "O to spawn avatar.");
	
	if (pathMade && showPath) {
		render->DrawSearchHistory(finalpath, 2.5f / float(gridSize));
	}
	
}

void Net1_Client::ProcessNetworkEvent(const ENetEvent& evnt)
{
	switch (evnt.type)
	{
	//New connection request or an existing peer accepted our connection request
	case ENET_EVENT_TYPE_CONNECT:
		{
			if (evnt.peer == serverConnection)
			{
				NCLDebug::Log(status_color3, "Network: Successfully connected to server!");

				//Send a 'hello' packet
				char* text_data = "s:Hellooo!";
				ENetPacket* packet = enet_packet_create(text_data, strlen(text_data) + 1, 0);
				enet_peer_send(serverConnection, 0, packet);
			
				////Send a maze parameter
				gridSizePacket p;
				p.size = gridSize;
				ENetPacket* gridSize = enet_packet_create(&p, sizeof(p), 0);
				enet_peer_send(serverConnection, 0, gridSize);

				mazeDensityPacket d;
				d.density = density;
				ENetPacket* densityP = enet_packet_create(&d, sizeof(d), 0);
				enet_peer_send(serverConnection, 0, densityP);
			
			}
		}
		break;


	//Server has sent us a new packet
	case ENET_EVENT_TYPE_RECEIVE:
		{
		  
			if (evnt.packet->dataLength == sizeof(Vector3))
			{
			//	cout << "position recieved from server. \n";
			/*	Vector3 pos;
				memcpy(&pos, evnt.packet->data, sizeof(Vector3));
				box->Physics()->SetPosition(pos);*/
			}
			else if (evnt.packet->dataLength == sizeof(mazePacket))
			{
				this->RemoveGameObject(render);
				mazePacket m;

				memcpy(&m, evnt.packet->data, sizeof(mazePacket));


				cout << "maze recieved from server.(client) \n";
				//render maze here
				int size = (uint)m.size;
				uint base_offset = size * (size - 1);

				generator->Generate(size, 0.0f);
				generator->setStartNode(m.startId);
				generator->setEndNode(m.endId);

				for (int i = 0; (uint)i <= base_offset * 2; ++i) {
					if (m.allEdges[i] == (uint)1) {
						generator->allEdges[i]._iswall = true;
					}
					else if (m.allEdges[i] == (uint)0) {
						generator->allEdges[i]._iswall = false;

					
					}
				}


				render = new MazeRenderer(generator);

				const Vector3 pos_maze3 = Vector3(10.f, 0.f, 10.f);
				Matrix4 maze_scalar = Matrix4::Scale(Vector3(15.f, 15.0f / float(m.size),15.f)) * Matrix4::Translation(Vector3(-0.5f, 0.f, -0.5f));
				render->Render()->SetTransform(Matrix4::Translation(pos_maze3) * maze_scalar);
				this->AddGameObject(render);

				//send start/end nodes
				startendPacket se;
				se.endId = m.endId;
				se.startId = m.startId;
				ENetPacket* seP = enet_packet_create(&se, sizeof(se), 0);
				enet_peer_send(serverConnection, 0, seP);

				cout << "start/end nodes sent to server.(client) \n";

			}
			else if (evnt.packet->dataLength == sizeof(pathPacket))
			{
				cout << "Path recived from server.(client)" << endl;
				pathPacket p;
				pathNodes.clear();
				finalpath.clear();
				memcpy(&p, evnt.packet->data, sizeof(pathPacket));
				
				
				for (int i = 0; (uint)i <= p.pathLength; ++i) {
					pathNodes.push_back(&generator->allNodes[p.nodes[i]]);
				}
				
				for (int i = 0; (uint)i <= p.pathLength - 2; ++i) {
					finalpath.push_back({ pathNodes.at(i),  pathNodes.at(i + 1) });
				}
				pathMade = true;

			}
			else if (evnt.packet->dataLength == sizeof(avatarPacket))
			{
				cout << "avatarPacket recived from server.(client)" << endl;
				avatarPacket p;
				memcpy(&p, evnt.packet->data, sizeof(avatarPacket));


				float grid_scalar = 1.0f / (float)generator->size;

				Matrix4 transform = render->Render()->GetWorldTransform();

				Vector3 point = transform * Vector3(
					(finalpath.begin()->first->_pos.x + 0.5f) * grid_scalar,
					(finalpath.begin()->first->_pos.z + 0.5f) * grid_scalar * 3.05f,
					(finalpath.begin()->first->_pos.y + 0.5f) * grid_scalar);

				box->Physics()->SetPosition(point);
		
			}
			else
			{
				NCLERROR("Recieved Invalid Network Packet!");
			}

		}
		break;


	//Server has disconnected
	case ENET_EVENT_TYPE_DISCONNECT:
		{
			NCLDebug::Log(status_color3, "Network: Server has disconnected!");
		}
		break;
	}
}