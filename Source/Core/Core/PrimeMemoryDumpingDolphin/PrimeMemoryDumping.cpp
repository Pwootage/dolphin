// Copyright 2016 Dolphin Emulator Project
// TODO: Verify what this copyright should be >.>
// Licensed under GPLv2+
// Refer to the license.txt file included.

// This file is a big hack that lets me dump information from Prime every frame.
// For the moment, this is all hard-coded to look for Prime 1 or 2 stuff.

#include <thread>
#include <mutex>
#include <deque>
#include <Common/Assert.h>
#include "Core/PowerPC/PowerPC.h"
#include "SFML/Network.hpp"
#include "Common/MsgHandler.h"
#include "PrimeMemoryDumping.h"
#include "prime1/Prime1JsonDumper.hpp"
#include "json.hpp"

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

    void DumpMemoryForFrame() {
      if (!initalized) {
        acceptThread = thread(NetworkThread);
        initalized = true;
      }

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