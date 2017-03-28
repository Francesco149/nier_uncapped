/*
    This code is public domain and comes with no warranty.
    You are free to do whatever you want with it.
    I hope you will find the code useful or at least
    interesting to read. Have fun!
    -----------------------------------------------------------
    This file is part of "NieR: Uncapped", a fps uncap patch for
	NieR: Automata.
	
	All credits go to Altimor: https://www.reddit.com/user/Altimor
	who discovered this method to manipulate min/max timestep.
	
	WARNING: this code was put together in <2 hours and still needs
	         massive clean-up and optimization.
*/

#include <Windows.h>

#define UNCAPPED_VERSION "0.1.3"
#define UNCAPPED_TITLE "NieR: Uncapped"
#define UNCAPPED_WNDCLASS "NieRUncappedWnd"
#define GAME_WNDCLASS "NieR:Automata_MainWindow"

#define internal static
#define globvar static

typedef DWORD u32;
typedef BYTE  u8;
typedef BOOL  b32;

globvar HWND wnd = INVALID_HANDLE_VALUE;

#define err(msg) 					\
MessageBoxA(						\
	NULL,							\
	__FUNCTION__ " " msg " failed", \
	UNCAPPED_TITLE, 				\
	MB_OK							\
);

/* ------------------------------------------------------------- */

#define mbegin(addr, len) 		\
	VirtualProtect(				\
		addr,					\
		len,					\
		PAGE_EXECUTE_READWRITE,	\
		&old_protect_mask		\
	);							
	
#define mend(addr, len) 		\
	VirtualProtect(				\
		addr,					\
		len,					\
		old_protect_mask,		\
		&old_protect_mask		\
	);							

u8* psleep;
u8* pspinlock; 
u8* pmin_tstep;
u8* pmax_tstep;

b32* pmenu;
b32* ptitle_or_load;
b32* phacking;

void init_addresses()
{
	u8* base = (u8*)GetModuleHandle(0);
	
	/* FF 15 ? ? ? ? 48 ? ? 24 ? FF 15 ? ? ? ? 48 ? ? 24 ? 0F */
	psleep     = base + 0x92E887;
	pspinlock  = base + 0x92E8CF; /* + 72 */
	
	/* 73 ? C7 05 ? ? ? ? 00 00 80 3F 48 */
	pmin_tstep = base + 0x805DEC;
	pmax_tstep = base + 0x805E18; /* + 44 */
	
	/* 48 ? 1D ? ? ? ? 48 ? 0D ? ? ? ? C7 ? ? ? ? ? 00 00 00 00 */
	pmenu          = (b32*)(base + 0x18F39C4);
	
	/* ? 3D ? ? ? ? 74 2A ? 3D ? ? ? ? EB */
	ptitle_or_load = (b32*)(base + 0x1975520);
	
	/* 48 ? ? ? ? ? ? 00 8B ? 0F 85 ? ? ? ? 48 */
	phacking       = (b32*)(base + 0x10E0AB4);
}

void fps_uncap()
{
	u32 old_protect_mask;

	mbegin(psleep, 6)
	memset(psleep, 0x90, 6);
	mend(psleep, 6)
	
	mbegin(pspinlock, 2)
	memset(pspinlock, 0x90, 2);
	mend(pspinlock, 2)
	
	mbegin(pmin_tstep, 1)
	*pmin_tstep = 0xEB;
	mend(pmin_tstep, 1)
	
	mbegin(pmax_tstep, 2)
	pmax_tstep[0] = 0x90;
	pmax_tstep[1] = 0xE9;
	mend(pmax_tstep, 2)
}

void fps_cap()
{
	u32 old_protect_mask;
	
	u32* psleep_delta = (u32*)(psleep + 2);
	
	mbegin(psleep, 6)
	psleep[0] = 0xFF;
	psleep[1] = 0x15;
	*psleep_delta = 0x062C4BD3;
	mend(psleep, 6)
	
	mbegin(pspinlock, 2)
	pspinlock[0] = 0x77;
	pspinlock[1] = 0x9F;
	mend(pspinlock, 2)
	
	mbegin(pmin_tstep, 1)
	*pmin_tstep = 0x73;
	mend(pmin_tstep, 1)
	
	mbegin(pmax_tstep, 2)
	pmax_tstep[0] = 0x0F;
	pmax_tstep[1] = 0x86;
	mend(pmax_tstep, 2)
}

