/*
 * usb cdc device
 */

#include <string.h>

#include "target.h"
#include "ittrium.h"
#include "drv_usb_hw.h"
#include "drv_usbd_int.h"
#include "cdc_acm_core.h"

void usb_intr_config(void);

#define USB_TASK_STACK_SIZE		1024
static UW usb_task_stack[USB_TASK_STACK_SIZE / sizeof(UW)];

UW init_tsk_stack[INIT_TASK_STACK_SIZE / sizeof(UW)];

static usb_core_driver cdc_acm;

void task_idle_c(void *arg);
extern int idle_stack[];

/*
 * usb interrupt handler
 *
 * only set flag
 * all processes is in usb_task
 */
void usbd_irq_handler(void)
{
	NVIC_DisableIRQ(USBFS_IRQn);
	NVIC_ClearPendingIRQ(USBFS_IRQn);
	iset_flg(USB_FLAG_ID, 1);
}

/*
 * usb wakeup interrupt handler
 *
 * only set flag
 * all processes is in usb_task
 */
void USBFS_WKUP_IRQHandler(void)
{
#if 0
	if(cdc_acm.bp.low_power) {
		SystemInit();
		rcu_usbfs_clock_config(usbfs_prescaler);
		rcu_periph_clock_enable(RCU_USBFS);
		usb_clock_active(&cdc_acm);
	}
#endif
	iset_flg(USB_FLAG_ID, 2);
	exti_interrupt_flag_clear(EXTI_18);
}

void USBFS_IRQHandler(void)
{
	interrupt_handler(USB_IRQ_VEC_NO);
}

static void create_idle_task(void)
{
	T_CTSK pk_ctsk;
	pk_ctsk.tskatr = IDLE_TASK_ATTR;
	pk_ctsk.exinf = IDLE_TASK_EXINF;
	pk_ctsk.task = (FP)task_idle_c;
	pk_ctsk.itskpri = IDLE_TASK_PRIO;
	pk_ctsk.stksz = IDLE_TSK_STACK_SIZE;
	pk_ctsk.stk = idle_stack;

	cre_tsk(IDLE_TASK_ID, &pk_ctsk);
	act_tsk(IDLE_TASK_ID);

}

int dev_shell(void *in, int ilen, void *out, int olen)
{
	char *cmd = (char *)in;
	char *reply = (char *)out;
	int rlen = 0;

	while (*cmd != '\0' && ilen != 0 && olen != 0) {
		if (*cmd != '\n' && *cmd != '\r') {
			*reply++ = *cmd;
			olen--;
			rlen++;
		} else if (*cmd == '\r') {
			*reply++ = '\r';
			olen--;
			rlen++;
			if (olen) {
				*reply++ = '\n';
				olen--;
				rlen++;
			}
			if (olen) {
				*reply++ = '>';
				olen--;
				rlen++;
			}
		}
		ilen--;
		cmd++;
	}
	return rlen;
}

volatile float test_a;
void usb_task(void *exinf)
{
	FLGPTN flgptn;
	usb_core_driver *udev = (usb_core_driver *)exinf;
	static char buf[USB_CDC_DATA_PACKET_SIZE + 32];
	static char reply[USB_CDC_DATA_PACKET_SIZE + 32];
	int len;
	usb_cdc_handler *cdc;

	for (;;) {
		/* float test */
		test_a = (test_a + 12.1234) / 3.2;

		/* process events from usb hardware */
		if (E_OK == wai_flg(USB_FLAG_ID, 1, TWF_ORW, &flgptn)) {
			usbd_isr(udev);
			NVIC_EnableIRQ(USBFS_IRQn);

			if (USBD_CONFIGURED == udev->dev.cur_status) {
				if (0U == cdc_acm_check_ready(udev)) {
					cdc_acm_data_receive(udev);
				} else {
					cdc = (usb_cdc_handler *)udev->dev.class_data[CDC_COM_INTERFACE];
					if (cdc->receive_length) {
						len = cdc->receive_length;
						if (len > sizeof(buf))
							len = sizeof(buf);
						cdc->packet_sent = 0U;
						cdc->receive_length = 0U;

						memcpy(buf, cdc->data, len);
						len = dev_shell(buf, len, reply, sizeof(reply));
						if (len)
							usbd_ep_send(udev, CDC_DATA_IN_EP, (uint8_t *)reply, len);
						else
							cdc->packet_sent = 1;
					}
				}
#if 0
				/*  Simple echo */
				if (0U == cdc_acm_check_ready(udev)) {
					cdc_acm_data_receive(udev);
				} else {
					cdc_acm_data_send(udev);
				}
#endif
			}
		}
	}
}

static void usb_init(void)
{
	T_CFLG pk_cflg;
	T_CTSK pk_ctsk;

	/*
	 * usb flag attributes:
	 * fifo order, wait multiple tasks, self cleared flag
	 */
	pk_cflg.flgatr = TA_TFIFO|TA_CLR|TA_WMUL;
	/* initial flag value */
	pk_cflg.iflgptn = 0;
	cre_flg(USB_FLAG_ID, &pk_cflg);

	pk_ctsk.tskatr = TA_HLNG | TA_FPU;
	pk_ctsk.exinf = (VP_INT)&cdc_acm;
	pk_ctsk.task = (FP)usb_task;
	pk_ctsk.itskpri = USB_TASK_PRIO;
	pk_ctsk.stksz = USB_TASK_STACK_SIZE;
	pk_ctsk.stk = usb_task_stack;

	/* create task */
	cre_tsk(USB_TASK_ID, &pk_ctsk);

	install_handler(usbd_irq_handler, USB_IRQ_VEC_NO, USB_IRQ_PRIO);
	usbd_init(&cdc_acm, USB_CORE_ENUM_FS, &cdc_desc, &cdc_class);
	usb_intr_config();

	/* activate task */
	act_tsk(USB_TASK_ID);
}

void init_tsk(void *arg)
{
	create_idle_task();

	usb_init();
	chg_pri(TSK_SELF, LOW_PRIO - 1);
	for (;;) {
		dly_tsk(1);
	}
}
