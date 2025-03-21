#include "jvs_frame.hh"

// Support all speeds.
const uint8_t JVS_COMM_SUPPORT = 0x07;
// in bits per second.
const uint JVS_COMM_SPEEDS[3] = {115200, 1000000, 3000000};
// JVS Dash debug LED pin.
const int JVS_DASH_LED = 27;

enum JVSCommand : uint8_t
{
  // Global
  RESET = 0xF0,
  SET_ADDR = 0xF1,
  // Init commands
  COMM_SPEED_CHANGE = 0xF2,
  IO_IDENTIFY = 0x10,
  COMMAND_REVISION = 0x11,
  JVS_REVISION = 0x12,
  COMM_VERSION = 0x13,
  FEATURE_CHECK = 0x14,
  MAIN_BRD_ID = 0x15,

  // I/O
  GET_SWITCH_INPUTS = 0x20,
  GET_COIN_INPUTS = 0x21,
  GET_ANALOG_INPUTS = 0x22,
  SET_OUTPUT = 0x32,
  SET_OUTPUT_BYTE = 0x37,
  SET_OUTPUT_BIT = 0x38,

  INCREASE_COIN = 0x31,
  DECREASE_COIN = 0x30,

  RETRANSMIT = 0x2F,
  // Communication Support Query.
  COMMSUP = 0xD0
};

inline const char* get_command_type_str(JVSCommand cmd)
{
  switch (cmd)
  {
  case JVSCommand::RESET:
    return "RESET";
  case JVSCommand::SET_ADDR:
    return "SET_ADDR";
  case JVSCommand::COMM_SPEED_CHANGE:
    return "COMM_SPEED_CHANGE";
  case JVSCommand::IO_IDENTIFY:
    return "IO_IDENTIFY";
  case JVSCommand::COMMAND_REVISION:
    return "COMMAND_REVISION";
  case JVSCommand::JVS_REVISION:
    return "JVS_REVISION";
  case JVSCommand::COMM_VERSION:
    return "COMM_VERSION";
  case JVSCommand::FEATURE_CHECK:
    return "FEATURE_CHECK";
  case JVSCommand::MAIN_BRD_ID:
    return "MAIN_BRD_ID";
  case JVSCommand::GET_SWITCH_INPUTS:
    return "GET_SWITCH_INPUTS";
  case JVSCommand::GET_COIN_INPUTS:
    return "GET_COIN_INPUTS";
  case JVSCommand::INCREASE_COIN:
    return "INCREASE_COIN";
  case JVSCommand::DECREASE_COIN:
    return "DECREASE_COIN";
  case JVSCommand::RETRANSMIT:
    return "RETRANSMIT";
  case JVSCommand::SET_OUTPUT:
    return "OUTPUT1";
  case JVSCommand::GET_ANALOG_INPUTS:
    return "GET_ANALOG_INPUTS";
  case JVSCommand::SET_OUTPUT_BYTE:
    return "OUTPUT2";
  case JVSCommand::SET_OUTPUT_BIT:
    return "OUTPUT3";
  case JVSCommand::COMMSUP:
    return "COMMSUP";
  }
  return "unknown";
}

class JVSSystem
{
public:
  size_t process_message(const uint8_t id, const char* request_buffer, size_t request_len, JVSResponse& response,
                         bool& send_response);
  uint8_t id() const;
  bool has_id() const;
  void reset();
  bool has_analog{true};
  bool has_coin{true};
  bool has_buttons{true};
  bool has_output{true};
  uint16_t analog_values[8];

private:
  uint8_t id_{-1};
  bool fake_use_coin{true};
};
