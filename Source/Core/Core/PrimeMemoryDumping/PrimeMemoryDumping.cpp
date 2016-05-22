// Copyright 2016 Dolphin Emulator Project
// TODO: Verify what this copyright should be >.>
// Licensed under GPLv2+
// Refer to the license.txt file included.

// This file is a big hack that lets me dump information from Prime every frame.
// For the moment, this is all hard-coded to look for Prime 1 or 2 stuff.

#include <thread>
#include <mutex>
#include <deque>
#include "Core/PowerPC/PowerPC.h"
#include "SFML/Network.hpp"
#include "Common/MsgHandler.h"
#include "Core/PrimeMemoryDumping/PrimeMemoryDumping.h"

using namespace std;

namespace PrimeMemoryDumping {
    static bool initialized = false;
	static sf::UdpSocket socket;
	static sf::IpAddress target = "192.168.1.50";
	static constexpr u16 port = 43673;
	static constexpr int INVENTORY_SIZE = 0x29;

    void Init() {
		initialized = true;
    }

	union FloatInt {
		u32 intVal;
		float floatVal;
	};

	inline float ReadFloat(u32 addr) {
		FloatInt res;
		res.intVal = PowerPC::HostRead_U32(addr);
		return res.floatVal;
	}

	inline u64 read64(u32 addr) {
		return (static_cast<u64>(PowerPC::HostRead_U32(addr)) << 32) | 
			(static_cast<u64>(PowerPC::HostRead_U32(addr + 4)));
	}

    void DumpMemoryForFrame() {
        if (!initialized) {
            Init();
        }

		u32 gameID = PowerPC::HostRead_U32(0x00);
		u16 makerID = PowerPC::HostRead_U16(0x04);

		//Prime 1
		if (true || gameID == 0x474D3845 && makerID == 0x3031) {
			u32 ptr = PowerPC::HostRead_U32(0x004578CC) - 0x80000000;

			sf::Packet packet;
			packet << gameID;
			packet << makerID;
			packet << ReadFloat(0x46BAB4); //Speed x
			packet << ReadFloat(0x46BAB8); //Speed y
			packet << ReadFloat(0x46BABC); //Speed Z
			packet << ReadFloat(0x46B9BC); //Pos X
			packet << ReadFloat(0x46B9CC); //Pos y
			packet << ReadFloat(0x46B9DC); //Pos z
			packet << PowerPC::HostRead_U32(0x45AA74); //room
			packet << ptr;
			packet << ReadFloat(ptr + 0x2AC); //heath
			for (int i = 0; i < INVENTORY_SIZE * 2; i++) { //Inventory (count, capacity)xINVENTORY_SIZE
				packet << PowerPC::HostRead_U32(ptr + 0x2C8 + i * 4);
			}
			packet << PowerPC::HostRead_U32(ptr + 0xA0); // Timer high bits
			packet << PowerPC::HostRead_U32(ptr + 0xA0 + 0x4); // Timer low bits

			if (socket.send(packet, target, port) != sf::Socket::Done) {
				PanicAlertT("Failed to dump data to socket!");
			}
		}
    }
}