#undef mbegin
#undef mend

/* ------------------------------------------------------------- */

internal LRESULT CALLBACK wndproc(HWND, UINT, WPARAM, LPARAM);
 
internal
void init_wnd(HINSTANCE instance)
{
    WNDCLASSEXA wc = { 0 };
 
    wc.cbSize = sizeof(WNDCLASSEXA);
    wc.lpfnWndProc = wndproc;
    wc.hInstance = instance;
    wc.lpszClassName = UNCAPPED_WNDCLASS;
 
    if (!RegisterClassExA(&wc))
	{
        err("RegisterClassExA");
        return;
    }
 
    wnd = CreateWindowExA(
        0,
        UNCAPPED_WNDCLASS,
        UNCAPPED_TITLE,
        0, 0, 0, 0, 0,
		HWND_MESSAGE,
        0,
		instance,
		0
	);
	
    if (!wnd) {
        err("CreateWindowExA");
        return;
    }
 
    ShowWindow(wnd, SW_HIDE);
}

#define ID_STARTUP_TIMER	1
#define ID_TIMER			2
#define ID_CLOSE_TIMER		3

b32 capped = 1;

internal
LRESULT CALLBACK wndproc(HWND wnd, UINT msg, WPARAM wp, LPARAM lp)
{
    switch (msg)
	{
		case WM_CREATE:
			SetTimer(wnd, ID_STARTUP_TIMER, 5000, 0);
			break;
	 
		case WM_CLOSE:
			DestroyWindow(wnd);
			break;
			
		case WM_TIMER:
			switch (wp)
			{
				/* 5s after game starts */
				case ID_STARTUP_TIMER:
					init_addresses();
					KillTimer(wnd, ID_STARTUP_TIMER);
					SetTimer(wnd, ID_TIMER, 200, 0);
					SetTimer(wnd, ID_CLOSE_TIMER, 1000, 0);
					break;
				
				/* fast timer */
				case ID_TIMER:
				{
					b32 should_be_capped =
						*pmenu || *ptitle_or_load || *phacking;
					
					if (!capped && should_be_capped) {
						fps_cap();
						capped ^= 1;
					} else if (capped && !should_be_capped) {
						fps_uncap();
						capped ^= 1;
					}

					break;
				}
				
				/* slow timer */
				case ID_CLOSE_TIMER:
					if (!FindWindow(GAME_WNDCLASS, 0)) {
						CloseWindow(wnd);
					}
			}
			break;
	 
		case WM_DESTROY:
			KillTimer(wnd, ID_TIMER);
			KillTimer(wnd, ID_STARTUP_TIMER);
			PostQuitMessage(0);
			break;
	 
		default:
			return DefWindowProcA(wnd, msg, wp, lp);
    }
	
    return 0;
}

/* ------------------------------------------------------------- */

#define LODWORD(l) ((DWORD)((DWORDLONG)(l)))
#define HIDWORD(l) ((DWORD)(((DWORDLONG)(l)>>32)&0xFFFFFFFF))

#define X64_JMP_LEN 14

internal
void write_64bit_jmp(void* src, void* dst, u32 nops)
{
	u8 bytecode[X64_JMP_LEN] =
	{
		/* put 64bit addr on the stack (placeholder zero values) */
		0x68,                   0x00, 0x00, 0x00, 0x00, /* lo-DW */
		0xC7, 0x44, 0x24, 0x04, 0x00, 0x00, 0x00, 0x00, /* hi-DW */
		
		/* jump to it with a ret */
		0xC3
	};

	u32* pldword = (u32*)&bytecode[1];
	u32* phdword = (u32*)&bytecode[9];
	
	*pldword = LODWORD(dst);
	*phdword = HIDWORD(dst);
	
	memcpy(src, bytecode, sizeof(bytecode));
	memset((u8*)src + sizeof(bytecode), 0x90, nops);
}

