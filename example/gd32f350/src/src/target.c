/*
 *
 */

#include "target.h"
#include "ittrium.h"

#include "drv_usb_hw.h"
#include "cdc_acm_core.h"

__IO uint16_t timer_prescaler = 5U;
uint32_t usbfs_prescaler = 0U;

/*!
    \brief      delay in micro seconds
    \param[in]  usec: value of delay required in micro seconds
    \param[out] none
    \retval     none
*/
void usb_udelay(const uint32_t usec)
{
	uint32_t ticks = usec * 1000;
	while (ticks--)
		cpu_nop();
}

/*!
    \brief      delay in milli seconds
    \param[in]  msec: value of delay required in milli seconds
    \param[out] none
    \retval     none
*/
void usb_mdelay(const uint32_t msec)
{
#if 0
	uint32_t ticks = msec * TIC_DENO / 1000;
	if (ticks == 0)
		ticks = 1;
	dly_tsk(ticks);
#else
	uint32_t ms = msec;
	while (ms--)
		usb_udelay(1000);
#endif
}


/*!
    \brief      configure USB interrupt
    \param[in]  none
    \param[out] none
    \retval     none
*/
void usb_intr_config(void)
{
    nvic_priority_group_set(NVIC_PRIGROUP_PRE2_SUB2);
    nvic_irq_enable((uint8_t)USBFS_IRQn, 2U, 0U);

    /* enable the power module clock */
    rcu_periph_clock_enable(RCU_PMU);

    /* USB wakeup EXTI line configuration */
    exti_interrupt_flag_clear(EXTI_18);
    exti_init(EXTI_18, EXTI_INTERRUPT, EXTI_TRIG_RISING);
    exti_interrupt_enable(EXTI_18);

    nvic_irq_enable((uint8_t)USBFS_WKUP_IRQn, 1U, 0U);
}


/*!
    \brief      configure USB clock
    \param[in]  none
    \param[out] none
    \retval     none
*/
void usb_rcu_config(void)
{
	uint32_t system_clock = rcu_clock_freq_get(CK_SYS);

	if(48000000U == system_clock) {
		usbfs_prescaler = RCU_USBFS_CKPLL_DIV1;
		timer_prescaler = 3U;
	} else if(72000000U == system_clock) {
		usbfs_prescaler = RCU_USBFS_CKPLL_DIV1_5;
		timer_prescaler = 5U;
	} else if(96000000U == system_clock) {
		usbfs_prescaler = RCU_USBFS_CKPLL_DIV2;
		timer_prescaler = 7U;
	} else {
		/* reserved */
	}

	rcu_usbfs_clock_config(usbfs_prescaler);

	rcu_periph_clock_enable(RCU_USBFS);
}

void usb_init(void)
{
	usb_rcu_config();
}

void _low_level_init(void)
{
	/* 108MHz internal RC by default */
	SystemInit();

	/* SysTick */
	SysTick_Config(SystemCoreClock / TIC_DENO);
	NVIC_SetPriority(SysTick_IRQn, 0x00);

	/* USB init */
	usb_init();
}

