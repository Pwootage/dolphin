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


using namespace std;
using namespace nlohmann;


namespace PrimeMemoryDumping {
  static bool initalized = false;

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

  static int currentRoom = 0;
  static double currentTime = 0;
  static double currentRoomStart = 0;
  static double lastRoomTime = 0;
  static double speed[3];
  static double rotationalSpeed[3];

  void ShowMessages() {
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
        << setw(7) << rotationalSpeed[0] << "x "
        << setw(7) << rotationalSpeed[1] << "y "
        << setw(7) << rotationalSpeed[2] << "z "
        << setw(7) << veclen(rotationalSpeed) << " total" << endl;
    OSD::AddMessage(msg.str(), 1, 0xFF33FFFF);
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

  void DumpMemoryForFrame() {
    if (!initalized) {
      initalized = true;
    }

    CalcMessages();
    ShowMessages();

    u32 gameID = PowerPC::HostRead_U32(0x80000000);
    u16 makerID = PowerPC::HostRead_U16(0x80000004);

    json json_message;
    //Prime 1
    if (gameID == 0x474D3845 && makerID == 0x3031) {
////        json_message["heap"] = Prime1JsonDumper::parseHeap();
//        json_message["camera"] = Prime1JsonDumper::parseCamera();
////        json_message["player"] = Prime1JsonDumper::parsePlayer();
//        json_message["player_raw"] = Prime1JsonDumper::parsePlayerRaw();
//        json_message["world"] = Prime1JsonDumper::parseWorld();
//        json_message["pool_summary"] = Prime1JsonDumper::parsePoolSummary();
//        json_message["heap_stats"] = Prime1JsonDumper::parseHeapStats();
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
