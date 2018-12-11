#include <windows.h>
#include <Psapi.h>
#include <winternl.h>

#include <iostream>
#include <thread>
#include <future>

class c_player {
public:
	char pad_0000[400]; //0x0000
	float m_unk_timer; //0x0190
	char pad_0194[12]; //0x0194
	float m_score; //0x01A0
	bool m_alive; //0x01A4
	char pad_01A8[24]; //0x01A8
	uint32_t m_gems; //0x01C0
	char pad_01C4[1200]; //0x01C4
}; //Size: 0x0674

void nop( void *addr, const int bytes ) {
	DWORD old_protect, old_protect2;
	VirtualProtect( addr, bytes, PAGE_EXECUTE_READWRITE, &old_protect );
	memset( addr, 0x90, bytes );
	VirtualProtect( addr, bytes, old_protect, &old_protect2 );
}

static unsigned long __stdcall cheat_init( void *arg ) {
	Beep( 500, 350 );

	AllocConsole();
	_iobuf *data;
	freopen_s( &data, "CONIN$", "r", stdin );
	freopen_s( &data, "CONOUT$", "w", stdout );

	SetConsoleTitleA( "debug" );

	const auto base = reinterpret_cast< uintptr_t >( GetModuleHandleA( nullptr ) );
	if( !base )
		return 1;

	std::cout << "base addr 0x" << std::hex << std::uppercase << base << "\n";

	const auto player_addr = base + 0x1F4AA0;
	std::cout << "player addr 0x" << std::hex << std::uppercase << player_addr << "\n\n";

	// dd.exe+4C067 - 01 06 - add [esi],eax
	// dd.exe+4C074 - 8B 01 - mov eax,[ecx]
	// dd.exe+4C084 - 89 01 - mov [ecx],eax
	// nop( (PVOID)( base + 0x4C067 ), 2 );
	// nop( (PVOID)( base + 0x4C074 ), 2 );
	// nop( (PVOID)( base + 0x4C084 ), 2 );

	auto *player = *reinterpret_cast< c_player ** >( player_addr );
	if( !player )
		return 1;

	while( true ) {
		std::cout << " player alive: " << player->m_alive 
		          << " player gems: " << player->m_gems
		          << " player score: " << player->m_score << "\r";

		// anti-cheat catches you ;)
		// player->m_score = 1337.f;

		std::this_thread::sleep_for( std::chrono::milliseconds( 5 ) );
	}
}

int __stdcall DllMain( const HMODULE self, const unsigned long reason_for_call, void *reserved ) {
	if( reason_for_call == DLL_PROCESS_ATTACH ) {
		CreateThread( nullptr, 0, &cheat_init, nullptr, 0, nullptr );
		return 1;
	}

	return 0;
}

