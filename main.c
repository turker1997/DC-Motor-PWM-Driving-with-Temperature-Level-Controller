#define VREFIN_CAL ((uint16_t*)0x1FFF7A2A)
#define V25 (float) 0.76
#define AVG_SLOPE (float) 0.0025
uint16_t adc_value[3];
float Vadc = 0, Vsense = 0, Vdda = 0;
float temperature = 0;
float temperature2 = 0;
int count = 0;
int ledState = 0;
int pwm_val = 0;
int tempLevel = 0;
uint8_t segmentNumber[11] = { // Segment numbers stored in the array as binary
 0x3f, // 0
 0x06, // 1
 0x5b, // 2
 0x4f, // 3
 0x66, // 4
 0x6d, // 5
 0x7d, // 6
 0x07, // 7
 0x7f, // 8
 0x67, // 9
0x00 // OFF
};
void SevenSegmentConf(uint8_t number)
{
HAL_GPIO_WritePin(m_DisplayA_GPIO_Port, m_DisplayA_Pin, ((number>>0)&0x01));
HAL_GPIO_WritePin(m_DisplayB_GPIO_Port, m_DisplayB_Pin, ((number>>1)&0x01));
HAL_GPIO_WritePin(m_DisplayC_GPIO_Port, m_DisplayC_Pin, ((number>>2)&0x01));
HAL_GPIO_WritePin(m_DisplayD_GPIO_Port, m_DisplayD_Pin, ((number>>3)&0x01));
HAL_GPIO_WritePin(m_DisplayE_GPIO_Port, m_DisplayE_Pin, ((number>>4)&0x01));
HAL_GPIO_WritePin(m_DisplayF_GPIO_Port, m_DisplayF_Pin, ((number>>5)&0x01));
HAL_GPIO_WritePin(m_DisplayG_GPIO_Port, m_DisplayG_Pin, ((number>>6)&0x01));
}
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
 if(GPIO_Pin == m_Button_Pin)
 {
 ledState = !ledState;
 while(HAL_GPIO_ReadPin(m_Button_GPIO_Port, m_Button_Pin));
 for(int i = 0; i<3000;i++);
 }
}
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
adc_value[count] = HAL_ADC_GetValue(&hadc1);
count++; //count 0 -> channel 0, count 1 -> vrefint, count 2 -> temperature
if(count == 3)
count = 0;
Vdda = (float) 3.3 * (*VREFIN_CAL) / adc_value[1];
Vsense = Vdda * adc_value[2] / 4095;
temperature = ((Vsense - V25) / AVG_SLOPE) + 25;
temperature2 = (adc_value[0]*330.0)/4095.0;
}
int main(void)
{
HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);
 HAL_ADC_Start_IT(&hadc1);
 while (1)
 {
 HAL_GPIO_WritePin(m_GreenLed_GPIO_Port, m_GreenLed_Pin, ledState);
 if(ledState != 1)
 {
 pwm_val = 0;
 SevenSegmentConf(segmentNumber[10]);
 }
 else if(ledState)
 {
 pwm_val = tempLevel*10 + 10;
 __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, pwm_val);
 HAL_ADC_Start_IT(&hadc1);
 tempLevel = adc_value[0]*10/4096;
 SevenSegmentConf(segmentNumber[tempLevel]);
 }
 HAL_Delay(100);
 }
}