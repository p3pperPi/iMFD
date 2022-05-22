#include "iMFD.h"

iMFD_Sensor sns_AF(AIR_FUEL);
unsigned long value;

void setup() {
	iMFD.begin(true,200);
}

void loop() {
	value += 50;
	if(value > 1000)
		value = 0;
	sns_AF = value;

	// NO_TIMER_INT を有効化しない場合	
	delay(1000);

	// NO_TIMER_INT を有効化した場合
	// delay(100);
	// timerHandler();
}
