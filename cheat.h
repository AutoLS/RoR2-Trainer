#ifndef CHEAT_H
#define CHEAT_H

enum cheat_buttons
{
	CHEAT_BUTTON_GOLD,
	CHEAT_BUTTON_EXP,
	CHEAT_BUTTON_LUNAR,
	CHEAT_BUTTON_MAX,
};

enum cheat_checkbox
{
	CHECKBOX_UNLIMITED_JUMPS,
	CHECKBOX_GOD_MODE,
	CHECKBOX_NO_TP_CHARGE,
	CHECKBOX_SKIP_TP,
	CHECKBOX_MAX,
};

enum cheat_state
{
	STATE_MAIN,
	STATE_HELP,
};

struct help_menu
{
	label HotKeyText;
	label HotKeys[CHECKBOX_MAX + CHEAT_BUTTON_MAX];
	button Back;
};

#endif