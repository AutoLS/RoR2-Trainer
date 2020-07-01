#include "cheat.h"

help_menu InitHelpMenu(TTF_Font* LabelFont, TTF_Font* ButtonFont, render* Graphics, image_data* UITextures)
{
	help_menu HelpMenu = {};
	
	SDL_Color White = {255, 255, 255, 255};
	
	HelpMenu.HotKeyText = CreateLabel(V2(15, 15), "Hotkeys", LabelFont, White, Graphics->Renderer);
	
	char* HotkeyText[] = 
	{
		"G - Increase gold",
		"H - Increase EXP",
		"L - Increase lunar coins",
		"F1 - Toggle unlimited jumps",
		"F2 - Toggle god mode",
		"F3 - Toggle no teleport charge",
		"F4 - Toggle skip teleport"
	};
	
	real32 FontH = (real32)TTF_FontHeight(LabelFont);
	
	for(int i = 0; i < ArraySize(HelpMenu.HotKeys); ++i)
	{
		HelpMenu.HotKeys[i] = CreateLabel(V2(15, 30 + (i*FontH + 10)), HotkeyText[i], LabelFont, White, Graphics->Renderer);
	}
	
	HelpMenu.Back = CreateButton(V2(15, Graphics->WinDim.y - 65), V2(50, 50), UITextures[0], UITextures[1], Graphics->Renderer, ButtonFont, "Back");
	
	return HelpMenu;
}

void UpdateHelpMenu(help_menu* HelpMenu, game_input* Input)
{
	HandleButton(&HelpMenu->Back, Input);
}

void RenderHelpMenu(help_menu* HelpMenu, SDL_Renderer* Renderer)
{
	RenderLabel(&HelpMenu->HotKeyText, Renderer);
	RenderLabels(HelpMenu->HotKeys, Renderer, ArraySize(HelpMenu->HotKeys));
	RenderButton(&HelpMenu->Back, Renderer);
};