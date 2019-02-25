// Copyright 2016 Dolphin Emulator Project
// TODO: Verify what this copyright should be >.>
// Licensed under GPLv2+
// Refer to the license.txt file included.

// This file is a big hack that lets me dump information from Prime every frame.
// For the moment, this is all hard-coded to look for Prime 1 or 2 stuff.
#include <thread>
#include <mutex>
#include <deque>
#include <sstream>
#include <Common/Assert.h>
#include <Common/Timer.h>
#include <VideoCommon/OnScreenDisplay.h>
#include <VideoCommon/RenderBase.h>
#include "Core/PowerPC/MMU.h"
#include "SFML/Network.hpp"
#include "Common/MsgHandler.h"
#include "PrimeMemoryDumping.h"
#include "../PrimeMemoryDumping/prime1/Prime1JsonDumper.hpp"
#include "../PrimeMemoryDumping/common/json.hpp"

#include "../PrimeMemoryDumping/prime1/actors/CPlayer.hpp"
#include "../PrimeMemoryDumping/prime1/CWorld.hpp"
#include "../PrimeMemoryDumping/prime1/CStateManager.hpp"
#include "../PrimeMemoryDumping/prime1/CGameGlobalObjects.hpp"
#include "../PrimeMemoryDumping/prime1/CGameState.hpp"

#ifdef SFML_SYSTEM_WINDOWS
#include <winsock2.h>
#else
#include <sys/socket.h>
#endif


using namespace std;
using namespace nlohmann;

//Reuse SO_REUSEADDR for the sake of convenience (maybe should disable later? idk)
class ReuseableListener : public sf::TcpListener {
public:
    void reuse() {
      char reuse = 1;
      setsockopt(getHandle(), SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));
    }
};


namespace PrimeMemoryDumping {

    static ReuseableListener serverSocket;
    static thread acceptThread;
    static bool initalized = false;
    static bool addedCallback = false;
    static mutex clientListMutex;
    static vector<unique_ptr<sf::TcpSocket>> clients;
    static constexpr u16 port = 43673;

    void NetworkThread() {
      serverSocket.reuse();
      while (serverSocket.listen(port) != sf::Socket::Status::Done) {
        NOTICE_LOG(ACTIONREPLAY, "Failed to listen on port %u", port);
        this_thread::sleep_for(10s);
      }
      NOTICE_LOG(ACTIONREPLAY, "Listening on port %u", port);

      while (true) {
        auto sock = unique_ptr<sf::TcpSocket>(new sf::TcpSocket());
        sf::Socket::Status res = serverSocket.accept(*sock);
        if (res == sf::Socket::Status::Done) {
          std::lock_guard<std::mutex> lock(clientListMutex);
          clients.push_back(move(sock));
        } else if (res == sf::Socket::Status::Error) {
          break;
        }
      }
    }

    int timeToFrames(double time) {
      return round(time * 60);
    }

    double veclen(double vec[3]) {
      return sqrt(
        vec[0] * vec[0] +
        vec[1] * vec[1] +
        vec[2] * vec[2]
      );
    }

