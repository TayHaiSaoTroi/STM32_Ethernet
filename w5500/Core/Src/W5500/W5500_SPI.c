#include "W5500_SPI.h"
#include "stm32f1xx_hal.h"
#include "stm32f103xb.h"
#include "stm32f1xx_hal_rcc.h"
#include "stm32f1xx_hal_gpio.h"
#include "wizchip_conf.h"
#include "stdio.h"

extern SPI_HandleTypeDef hspi1;

uint8_t SPIReadWrite(uint8_t data)
{
    while((hspi1.Instance ->SR & SPI_FLAG_TXE) != SPI_FLAG_TXE);
        *(volatile uint8_t*)&hspi1.Instance ->DR = data;
    while((hspi1.Instance ->DR & SPI_FLAG_RXNE) != SPI_FLAG_RXNE);
     
    return  (*(volatile uint8_t *)&hspi1.Instance ->DR);
    
}

void wizchip_select(void)
{
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_RESET);
}

void wizchip_deSelect(void)
{
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_SET);
}

uint8_t wizchip_read()
{
    uint8_t rb;
    rb = SPIReadWrite(0x00);
    return rb;
}

void wizchip_write(uint8_t wb)
{
    SPIReadWrite(wb);
}

void wizchip_readbust(uint8_t * buffer, uint16_t len)
{
    for(uint16_t index =0; index != len; index++)
    {
*buffer =SPIReadWrite(0x00);
buffer ++;
    }
}

void wizchip_writebust(uint8_t * buffer, uint16_t len)
{
    for(uint16_t index =0; index != len; index++)
    {
        SPIReadWrite(*buffer);
        buffer++;
    }
}

void W5500_io_init(void)
{

    GPIO_InitTypeDef GPIO_InitStruct ={0};
    __HAL_RCC_GPIOA_CLK_ENABLE();
    GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA,&GPIO_InitStruct);
}

void W5500_Init(void)
{
 uint8_t temp;
 uint8_t memsize[2][8] ={{2,2,2,2,2,2,2,2},{2,2,2,2,2,2,2,2}};
 W5500_io_init();
 HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_SET); // default ship select always high 

 HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_RESET);
 temp =0xFF;
 while(temp --);
 HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_SET);
 reg_wizchip_cs_cbfunc(wizchip_select,wizchip_deSelect);
 reg_wizchip_spi_cbfunc(wizchip_read,wizchip_write);
 reg_wizchip_spiburst_cbfunc(wizchip_readbust, wizchip_writebust);

 if(ctlwizchip(CW_INIT_WIZCHIP, (void*)memsize) == -1)
 {
    printf("WIZCHIP Initialize  fail. \n");
    while(1);
 }
 printf("WIZCHIP Initialize success. \n");

}