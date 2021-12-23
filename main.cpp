#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <cstdint>
#include <thread>

namespace offset
{
	// client
	constexpr std::ptrdiff_t dwLocalPlayer = 0xDB458C;
	constexpr std::ptrdiff_t dwForceJump = 0x527998C;

	// player
	constexpr std::ptrdiff_t m_iHealth = 0x100;
	constexpr std::ptrdiff_t m_fFlags = 0x104;
}

// hack fn
void BunnyHop(const HMODULE instance) noexcept
{
	const auto client = reinterpret_cast<std::uintptr_t>(GetModuleHandle("client.dll"));

	// hack loop
	while (!GetAsyncKeyState(VK_END))
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(1));

		// make sure space is pressed
		if (!GetAsyncKeyState(VK_SPACE))
			continue;

		// get local player
		const auto localPlayer = *reinterpret_cast<std::uintptr_t*>(client + offset::dwLocalPlayer);

		// is valid
		if (!localPlayer)
			continue;

		const auto health = *reinterpret_cast<std::int32_t*>(localPlayer + offset::m_iHealth);

		// is alive
		if (!health)
			continue;

		const auto flags = *reinterpret_cast<std::int32_t*>(localPlayer + offset::m_fFlags);

		// on ground
		(flags & (1 << 0)) ?
			*reinterpret_cast<std::uintptr_t*>(client + offset::dwForceJump) = 6 : // force jump
			*reinterpret_cast<std::uintptr_t*>(client + offset::dwForceJump) = 4; // reset
	}

	// uninject
	FreeLibraryAndExitThread(instance, 0);
}

// entry point
int __stdcall DllMain(
	const HMODULE instance,
	const std::uintptr_t reason,
	const void* reserved
)
{
	// DLL_PROCESS_ATTACH
	if (reason == 1)
	{
		DisableThreadLibraryCalls(instance);

		// create hack thread
		const auto thread = CreateThread(
			nullptr,
			0,
			reinterpret_cast<LPTHREAD_START_ROUTINE>(BunnyHop),
			instance,
			0,
			nullptr
		);

		if (thread)
			CloseHandle(thread);
	}

	return 1;
}
