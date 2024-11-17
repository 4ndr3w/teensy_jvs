#pragma once

#include "jvs_frame.hh"

void setup_io();
void build_io_packet(int player, JVSResponse& response);
void update_analog(uint16_t (&analog_values)[8]);
void update_joysticks();
