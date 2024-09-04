#pragma once
#include <windows.h>
#include <thread>
#include <vector>
#include <fstream>
#include <string>
#include <iostream>
#include <cstdint>
#include "detours/detours.h"


inline std::vector<std::pair<uintptr_t, void*>> Hooks;

inline uintptr_t ImageBase = uintptr_t(GetModuleHandle(0));

void DetachHooks();

void AttachHook(uintptr_t Address, PVOID Hook, PVOID OG = nullptr);
