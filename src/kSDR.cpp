#include "Env.h"
#include "Environment.h"
#include "Config.h"

#include "Display.h"
#include "SoundProcessorThread.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#include "CircleBufferNew.h"

Environment env;

//Создаем объект дисплей
Display* display = new Display(&env);
//Сразу же инициализируем статическую переменную класса. Она нужна для обработки событий.
Display& d = *display;
Display* Display::instance = &d;

int main() {

	//printf("%d\n", CircleBufferNew<int>::test2());

	//exit(0);

	/*int* p5 = new int[5];
	p5[0] = 1; 
	p5[1] = 2;
	p5[2] = 3;
	p5[3] = 4;
	p5[4] = 5;

	int* p6 = new int[6];
	p6[0] = 1;
	p6[1] = 2;
	p6[2] = 3;
	p6[3] = 4;
	p6[4] = 5;
	p6[5] = 6;

	int* p7 = new int[6];
	p7[0] = 8; 
	p7[1] = 9; 
	p7[2] = 10; 
	p7[3] = 11; 
	p7[4] = 12; 
	p7[5] = 13;

	int* p10 = new int[10];
	p10[0] = 1; 
	p10[1] = 2; 
	p10[2] = 3; 
	p10[3] = 4; 
	p10[4] = 5; 
	p10[5] = 6;
	p10[6] = 7; 
	p10[7] = 8; 
	p10[8] = 9; 
	p10[9] = 10;

	CircleBufferNew<int> cb(10);
	//cb.write(p5, 5);
	//cb.write(p6, 6);
	//cb.write(p7, 2);
	//cb.write(p10, 10);
	//cb.write(p7, 3);

	cb.write(p10, 7);
	printf("available: %d\n", cb.available());

	int* data = cb.read(7);
	Utils::printArray(data, 7);

	printf("available: %d\n", cb.available());

	cb.write(p10, 6);

	printf("available: %d\n", cb.available());

	data = cb.read(5);
	Utils::printArray(data, 5);

	printf("available: %d\n", cb.available());
	data = cb.read(1);
	Utils::printArray(data, 1);

	printf("available: %d\n", cb.available());
	//int* data = cb.read(5);
	//Utils::printArray(data, 5);
	//int* data = cb.read(10);
	//Utils::printArray(data, 10);

	/*data = cb.read(5);
	Utils::printArray(data, 5);*/

	/*cb.write(p10, 7);
	data = cb.read(5);
	Utils::printArray(data, 5);*/

	//printf("%d\n", cb.available());
	//cb.write(p10, 10);
	//cb.write(p7, 2);
	//cb.write(p10, 5);*/


	/*float* p = new float(5);
	p[0] = 1; p[1] = 2; p[2] = 3; p[3] = 4; p[4] = 5;

	float* p1 = new float(2);

	std::copy(p + 1, p + 1, p1);

	Utils::printArray(p1, 2);

	exit(0);*/

	env.startProcessing();

	display->init();
	display->mainLoop();

	delete display;

}