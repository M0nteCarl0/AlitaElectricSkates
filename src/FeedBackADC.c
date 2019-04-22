#include "FeedBackADC.h"

void  FeedBackADC_Init(void) {
  FeedBackADC_GPIOInit();
  FeedBackADC_ClockInit();
}

void FeedBackADC_ClockInit(void) {
  /*
  APB2 CLK             = 21 Mhz
  ADC_Prescaler        = 2
  ADC_CLK              = 10.5 Mhz
  T_ADC_CLK            = 0.095 uSec
  Channel_SampleTime   = 144(144*0.095us ->14 us)
  F_Channel_Conversiom = 67 Khz
  */

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

  ADC_CommonInitTypeDef ComonInitADC;
  ADC_CommonStructInit(&ComonInitADC);
  ComonInitADC.ADC_Prescaler = ADC_Prescaler_Div2;  // 21Mhz(APB2 CLK) /2 = 10.5 Mhz(0,095 us)
  ADC_CommonInit(&ComonInitADC);

  ADC_InitTypeDef ADCSetup;
  ADC_StructInit(&ADCSetup);
  ADCSetup.ADC_ContinuousConvMode   = ENABLE;
  ADCSetup.ADC_DataAlign            = ADC_DataAlign_Right;
  ADCSetup.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
  ADCSetup.ADC_Resolution           = ADC_Resolution_12b;
  ADCSetup.ADC_ScanConvMode         = ENABLE;
  ADCSetup.ADC_NbrOfConversion      = 1;
  ADC_Init(ADC1, &ADCSetup);

  ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_144Cycles);  //144*0.1ms = 14.4 us  - 70 KHz;

  ADC_Cmd(ADC1, ENABLE);
  ADC_SoftwareStartConv(ADC1);
}

void FeedBackADC_GPIOInit(void) {
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_0;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
}

uint16_t FeedBackADC_GetMeasuredVoltageLevel(void) {
  while (ADC_GetFlagStatus(ADC1,ADC_FLAG_EOC) != SET);
  return ADC_GetConversionValue(ADC1);
}

double FeedBackADC_GetMeasuredVoltageWithAvergage(uint32_t AvergageFactor) {
  double value = 0;
  for (uint32_t i = 0; i < AvergageFactor; i++) {
     value += (double)FeedBackADC_GetMeasuredVoltageLevel();
  }
  value /= (double)AvergageFactor;
  return value;
}
