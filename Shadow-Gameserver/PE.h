#pragma once
#include "misc.h"

inline void (*ProcessEvent_OG)(UObject* Object, UFunction* Function, void* Params) = decltype(ProcessEvent_OG)(ImageBase + Offsets::ProcessEvent);
void ProcessEventHook(UObject* Object, UFunction* Function, void* Params);
