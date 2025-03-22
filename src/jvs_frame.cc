#include "jvs_frame.hh"

JVSFrameReader::JVSFrameReader() : state_(JVSState::READ_ID) {}

void JVSFrameReader::reset()
{
  state_ = JVSState::READ_ID;
  len_ = 0;
  cur_len_ = 0;
}

bool JVSFrameReader::read(uint8_t& val)
{
  if (!Serial4.available())
  {
    return false;
  }

  uint8_t tmp = Serial4.read();
  if (tmp == 0xD0)
  {
    escape_ = true;
    return false;
  }

  val = tmp;

  if (escape_)
  {
    val += 1;
    escape_ = false;
  }
  return true;
}

bool JVSFrameReader::update()
{
  uint8_t val;
  if (!read(val))
  {
    return false;
  }

  if (val == 0xE0)
  {
    reset();
    return false;
  }
  
  switch (state_)
  {
  case JVSState::READY:
    reset();
    [[fallthrough]];
  case JVSState::READ_ID:
    id_ = val;
    state_ = JVSState::READ_LEN;
    return false;
  case JVSState::READ_LEN:
    len_ = val;
    state_ = JVSState::READ_DATA;
    return false;
  case JVSState::READ_DATA:
    buffer_[cur_len_++] = val;

    if (cur_len_ >= len_)
    {
      state_ = JVSState::READY;
      return true;
    }
    return false;
  }

  return false;
}

uint32_t JVSFrameReader::size() const { return len_; }

uint8_t JVSFrameReader::id() const { return id_; }

char* JVSFrameReader::buffer() { return buffer_; }

JVSResponse::JVSResponse() : id(buffer[0]), len(buffer[1]), status(*reinterpret_cast<JVSStatusCode*>(&buffer[2]))
{
  // Len just includes status to start
  len = 1;
  id = 0;
}

const uint8_t* JVSResponse::serialize(size_t& final_len)
{
  uint8_t checksum = 0;
  for (auto pos = 0; pos < len + data_offset; ++pos)
  {
    checksum += buffer[pos];
  }
  // We're going to add 1 to the checksum by incrementing the byte count.
  checksum += 1;
  append(checksum);
  // Payload + header
  final_len = len + data_offset;
  return buffer;
}

void JVSResponse::append(const uint8_t value) { buffer[data_offset + len++] = value; }

void JVSResponse::append_u16(const uint16_t value)
{
  union
  {
    uint8_t bytes[2];
    uint16_t number;
  } data;
  data.number = value;
  // JVS is big endian
  buffer[data_offset + len++] = data.bytes[1];
  buffer[data_offset + len++] = data.bytes[0];
}

void JVSResponse::append(const char* str, const size_t strlen)
{
  for (auto pos = 0; pos < strlen; pos++)
  {
    buffer[data_offset + len++] = str[pos];
  }
  buffer[data_offset + len++] = '\0';
}

void JVSResponse::append_str(const char* str)
{
  size_t pos = 0;
  while (str[pos] != '\0')
  {
    buffer[data_offset + len++] = str[pos];
    ++pos;
  }
  buffer[data_offset + len++] = '\0';
}
