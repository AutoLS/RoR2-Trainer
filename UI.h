#ifndef UI_H
#define UI_H

enum ui_textures
{
	UI_TEXTURE_BLUE_BUTTON,
	UI_TEXTURE_BLUE_BUTTON_ACTIVE,
	UI_TEXTURE_CHECKBOX,
	UI_TEXTURE_CHECKBOX_ACTIVE,
	UI_TEXTURE_CHECKBOX_MAX,
};

enum button_state
{
	BUTTON_STATE_NONE,
	BUTTON_STATE_L_CLICK,
	BUTTON_STATE_R_CLICK,
	BUTTON_STATE_HOVER,	
};

struct button
{
	image_data IdleTexture;
	image_data EventTexture;
	
	image_data Text;
	
	rect32 Rect;
	rect32 TextRect;
	
	button_state State;
};

struct label
{
	image_data Text;
	
	rect32 Rect;
	SDL_Rect RenderRect;
};

struct checkbox
{
	image_data Texture;
	image_data ActiveTexture;
	
	bool Active;
	
	rect32 Rect;
	SDL_Rect RenderRect;
};

#endif