#include "jvs_frame.hh"
#include <Arduino.h>
#include <bitset>

int game_button = 33;
int starts[2] = {36, 35};
int sticks[2][4] = {{5, 4, 22, 6}, {31, 30, 32, 23}};
int buttons[2][8] = {{7, 8, 9, 10, 11, 12, 28, 29}, {21, 20, 19, 18, 40, 39, 38, 37}};

void setup_io()
{
  pinMode(game_button, INPUT_PULLUP);
  pinMode(starts[0], INPUT_PULLUP);
  pinMode(starts[1], INPUT_PULLUP);
  for (int i = 0; i < 4; ++i)
  {
    pinMode(sticks[0][i], INPUT_PULLUP);
    pinMode(sticks[1][i], INPUT_PULLUP);
  }
  for (int i = 0; i < 8; ++i)
  {
    pinMode(buttons[0][i], INPUT_PULLUP);
    pinMode(buttons[1][i], INPUT_PULLUP);
  }
}

void build_io_packet(int player, JVSResponse& response)
{
  std::bitset<32> packet;
  packet.set(7, !digitalRead(starts[player]));

  // if the game button is pressed and the player is 0, set the player1 start button, and player1 button 1.
  if (!digitalRead(game_button) && player == 0)
  {
    // player 1 start button.
    packet.set(7, 1);
    // plyuer 1 button 1.
    packet.set(1, 1);
  }

  for (int i = 0; i < 4; ++i)
  {
    packet.set(i + 2, !digitalRead(sticks[player][i]));
  }

  bool press_service = true;
  for (int i = 0; i < 6; ++i)
  {
    press_service = press_service && !digitalRead(buttons[player][i]);
  }
  packet.set(6, press_service);

  if (!press_service)
  {
    packet.set(1, !digitalRead(buttons[player][0]));
    packet.set(0, !digitalRead(buttons[player][1]));
    packet.set(15, !digitalRead(buttons[player][2]));
    packet.set(14, !digitalRead(buttons[player][3]));
    packet.set(13, !digitalRead(buttons[player][4]));
    packet.set(12, !digitalRead(buttons[player][5]));
    packet.set(11, !digitalRead(buttons[player][6]));
    packet.set(10, !digitalRead(buttons[player][7]));
  }

  auto val = packet.to_ulong();
  response.append(static_cast<uint8_t>(val & 0xFF));
  val >>= 8;
  response.append(static_cast<uint8_t>(val & 0xFF));
}

void update_analog(uint16_t (&analog_values)[8])
{
  constexpr auto player = 0;
  constexpr auto middle = 0x8000;
  constexpr auto magnitude = 16384;
  analog_values[0] = middle;
  analog_values[1] = middle;

  if (!digitalRead(sticks[player][0]))
  {
    analog_values[1] += magnitude;
  }
  if (!digitalRead(sticks[player][1]))
  {
    analog_values[1] -= magnitude;
  }
  if (!digitalRead(sticks[player][2]))
  {
    analog_values[0] += magnitude;
  }
  if (!digitalRead(sticks[player][3]))
  {
    analog_values[0] -= magnitude;
  }
}
