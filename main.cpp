#include <AE/AutoEngine.h>

#define ArraySize(arr) (sizeof(arr)/sizeof(arr[0]))

#include "UI.cpp"
#include "proc.cpp"
#include "cheat.cpp"

#if DEBUG_MODE
int wmain()
#else
int WinMain(HINSTANCE hInstance,
			HINSTANCE hPrevInstance,
			LPSTR     lpCmdLine,
			int       nShowCmd)
#endif
{	
	char* ProcessName = "Risk of Rain 2.exe";
	char* ModuleName = "UnityPlayer.dll";
	
	InitLib(LIB_SDL_FULL);
	render Graphics = InitGraphics("RoR2 Trainer v1.0", {400, 500}, 0);
	
	attach_result Attach = AttachProcess(ProcessName);
	
	rect32 WindowRect = {V2(), Graphics.WinDim};

	game_input Input = {};
	Input.KeyState = SDL_GetKeyboardState(0);
	
	SDL_Event Event;
	uint64 LastCount = SDL_GetPerformanceCounter();
	real32 t = 0;
	
	uint32 CheatValue[CHEAT_BUTTON_MAX] = {};
	
	TTF_Font* ButtonFont = TTF_OpenFont("ast/LibreBaskerville-Regular.ttf", 12);
	TTF_Font* LabelsFont = TTF_OpenFont("ast/LibreBaskerville-Regular.ttf", 20);
	TTF_Font* LabelsFontS = TTF_OpenFont("ast/LibreBaskerville-Regular.ttf", 16);
	int LabelH = TTF_FontHeight(LabelsFont);
	
	label StatusLabelFailed = CreateLabel(V2(), "Cannot find Risk of Rain 2.exe...", LabelsFontS, {255, 0, 0, 255}, Graphics.Renderer);
	label StatusLabelSucceed = CreateLabel(V2(), "Opened Risk of Rain 2.exe for writing...", LabelsFontS, {0, 255, 0, 255}, Graphics.Renderer);
	
	image_data* UITextures = LoadUITextures(Graphics.Renderer);
	
	char* ButtonLabelsName[] = 
	{
		"Gold",
		"EXP",
		"Lunar"
	};
	
	char* AmountIncreaseText[] =
	{
		"+10k",
		"+1m",
		"+50"
	};
	
	char* CheckboxLabelNames[] = 
	{
		"Unlimited Jumps",
		"God Mode",
		"No TP Charge",
		"Skip TP Event"
	};

	button HelpButton = CreateButton(V2(Graphics.WinDim.x - 65, 15), V2(50, 50), UITextures[0], UITextures[1], Graphics.Renderer, ButtonFont, "HELP");
	
	help_menu HelpMenu = InitHelpMenu(LabelsFont, ButtonFont, &Graphics, UITextures);
	
	button CheatButtons[CHEAT_BUTTON_MAX] = {};
	label ButtonLabels[CHEAT_BUTTON_MAX] = {};
	for(int i = 0; i < CHEAT_BUTTON_MAX; ++i)
	{
		CheatButtons[i] = CreateButton(V2(100, 50 + (i*60.0f)), V2(50, 50), UITextures[0], UITextures[1], Graphics.Renderer, ButtonFont, AmountIncreaseText[i]);
		ButtonLabels[i] = CreateLabel(V2(10, CheatButtons[i].Pos.y+25-LabelH*0.5f), ButtonLabelsName[i], LabelsFont, {255, 255, 255, 255}, Graphics.Renderer);
	}
	
	checkbox CheatCB[CHECKBOX_MAX] = {};
	label CheckboxLabels[CHECKBOX_MAX] = {};
	for(int i = 0; i < CHECKBOX_MAX; ++i)
	{
		CheatCB[i] = CreateCheckbox(V2(200, 230 + (i*50.0f)), V2(38, 36), UITextures[2], UITextures[3]);
		CheckboxLabels[i] = CreateLabel(V2(10, CheatCB[i].Rect.Pos.y+18-LabelH*0.5f), CheckboxLabelNames[i], LabelsFont, {255, 255, 255, 255}, Graphics.Renderer);
	}
	
	v3 PlayerCoord = {};
	
	timer AttachProcessTimer = CreateTimer(3.0f);
	timer FreezeTPChargeTimer = CreateTimer(3.0f);
	
	bool Running = true;
	cheat_state CheatState = STATE_MAIN;
	
	while(Running)
	{
		image_data ValueText[CHEAT_BUTTON_MAX] = {};
		image_data PositionText = {};
		
		ProcessTimer(&AttachProcessTimer, t);
		ProcessTimer(&FreezeTPChargeTimer, t);
		if(Attach.Status == ATTACH_SUCCEED)
		{
			DWORD ExitCode = 0;
			GetExitCodeProcess(Attach.Process, &ExitCode);
			if(ExitCode == 0)
			{
				Attach.Status = ATTACH_FAILED;
			}
		}
		else
		{
			if(AttachProcessTimer.Complete)
			{
				StartTimer(&AttachProcessTimer);
				Attach = AttachProcess(ProcessName);
			}
		}
		
		ResetKeyState(&Input.Mouse, BUTTON_MAX);
		ResetKeyState(&Input.Keyboard, KEY_MAX);
		
		while(SDL_PollEvent(&Event) != 0)
		{
			if(HandleEvents(&Event, &Input, &Graphics))
			{
				Running = false;
			}
		}
		
		SDL_SetRenderDrawColor(Graphics.Renderer, 50, 50, 50, 255);
		SDL_RenderClear(Graphics.Renderer);
		
		switch(CheatState)
		{
			case STATE_MAIN:
			{
				HandleButton(&HelpButton, &Input);
				HandleButtons(CheatButtons, &Input, CHEAT_BUTTON_MAX);
				HandleCheckboxes(CheatCB, &Input, CHECKBOX_MAX);
				
				if(HelpButton.State == BUTTON_STATE_L_CLICK)
				{
					CheatState = STATE_HELP;
				}				
			} break;
			case STATE_HELP:
			{
				UpdateHelpMenu(&HelpMenu, &Input);
				if(GetAsyncKeyState(VK_ESCAPE) & 1 || 
				   HelpMenu.Back.State == BUTTON_STATE_L_CLICK)
				{
					CheatState = STATE_MAIN;
				}
			} break;
		}
		
		if(Attach.Status == ATTACH_SUCCEED)
		{
			if(GetAsyncKeyState(VK_F1) & 1)
			{
				CheatCB[CHECKBOX_UNLIMITED_JUMPS].Active = !CheatCB[CHECKBOX_UNLIMITED_JUMPS].Active;
			}
			if(GetAsyncKeyState(VK_F2) & 1)
			{
				CheatCB[CHECKBOX_GOD_MODE].Active = !CheatCB[CHECKBOX_GOD_MODE].Active;
			}
			if(GetAsyncKeyState(VK_F3) & 1)
			{
				CheatCB[CHECKBOX_NO_TP_CHARGE].Active = !CheatCB[CHECKBOX_NO_TP_CHARGE].Active;
			}
			if(GetAsyncKeyState(VK_F4) & 1)
			{
				CheatCB[CHECKBOX_SKIP_TP].Active = !CheatCB[CHECKBOX_SKIP_TP].Active;
			}
		}
		
		if(Attach.Status == ATTACH_SUCCEED)
		{
			uint32 GoldAddrOffset[] = {0x128, 0x760, 0x10, 0x28, 0x58, 0xA0};
			uint32 GoldModuleOffset = 0x015A95D8;

			uintptr_t GoldAddr = GetDAMAddr(&Attach, ModuleName, GoldModuleOffset, GoldAddrOffset, ArraySize(GoldAddrOffset));
			
			ReadWriteMemoryU32(&CheatButtons[CHEAT_BUTTON_GOLD], &Attach, GoldAddr, &CheatValue[CHEAT_BUTTON_GOLD], 10000, 'G');
			
			uint32 EXPAddrOffset[] = {0x10, 0x600, 0x558, 0x30, 0x28};
			uint32 EXPModuleOffset = 0x491DC8;
			
			uintptr_t EXPAddr = GetDAMAddr(&Attach, "mono-2.0-bdwgc.dll", EXPModuleOffset, EXPAddrOffset, ArraySize(EXPAddrOffset));
			
			ReadWriteMemoryU32(&CheatButtons[CHEAT_BUTTON_EXP], &Attach, EXPAddr, &CheatValue[CHEAT_BUTTON_EXP], 1000000, 'H');
			
			uint32 LunarCoinOffset[] = {0x198, 0x428, 0x6E8, 0xD0, 0xA4};
			uint32 LunarCoinModuleOffset = 0x4971D8;
			
			uintptr_t LunarCoinAddr = GetDAMAddr(&Attach, "mono-2.0-bdwgc.dll", LunarCoinModuleOffset, LunarCoinOffset, ArraySize(LunarCoinOffset));
			
			ReadWriteMemoryU32(&CheatButtons[CHEAT_BUTTON_LUNAR], &Attach, LunarCoinAddr, &CheatValue[CHEAT_BUTTON_LUNAR], 50, 'L');
		}
		
		if(Attach.Status == ATTACH_SUCCEED)
		{
			if(CheatCB[CHECKBOX_UNLIMITED_JUMPS].Active)
			{
				uint32 JumpModuleOffset = 0x01532288;
				uint32 JumpAddrOffset[] = 
				{0x50, 0x188, 0x378, 0x60, 0x30, 0x40, 0x8C};
				uintptr_t JumpAddr = GetDAMAddr(&Attach, ModuleName, JumpModuleOffset, JumpAddrOffset, ArraySize(JumpAddrOffset));
				
				uint32 Value = 0;
				uint32 Jumps = 0;
				
				WriteProcessMemory(Attach.Process, (void*)JumpAddr, &Jumps, sizeof(Jumps), 0);
				ReadProcessMemory(Attach.Process, (void*)JumpAddr, &Value, sizeof(Value), 0);
				//printf("JumpAddr: %llx, Value: %d\n", JumpAddr, Value);
			}
			
			if(CheatCB[CHECKBOX_GOD_MODE].Active)
			{
				uint32 ModuleOffset = 0x00491DE8;
				uint32 AddrOffset[] = {0x490, 0x1B8, 0x60, 0x60, 0x68, 0xA8, 0x68};
				
				uintptr_t HPAddr = GetDAMAddr(&Attach, "mono-2.0-bdwgc.dll", ModuleOffset, AddrOffset, ArraySize(AddrOffset));

				real32 Value = 9999;
				WriteProcessMemory(Attach.Process, (void*)HPAddr, &Value, sizeof(Value), 0);			   
			}
			
			if(CheatCB[CHECKBOX_SKIP_TP].Active)
			{
				uint32 ModuleOffset = 0x491DC8;
				uint32 AddrOffset[] = {0x18, 0xA80, 0x210, 0x78, 0x94};
				
				uintptr_t ChargeAddr = GetDAMAddr(&Attach, "mono-2.0-bdwgc.dll", ModuleOffset, AddrOffset, ArraySize(AddrOffset));

				real32 Value = 1;
				WriteProcessMemory(Attach.Process, (void*)ChargeAddr, &Value, sizeof(Value), 0);
			}
			
			if(CheatCB[CHECKBOX_NO_TP_CHARGE].Active && !CheatCB[CHECKBOX_SKIP_TP].Active)
			{
				if(FreezeTPChargeTimer.Complete)
				{
					StartTimer(&FreezeTPChargeTimer);
					uint32 ModuleOffset = 0x491DC8;
					uint32 AddrOffset[] = {0x18, 0xA80, 0x210, 0x78, 0x94};
					
					uintptr_t ChargeAddr = GetDAMAddr(&Attach, "mono-2.0-bdwgc.dll", ModuleOffset, AddrOffset, ArraySize(AddrOffset));

					real32 Value = 0.99f;
					WriteProcessMemory(Attach.Process, (void*)ChargeAddr, &Value, sizeof(Value), 0);	
				}
			}
			
			uint32 CoordModuleOffset = 0x491DE8;
			uint32 CoordAddrOffset[] = {0x490, 0x1B8, 0x60, 0x30, 0x40, 0x30, 0x258};
			
			uintptr_t YCoordAddr = GetDAMAddr(&Attach, "mono-2.0-bdwgc.dll", CoordModuleOffset, CoordAddrOffset, ArraySize(CoordAddrOffset));
			uintptr_t XCoordAddr = YCoordAddr - 0x4; 
			uintptr_t ZCoordAddr = YCoordAddr + 0x4; 
			
			ReadProcessMemory(Attach.Process, (void*)XCoordAddr, &PlayerCoord.x, sizeof(real32), 0);
			ReadProcessMemory(Attach.Process, (void*)YCoordAddr, &PlayerCoord.y, sizeof(real32), 0);
			ReadProcessMemory(Attach.Process, (void*)ZCoordAddr, &PlayerCoord.z, sizeof(real32), 0);
		}
		
		real32 FrameCompleteTime = 
		Win32GetSecondElapsed(LastCount, SDL_GetPerformanceCounter());
		
		if(FrameCompleteTime < Graphics.Display.TargetSecPerFrame)
		{
			int SleepTime = (int)((Graphics.Display.TargetSecPerFrame - Win32GetSecondElapsed(LastCount, SDL_GetPerformanceCounter())) * 1000) - 1;
			if(SleepTime > 0)
			{
				Sleep(SleepTime);
			}
			//assert(Win32GetSecondElapsed(LastCount, SDL_GetPerformanceCounter()) < TargetSecPerFrame);
			while(Win32GetSecondElapsed(LastCount, SDL_GetPerformanceCounter()) < Graphics.Display.TargetSecPerFrame);
		}
		
		uint64 EndCount = SDL_GetPerformanceCounter();
		
		//Render
		switch(CheatState)
		{
			case STATE_MAIN:
			{
				for(int i = 0; i < CHEAT_BUTTON_MAX; ++i)
				{
					char Buffer[100] = {};
					sprintf(Buffer, "%d", CheatValue[i]);
					LoadText(Graphics.Renderer, LabelsFont, &ValueText[i], Buffer, {255, 0, 0, 255});
							 
					rect32 TextRect = ButtonLabels[i].Rect;
					TextRect.Pos += V2(160, 0);
					TextRect.Dim = V2(ValueText[i].Dim);
					
					ValueText[i].Rect = SetRectRounded(TextRect);
				}
				
				char Buffer[100] = {};
				sprintf(Buffer, "X: %.2f Y: %.2f Z: %.2f", PlayerCoord.x, PlayerCoord.y, PlayerCoord.z);
				LoadText(Graphics.Renderer, LabelsFont, &PositionText, Buffer, {0, 255, 0, 255});
				rect32 TextRect = {V2(), V2(PositionText.Dim)};
				SetRect32ScreenSpace(&TextRect, &WindowRect, {}, POSITION_BOTTOM_RIGHT);
				PositionText.Rect = SetRectRounded(TextRect);
				
				Attach.Status ? RenderLabel(&StatusLabelSucceed, Graphics.Renderer) : RenderLabel(&StatusLabelFailed, Graphics.Renderer);
		
				RenderButton(&HelpButton, Graphics.Renderer);
				RenderButtons(CheatButtons, Graphics.Renderer, CHEAT_BUTTON_MAX);
				RenderCheckboxes(CheatCB, Graphics.Renderer, CHECKBOX_MAX);
				RenderLabels(ButtonLabels, Graphics.Renderer, CHEAT_BUTTON_MAX);
				RenderLabels(CheckboxLabels, Graphics.Renderer, CHECKBOX_MAX);
				
				SDL_RenderCopy(Graphics.Renderer, PositionText.Texture, 0, &PositionText.Rect);
				SDL_DestroyTexture(PositionText.Texture);
				
				for(int i = 0; i < CHEAT_BUTTON_MAX; ++i)
				{
					SDL_RenderCopy(Graphics.Renderer, ValueText[i].Texture, 0, &ValueText[i].Rect);
					SDL_DestroyTexture(ValueText[i].Texture);
				}
			} break;
			case STATE_HELP:
			{
				RenderHelpMenu(&HelpMenu, Graphics.Renderer);
			} break;
		}
		
		SDL_RenderPresent(Graphics.Renderer);
		
		real32 ElapsedTime = Win32GetSecondElapsed(LastCount, EndCount);
		t = ElapsedTime;
		
		LastCount = EndCount;
	}
	
	return 0;
}