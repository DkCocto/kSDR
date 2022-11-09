#pragma once

#include "stdio.h"
#include "thread"
#include "Utils.h"

#define M_PI						3.1415926535897932384

//#define DEFAULT_CIRCLE_BUFFER_LEN	8 * 1024 * 1024

//Valid rate for rtl
//225001 - 300000 Hz
//900001 - 3200000 Hz
//sample loss is to be expected for rates > 2400000

//2000000 64
//1408000 32
//910000 16
//300000 8

//#define INPUT_SAMPLE_RATE			125000
//#define OUTPUT_SAMPLE_RATE			INPUT_SAMPLE_RATE / DECIMATION_LEVEL
//#define DECIMATION_LEVEL			4
//#define NUM_INPUT_CHANNELS			1
//#define NUM_OUTPUT_CHANNELS			1
#define PA_SAMPLE_TYPE				paFloat32

//размер буффера записи данных в звуковую карту
//#define BUFFER_WRITER				(DECIMATION_LEVEL * 2) * 4

//размер считывания из буффера обработки данных
//#define SOUND_PROCESSOR_BUFFER		(DECIMATION_LEVEL * 2) * 32

//размер записи в буффер обработки сигнала
//#define FRAMES_PER_BUFFER			(DECIMATION_LEVEL * 2) * 32
//#define READ_SIZE					FRAMES_PER_BUFFER * NUM_INPUT_CHANNELS 

//Размер данных считываемых из устройства RTL
//512
#define RTL_READ_BUFFER				512

//#define HILBERT_TRANSFORM_LEN		255

//512
//#define POLYPHASE_FILTER_LEN		512
//#define DEFAULT_FILTER_WIDTH		3000

//#define FFT_LENGTH					4096
//#define FFT_BANDWIDTH				(float) INPUT_SAMPLE_RATE / (float) FFT_LENGTH

#define APP_NAME					"kSDR v1.0"

struct Signal {
	float I;
	float Q;
};


#define USB		0
#define LSB		1
#define AM		2