#pragma once
#include "stdafx.h"
#include <windows.h>
#include <vector>

//https://guidedhacking.com/threads/simple-x86-c-trampoline-hook.14188/

namespace mem
{
	bool Detour32(BYTE* src, BYTE* dst, const uintptr_t len);
	BYTE* TrampHook32(BYTE* src, BYTE* dst, const uintptr_t len);
}