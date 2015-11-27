#include "uart.h"

uint8_t rx_buffer[256] = { 0 };
uint8_t rx_full = 0;
on_receive_listener *uart1_rx_listener;
on_receive_listener *uart3_rx_listener;
uint8_t uart1_listener_empty = 1;
uint8_t uart3_listener_empty = 1;

#ifdef __GNUC__
/* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
   set to 'Yes') calls __io_putchar() */
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */

USART_TypeDef* COM_USART[COMn] = {USART1, USART3};
GPIO_TypeDef* COM_TX_PORT[COMn] = {COM1_TX_GPIO_PORT, COM3_TX_GPIO_PORT};
GPIO_TypeDef* COM_RX_PORT[COMn] = {COM1_RX_GPIO_PORT, COM3_RX_GPIO_PORT};
uc32 COM_USART_CLK[COMn] = {COM1_CLK, COM3_CLK};
uc32 COM_TX_PORT_CLK[COMn] = {COM1_TX_GPIO_CLK, COM3_TX_GPIO_CLK};
uc32 COM_RX_PORT_CLK[COMn] = {COM1_RX_GPIO_CLK, COM3_RX_GPIO_CLK};
uc16 COM_TX_PIN[COMn] = {COM1_TX_PIN, COM3_TX_PIN};
uc16 COM_RX_PIN[COMn] = {COM1_RX_PIN, COM3_RX_PIN};
uc16 COM_IRQ[COMn] = {USART1_IRQn, USART3_IRQn};

COM_TypeDef printf_COMx;

/**
 * @brief  Inintialization of USART
 * @param  COM: which USART to inialialize
 * @param  br: Baudrate used for USART
 * @retval None
 */
void uart_init(COM_TypeDef COM, u32 br) {
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;

	RCC_APB2PeriphClockCmd(COM_TX_PORT_CLK[COM] | COM_RX_PORT_CLK[COM] | RCC_APB2Periph_AFIO, ENABLE);

	if(COM == COM1) {
		RCC_APB2PeriphClockCmd(COM_USART_CLK[COM], ENABLE);
	} else {
		RCC_APB1PeriphClockCmd(COM_USART_CLK[COM], ENABLE);
	}

	/* Configure USART Tx as alternate function push-pull */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin = COM_TX_PIN[COM];
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(COM_TX_PORT[COM], &GPIO_InitStructure);

	/* Configure USART Rx as input floating */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Pin = COM_RX_PIN[COM];
	GPIO_Init(COM_RX_PORT[COM], &GPIO_InitStructure);

	/* USART configuration */
	USART_InitStructure.USART_BaudRate = br;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

	USART_Init(COM_USART[COM], &USART_InitStructure);
	USART_ITConfig(COM_USART[COM],USART_IT_RXNE,ENABLE);
	USART_Cmd(COM_USART[COM], ENABLE);
}

/**
 * @brief  Enable the interrupt of USART
 * @param  COM: which USART to enable interrupt
 * @retval None
 */
void uart_interrupt(COM_TypeDef COM) {
	NVIC_InitTypeDef NVIC_InitStructure;

	#ifdef VECT_TAB_RAM
	NVIC_SetVectorTable(NVIC_VectTab_RAM,0x0);
	#else
	NVIC_SetVectorTable(NVIC_VectTab_FLASH,0x0);
	#endif

	NVIC_InitStructure.NVIC_IRQChannel = COM_IRQ[COM];
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	/* Enables the USART receive interrupt */
	USART_ITConfig(COM_USART[COM],USART_IT_RXNE,ENABLE);
}

/**
 * @brief  Enable the function of sending data of Printf via USART
 * @param  COM: which USART to be used for Printf
 * @retval None
 */
void uart_printf_enable(COM_TypeDef COM) {
	printf_COMx = COM;
}

/**
 * @brief  Disable the function of sending data of Printf via UART
 * @param  None
 * @retval None
 */
void uart_printf_disable(void) {
	printf_COMx = COM_NULL;
}

/**
 * @brief  Sending one byte of data via USART
 * @param  COM: which USART to be used for sending data
 * @param  data: one byte data to be sent
 * @retval None
 */
void uart_tx_byte(COM_TypeDef COM, uc8 data) {
	while(USART_GetFlagStatus(COM_USART[COM], USART_FLAG_TC) == RESET) ;
	USART_SendData(COM_USART[COM], (uint16_t)data);
}

/**
 * @brief  Sending multiple bytes of data via USART
 * @param  COM: which USART to be used for sending data
 * @param  tx_buf: string to be sent
 * @retval None
 */
void uart_tx(COM_TypeDef COM, const uc8 * tx_buf, ...) {
	va_list arglist;
	u8 buf[255], *fp;

	va_start(arglist, tx_buf);
	vsprintf((char*)buf, (const char*)tx_buf, arglist);
	va_end(arglist);

	fp = buf;
	while(*fp)
		uart_tx_byte(COM, (uint16_t)*fp++);
}
/**
 * @brief  Receiving one byte of data via USART
 * @param  COM: which USART to be used for receiving data
 * @retval One byte of data received
 */
