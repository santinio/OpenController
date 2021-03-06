#include "controls.h"
#include "stdbool.h"
#include "memory-buffers/src/bufferMemory.h"
#include "stdint.h"
#include "stddef.h"
#include "../common/debug.h"
//#include <stdio.h>
//#include "nrf_log.h"
#include "controller.h"
//#define NUMBER_OF_CONTROLS 5

controller_t controller;
#define MIN_CONTROL_VALUE 0
#define MAX_CONTROL_VALUE 1024

//static uint8_t adcInputs[5] = {0,1,2,3,4};
static CONTROLS_STATE controlsState;
//const uint16_t testDataMin[MAX_CHANNELS] = {0,15,242,12,0,0,0};
//const uint16_t testDataMax[MAX_CHANNELS] = {1024,864,1100,1000,1024};

buffer_t controlsBuffer;
bool controlsInit(){
int count=0;
control_t control;
	if(controlsState==CONTROLS_STATE_OFF){
		/*Enable the controls*/
		bufferCreate(&controlsBuffer,BUFFER_TYPE_LINKED,sizeof(control_t),controller.numberOfChannels,&controlAlloc,&controlFree,&controlError);
		for(count=0;count<controller.numberOfChannels;count++){
			control.id=count;
			if(bufferAddData(&controlsBuffer,&control,sizeof(control_t))!=BUFFER_STATUS_OK){
				//NRF_LOG_INFO("Not enough memory to store controls\r\n");
				return false;
			}
		}
		//NRF_LOG_INFO("Controls added in buffer\r\n");
		/*Enable the ADC channels*/
		controlsState=CONTROLS_STATE_INITIALISED;
		return true;
	}
	return false;
}

bool controlsDeInit(){
	if(controlsState==CONTROLS_STATE_STOPPED)
		return true;
	return false;
}
bool controlsStartSampling(){
	if(controlsState==CONTROLS_STATE_INITIALISED)
		/*Start timer?adc?pwm?*/
		return true;
	return false;
}
bool controlsStopSampling(){
	if(controlsState==CONTROLS_STATE_ACTIVATED)
		return true;
	return false;
}

void controlAlloc(){
}
void controlFree(){
}
void controlError(){
}


void controlSetType(control_t *control,CONTROL_TYPE type)
{
	control->controlType = type;
}
void controlSetChannel(control_t *control, uint8_t channel)
{
	control->channel = channel;
}
void controlCreate(control_t *control)
{
	/*A bad way to force calibration*/
	control->minimum = MAX_CONTROL_VALUE;
	control->maximum = MIN_CONTROL_VALUE;
}
void controlSetPin(control_t *control,uint8_t pin)
{
	DEBUG(DEBUG_NOTE,"set pin\n");
}
void controlCalibrateCalculate(control_t *control)
{
	//control->minimum = min;
	//control->maximum = max;
	//control->center = center;
	uint16_t space = control->maximum-control->minimum;
	control->multiplier = (float)1024/space;
	control->offset = control->minimum;;
	DEBUG(DEBUG_NOTE,"Channel %d multiplier = %f offset = %d\n",control->channel,control->multiplier,control->offset);
        DEBUG(DEBUG_NOTE,"calibrated\n");
}

void controlCalibrate(control_t *control)
{
	if(control->adcInput>control->maximum)
	{
		control->maximum = control->adcInput;
		DEBUG(DEBUG_NOTE,"New max on channel %d %d\n",control->channel,control->maximum);
	}
	if(control->adcInput<control->minimum)
	{
		control->minimum = control->adcInput;
		DEBUG(DEBUG_NOTE,"New min on channel %d %d\n",control->channel,control->minimum);
	}
}

void controlCalculate(control_t *control)
{
	control->value = (control->adcInput - control->offset) * control->multiplier;
}
