
  internal void input_init() {
  AssertNoReentry();
  MemoryZeroStruct(&InputState);
  
  InputState.mouse_current.screen_space_x = ProgramState.window_width/2;
  InputState.mouse_current.screen_space_y = ProgramState.window_height/2;;
  
  InputState.mouse_previous.screen_space_x = ProgramState.window_width/2;
  InputState.mouse_previous.screen_space_y = ProgramState.window_height/2;;
}

internal void input_update() {
  MemoryCopy(&InputState.keyboard_previous, &InputState.keyboard_current, sizeof(Keyboard_State));
  MemoryCopy(&InputState.mouse_previous, &InputState.mouse_current, sizeof(Mouse_State));
}

internal void input_process_keyboard_key(Keyboard_Key key, b8 is_pressed) {
  if (InputState.keyboard_current.keys[key] != is_pressed) {
    InputState.keyboard_current.keys[key] = is_pressed;
  }
}

internal b32 input_is_key_up(Keyboard_Key key) {
  b32 result = InputState.keyboard_current.keys[key] == 0;
  return result;
}

internal b32 input_is_key_down(Keyboard_Key key) {
  b32 result = InputState.keyboard_current.keys[key] == 1;
  return result;
}

internal b32 input_was_key_up(Keyboard_Key key) {
  b32 result = InputState.keyboard_previous.keys[key] == 0;
  return result;
}

internal b32 input_was_key_down(Keyboard_Key key) {
  b32 result = InputState.keyboard_previous.keys[key] == 1;
  return result;
}

internal b32 input_is_key_pressed(Keyboard_Key key) {
  b32 result = input_is_key_down(key) && input_was_key_up(key);
  return result;
}

internal void input_process_mouse_button(Mouse_Button button, b32 is_pressed) {
  if (InputState.mouse_current.buttons[button] != is_pressed) {
    InputState.mouse_current.buttons[button] = is_pressed;
  }
}

internal void input_process_mouse_cursor(s32 x, s32 y) {
  InputState.mouse_current.screen_space_x = x;
  InputState.mouse_current.screen_space_y = y;
}

internal b32 input_is_button_up(Mouse_Button button) {
  b32 result = InputState.mouse_current.buttons[button] == 0;
  return result;
}

internal b32 input_is_button_down(Mouse_Button button) {
  b32 result = InputState.mouse_current.buttons[button] == 1;
  return result;
}

internal b32 input_was_button_up(Mouse_Button button) {
  b32 result = InputState.mouse_previous.buttons[button] == 0;
  return result;
}

internal b32 input_was_button_down(Mouse_Button button) {
  b32 result = InputState.mouse_previous.buttons[button] == 1;
  return result;
}

internal b32 input_is_button_pressed(Mouse_Button button) {
  b32 result = input_is_button_down(button) && input_was_button_up(button);
  return result;
}
