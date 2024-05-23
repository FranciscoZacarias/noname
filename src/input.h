/* date = May 23rd 2024 2:56 pm */

#ifndef INPUT_H
#define INPUT_H

typedef enum Mouse_Buttons {
  MouseButton_Left,
  MouseButton_Right,
  MouseButton_Middle,
  
  MouseButton_Count,
} Mouse_Buttons;

#define KeyboardKey(name, code) KeyboardKey_##name = code

typedef enum Keyboard_Key {
  KeyboardKey(BACKSPACE, 0x08),
  KeyboardKey(ENTER,     0x0D),
  KeyboardKey(TAB,       0x09),
  KeyboardKey(SHIFT,     0x10),
  KeyboardKey(CONTROL,   0x11),
  
  KeyboardKey(PAUSE,     0x13),
  KeyboardKey(CAPS_LOCK, 0x14),
  
  KeyboardKey(ESCAPE, 0x1B),
  
  KeyboardKey(CONVERT,    0x1C),
  KeyboardKey(NONCONVERT, 0x1D),
  KeyboardKey(ACCEPT,     0x1E),
  KeyboardKey(MODECHANGE, 0x1F),
  
  KeyboardKey(SPACE,       0x20),
  KeyboardKey(PRIOR,       0x21),
  KeyboardKey(NEXT,        0x22),
  KeyboardKey(END,         0x23),
  KeyboardKey(HOME,        0x24),
  KeyboardKey(ARROW_LEFT,  0x25),
  KeyboardKey(ARROW_UP,    0x26),
  KeyboardKey(ARROW_RIGHT, 0x27),
  KeyboardKey(ARROW_DOWN,  0x28),
  KeyboardKey(SELECT,      0x29),
  KeyboardKey(PRINT,       0x2A),
  KeyboardKey(EXECUTE,     0x2B),
  KeyboardKey(SNAPSHOT,    0x2C),
  KeyboardKey(INSERT,      0x2D),
  KeyboardKey(DELETE,      0x2E),
  KeyboardKey(HELP,        0x2F),
  
  KeyboardKey(A, 0x41),
  KeyboardKey(B, 0x42),
  KeyboardKey(C, 0x43),
  KeyboardKey(D, 0x44),
  KeyboardKey(E, 0x45),
  KeyboardKey(F, 0x46),
  KeyboardKey(G, 0x47),
  KeyboardKey(H, 0x48),
  KeyboardKey(I, 0x49),
  KeyboardKey(J, 0x4A),
  KeyboardKey(K, 0x4B),
  KeyboardKey(L, 0x4C),
  KeyboardKey(M, 0x4D),
  KeyboardKey(N, 0x4E),
  KeyboardKey(O, 0x4F),
  KeyboardKey(P, 0x50),
  KeyboardKey(Q, 0x51),
  KeyboardKey(R, 0x52),
  KeyboardKey(S, 0x53),
  KeyboardKey(T, 0x54),
  KeyboardKey(U, 0x55),
  KeyboardKey(V, 0x56),
  KeyboardKey(W, 0x57),
  KeyboardKey(X, 0x58),
  KeyboardKey(Y, 0x59),
  KeyboardKey(Z, 0x5A),
  
  KeyboardKey(LEFT_WIN,  0x5B),
  KeyboardKey(RIGHT_WIN, 0x5C),
  KeyboardKey(APPS,      0x5D),
  
  KeyboardKey(SLEEP, 0x5F),
  
  KeyboardKey(NUMPAD0, 0x60),
  KeyboardKey(NUMPAD1, 0x61),
  KeyboardKey(NUMPAD2, 0x62),
  KeyboardKey(NUMPAD3, 0x63),
  KeyboardKey(NUMPAD4, 0x64),
  KeyboardKey(NUMPAD5, 0x65),
  KeyboardKey(NUMPAD6, 0x66),
  KeyboardKey(NUMPAD7, 0x67),
  KeyboardKey(NUMPAD8, 0x68),
  KeyboardKey(NUMPAD9, 0x69),
  KeyboardKey(NUMPAD_MULTIPLY,  0x6A),
  KeyboardKey(NUMPAD_ADD,       0x6B),
  KeyboardKey(NUMPAD_SEPARATOR, 0x6C),
  KeyboardKey(NUMPAD_SUBTRACT,  0x6D),
  KeyboardKey(NUMPAD_DECIMAL,   0x6E),
  KeyboardKey(NUMPAD_DIVIDE,    0x6F),
  
  KeyboardKey(F1,  0x70),
  KeyboardKey(F2,  0x71),
  KeyboardKey(F3,  0x72),
  KeyboardKey(F4,  0x73),
  KeyboardKey(F5,  0x74),
  KeyboardKey(F6,  0x75),
  KeyboardKey(F7,  0x76),
  KeyboardKey(F8,  0x77),
  KeyboardKey(F9,  0x78),
  KeyboardKey(F10, 0x79),
  KeyboardKey(F11, 0x7A),
  KeyboardKey(F12, 0x7B),
  KeyboardKey(F13, 0x7C),
  KeyboardKey(F14, 0x7D),
  KeyboardKey(F15, 0x7E),
  KeyboardKey(F16, 0x7F),
  KeyboardKey(F17, 0x80),
  KeyboardKey(F18, 0x81),
  KeyboardKey(F19, 0x82),
  KeyboardKey(F20, 0x83),
  KeyboardKey(F21, 0x84),
  KeyboardKey(F22, 0x85),
  KeyboardKey(F23, 0x86),
  KeyboardKey(F24, 0x87),
  
  KeyboardKey(NUMLOCK, 0x90),
  KeyboardKey(SCROLL,  0x91),
  
  KeyboardKey(NUMPAD_EQUAL,0x92),
  
  KeyboardKey(LEFT_SHIFT,    0xA0),
  KeyboardKey(RIGHT_SHIFT,   0xA1),
  KeyboardKey(LEFT_CONTROL,  0xA2),
  KeyboardKey(RIGHT_CONTROL, 0xA3),
  KeyboardKey(LEFT_MENU,     0xA4),
  KeyboardKey(RIGHT_MENU,    0xA5),
  
  KeyboardKey(SEMICOLON, 0xBA),
  KeyboardKey(PLUS,      0xBB),
  KeyboardKey(COMMA,     0xBC),
  KeyboardKey(MINUS,     0xBD),
  KeyboardKey(PERIOD,    0xBE),
  KeyboardKey(SLASH,     0xBF),
  KeyboardKey(GRAVE,     0xC0),
  
  KeyboardKey_Count,
} Keyboard_Key;

#define KEYBOARD_STATE_SIZE 256
typedef struct Keyboard_State {
  b8 keys[KEYBOARD_STATE_SIZE];
} Keyboard_State;

// TODO(fz): Replace the current Mouse_State with this _Mouse_State
typedef struct _Mouse_State {
  s32 x;
  s32 y;
  b8 buttons[MouseButton_Count];
} _Mouse_State;

typedef struct Input_State {
  Keyboard_State keyboard_current;
  Keyboard_State keyboard_previous;
  _Mouse_State   mouse_current;
  _Mouse_State   mouse_previous;
} Input_State;

global Input_State InputState = { 0 };

internal void input_init();
internal void input_update();

internal void input_process_keyboard_key(Keyboard_Key key, b8 is_pressed);

internal b32 input_is_key_up(Keyboard_Key key);
internal b32 input_is_key_down(Keyboard_Key key);
internal b32 input_was_key_up(Keyboard_Key key);
internal b32 input_was_key_down(Keyboard_Key key);

internal b32 input_is_key_pressed(Keyboard_Key key);

#endif //INPUT_H
