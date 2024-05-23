
  internal void input_init() {
  AssertNoReentry();
  MemoryZeroStruct(&InputState);
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
