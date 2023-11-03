#pragma once
#include <Windows.h>
#include <iostream>

class Memory
{
public:
	DWORD PID;
	HWND GameHwnd;
	HANDLE pHandle;

	bool Init();

	template <typename T>
	constexpr const T RPM(const uintptr_t& adder) const noexcept
	{
		T v = { };
		ReadProcessMemory(pHandle, reinterpret_cast<const void*>(adder), &v, sizeof(T), NULL);
		return v;
	}

	template <typename T>
	constexpr void WPM(const uintptr_t& adder, const T& v) const noexcept
	{
		WriteProcessMemory(pHandle, reinterpret_cast<void*>(adder), &v, sizeof(T), NULL);
	}
};

extern Memory m;