    int currentRoom = 0;
    double currentTime = 0;
    double currentRoomStart = 0;
    double lastRoomTime = 0;
    double speed[3];
    double rotationalSpeed[3];
    void ShowMessages() {
      //int width = g_renderer->GetBackbufferWidth();
      //int height = g_renderer->GetBackbufferHeight();
      stringstream msg;
      msg << fixed << setprecision(3);
      msg << "Current IGT: " << setw(7) << currentTime << "s/"
        << setw(7) << timeToFrames(currentTime) << " frames" << endl;

      msg << "Current room time: " << setw(7) << (currentTime - currentRoomStart) << "s/"
        << setw(7) << timeToFrames(currentTime - currentRoomStart) << " frames" << endl;

      msg << "Last room time: " << setw(7) << lastRoomTime
        << "s/" << setw(7) << timeToFrames(lastRoomTime) << " frames" << endl;

      msg << "Speed: "
        << setw(7) << speed[0] << "x "
        << setw(7) << speed[1] << "y "
        << setw(7) << speed[2] << "z "
        << setw(7) << sqrt(speed[0] * speed[0] + speed[1] * speed[1]) << " horizontal" << endl;

      msg << "Rotation: "
        << setw(7) << rotationalSpeed[0]<< "x "
        << setw(7) << rotationalSpeed[1] << "y "
        << setw(7) << rotationalSpeed[2] << "z "
        << setw(7) << veclen(rotationalSpeed) << " total" << endl;
//      g_renderer->RenderText(msg.str(), 10, 20, 0xFF33FFFF);
    }
    void CalcMessages() {
      CGameGlobalObjects global(CGameGlobalObjects::LOCATION);
      CStateManager stateManager(CStateManager::LOCATION);
      CGameState gameState = global.gameState.deref();
      CPlayer player = stateManager.player.deref();
      CPlayerState playerState = stateManager.playerState.deref().deref();
      CWorld world = stateManager.world.deref();

      currentTime = gameState.playTime.read();
      int room = world.currentAreaID.read();
      if (room != currentRoom) {
        currentRoom = room;
        lastRoomTime = currentTime - currentRoomStart;
        currentRoomStart = currentTime;
      }

      speed[0] = player.velocity.x.read();
      speed[1] = player.velocity.y.read();
      speed[2] = player.velocity.z.read();

      rotationalSpeed[0] = player.angularVelocity.x.read();
      rotationalSpeed[1] = player.angularVelocity.y.read();
      rotationalSpeed[2] = player.angularVelocity.z.read();
    }

    void OnShutdown() {
      addedCallback = false;
    }

    void DumpMemoryForFrame() {
      if (!initalized) {
        acceptThread = thread(NetworkThread);
        initalized = true;
      }

      if (!addedCallback) {
        addedCallback = true;
        OSD::AddCallback(OSD::CallbackType::Shutdown, OnShutdown);
        OSD::AddCallback(OSD::CallbackType::DrawMessages, ShowMessages);
      }

      CalcMessages();

      u32 gameID = PowerPC::HostRead_U32(0x80000000);
      u16 makerID = PowerPC::HostRead_U16(0x80000004);

      json json_message;
      //Prime 1
      if (gameID == 0x474D3845 && makerID == 0x3031) {
//        json_message["heap"] = Prime1JsonDumper::parseHeap();
        json_message["camera"] = Prime1JsonDumper::parseCamera();
//        json_message["player"] = Prime1JsonDumper::parsePlayer();
        json_message["player_raw"] = Prime1JsonDumper::parsePlayerRaw();
        json_message["world"] = Prime1JsonDumper::parseWorld();
        json_message["pool_summary"] = Prime1JsonDumper::parsePoolSummary();
        json_message["heap_stats"] = Prime1JsonDumper::parseHeapStats();
      }

      sf::Packet packet;
      string json_string = json_message.dump();
      uint32_t packetLen = static_cast<uint32_t>(json_string.size());
      packet.append(json_string.c_str(), packetLen * sizeof(std::string::value_type));

      std::lock_guard<std::mutex> lock(clientListMutex);
      for (auto client = clients.begin(); client != clients.end();) {
        if ((*client)->send(packet) != sf::Socket::Status::Done) {
          ERROR_LOG(ACTIONREPLAY, "Client didn't send, removing");
          client = clients.erase(client);
        } else {
          client++;
        }
      }
    }

    void LogRead(u64 offset, u64 len) {
//      sf::Packet packet;
//      packet << PACKET_TYPE_RAW_DISC_READ;
//      packet << static_cast<u32>((offset >> 32) & 0xFFFFFFFF);
//      packet << static_cast<u32>(offset & 0xFFFFFFFF);
//      packet << static_cast<u32>((len >> 32) & 0xFFFFFFFF);
//      packet << static_cast<u32>(len & 0xFFFFFFFF);

//      if (socket.send(packet, target, port) != sf::Socket::Done) {
//        PanicAlertT("Failed to dump data to socket!");
//      }
    }
}
