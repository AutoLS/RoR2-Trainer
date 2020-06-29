#include <AE/AutoEngine.h>

#define ArraySize(arr) (sizeof(arr)/sizeof(arr[0]))

#include "proc.cpp"
#include "UI.cpp"

enum cheat_buttons
{
	CHEAT_BUTTON_GOLD,
	CHEAT_BUTTON_EXP,
	CHEAT_BUTTON_MAX,
};

enum cheat_checkbox
{
	CHECKBOX_UNLIMITED_JUMPS,
	CHECKBOX_GOD_MODE,
	CHECKBOX_MAX,
};

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
	render Graphics = InitGraphics("RoR2 Trainer v1.0", {400, 400}, 0);
	
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
		"EXP"
	};
	
	char* CheckboxLabelNames[] = 
	{
		"Unlimited Jumps",
		"God Mode"
	};
	
	button CheatButtons[CHEAT_BUTTON_MAX] = {};
	label ButtonLabels[CHEAT_BUTTON_MAX] = {};
	for(int i = 0; i < CHEAT_BUTTON_MAX; ++i)
	{
		CheatButtons[i] = CreateButton(V2(100, 50 + (i*60.0f)), V2(50, 50), UITextures[0], UITextures[1], Graphics.Renderer, ButtonFont, "+10k");
		ButtonLabels[i] = CreateLabel(V2(10, CheatButtons[i].Pos.y+25-LabelH*0.5f), ButtonLabelsName[i], LabelsFont, {255, 255, 255, 255}, Graphics.Renderer);
	}
	
	checkbox CheatCB[CHECKBOX_MAX] = {};
	label CheckboxLabels[CHECKBOX_MAX] = {};
	for(int i = 0; i < CHECKBOX_MAX; ++i)
	{
		CheatCB[i] = CreateCheckbox(V2(200, 230 + (i*60.0f)), V2(38, 36), UITextures[2], UITextures[3]);
		CheckboxLabels[i] = CreateLabel(V2(10, CheatCB[i].Rect.Pos.y+18-LabelH*0.5f), CheckboxLabelNames[i], LabelsFont, {255, 255, 255, 255}, Graphics.Renderer);
	}
							
	timer AttachProcessTimer = CreateTimer(5.0f);
	
	bool Running = true;
	
	while(Running)
	{
		image_data ValueText[CHEAT_BUTTON_MAX] = {};
		
		ProcessTimer(&AttachProcessTimer, t);
		if(AttachProcessTimer.Complete)
		{
			StartTimer(&AttachProcessTimer);
			Attach = AttachProcess(ProcessName);
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
		
		while(t > 0)
		{
			real32 dt = Min(t, Graphics.Display.TargetSecPerFrame);
			//Logic
			t -= dt;
		}
		
		HandleButtons(CheatButtons, &Input, CHEAT_BUTTON_MAX);
		HandleCheckboxes(CheatCB, &Input, CHECKBOX_MAX);
		
		if(Attach.Status == ATTACH_SUCCEED)
		{
			uint32 GoldAddrOffset[] = {0x128, 0x760, 0x10, 0x28, 0x58, 0xA0};
			uint32 GoldModuleOffset = 0x015A95D8;

			uintptr_t GoldAddr = GetDAMAddr(&Attach, ModuleName, GoldModuleOffset, GoldAddrOffset, ArraySize(GoldAddrOffset));
											 
			if(ReadProcessMemory(Attach.Process, (void*)GoldAddr, &CheatValue[CHEAT_BUTTON_GOLD], sizeof(CheatValue[CHEAT_BUTTON_GOLD]), 0))
			{
				if(CheatButtons[CHEAT_BUTTON_GOLD].State == BUTTON_STATE_L_CLICK)
				{						 
					CheatValue[CHEAT_BUTTON_GOLD] += 10000;
					WriteProcessMemory(Attach.Process, (void*)GoldAddr, &CheatValue[CHEAT_BUTTON_GOLD], sizeof(CheatValue[CHEAT_BUTTON_GOLD]), 0);
				}
			}
			
			uint32 EXPAddrOffset[] = {0x10, 0x600, 0x558, 0x30, 0x28};
			uint32 EXPModuleOffset = 0x491DC8;
			
			uintptr_t EXPAddr = GetDAMAddr(&Attach, "mono-2.0-bdwgc.dll", EXPModuleOffset, EXPAddrOffset, ArraySize(EXPAddrOffset));
			
			if(ReadProcessMemory(Attach.Process, (void*)EXPAddr, &CheatValue[CHEAT_BUTTON_EXP], sizeof(CheatValue[CHEAT_BUTTON_EXP]), 0))
			{
				if(CheatButtons[CHEAT_BUTTON_EXP].State == BUTTON_STATE_L_CLICK)
				{						 
					CheatValue[CHEAT_BUTTON_EXP] += 10000;
					WriteProcessMemory(Attach.Process, (void*)EXPAddr, &CheatValue[CHEAT_BUTTON_EXP], sizeof(CheatValue[CHEAT_BUTTON_EXP]), 0);
				}
			}
		}
		
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
		}
		
		real32 FrameCompleteTime = 
		Win32GetSecondElapsed(LastCount, SDL_GetPerformanceCounter());
		
		if(FrameCompleteTime < Graphics.Display.TargetSecPerFrame)
		{
			int SleepTime = (int)(Graphics.Display.TargetSecPerFrame - 
								  FrameCompleteTime * 1000) - 1;
			if(SleepTime > 0)
			{
				Sleep(SleepTime);
			}
			//assert(Win32GetSecondElapsed(LastCount, SDL_GetPerformanceCounter()) < TargetSecPerFrame);
			while(Win32GetSecondElapsed(LastCount, SDL_GetPerformanceCounter()) < Graphics.Display.TargetSecPerFrame);
		}
		
		uint64 EndCount = SDL_GetPerformanceCounter();
		
		//Render
		Attach.Status ? RenderLabel(&StatusLabelSucceed, Graphics.Renderer) : RenderLabel(&StatusLabelFailed, Graphics.Renderer);
		
		RenderButtons(CheatButtons, Graphics.Renderer, CHEAT_BUTTON_MAX);
		RenderCheckboxes(CheatCB, Graphics.Renderer, CHECKBOX_MAX);
		RenderLabels(ButtonLabels, Graphics.Renderer, CHEAT_BUTTON_MAX);
		RenderLabels(CheckboxLabels, Graphics.Renderer, CHECKBOX_MAX);
		
		for(int i = 0; i < CHEAT_BUTTON_MAX; ++i)
		{
			SDL_RenderCopy(Graphics.Renderer, ValueText[i].Texture, 0, &ValueText[i].Rect);
			SDL_DestroyTexture(ValueText[i].Texture);
		}
		
		SDL_RenderPresent(Graphics.Renderer);
		
		real32 ElapsedTime = Win32GetSecondElapsed(LastCount, EndCount);
		t = ElapsedTime;
		
		LastCount = EndCount;
	}
	
	return 0;
}