#include "NetDriver.h"


void TickFlush(UNetDriver* Driver, float DeltaSeconds)
{
	if (Driver && Driver->ReplicationDriver && Driver->ClientConnections.Num() > 0 && !Driver->ClientConnections[0]->InternalAck)
	{
		ServerReplicateActors(Driver->ReplicationDriver);
	}
	return TickFlush_OG(Driver, DeltaSeconds);
}