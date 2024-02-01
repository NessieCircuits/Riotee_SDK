#include "riotee_i2c.h"
#include "riotee_max20361.h"
#include "nrf.h"
#include "nrf_gpio.h"
#include "riotee.h"
#include "riotee_thresholds.h"
#include "runtime.h"

#define STACK_TOP 0x20018000

extern unsigned long _stext;
extern unsigned long __bss_start__;
extern unsigned long __bss_end__;
extern unsigned long __bss_retained_start__;
extern unsigned long __bss_retained_end__;
extern unsigned long __etext;
extern unsigned long __data_start__;
extern unsigned long __data_end__;
extern unsigned long __data_retained_start__;
extern unsigned long __data_retained_end__;
extern unsigned long _isr_vector_start;
extern unsigned long _isr_vector_end;

/* Exceptions */
void NMI_Handler(void) __attribute__((weak, alias("Default_Handler")));
void HardFault_Handler(void) __attribute__((weak, alias("Default_Handler")));
void MemManage_Handler(void) __attribute__((weak, alias("Default_Handler")));
void BusFault_Handler(void) __attribute__((weak, alias("Default_Handler")));
void UsageFault_Handler(void) __attribute__((weak, alias("Default_Handler")));
void SVC_Handler(void) __attribute__((weak, alias("Default_Handler")));
void DebugMon_Handler(void) __attribute__((weak, alias("Default_Handler")));
void PendSV_Handler(void) __attribute__((weak, alias("Default_Handler")));
void SysTick_Handler(void) __attribute__((weak, alias("Default_Handler")));

void POWER_CLOCK_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void RADIO_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void UARTE0_UART0_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void SPIM0_SPIS0_TWIM0_TWIS0_SPI0_TWI0_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void SPIM1_SPIS1_TWIM1_TWIS1_SPI1_TWI1_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void NFCT_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void GPIOTE_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void SAADC_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void TIMER0_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void TIMER1_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void TIMER2_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void RTC0_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void TEMP_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void RNG_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void ECB_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void CCM_AAR_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void WDT_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void RTC1_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void QDEC_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void COMP_LPCOMP_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void SWI0_EGU0_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void SWI1_EGU1_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void SWI2_EGU2_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void SWI3_EGU3_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void SWI4_EGU4_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void SWI5_EGU5_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void TIMER3_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void TIMER4_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void PWM0_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void PDM_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void MWU_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void PWM1_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void PWM2_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void SPIM2_SPIS2_SPI2_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void RTC2_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void I2S_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void FPU_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void USBD_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void UARTE1_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void QSPI_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void CRYPTOCELL_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void PWM3_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void SPIM3_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));

void c_startup(void);
void dummy_fn(void);

void runtime_start();

__attribute__((section(".isr_vector"))) void (*vectors[])(void) = {
    (void *)STACK_TOP,
    c_startup,
    NMI_Handler,
    HardFault_Handler,
    MemManage_Handler,
    BusFault_Handler,
    UsageFault_Handler,
    0UL,
    0UL,
    0UL,
    0UL,
    SVC_Handler,
    DebugMon_Handler,
    0UL,
    PendSV_Handler,
    SysTick_Handler,
    POWER_CLOCK_IRQHandler,
    RADIO_IRQHandler,
    UARTE0_UART0_IRQHandler,
    SPIM0_SPIS0_TWIM0_TWIS0_SPI0_TWI0_IRQHandler,
    SPIM1_SPIS1_TWIM1_TWIS1_SPI1_TWI1_IRQHandler,
    NFCT_IRQHandler,
    GPIOTE_IRQHandler,
    SAADC_IRQHandler,
    TIMER0_IRQHandler,
    TIMER1_IRQHandler,
    TIMER2_IRQHandler,
    RTC0_IRQHandler,
    TEMP_IRQHandler,
    RNG_IRQHandler,
    ECB_IRQHandler,
    CCM_AAR_IRQHandler,
    WDT_IRQHandler,
    RTC1_IRQHandler,
    QDEC_IRQHandler,
    COMP_LPCOMP_IRQHandler,
    SWI0_EGU0_IRQHandler,
    SWI1_EGU1_IRQHandler,
    SWI2_EGU2_IRQHandler,
    SWI3_EGU3_IRQHandler,
    SWI4_EGU4_IRQHandler,
    SWI5_EGU5_IRQHandler,
    TIMER3_IRQHandler,
    TIMER4_IRQHandler,
    PWM0_IRQHandler,
    PDM_IRQHandler,
    0,
    0,
    MWU_IRQHandler,
    PWM1_IRQHandler,
    PWM2_IRQHandler,
    SPIM2_SPIS2_SPI2_IRQHandler,
    RTC2_IRQHandler,
    I2S_IRQHandler,
    FPU_IRQHandler,
    USBD_IRQHandler,
    UARTE1_IRQHandler,
    0,
    0,
    0,
    0,
    PWM3_IRQHandler,
    0,
    SPIM3_IRQHandler,
};

void Default_Handler(void) {
  asm volatile("bkpt");
  while (1) {
    __NOP();
  }
}

