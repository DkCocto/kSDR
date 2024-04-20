#include "SoundCard.h"
#include "CircleBuffer.h"

class SoundReaderThread {

	SoundCard* soundCard;
	CircleBuffer* circleBuffer;
	Config* config;

public:

	SoundReaderThread(Config* config, SoundCard* sc, CircleBuffer* cb);

	void run();

	std::thread start();
};