u8 uart_rx_byte(COM_TypeDef COM) {
	while(USART_GetFlagStatus(COM_USART[COM], USART_FLAG_TC) == RESET) ;
	return (u8)USART_ReceiveData(COM_USART[COM]);
}

void uart_interrupt_init(COM_TypeDef COM, on_receive_listener *listener) {
	if(COM == COM1) {
		uart1_rx_listener = listener;
		uart1_listener_empty = 0;
	} else if(COM == COM3) {
		uart3_rx_listener = listener;
		uart3_listener_empty = 0;
	} else
		assert(0);

	uart_interrupt(COM);
}

void USART1_IRQHandler(void) {
	if(USART_GetITStatus(USART1,USART_IT_RXNE) != RESET) { // check RX interrupt
		if(!uart1_listener_empty)
			(*uart1_rx_listener)(USART_ReceiveData(USART1));
		USART_ClearITPendingBit(USART1, USART_IT_RXNE);
	}
}

void USART3_IRQHandler(void) {
	if(USART_GetITStatus(USART3,USART_IT_RXNE) != RESET) { // check RX interrupt
		if(!uart3_listener_empty)
			(*uart3_rx_listener)(USART_ReceiveData(USART3));
		USART_ClearITPendingBit(USART3, USART_IT_RXNE);
	}
}

/**
 * @brief  Binding of function Printf
 * @param  None
 * @retval None
 */
PUTCHAR_PROTOTYPE
{
	if(printf_COMx == COM_NULL)
		return ch;

	while(USART_GetFlagStatus(COM_USART[printf_COMx], USART_FLAG_TC) == RESET) ;
	USART_SendData(COM_USART[printf_COMx], (uint8_t) ch);
	return ch;
}

/********************/
typedef struct {
	uint8_t data_buf[FIFO_BUFFER_SIZE];     // FIFO buffer
	uint16_t i_first;                       // index of oldest data byte in buffer
	uint16_t i_last;                        // index of newest data byte in buffer
	uint16_t num_bytes;                     // number of bytes currently in buffer
}sw_fifo_typedef;

sw_fifo_typedef rx_fifo = {{0},0,0,0};

volatile uint8_t uart_rx_fifo_not_empty_flag; // this flag is automatically set and cleared by the software buffer
volatile uint8_t uart_rx_fifo_full_flag;      // this flag is automatically set and cleared by the software buffer
volatile uint8_t uart_rx_fifo_ovf_flag;

void listener(uint8_t byte) {

	if(rx_fifo.num_bytes == FIFO_BUFFER_SIZE) {         // if the sw buffer is full
		uart_rx_fifo_ovf_flag = 1;                           // set the overflow flag
	} else if(rx_fifo.num_bytes < FIFO_BUFFER_SIZE) {   // if there's room in the sw buffer
		rx_fifo.data_buf[rx_fifo.i_last] = byte;
		rx_fifo.i_last+=1;                                    // increment the index of the most recently added element
		rx_fifo.num_bytes+=1;                                 // increment the bytes counter
	}
	if(rx_fifo.num_bytes == FIFO_BUFFER_SIZE) {         // if sw buffer just filled up
		uart_rx_fifo_full_flag = 1;                          // set the RX FIFO full flag
	}
	if(uart_rx_fifo_full_flag == 1) {

	}
	if(rx_fifo.i_last == FIFO_BUFFER_SIZE) {            // if the index has reached the end of the buffer,
		rx_fifo.i_last = 0;                                  // roll over the index counter
	}
	uart_rx_fifo_not_empty_flag = 1;                    // set received-data flag
} // end UART RX IRQ handler

uint8_t uart_get_byte(void) {

	///////////////////////////////////////////////////////////
	/* disable interrupts while manipulating buffer pointers */
	///////////////////////////////////////////////////////////
	// The above is disable the interrupt during the following operation. The goal is to disable the UART for the moment.
	// try omit this part by using a larger interval that prevent the interrupt

	uint8_t byte = 0;
	if(rx_fifo.num_bytes == FIFO_BUFFER_SIZE) {        // if the sw buffer is full
		uart_rx_fifo_full_flag = 0;                       // clear the buffer full flag because we are about to make room
	}
	if(rx_fifo.num_bytes > 0) {                        // if data exists in the sw buffer
		byte = rx_fifo.data_buf[rx_fifo.i_first];         // grab the oldest element in the buffer
		rx_fifo.data_buf[rx_fifo.i_first] = '|';
		rx_fifo.i_first++;                                // increment the index of the oldest element
		rx_fifo.num_bytes--;                              // decrement the bytes counter
	} else {                                           // RX sw buffer is empty
		uart_rx_fifo_not_empty_flag = 0;                  // clear the rx flag
	}
	if(rx_fifo.i_first == FIFO_BUFFER_SIZE) {          // if the index has reached the end of the buffer,
		rx_fifo.i_first = 0;                              // roll over the index counter
	}

	///////////////////////
	/* enable interrupts */
	///////////////////////
	// The above is to enable the interrupt again
	// try omit this enabling and just use a larger interval

	return byte;                                   // return the data byte
}
