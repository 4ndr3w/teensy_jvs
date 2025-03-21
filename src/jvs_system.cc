#include "jvs_system.hh"
#include "io.hh"

size_t JVSSystem::process_message(const uint8_t id, const char* request_buffer, size_t request_len,
                                  JVSResponse& response, bool& send_response)
{
  if (id != 0xFF && id != id_)
  {
    Serial.print("message isn't for me ");
    Serial.println(id);
    return 0;
  }

  const auto command = static_cast<JVSCommand>(request_buffer[0]);

  char debug_msg[256];
  sprintf(debug_msg, "Got %d message: %s %02X request length is %d", id_, get_command_type_str(command), command,
          request_len);
  Serial.println(debug_msg);

  switch (command)
  {
  case JVSCommand::RESET:
    if (request_buffer[1] != 0xD9)
    {
      break;
    }
    reset();
    return 2;
  case JVSCommand::SET_ADDR:
    if (request_len < 3)
    {
      break;
    }
    id_ = request_buffer[1];
    Serial.print("ID is now ");
    Serial.println(id_);
    response.append(NORMAL_REPORT);
    return 1;
  case JVSCommand::IO_IDENTIFY:
    response.append(NORMAL_REPORT);

    // For a more legit looking response
    // response.append_str("SEGA ENTERPRISESLTD.;I/O BD JVS;837-13551;Ver1.00;98/10");

    sprintf(debug_msg, "BASED IO #%d;I/O BD JVS;837-13551;Ver1.00;2024", id_);
    response.append_str(debug_msg);

    return 0;
  case JVSCommand::COMMAND_REVISION:
    response.append(NORMAL_REPORT);
    response.append(0x13);
    return 0;
  case JVSCommand::COMM_VERSION:
    response.append(NORMAL_REPORT);
    response.append(0x10);
    return 0;
  case JVSCommand::JVS_REVISION:
    response.append(NORMAL_REPORT);
    response.append(0x30);
    return 0;
  case JVSCommand::FEATURE_CHECK:
    response.append(NORMAL_REPORT);
    if (has_buttons)
    {
      // Switch inputs
      response.append(0x01);
      // Two player
      response.append(2);
      // Button count
      response.append(12);
      // Empty
      response.append(0x0);
    }

    if (has_coin)
    {
      // Coin slots
      response.append(0x02);
      response.append(2);
      // Empty
      response.append(0x0);
      // Empty
      response.append(0x0);
    }

    if (has_analog)
    {
      // Analog
      response.append(3);
      response.append(8);
      response.append(0);
      response.append(0);
    }

    if (has_output)
    {
      response.append(0x12);
      response.append(6);
      response.append(0);
      response.append(0);
    }

    // End
    response.append(0x0);
    return 0;
  case JVSCommand::GET_ANALOG_INPUTS:
  {
    response.append(NORMAL_REPORT);
    update_analog(analog_values);
    for (const auto val : analog_values)
    {
      response.append_u16(val);
    }
  }
    return 1;
  case JVSCommand::GET_COIN_INPUTS:
  {
    response.append(NORMAL_REPORT);
    response.append(0);
    response.append(0);

    response.append(0);
    response.append(0);
    return 1;
  }
  case JVSCommand::DECREASE_COIN:
    response.append(NORMAL_REPORT);
    fake_use_coin = true;
    return 3;
  case JVSCommand::GET_SWITCH_INPUTS:
  {
    response.append(NORMAL_REPORT);
    auto players = request_buffer[1];
    auto bytes_to_send = request_buffer[2];
    response.append(0);
    if (players > 0)
    {
      build_io_packet(0, response);
    }
    if (players == 2)
    {
      build_io_packet(1, response);
    }
    return 2;
  }
  case JVSCommand::SET_OUTPUT:
  {
    response.append(NORMAL_REPORT);
    auto byte_count = request_buffer[1];
    return byte_count + 1;
  }
  case JVSCommand::SET_OUTPUT_BYTE:
  case JVSCommand::SET_OUTPUT_BIT:
  {
    response.append(NORMAL_REPORT);
    return 2;
  }
  case JVSCommand::COMM_SPEED_CHANGE:
  {
    auto method_code = request_buffer[1];
    if (method_code < (sizeof(JVS_COMM_SPEEDS) / sizeof(JVS_COMM_SPEEDS[0])))
    {
      Serial4.end();
      Serial4.begin(JVS_COMM_SPEEDS[method_code]);
      Serial4.clear();
      Serial4.transmitterEnable(15);
      // must wait at least 5ms.
      delay(5);
      // if not 0 then we are in dash mode.
      if (method_code != 0)
      {
        pinMode(JVS_DASH_LED, OUTPUT);
      }
    }
    return 1;
  }
  case JVSCommand::COMMSUP:
  {
    response.append(NORMAL_REPORT);
    response.append(JVS_COMM_SUPPORT);
    return 0;
  }
  default:
    Serial.println("unknown command!");
    response.status = UNKNOWN_COMMAND_STATUS;
    return 0;
    break;
  }

  return 0;
}

bool JVSSystem::has_id() const { return id_ != 0xFF; }

uint8_t JVSSystem::id() const { return id_; }

void JVSSystem::reset()
{
  id_ = 0xFF;
  fake_use_coin = true;
}
