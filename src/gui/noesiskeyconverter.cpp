// Legacy Noesis-only Qt key translation, deliberately separated from Global.
// This file is part of the legacy GUI and is omitted from Noesis-free builds.
#include "noesiskeyconverter.h"
#include <QHash>

namespace {
QHash<Qt::Key, Noesis::Key> BuildKeyConvertMap()
{
	QHash<Qt::Key, Noesis::Key> keyConvertMap;

	//keyConvertMap.insert( Qt::Key_, Noesis::Key_None );

	keyConvertMap.insert( Qt::Key_Cancel, Noesis::Key_Cancel );
	keyConvertMap.insert( Qt::Key_Backspace, Noesis::Key_Back );
	keyConvertMap.insert( Qt::Key_Tab, Noesis::Key_Tab );
	//keyConvertMap.insert( Qt::Key_, Noesis::Key_LineFeed );
	keyConvertMap.insert( Qt::Key_Clear, Noesis::Key_Clear );
	keyConvertMap.insert( Qt::Key_Return, Noesis::Key_Return );
	keyConvertMap.insert( Qt::Key_Enter, Noesis::Key_Enter );
	keyConvertMap.insert( Qt::Key_Pause, Noesis::Key_Pause );
	//keyConvertMap.insert( Qt::Key_, Noesis::Key_Capital );
	keyConvertMap.insert( Qt::Key_CapsLock, Noesis::Key_CapsLock );

	keyConvertMap.insert( Qt::Key_Escape, Noesis::Key_Escape );

	//keyConvertMap.insert( Qt::Key_, Noesis::Key_ImeConvert );
	//keyConvertMap.insert( Qt::Key_, Noesis::Key_ImeNonConvert );
	//keyConvertMap.insert( Qt::Key_, Noesis::Key_ImeAccept );
	//keyConvertMap.insert( Qt::Key_, Noesis::Key_ImeModeChange );

	keyConvertMap.insert( Qt::Key_Space, Noesis::Key_Space );

	//keyConvertMap.insert( Qt::Key_, Noesis::Key_Prior );
	keyConvertMap.insert( Qt::Key_PageUp, Noesis::Key_PageUp );
	//keyConvertMap.insert( Qt::Key_, Noesis::Key_Next );
	keyConvertMap.insert( Qt::Key_PageDown, Noesis::Key_PageDown );
	keyConvertMap.insert( Qt::Key_End, Noesis::Key_End );
	keyConvertMap.insert( Qt::Key_Home, Noesis::Key_Home );

	keyConvertMap.insert( Qt::Key_Left, Noesis::Key_Left );
	keyConvertMap.insert( Qt::Key_Up, Noesis::Key_Up );
	keyConvertMap.insert( Qt::Key_Right, Noesis::Key_Right );
	keyConvertMap.insert( Qt::Key_Down, Noesis::Key_Down );

	keyConvertMap.insert( Qt::Key_Select, Noesis::Key_Select );
	keyConvertMap.insert( Qt::Key_Print, Noesis::Key_Print );
	keyConvertMap.insert( Qt::Key_Execute, Noesis::Key_Execute );
	//keyConvertMap.insert( Qt::Key_, Noesis::Key_Snapshot );
	//keyConvertMap.insert( Qt::Key_, Noesis::Key_PrintScreen );
	keyConvertMap.insert( Qt::Key_Insert, Noesis::Key_Insert );
	keyConvertMap.insert( Qt::Key_Delete, Noesis::Key_Delete );
	keyConvertMap.insert( Qt::Key_Help, Noesis::Key_Help );

	keyConvertMap.insert( Qt::Key_0, Noesis::Key_D0 );
	keyConvertMap.insert( Qt::Key_1, Noesis::Key_D1 );
	keyConvertMap.insert( Qt::Key_2, Noesis::Key_D2 );
	keyConvertMap.insert( Qt::Key_3, Noesis::Key_D3 );
	keyConvertMap.insert( Qt::Key_4, Noesis::Key_D4 );
	keyConvertMap.insert( Qt::Key_5, Noesis::Key_D5 );
	keyConvertMap.insert( Qt::Key_6, Noesis::Key_D6 );
	keyConvertMap.insert( Qt::Key_7, Noesis::Key_D7 );
	keyConvertMap.insert( Qt::Key_8, Noesis::Key_D8 );
	keyConvertMap.insert( Qt::Key_9, Noesis::Key_D9 );

	keyConvertMap.insert( Qt::Key_A, Noesis::Key_A );
	keyConvertMap.insert( Qt::Key_B, Noesis::Key_B );
	keyConvertMap.insert( Qt::Key_C, Noesis::Key_C );
	keyConvertMap.insert( Qt::Key_D, Noesis::Key_D );
	keyConvertMap.insert( Qt::Key_E, Noesis::Key_E );
	keyConvertMap.insert( Qt::Key_F, Noesis::Key_F );
	keyConvertMap.insert( Qt::Key_G, Noesis::Key_G );
	keyConvertMap.insert( Qt::Key_H, Noesis::Key_H );
	keyConvertMap.insert( Qt::Key_I, Noesis::Key_I );
	keyConvertMap.insert( Qt::Key_J, Noesis::Key_J );
	keyConvertMap.insert( Qt::Key_K, Noesis::Key_K );
	keyConvertMap.insert( Qt::Key_L, Noesis::Key_L );
	keyConvertMap.insert( Qt::Key_M, Noesis::Key_M );
	keyConvertMap.insert( Qt::Key_N, Noesis::Key_N );
	keyConvertMap.insert( Qt::Key_O, Noesis::Key_O );
	keyConvertMap.insert( Qt::Key_P, Noesis::Key_P );
	keyConvertMap.insert( Qt::Key_Q, Noesis::Key_Q );
	keyConvertMap.insert( Qt::Key_R, Noesis::Key_R );
	keyConvertMap.insert( Qt::Key_S, Noesis::Key_S );
	keyConvertMap.insert( Qt::Key_T, Noesis::Key_T );
	keyConvertMap.insert( Qt::Key_U, Noesis::Key_U );
	keyConvertMap.insert( Qt::Key_V, Noesis::Key_V );
	keyConvertMap.insert( Qt::Key_W, Noesis::Key_W );
	keyConvertMap.insert( Qt::Key_X, Noesis::Key_X );
	keyConvertMap.insert( Qt::Key_Y, Noesis::Key_Y );
	keyConvertMap.insert( Qt::Key_Z, Noesis::Key_Z );

	keyConvertMap.insert( Qt::Key_ApplicationLeft, Noesis::Key_LWin );
	keyConvertMap.insert( Qt::Key_ApplicationRight, Noesis::Key_RWin );
	//keyConvertMap.insert( Qt::Key_, Noesis::Key_Apps );
	keyConvertMap.insert( Qt::Key_Sleep, Noesis::Key_Sleep );
	/*
	keyConvertMap.insert( Qt::Key_0, Noesis::Key_NumPad0 );
	keyConvertMap.insert( Qt::Key_1, Noesis::Key_NumPad1 );
	keyConvertMap.insert( Qt::Key_2, Noesis::Key_NumPad2 );
	keyConvertMap.insert( Qt::Key_3, Noesis::Key_NumPad3 );
	keyConvertMap.insert( Qt::Key_4, Noesis::Key_NumPad4 );
	keyConvertMap.insert( Qt::Key_5, Noesis::Key_NumPad5 );
	keyConvertMap.insert( Qt::Key_6, Noesis::Key_NumPad6 );
	keyConvertMap.insert( Qt::Key_7, Noesis::Key_NumPad7 );
	keyConvertMap.insert( Qt::Key_8, Noesis::Key_NumPad8 );
	keyConvertMap.insert( Qt::Key_9, Noesis::Key_NumPad9 );
	*/
	keyConvertMap.insert( Qt::Key_multiply, Noesis::Key_Multiply );
	keyConvertMap.insert( Qt::Key_Plus, Noesis::Key_Add );
	//keyConvertMap.insert( Qt::Key_, Noesis::Key_Separator );
	keyConvertMap.insert( Qt::Key_Minus, Noesis::Key_Subtract );
	keyConvertMap.insert( Qt::Key_Colon, Noesis::Key_Decimal );
	keyConvertMap.insert( Qt::Key_division, Noesis::Key_Divide );

	keyConvertMap.insert( Qt::Key_F1, Noesis::Key_F1 );
	keyConvertMap.insert( Qt::Key_F2, Noesis::Key_F2 );
	keyConvertMap.insert( Qt::Key_F3, Noesis::Key_F3 );
	keyConvertMap.insert( Qt::Key_F4, Noesis::Key_F4 );
	keyConvertMap.insert( Qt::Key_F5, Noesis::Key_F5 );
	keyConvertMap.insert( Qt::Key_F6, Noesis::Key_F6 );
	keyConvertMap.insert( Qt::Key_F7, Noesis::Key_F7 );
	keyConvertMap.insert( Qt::Key_F8, Noesis::Key_F8 );
	keyConvertMap.insert( Qt::Key_F9, Noesis::Key_F9 );
	keyConvertMap.insert( Qt::Key_F10, Noesis::Key_F10 );
	keyConvertMap.insert( Qt::Key_F11, Noesis::Key_F11 );
	keyConvertMap.insert( Qt::Key_F12, Noesis::Key_F12 );
	keyConvertMap.insert( Qt::Key_F13, Noesis::Key_F13 );
	keyConvertMap.insert( Qt::Key_F14, Noesis::Key_F14 );
	keyConvertMap.insert( Qt::Key_F15, Noesis::Key_F15 );
	keyConvertMap.insert( Qt::Key_F16, Noesis::Key_F16 );
	keyConvertMap.insert( Qt::Key_F17, Noesis::Key_F17 );
	keyConvertMap.insert( Qt::Key_F18, Noesis::Key_F18 );
	keyConvertMap.insert( Qt::Key_F19, Noesis::Key_F19 );
	keyConvertMap.insert( Qt::Key_F20, Noesis::Key_F20 );
	keyConvertMap.insert( Qt::Key_F21, Noesis::Key_F21 );
	keyConvertMap.insert( Qt::Key_F22, Noesis::Key_F22 );
	keyConvertMap.insert( Qt::Key_F23, Noesis::Key_F23 );
	keyConvertMap.insert( Qt::Key_F24, Noesis::Key_F24 );

	keyConvertMap.insert( Qt::Key_NumLock, Noesis::Key_NumLock );
	keyConvertMap.insert( Qt::Key_ScrollLock, Noesis::Key_Scroll );

	keyConvertMap.insert( Qt::Key_Shift, Noesis::Key_LeftShift );
	//keyConvertMap.insert( Qt::Key_, Noesis::Key_RightShift );
	keyConvertMap.insert( Qt::Key_Control, Noesis::Key_LeftCtrl );
	//keyConvertMap.insert( Qt::Key_, Noesis::Key_RightCtrl );
	keyConvertMap.insert( Qt::Key_Alt, Noesis::Key_LeftAlt );
	keyConvertMap.insert( Qt::Key_AltGr, Noesis::Key_RightAlt );

	//keyConvertMap.insert( Qt::Key_, Noesis::Key_PageLeft );
	//keyConvertMap.insert( Qt::Key_, Noesis::Key_PageRight );
	return keyConvertMap;
}
} // namespace

Noesis::Key NoesisKeyConverter::fromQt(Qt::Key key)
{
    static const auto map = BuildKeyConvertMap();
    return map.value(key, Noesis::Key_None);
}