static void wait_for_high(void) {
  NRF_P0->DETECTMODE = GPIO_DETECTMODE_DETECTMODE_LDETECT;
  NRF_GPIOTE->EVENTS_PORT = 0;
  NRF_GPIOTE->INTENSET = GPIOTE_INTENSET_PORT_Msk;

  /* Allow pending interrupts to wakeup CPU */
  SCB->SCR |= SCB_SCR_SEVONPEND_Msk;

  NRF_P0->LATCH |= (1 << PIN_PWRGD_H);

  NRF_P0->PIN_CNF[PIN_PWRGD_H] = (GPIO_PIN_CNF_INPUT_Connect << GPIO_PIN_CNF_INPUT_Pos);
  NRF_P0->PIN_CNF[PIN_PWRGD_H] |= (GPIO_PIN_CNF_SENSE_High << GPIO_PIN_CNF_SENSE_Pos);

  while (NRF_GPIOTE->EVENTS_PORT == 0) {
    __WFE();
    __SEV();
    __WFE();
  }

  NRF_P0->PIN_CNF[PIN_PWRGD_H] = (GPIO_PIN_CNF_INPUT_Disconnect << GPIO_PIN_CNF_INPUT_Pos) |
                                 (GPIO_PIN_CNF_SENSE_Disabled << GPIO_PIN_CNF_SENSE_Pos);
  NRF_P0->LATCH |= (1 << PIN_PWRGD_H);
  NRF_GPIOTE->EVENTS_PORT = 0;
  NVIC_ClearPendingIRQ(GPIOTE_IRQn);
  NRF_GPIOTE->INTENCLR = GPIOTE_INTENSET_PORT_Msk;

  /* Disable pending interrupts to wakeup CPU */
  SCB->SCR &= ~SCB_SCR_SEVONPEND_Msk;
}

void __libc_init_array(void);

__attribute__((weak)) void startup_callback(void){

};

void c_startup(void) {
  volatile unsigned long *src, *dst;

  /* Configure NFC pins as normal GPIO */
  NRF_NVMC->CONFIG = NVMC_CONFIG_WEN_Wen << NVMC_CONFIG_WEN_Pos;
  while (NRF_NVMC->READY == NVMC_READY_READY_Busy) {
  }
  NRF_UICR->NFCPINS = 0x0;
  while (NRF_NVMC->READY == NVMC_READY_READY_Busy) {
  }
  NRF_NVMC->CONFIG = NVMC_CONFIG_WEN_Ren << NVMC_CONFIG_WEN_Pos;

  /* Set shared pins to a defined level to avoid leakage inside MSP430 */
  nrf_gpio_cfg_input(PIN_C2C_MOSI, NRF_GPIO_PIN_PULLDOWN);
  nrf_gpio_cfg_input(PIN_C2C_CLK, NRF_GPIO_PIN_PULLDOWN);
  nrf_gpio_cfg_input(PIN_C2C_CS, NRF_GPIO_PIN_PULLDOWN);
  nrf_gpio_cfg_input(PIN_C2C_MISO, NRF_GPIO_PIN_PULLDOWN);

  riotee_thresholds_low_set(THR_LOW_3V1);
  riotee_thresholds_high_set(THR_HIGH_4V6);

  nrf_gpio_cfg_input(PIN_D0, NRF_GPIO_PIN_PULLDOWN);
  nrf_gpio_cfg_input(PIN_D1, NRF_GPIO_PIN_PULLDOWN);

  nrf_gpio_cfg_input(PIN_D4, NRF_GPIO_PIN_PULLDOWN);
  nrf_gpio_cfg_input(PIN_D5, NRF_GPIO_PIN_PULLDOWN);
  nrf_gpio_cfg_input(PIN_D6, NRF_GPIO_PIN_PULLDOWN);
  nrf_gpio_cfg_input(PIN_D7, NRF_GPIO_PIN_PULLDOWN);
  nrf_gpio_cfg_input(PIN_D8, NRF_GPIO_PIN_PULLDOWN);
  nrf_gpio_cfg_input(PIN_D9, NRF_GPIO_PIN_PULLDOWN);
  nrf_gpio_cfg_input(PIN_D10, NRF_GPIO_PIN_PULLDOWN);
  nrf_gpio_cfg_input(PIN_LED_CTRL, NRF_GPIO_PIN_PULLDOWN);
  nrf_gpio_cfg_input(PIN_MAX_INT, NRF_GPIO_PIN_PULLDOWN);
  nrf_gpio_cfg_input(PIN_RTC_INT, NRF_GPIO_PIN_PULLDOWN);

  nrf_gpio_cfg_default(PIN_VCAP_SENSE);

  /* nRF52833 Errata 246 */
  *(volatile uint32_t *)0x4007AC84ul = 0x00000002ul;

  /* This must happen soon to avoid max20361 cutting power */
  riotee_i2c_init();
  riotee_max20361_init();
  startup_callback();

#ifndef DISABLE_CAP_MONITOR
  wait_for_high();
#endif

  /* Copy static/global system variables from flash to RAM */
  src = &__etext;
  dst = &__data_start__;
  while (dst < &__data_end__)
    *(dst++) = *(src++);

  /* Zero initialize static/global system variables */
  src = &__bss_start__;
  while (src < &__bss_end__)
    *(src++) = 0;

    /* Activate FPU if compiler says that it's used */
#if (__FPU_USED == 1)
  SCB->CPACR |= (3UL << 20) | (3UL << 22);
  __DSB();
  __ISB();
#endif

  /* First call to static constructors via newlibc. There is a second when nonretained memory gets set up. */
  __libc_init_array();
  runtime_start();
}
