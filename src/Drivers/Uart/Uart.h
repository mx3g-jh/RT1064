#ifndef _UART_H_
#define _UART_H_

#include "fsl_adapter_uart.h"
#include "fsl_lpuart_edma.h"
#include "fsl_component_timer_manager.h"

class Uart
{
private:
// Define a uart dma handle
	UART_HANDLE_DEFINE(g_uartHandle);
	UART_DMA_HANDLE_DEFINE(g_UartDmaHandle);
	hal_uart_handle_t uart_handle = (hal_uart_handle_t)g_uartHandle;
	hal_uart_dma_handle_t uart_dma_handle = (hal_uart_dma_handle_t)g_UartDmaHandle;
public:
	Uart(/* args */);
	~Uart();
	bool Init();
	bool send_test();
};

#endif