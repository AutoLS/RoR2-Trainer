#include <AE/AutoEngine.h>

#define ArraySize(arr) (sizeof(arr)/sizeof(arr[0]))

#include "proc.cpp"
#include "UI.cpp"

enum cheat_toggles
{
	UNLIMITED_JUMPS,
	TOGGLES_MAX,
};

bool Toggles[TOGGLES_MAX] = {};

void fflush_stdin()
{ int c; while ((c = getchar()) != '\n' && c != EOF); }

bool ContainsAllNumbers(char* str)
{
	bool Result = false;
	int Count = (int)strlen(str);
	for(int i = 0; i < Count; ++i)
	{
		Result = str[i] >= 48 && str[i] <= 57;
	}
	return Result;
}

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
	
	uint32 GoldValue = 0;
	
	TTF_Font* ButtonFont = TTF_OpenFont("ast/LibreBaskerville-Regular.ttf", 12);
	TTF_Font* LabelsFont = TTF_OpenFont("ast/LibreBaskerville-Regular.ttf", 20);
	TTF_Font* LabelsFontS = TTF_OpenFont("ast/LibreBaskerville-Regular.ttf", 16);
	int LabelH = TTF_FontHeight(LabelsFont);
	
	label StatusLabelFailed = CreateLabel(V2(), "Cannot find Risk of Rain 2.exe...", 
										  LabelsFontS, 
									     {255, 0, 0, 255}, Graphics.Renderer);
	label StatusLabelSucceed = CreateLabel(V2(), 
										   "Opened Risk of Rain 2.exe for writing...", 
										   LabelsFontS, 
										   {0, 255, 0, 255}, Graphics.Renderer);
	
	image_data* UITextures = LoadUITextures(Graphics.Renderer);
	
	v2 GoldPos = V2(100, 50);
	button GoldButton = CreateButton(GoldPos, V2(50, 50), 
									 UITextures[0], UITextures[1],
									 Graphics.Renderer, ButtonFont, "+10k");
	label GoldLabel = CreateLabel(V2(10, GoldPos.y+25-LabelH*0.5f), "Gold", 
    							  LabelsFont, {255, 255, 255, 255},
								  Graphics.Renderer);
					
	v2 ULJPos = GoldPos + V2(100, 60);
	checkbox UnlimitedJumpsCB = CreateCheckbox(ULJPos, V2(38, 36),
											   UITextures[2], UITextures[3]);
											   
	label UnlimitedJumpsLabel = CreateLabel(V2(10, ULJPos.y+18-LabelH*0.5f), 
											"Unlimited Jumps", 
											LabelsFont, {255, 255, 255, 255},
											Graphics.Renderer);
											
	v2 GodModePos = ULJPos + V2(0, 60);
	checkbox GodModeCB = CreateCheckbox(GodModePos, V2(38, 36),
										UITextures[2], UITextures[3]);
											   
	label GodModeLabel = CreateLabel(V2(10, GodModePos.y+18-LabelH*0.5f), 
									 "God mode", 
									 LabelsFont, {255, 255, 255, 255},
									 Graphics.Renderer);
							
	timer AttachProcessTimer = CreateTimer(2.0f);
	
	bool Running = true;
	
	while(Running)
	{
		image_data GoldValueText = {};
		
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
		
		HandleButton(&GoldButton, &Input);
		HandleCheckbox(&UnlimitedJumpsCB, &Input);
		HandleCheckbox(&GodModeCB, &Input);
		
		if(Attach.Status == ATTACH_SUCCEED)
		{
			uint32 GoldAddrOffset[] = {0x128, 0x760, 0x10, 0x28, 0x58, 0xA0};
			uint32 GoldModuleOffset = 0x015A95D8;

			uintptr_t GoldAddr = GetDAMAddr(&Attach, ModuleName, GoldModuleOffset,
											GoldAddrOffset, ArraySize(GoldAddrOffset));
											 
			if(ReadProcessMemory(Attach.Process, 
							 (void*)GoldAddr, &GoldValue, 
							  sizeof(GoldValue), 0))
			{
				if(GoldButton.State == BUTTON_STATE_L_CLICK)
				{						 
					GoldValue += 10000;
					WriteProcessMemory(Attach.Process, (void*)GoldAddr, 
									   &GoldValue, sizeof(GoldValue), 0);
				}
			}		
		}
		
		char Buffer[100] = {};
		sprintf(Buffer, "%d", GoldValue);
		LoadText(Graphics.Renderer, LabelsFont, &GoldValueText, 
				 Buffer, {255, 0, 0, 255});
				 
		rect32 TextRect = GoldLabel.Rect;
		TextRect.Pos += V2(150, 0);
		TextRect.Dim = V2(GoldValueText.Dim);
		
		GoldValueText.Rect = SetRectRounded(TextRect);
		
		if(Attach.Status == ATTACH_SUCCEED)
		{
			if(UnlimitedJumpsCB.Active)
			{
				uint32 JumpModuleOffset = 0x01532288;
				uint32 JumpAddrOffset[] = 
				{0x50, 0x188, 0x378, 0x60, 0x30, 0x40, 0x8C};
				uintptr_t JumpAddr = GetDAMAddr(&Attach, ModuleName, JumpModuleOffset,
												JumpAddrOffset, ArraySize(JumpAddrOffset));
				
				uint32 Value = 0;
				uint32 Jumps = 0;
				
				WriteProcessMemory(Attach.Process, (void*)JumpAddr, 
								   &Jumps, sizeof(Jumps), 0);
				ReadProcessMemory(Attach.Process, 
							 (void*)JumpAddr, &Value, 
							  sizeof(Value), 0);
				//printf("JumpAddr: %llx, Value: %d\n", JumpAddr, Value);
			}
			
			if(GodModeCB.Active)
			{
				uint32 ModuleOffset = 0x00491DE8;
				uint32 AddrOffset[] = {0x490, 0x1B8, 0x60, 0x60, 0x68, 0xA8, 0x68};
				
				uintptr_t HPAddr = GetDAMAddr(&Attach, "mono-2.0-bdwgc.dll", ModuleOffset,
										      AddrOffset, ArraySize(AddrOffset));

				real32 Value = 9999;
				WriteProcessMemory(Attach.Process, (void*)HPAddr, 
								   &Value, sizeof(Value), 0);			   
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
			while(Win32GetSecondElapsed(LastCount, SDL_GetPerformanceCounter()) <
				  Graphics.Display.TargetSecPerFrame);
		}
		
		uint64 EndCount = SDL_GetPerformanceCounter();
		
		//Render
		Attach.Status ? RenderLabel(&StatusLabelSucceed, 
									 Graphics.Renderer) 
					  : RenderLabel(&StatusLabelFailed, 
									 Graphics.Renderer);
		RenderButton(&GoldButton, Graphics.Renderer);
		RenderLabel(&GoldLabel, Graphics.Renderer);
		RenderCheckbox(&UnlimitedJumpsCB, Graphics.Renderer);
		RenderCheckbox(&GodModeCB, Graphics.Renderer);
		RenderLabel(&UnlimitedJumpsLabel, Graphics.Renderer);
		RenderLabel(&GodModeLabel, Graphics.Renderer);
		SDL_RenderCopy(Graphics.Renderer, GoldValueText.Texture,
					   0, &GoldValueText.Rect);
		SDL_DestroyTexture(GoldValueText.Texture);
		SDL_RenderPresent(Graphics.Renderer);
		
		real32 ElapsedTime = Win32GetSecondElapsed(LastCount, EndCount);
		t = ElapsedTime;
		
		LastCount = EndCount;
	}
	
	return 0;
}