/*
	USER32.CreateWindowExA
	773FA230 - 4C 8B DC              - mov r11,rsp
	773FA233 - 48 83 EC 78           - sub rsp,78
	773FA237 - 48 8B 84 24 D8000000  - mov rax,[rsp+000000D8]
	773FA23F - C7 44 24 60 01000040  - mov [rsp+60],40000001
	773FA247 - 49 89 43 E0           - mov [r11-20],rax
	773FA24B - 48 8B 84 24 D0000000  - mov rax,[rsp+000000D0]
	773FA253 - 49 89 43 D8           - mov [r11-28],rax
*/

const
u8 original_code[15] =
{
	0x4C, 0x8B, 0xDC,
	0x48, 0x83, 0xEC, 0x78,
	0x48, 0x8B, 0x84, 0x24, 0xD8, 0x00, 0x00, 0x00
};

globvar u8 trampoline[sizeof(original_code) + X64_JMP_LEN];

#define CREATE_WINDOW_EX_A_SIG(name) \
HWND WINAPI name(			\
	u32 ex_style_mask,		\
	char const* class_name, \
	char const* wnd_name,	\
	u32 style_mask,			\
	int x, int y,			\
	int w, int h,			\
	HWND parent_wnd,		\
	HMENU menu,				\
	HINSTANCE instance,		\
	void* param)
	
typedef CREATE_WINDOW_EX_A_SIG(sig_CreateWindowExA);

globvar u32 old_protect_mask; /* to restore CreateWindow mem */

globvar sig_CreateWindowExA* pCreateWindowExA = 0;
globvar sig_CreateWindowExA* pCreateWindowExA_original = 0;

internal void unhook();

internal
CREATE_WINDOW_EX_A_SIG(CreateWindowExA_hook)
{	
	if (!strcmp(class_name, GAME_WNDCLASS))
	{
		init_wnd(instance);
		unhook();
	}
	
	return
		pCreateWindowExA(
			ex_style_mask,
			class_name,
			wnd_name,
			style_mask,
			x, y,
			w, h,
			parent_wnd,
			menu,
			instance,
			param
		);
}

internal
void hook()
{
	b32 res;
	u32 trash;
	
	pCreateWindowExA =
	pCreateWindowExA_original =
		(sig_CreateWindowExA*)GetProcAddress(
			GetModuleHandle("USER32.DLL"),
			"CreateWindowExA"
		);
		
	if (!pCreateWindowExA) {
		err("GetProcAddress");
		return;
	}
	
	/* create trampoline to original CreateWindowExA */
	memcpy(trampoline, original_code, sizeof(original_code));
	write_64bit_jmp(
		trampoline + sizeof(original_code),
		(u8*)pCreateWindowExA_original + sizeof(original_code),
		0
	);
	
	res =
		VirtualProtect(
			trampoline,
			sizeof(trampoline),
			PAGE_EXECUTE_READWRITE,
			&trash
		);
		
	if (!res) {
		err("VirtualProtect@trampoline");
		return;
	}
	
	pCreateWindowExA = (sig_CreateWindowExA*)&trampoline[0];
	
	/* hook CreateWindowExA */
	res =
		VirtualProtect(
			pCreateWindowExA_original,
			sizeof(original_code),
			PAGE_EXECUTE_READWRITE,
			&old_protect_mask
		);
		
	if (!res) {
		err("VirtualProtect@CreateWindowExA");
		return;
	}
	
	write_64bit_jmp(
		pCreateWindowExA_original,
		CreateWindowExA_hook,
		sizeof(original_code) - X64_JMP_LEN
	);
}

internal
void unhook()
{
	b32 res;
	
	memcpy(
		pCreateWindowExA_original,
		original_code,
		sizeof(original_code)
	);
	
	res =
		VirtualProtect(
			pCreateWindowExA_original,
			sizeof(original_code),
			old_protect_mask,
			&old_protect_mask
		);
		
	if (!res) {
		err("VirtualProtect");
	}
	
	pCreateWindowExA = pCreateWindowExA_original;
}

b32 APIENTRY DllMain(
	HMODULE module,
	u32 reason_for_call,
	void* reserved)
{
	(void)reserved;
	
	switch (reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
			CreateThread(
				NULL, 0,
				(LPTHREAD_START_ROUTINE)hook,
				0, 0, 0
			);
			
		case DLL_PROCESS_DETACH:
			if (wnd != INVALID_HANDLE_VALUE) {
				CloseWindow(wnd);
				/* unsafe if detached while the init func is
				   running but who cares */
			}
	}
	
	return 1;
}