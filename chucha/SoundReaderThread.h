#include "SoundCard.h"
#include "CircleBuffer.h"

class SoundReaderThread {

	SoundCard* soundCard;
	CircleBuffer* �ircleBuffer;

public:

	SoundReaderThread(SoundCard* sc, CircleBuffer* cb);

	void run();

	std::thread start();
};