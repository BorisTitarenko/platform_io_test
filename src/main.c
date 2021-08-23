#include <stm32f102x6.h>
#include <system_stm32f1xx.h>

#define DEFAULT_DELAY   1000

void SetClock(void) {
    RCC->CR |= RCC_CR_HSEON; // Enable HSE
    while (!(RCC->CR & RCC_CR_HSERDY)); // Wait to become ready

    // configure flash for high speed clock
    FLASH->ACR |= FLASH_ACR_PRFTBE; 
    FLASH->ACR |= FLASH_ACR_LATENCY | FLASH_ACR_LATENCY_2;

    //Set prescalers for 72 MHz
    RCC->CFGR |= RCC_CFGR_HPRE_DIV1;
    RCC->CFGR |= RCC_CFGR_PPRE2_DIV1;
    RCC->CFGR |= RCC_CFGR_PPRE1_DIV2;
    RCC->CFGR |= RCC_CFGR_PLLSRC | RCC_CFGR_PLLXTPRE | RCC_CFGR_PLLMULL | RCC_CFGR_PLLMULL9;

    // Enable PPL and wait for it become ready
    RCC->CR |= RCC_CR_PLLON;
    while (!(RCC->CR & RCC_CR_PLLRDY));

    // Set PPL as system clock and wait for it set
    RCC->CFGR |= RCC_CFGR_SW_PLL;
    while (!(RCC->CFGR & RCC_CFGR_SWS_PLL));
    
}

void GPIOConfig() {
    // enable clocking port C as GPIOC is connected to APB2
    RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;

    GPIOC->CRH &= ~(GPIO_CRH_MODE13 | GPIO_CRH_CNF13);
    GPIOC->CRH |= GPIO_CRH_MODE13;
}

volatile uint32_t time = DEFAULT_DELAY;

void delay (uint32_t t)
{
	time = t;
  while (time);
  
}

void SysTick_Handler(void) {
    --time;
}

int main(void) {
    SystemInit();

    SetClock();

    SystemCoreClockUpdate();

    GPIOConfig();
    
    SysTick_Config(SystemCoreClock / 1000);
    
    while (1)
    {
      GPIOC->BSRR |= GPIO_BSRR_BS13;  // Set the pin PC13
	    delay(DEFAULT_DELAY);
	    GPIOC->BSRR |= GPIO_BSRR_BR13;  // RESET the pin PC13
	    delay(DEFAULT_DELAY);
    }

}