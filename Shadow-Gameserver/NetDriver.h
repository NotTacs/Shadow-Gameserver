#pragma once
#include "misc.h"

inline void (*TickFlush_OG)(UNetDriver* Driver) = decltype(TickFlush_OG)(ImageBase + 0x437d9b0);

void TickFlush(UNetDriver*);

inline int32 (*ServerReplicateActors)(UReplicationDriver*) = decltype(ServerReplicateActors)(UReplicationDriver::GetDefaultObj()->VTable[0x5d]);