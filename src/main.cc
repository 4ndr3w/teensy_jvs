#include "io.hh"
#include "jvs_system.hh"
#include <Arduino.h>
#include <array>

#define SENSE_PIN 41

JVSFrameReader jvs_reader;
constexpr size_t jvs_count = 1;
JVSSystem jvses[jvs_count];

void setup()
{
  Serial.begin(9600);
  Serial4.begin(115200);
  Serial4.transmitterEnable(15);
  pinMode(SENSE_PIN, OUTPUT);
  pinMode(13, OUTPUT);
  digitalWrite(SENSE_PIN, HIGH);
  setup_io();
  delay(5);

  Serial.println("Starting!");
}

void handle_jvs_message(JVSSystem& jvs, const char* buf)
{
  size_t buffer_offset = 0;
  bool send_response = true;
  JVSResponse response;
  response.status = NORMAL_STATUS;

//  for (size_t pos = 0; pos < jvs_reader.size(); ++pos)
//  {
//    Serial.print(pos, HEX);
//    Serial.print(" ");
//  }
//  Serial.println();

  while (buffer_offset < jvs_reader.size() - 1)
  {
    int consumed_bytes = jvs.process_message(jvs_reader.id(), jvs_reader.buffer() + buffer_offset,
                                             jvs_reader.size() - buffer_offset, response, send_response);
    if (consumed_bytes < jvs_reader.size() - buffer_offset)
    {
      buffer_offset += consumed_bytes + 1;
    }
  }

  if (send_response)
  {
    size_t response_len = 0;
    char* response_buf = response.serialize(response_len);
    Serial4.write(0xE0);
    for (size_t idx = 0; idx < response_len; idx++)
    {
      uint8_t val = response_buf[idx];
      if (val == 0xD0 || val == 0xE0)
      {
        Serial4.write(0xD0);
        val--;
      }
      Serial4.write(val);
    }
  }
}

void loop()
{
  if (jvs_reader.update())
  {
    const char* buf = jvs_reader.buffer();
    const auto command = static_cast<JVSCommand>(buf[0]);
    if (jvs_reader.id() == 0xFF)
    {
      switch (command)
      {
      case JVSCommand::RESET:
        Serial.println("System reset");
        for (auto& jvs : jvses)
        {
          jvs.reset();
        }
        break;
      case JVSCommand::SET_ADDR:
        for (auto& jvs : jvses)
        {
          if (!jvs.has_id())
          {
            handle_jvs_message(jvs, buf);
            break;
          }
        }
        break;
      default:
        break;
      }
    }
    else if (jvs_reader.id() == 1)
    {
      handle_jvs_message(jvses[0], buf);
      if (!jvses[0].has_id()) {
        Serial.println("not configured");
      }
    }

    bool setup = true;
    for (const auto& jvs : jvses)
    {
      setup &= jvs.has_id();
    }
    if (!setup) {
      Serial.println("not configured");
    }
    digitalWrite(SENSE_PIN, !setup);
    digitalWrite(13, setup);
  }
}
