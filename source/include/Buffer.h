#pragma once

#include "CircleBufferNew.h"

class Buffer : public CircleBufferNew<uint8_t>, public CircleBufferNew<float> { };