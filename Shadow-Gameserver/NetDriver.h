#pragma once
#include "misc.h"

inline void (*TickFlush_OG)(UNetDriver* Driver, float DeltaSeconds);

void TickFlush(UNetDriver*, float DeltaSeconds);

inline int32 (*ServerReplicateActors)(UReplicationDriver* RepDriver) = decltype(ServerReplicateActors)(UReplicationDriver::GetDefaultObj()->VTable[0x5d]);