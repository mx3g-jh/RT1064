#include "Uart.h"


Uart::Uart(/* args */)
{
}

Uart::~Uart()
{
}

bool Uart::Init()
{
// Init TimerManager, only used in UART without Idleline interrupt
	// timer_config_t timerConfig;
	// timerConfig.srcClock_Hz    = 16000000;
	// timerConfig.instance       = 0;
	// TM_Init(&timerConfig);

	edma_config_t edmaConfig;
	edmaConfig.enableContinuousLinkMode = true;
	edmaConfig.enableDebugMode = false;
	edmaConfig.enableHaltOnError = true;
	edmaConfig.enableRoundRobinArbitration = false;
// Init the DMA module
	EDMA_Init(DMA0, &edmaConfig);

// Configure uart settings
	hal_uart_config_t uartConfig;
	uartConfig.srcClock_Hz  = 16000000;
	uartConfig.baudRate_Bps = 115200;
	uartConfig.parityMode   = kHAL_UartParityDisabled;
	uartConfig.stopBitCount = kHAL_UartOneStopBit;
	uartConfig.enableRx     = 1;
	uartConfig.enableTx     = 1;
	uartConfig.enableRxRTS  = 0;
	uartConfig.enableTxCTS  = 0;
	uartConfig.instance     = 1;
// Init uart
	HAL_UartInit((hal_uart_handle_t *)uart_handle, &uartConfig);
	// Configure uart dma settings
	hal_uart_dma_config_t dmaConfig;
	dmaConfig.uart_instance = 1;
	dmaConfig.dma_instance  = 0;
	dmaConfig.rx_channel    = 0;
	dmaConfig.tx_channel    = 1;
// Init uart dma
	HAL_UartDMAInit((hal_uart_handle_t *)uart_handle, (hal_uart_dma_handle_t *)uart_dma_handle, &dmaConfig);
	return true;
}

bool Uart::send_test()
{
	uint8_t test_buff[50] = {0};
	memset(test_buff, 0xaa, sizeof(test_buff));
	HAL_UartDMATransferSend((hal_uart_handle_t *)uart_handle, test_buff, sizeof(test_buff));
	return true;
}