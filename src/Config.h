#pragma once
class Config {

public:

	int circleBufferLen;

	int inputSamplerate;

	int inputSamplerateDivider;

	int outputSamplerate;

	int outputSamplerateDivider;

	int inputChannelNumber;
	int outputChannelNumber;

	//������ ������� ������ ������ � �������� �����
	int bufferWriteAudioLen;

	//������ ���������� �� ������� ��������� ������
	int readSoundProcessorBufferLen;

	//������ ������ � ������ ��������� �������
	int audioWriteFrameLen;

	int audioReadFrameLen;

	int readSize;

	int hilbertTransformLen;
	int polyphaseFilterLen;

	int fftLen;
	float fftBandwidth;

	int defaultFilterWidth = 3000;

	Config(int inputSamplerate, int inputSamplerateDivider, int outputSamplerateDivider);

};