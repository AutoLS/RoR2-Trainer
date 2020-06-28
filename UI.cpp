#include "UI.h"

image_data* LoadUITextures(SDL_Renderer* Renderer)
{
	image_data* UITextures = (image_data*)malloc(sizeof(image_data)*UI_TEXTURE_CHECKBOX_MAX);
	
	LoadTexture(&UITextures[UI_TEXTURE_BLUE_BUTTON], "ast/blue_button06.png", 
				Renderer, 255);
	LoadTexture(&UITextures[UI_TEXTURE_BLUE_BUTTON_ACTIVE], "ast/blue_button10.png", 
				Renderer, 255);
	LoadTexture(&UITextures[UI_TEXTURE_CHECKBOX], "ast/grey_box.png", 
				Renderer, 255);
	LoadTexture(&UITextures[UI_TEXTURE_CHECKBOX_ACTIVE], 
				"ast/blue_boxCheckmark.png", 
				Renderer, 255);
				
	return UITextures;
}

button CreateButton(v2 Pos, v2 Dim,  
					image_data Texture1, image_data Texture2,
					SDL_Renderer* Renderer, TTF_Font* Font = 0, char* Text = 0)
{
	button Button = {};
	Button.Rect = Rect32(Pos, Dim);
	if(Text)
	{
		LoadText(Renderer, Font, &Button.Text, Text, {});
		Button.TextRect = {V2(), V2(Button.Text.Dim)};
		SetRect32ScreenSpace(&Button.TextRect, &Button.Rect, V2(), 
							 POSITION_CENTERED);
	}		
	Button.IdleTexture = Texture1;
	Button.EventTexture = Texture2;
	
	return Button;
}

void HandleButton(button* Button, game_input* Input)
{
	v2 MouseCoord = V2(Input->MouseState);
	Button->State = BUTTON_STATE_NONE;
	if(IsPointInRect(MouseCoord, &Button->Rect))
	{
		Button->State = BUTTON_STATE_HOVER;
		
		if(Input->Mouse.Keys[BUTTON_LEFT].EndedDown)
		{
			Button->State = BUTTON_STATE_L_CLICK;
		}
		if(Input->Mouse.Keys[BUTTON_RIGHT].EndedDown)
		{
			Button->State = BUTTON_STATE_R_CLICK;
		}
	}
}

void HandleButtons(button* Button, game_input* Input, int Count)
{
	for(int i = 0; i < Count; ++i)
	{
		HandleButton(&Button[i], Input);
	}
}

void RenderButton(button* Button, SDL_Renderer* Renderer)
{
	switch(Button->State)
	{
		case BUTTON_STATE_NONE:
		{
			SDL_Rect ButtonRect = SetRectRounded(Button->Rect);
			SDL_RenderCopy(Renderer, Button->IdleTexture.Texture, 0, &ButtonRect);
			
			if(Button->Text.Texture)
			{
				SDL_Rect TextRect = SetRectRounded(Button->TextRect);
				SDL_RenderCopy(Renderer, Button->Text.Texture, 0, &TextRect);
			}
		} break;
		case BUTTON_STATE_HOVER:
		case BUTTON_STATE_L_CLICK:
		case BUTTON_STATE_R_CLICK:
		{
			SDL_Rect ButtonRect = SetRectRounded(Button->Rect);
			SDL_RenderCopy(Renderer, Button->EventTexture.Texture, 0, &ButtonRect);
			if(Button->Text.Texture)
			{
				SDL_Rect TextRect = SetRectRounded(Button->TextRect);
				SDL_RenderCopy(Renderer, Button->Text.Texture, 0, &TextRect);
			}
		} break;
	}
}

void RenderButtons(button* Buttons, SDL_Renderer* Renderer, int Count)
{
	for(int i = 0; i < Count; ++i)
	{
		RenderButton(&Buttons[i], Renderer);
	}
}

label CreateLabel(v2 Pos, char* Text, TTF_Font* Font, SDL_Color Color,
				  SDL_Renderer* Renderer)
{
	label Label = {};
	LoadText(Renderer, Font, &Label.Text, Text, Color);
	Label.Rect = Rect32(Pos, V2(Label.Text.Dim));
	Label.RenderRect = SetRectRounded(Label.Rect);
	
	return Label;
}

void RenderLabel(label* Label, SDL_Renderer* Renderer)
{
	SDL_RenderCopy(Renderer, Label->Text.Texture, 0, &Label->RenderRect);
}

void RenderLabels(label* Labels, SDL_Renderer* Renderer, int Count)
{
	for(int i = 0; i < Count; ++i)
	{
		RenderLabel(&Labels[i], Renderer);
	}
}

checkbox CreateCheckbox(v2 Pos, v2 Dim, image_data Texture1, image_data Texture2)
{
	checkbox Checkbox = {};
	Checkbox.Rect = Rect32(Pos, Dim);
	Checkbox.RenderRect = SetRectRounded(Checkbox.Rect);
	Checkbox.Texture = Texture1;
	Checkbox.ActiveTexture = Texture2;
	
	return Checkbox;
}

void HandleCheckbox(checkbox* Checkbox, game_input* Input)
{
	v2 MouseCoord = V2(Input->MouseState);
	if(IsPointInRect(MouseCoord, &Checkbox->Rect))
	{
		if(Input->Mouse.Keys[BUTTON_LEFT].EndedDown)
		{
			Checkbox->Active = !Checkbox->Active;
		}
	}
}

void RenderCheckbox(checkbox* Checkbox, SDL_Renderer* Renderer)
{
	if(Checkbox->Active)
	{
		SDL_RenderCopy(Renderer, Checkbox->ActiveTexture.Texture, 0, &Checkbox->RenderRect);
	}
	else
	{
		SDL_RenderCopy(Renderer, Checkbox->Texture.Texture, 0, &Checkbox->RenderRect);
	}
}