/*
 * Copyright (c) 2015 - 2016, Freescale Semiconductor, Inc.
 * Copyright 2016, 2019 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include "usb_virtual_com.h"
#include "main.h"
void USB_DeviceClockInit(void);
void USB_DeviceIsrEnable(void);

#if USB_DEVICE_CONFIG_USE_TASK
void USB_DeviceTaskFn(void *deviceHandle);
#endif

#if (defined(USB_DEVICE_CONFIG_LPCIP3511HS) && (USB_DEVICE_CONFIG_LPCIP3511HS > 0U))
#if !((defined FSL_FEATURE_SOC_USBPHY_COUNT) && (FSL_FEATURE_SOC_USBPHY_COUNT > 0U))
void USB_DeviceHsPhyChirpIssueWorkaround(void);
void USB_DeviceDisconnected(void);
#endif
#endif

usb_status_t USB_DeviceCdcVcomCallback(class_handle_t handle, uint32_t event, void *param);
usb_status_t USB_DeviceCallback(usb_device_handle handle, uint32_t event, void *param);

/*******************************************************************************
 * Variables
 ******************************************************************************/
extern usb_device_endpoint_struct_t g_UsbDeviceCdcVcomDicEndpoints[];
extern usb_device_class_struct_t g_UsbDeviceCdcVcomConfig;
/* Data structure of virtual com device */

/* Line coding of cdc device */
USB_DMA_INIT_DATA_ALIGN(USB_DATA_ALIGN_SIZE)
static uint8_t s_lineCoding[LINE_CODING_SIZE] = {
	/* E.g. 0x00,0xC2,0x01,0x00 : 0x0001C200 is 115200 bits per second */
	(LINE_CODING_DTERATE >> 0U) & 0x000000FFU,
	(LINE_CODING_DTERATE >> 8U) & 0x000000FFU,
	(LINE_CODING_DTERATE >> 16U) & 0x000000FFU,
	(LINE_CODING_DTERATE >> 24U) & 0x000000FFU,
	LINE_CODING_CHARFORMAT,
	LINE_CODING_PARITYTYPE,
	LINE_CODING_DATABITS
};

/* Abstract state of cdc device */
USB_DMA_INIT_DATA_ALIGN(USB_DATA_ALIGN_SIZE)
static uint8_t s_abstractState[COMM_FEATURE_DATA_SIZE] = {(STATUS_ABSTRACT_STATE >> 0U) & 0x00FFU,
							  (STATUS_ABSTRACT_STATE >> 8U) & 0x00FFU
							 };

/* Country code of cdc device */
USB_DMA_INIT_DATA_ALIGN(USB_DATA_ALIGN_SIZE)
static uint8_t s_countryCode[COMM_FEATURE_DATA_SIZE] = {(COUNTRY_SETTING >> 0U) & 0x00FFU,
							(COUNTRY_SETTING >> 8U) & 0x00FFU
						       };

/* CDC ACM information */
USB_DMA_NONINIT_DATA_ALIGN(USB_DATA_ALIGN_SIZE) static usb_cdc_acm_info_t s_usbCdcAcmInfo;
/* Data buffer for receiving and sending*/
USB_DMA_NONINIT_DATA_ALIGN(USB_DATA_ALIGN_SIZE) static uint8_t s_currRecvBuf[DATA_BUFF_SIZE];
// USB_DMA_NONINIT_DATA_ALIGN(USB_DATA_ALIGN_SIZE) static uint8_t s_currSendBuf[DATA_BUFF_SIZE];
volatile static uint32_t s_recvSize = 0;
volatile static uint32_t s_sendSize = 0;

/* USB device class information */
static usb_device_class_config_struct_t s_cdcAcmConfig[1] = {{
		USB_DeviceCdcVcomCallback,
		0,
		&g_UsbDeviceCdcVcomConfig,
	}
};

/* USB device class configuration information */
static usb_device_class_config_list_struct_t s_cdcAcmConfigList = {
	s_cdcAcmConfig,
	USB_DeviceCallback,
	1,
};

#if defined(FSL_FEATURE_USB_KHCI_KEEP_ALIVE_ENABLED) && (FSL_FEATURE_USB_KHCI_KEEP_ALIVE_ENABLED > 0U) && \
defined(USB_DEVICE_CONFIG_KEEP_ALIVE_MODE) && (USB_DEVICE_CONFIG_KEEP_ALIVE_MODE > 0U) &&             \
defined(FSL_FEATURE_USB_KHCI_USB_RAM) && (FSL_FEATURE_USB_KHCI_USB_RAM > 0U)
volatile static uint8_t s_waitForDataReceive = 0;
volatile static uint8_t s_comOpen            = 0;
#endif
/*******************************************************************************
 * Code
 ******************************************************************************/
/*!
 * @brief CDC class specific callback function.
 *
 * This function handles the CDC class specific requests. For the current case, device is waiting for data from host.
 * Once device receives data, kUSB_DeviceCdcEventRecvResponse event will be asserted. In kUSB_DeviceCdcEventRecvResponse
 * event, the received data lenght is saved to s_recvSize. If s_recvSize is 0, device will call USB_DeviceCdcAcmRecv to
 * schedule buffer for next receiving event directly and it means there is no data to echo to host. If s_recvSize is not
 * 0, USB_DeviceCdcVcomTask will copy the received data into s_currSendBuf then send back to host. Once data is echoed
 * back completely, USB_DeviceCdcAcmRecv is called in kUSB_DeviceCdcEventSendResponse event to be ready to receive the
 * next data from host. Instead, USB_DeviceCdcAcmRecv also can be called in kUSB_DeviceCdcEventRecvResponse event as
 * long as the received data is handled completely.
 *
 * @param handle          The CDC ACM class handle.
 * @param event           The CDC ACM class event type.
 * @param param           The parameter of the class specific request.
 *
 * @return A USB error code or kStatus_USB_Success.
 */
usb_status_t USB_DeviceCdcVcomCallback(class_handle_t handle, uint32_t event, void *param)
{
	usb_status_t error = kStatus_USB_InvalidRequest;
	#if ((defined USB_DEVICE_CONFIG_CDC_CIC_EP_DISABLE) && (USB_DEVICE_CONFIG_CDC_CIC_EP_DISABLE > 0U))
	#else
	uint32_t len;
	#endif
	uint8_t *uartBitmap;
	usb_cdc_acm_info_t *acmInfo = &s_usbCdcAcmInfo;
	usb_device_cdc_acm_request_param_struct_t *acmReqParam;
	usb_device_endpoint_callback_message_struct_t *epCbParam;
	acmReqParam = (usb_device_cdc_acm_request_param_struct_t *)param;
	epCbParam   = (usb_device_endpoint_callback_message_struct_t *)param;

	switch (event) {
	case kUSB_DeviceCdcEventSendResponse: {
			if ((epCbParam->length != 0) &&
			    (0U == (epCbParam->length % g_UsbDeviceCdcVcomDicEndpoints[0].maxPacketSize))) {
				/* If the last packet is the size of endpoint, then send also zero-ended packet,
				 ** meaning that we want to inform the host that we do not have any additional
				 ** data, so it can flush the output.
				 */
				error = USB_DeviceCdcAcmSend(handle, USB_CDC_VCOM_BULK_IN_ENDPOINT, NULL, 0);

			} else
				if ((1U == s_cdcVcom.attach) && (1U == s_cdcVcom.startTransactions)) {
					if ((epCbParam->buffer != NULL) || ((epCbParam->buffer == NULL) && (epCbParam->length == 0))) {
						/* User: add your own code for send complete event */
						/* In this case, the received data has been sent back to host, then now schedule buffer for next
						   receiving event. Note that USB_DeviceCdcAcmRecv also can be called in
						   kUSB_DeviceCdcEventRecvResponse as long as we make sure the received data can be handled properly
						 */
						error = USB_DeviceCdcAcmRecv(handle, USB_CDC_VCOM_BULK_OUT_ENDPOINT, s_currRecvBuf,
									     g_UsbDeviceCdcVcomDicEndpoints[1].maxPacketSize);
						#if defined(FSL_FEATURE_USB_KHCI_KEEP_ALIVE_ENABLED) && (FSL_FEATURE_USB_KHCI_KEEP_ALIVE_ENABLED > 0U) && \
						defined(USB_DEVICE_CONFIG_KEEP_ALIVE_MODE) && (USB_DEVICE_CONFIG_KEEP_ALIVE_MODE > 0U) &&             \
						defined(FSL_FEATURE_USB_KHCI_USB_RAM) && (FSL_FEATURE_USB_KHCI_USB_RAM > 0U)
						s_waitForDataReceive = 1;
						USB0->INTEN &= ~USB_INTEN_SOFTOKEN_MASK;
						#endif
					}

				} else {
				}
		}
		break;

	case kUSB_DeviceCdcEventRecvResponse: {
			if ((1U == s_cdcVcom.attach) && (1U == s_cdcVcom.startTransactions)) {
				/* Save the received data length, the data will be handled in USB_DeviceCdcVcomTask. Certainly, the
				   received data also can be handled by any other user's application. Meanwhile, once complete handling
				   the received data then we can also call USB_DeviceCdcAcmRecv here to be ready to receive the next
				   data.  */
				s_recvSize = epCbParam->length;
				error      = kStatus_USB_Success;

				#if defined(FSL_FEATURE_USB_KHCI_KEEP_ALIVE_ENABLED) && (FSL_FEATURE_USB_KHCI_KEEP_ALIVE_ENABLED > 0U) && \
				defined(USB_DEVICE_CONFIG_KEEP_ALIVE_MODE) && (USB_DEVICE_CONFIG_KEEP_ALIVE_MODE > 0U) &&             \
				defined(FSL_FEATURE_USB_KHCI_USB_RAM) && (FSL_FEATURE_USB_KHCI_USB_RAM > 0U)
				s_waitForDataReceive = 0;
				USB0->INTEN |= USB_INTEN_SOFTOKEN_MASK;
				#endif

				/* There is no data to echo to host (host sends 0 length data packet), now directly schedule buffer for
				 * next receiving event */
				if (0U == s_recvSize) {
					error = USB_DeviceCdcAcmRecv(handle, USB_CDC_VCOM_BULK_OUT_ENDPOINT, s_currRecvBuf,
								     g_UsbDeviceCdcVcomDicEndpoints[1].maxPacketSize);
					#if defined(FSL_FEATURE_USB_KHCI_KEEP_ALIVE_ENABLED) && (FSL_FEATURE_USB_KHCI_KEEP_ALIVE_ENABLED > 0U) && \
					defined(USB_DEVICE_CONFIG_KEEP_ALIVE_MODE) && (USB_DEVICE_CONFIG_KEEP_ALIVE_MODE > 0U) &&             \
					defined(FSL_FEATURE_USB_KHCI_USB_RAM) && (FSL_FEATURE_USB_KHCI_USB_RAM > 0U)
					s_waitForDataReceive = 1;
					USB0->INTEN &= ~USB_INTEN_SOFTOKEN_MASK;
					#endif
				}
			}
		}
		break;

	case kUSB_DeviceCdcEventSerialStateNotif:
		((usb_device_cdc_acm_struct_t *)handle)->hasSentState = 0;
		error                                                 = kStatus_USB_Success;
		break;

	case kUSB_DeviceCdcEventSendEncapsulatedCommand:
		break;

	case kUSB_DeviceCdcEventGetEncapsulatedResponse:
		break;

	case kUSB_DeviceCdcEventSetCommFeature:
		if (USB_DEVICE_CDC_FEATURE_ABSTRACT_STATE == acmReqParam->setupValue) {
			if (1U == acmReqParam->isSetup) {
				*(acmReqParam->buffer) = s_abstractState;
				*(acmReqParam->length) = sizeof(s_abstractState);

			} else {
				/* no action, data phase, s_abstractState has been assigned */
			}

			error = kStatus_USB_Success;

		} else
			if (USB_DEVICE_CDC_FEATURE_COUNTRY_SETTING == acmReqParam->setupValue) {
				if (1U == acmReqParam->isSetup) {
					*(acmReqParam->buffer) = s_countryCode;
					*(acmReqParam->length) = sizeof(s_countryCode);

				} else {
					/* no action, data phase, s_countryCode has been assigned */
				}

				error = kStatus_USB_Success;

			} else {
				/* no action, return kStatus_USB_InvalidRequest */
			}

		break;

	case kUSB_DeviceCdcEventGetCommFeature:
		if (USB_DEVICE_CDC_FEATURE_ABSTRACT_STATE == acmReqParam->setupValue) {
			*(acmReqParam->buffer) = s_abstractState;
			*(acmReqParam->length) = COMM_FEATURE_DATA_SIZE;
			error                  = kStatus_USB_Success;

		} else
			if (USB_DEVICE_CDC_FEATURE_COUNTRY_SETTING == acmReqParam->setupValue) {
				*(acmReqParam->buffer) = s_countryCode;
				*(acmReqParam->length) = COMM_FEATURE_DATA_SIZE;
				error                  = kStatus_USB_Success;

			} else {
				/* no action, return kStatus_USB_InvalidRequest */
			}

		break;

	case kUSB_DeviceCdcEventClearCommFeature:
		break;

	case kUSB_DeviceCdcEventGetLineCoding:
		*(acmReqParam->buffer) = s_lineCoding;
		*(acmReqParam->length) = LINE_CODING_SIZE;
		error                  = kStatus_USB_Success;
		break;

	case kUSB_DeviceCdcEventSetLineCoding: {
			if (1U == acmReqParam->isSetup) {
				*(acmReqParam->buffer) = s_lineCoding;
				*(acmReqParam->length) = sizeof(s_lineCoding);

			} else {
				/* no action, data phase, s_lineCoding has been assigned */
			}

			error = kStatus_USB_Success;
		}
		break;

	case kUSB_DeviceCdcEventSetControlLineState: {
			s_usbCdcAcmInfo.dteStatus = acmReqParam->setupValue;

			/* activate/deactivate Tx carrier */
			if (acmInfo->dteStatus & USB_DEVICE_CDC_CONTROL_SIG_BITMAP_CARRIER_ACTIVATION) {
				acmInfo->uartState |= USB_DEVICE_CDC_UART_STATE_TX_CARRIER;

			} else {
				acmInfo->uartState &= (uint16_t)~USB_DEVICE_CDC_UART_STATE_TX_CARRIER;
			}

			/* activate carrier and DTE. Com port of terminal tool running on PC is open now */
			if (acmInfo->dteStatus & USB_DEVICE_CDC_CONTROL_SIG_BITMAP_DTE_PRESENCE) {
				acmInfo->uartState |= USB_DEVICE_CDC_UART_STATE_RX_CARRIER;
			}

			/* Com port of terminal tool running on PC is closed now */
			else {
				acmInfo->uartState &= (uint16_t)~USB_DEVICE_CDC_UART_STATE_RX_CARRIER;
			}

			/* Indicates to DCE if DTE is present or not */
			acmInfo->dtePresent = (acmInfo->dteStatus & USB_DEVICE_CDC_CONTROL_SIG_BITMAP_DTE_PRESENCE) ? true : false;

			/* Initialize the serial state buffer */
			acmInfo->serialStateBuf[0] = NOTIF_REQUEST_TYPE;                /* bmRequestType */
			acmInfo->serialStateBuf[1] = USB_DEVICE_CDC_NOTIF_SERIAL_STATE; /* bNotification */
			acmInfo->serialStateBuf[2] = 0x00;                              /* wValue */
			acmInfo->serialStateBuf[3] = 0x00;
			acmInfo->serialStateBuf[4] = 0x00;                              /* wIndex */
			acmInfo->serialStateBuf[5] = 0x00;
			acmInfo->serialStateBuf[6] = UART_BITMAP_SIZE;                  /* wLength */
			acmInfo->serialStateBuf[7] = 0x00;
			/* Notify to host the line state */
			acmInfo->serialStateBuf[4] = acmReqParam->interfaceIndex;
			/* Lower byte of UART BITMAP */
			uartBitmap    = (uint8_t *)&acmInfo->serialStateBuf[NOTIF_PACKET_SIZE + UART_BITMAP_SIZE - 2];
			uartBitmap[0] = acmInfo->uartState & 0xFFu;
			uartBitmap[1] = (acmInfo->uartState >> 8) & 0xFFu;
			#if ((defined USB_DEVICE_CONFIG_CDC_CIC_EP_DISABLE) && (USB_DEVICE_CONFIG_CDC_CIC_EP_DISABLE > 0U))
			#else
			len = (uint32_t)(NOTIF_PACKET_SIZE + UART_BITMAP_SIZE);

			if (0U == ((usb_device_cdc_acm_struct_t *)handle)->hasSentState) {
				error = USB_DeviceCdcAcmSend(handle, USB_CDC_VCOM_INTERRUPT_IN_ENDPOINT, acmInfo->serialStateBuf, len);

				if (kStatus_USB_Success != error) {
					usb_echo("kUSB_DeviceCdcEventSetControlLineState error!");
				}

				((usb_device_cdc_acm_struct_t *)handle)->hasSentState = 1;
			}

			#endif

			/* Update status */
			if (acmInfo->dteStatus & USB_DEVICE_CDC_CONTROL_SIG_BITMAP_CARRIER_ACTIVATION) {
				/*  To do: CARRIER_ACTIVATED */
			} else {
				/* To do: CARRIER_DEACTIVATED */
			}

			if (1U == s_cdcVcom.attach) {
				s_cdcVcom.startTransactions = 1;
				#if defined(FSL_FEATURE_USB_KHCI_KEEP_ALIVE_ENABLED) && (FSL_FEATURE_USB_KHCI_KEEP_ALIVE_ENABLED > 0U) && \
				defined(USB_DEVICE_CONFIG_KEEP_ALIVE_MODE) && (USB_DEVICE_CONFIG_KEEP_ALIVE_MODE > 0U) &&             \
				defined(FSL_FEATURE_USB_KHCI_USB_RAM) && (FSL_FEATURE_USB_KHCI_USB_RAM > 0U)
				s_waitForDataReceive = 1;
				USB0->INTEN &= ~USB_INTEN_SOFTOKEN_MASK;
				s_comOpen = 1;
				usb_echo("USB_APP_CDC_DTE_ACTIVATED\r\n");
				#endif
			}

			error = kStatus_USB_Success;
		}
		break;

	case kUSB_DeviceCdcEventSendBreak:
		break;

	default:
		break;
	}

	return error;
}

/*!
 * @brief USB device callback function.
 *
 * This function handles the usb device specific requests.
 *
 * @param handle          The USB device handle.
 * @param event           The USB device event type.
 * @param param           The parameter of the device specific request.
 *
 * @return A USB error code or kStatus_USB_Success.
 */
usb_status_t USB_DeviceCallback(usb_device_handle handle, uint32_t event, void *param)
{
	usb_status_t error = kStatus_USB_InvalidRequest;
	uint16_t *temp16   = (uint16_t *)param;
	uint8_t *temp8     = (uint8_t *)param;

	switch (event) {
	case kUSB_DeviceEventBusReset: {
			s_cdcVcom.attach               = 0;
			s_cdcVcom.currentConfiguration = 0U;
			error                          = kStatus_USB_Success;

			#if (defined(USB_DEVICE_CONFIG_LPCIP3511HS) && (USB_DEVICE_CONFIG_LPCIP3511HS > 0U))
			#if !((defined FSL_FEATURE_SOC_USBPHY_COUNT) && (FSL_FEATURE_SOC_USBPHY_COUNT > 0U))
			/* The work-around is used to fix the HS device Chirping issue.
			 * Please refer to the implementation for the detail information.
			 */
			USB_DeviceHsPhyChirpIssueWorkaround();
			#endif
			#endif

			#if (defined(USB_DEVICE_CONFIG_EHCI) && (USB_DEVICE_CONFIG_EHCI > 0U)) || \
			(defined(USB_DEVICE_CONFIG_LPCIP3511HS) && (USB_DEVICE_CONFIG_LPCIP3511HS > 0U))

			/* Get USB speed to configure the device, including max packet size and interval of the endpoints. */
			if (kStatus_USB_Success == USB_DeviceClassGetSpeed(CONTROLLER_ID, &s_cdcVcom.speed)) {
				USB_DeviceSetSpeed(handle, s_cdcVcom.speed);
			}

			#endif
		}
		break;
		#if (defined(USB_DEVICE_CONFIG_DETACH_ENABLE) && (USB_DEVICE_CONFIG_DETACH_ENABLE > 0U))

	case kUSB_DeviceEventDetach: {
			#if (defined(USB_DEVICE_CONFIG_LPCIP3511HS) && (USB_DEVICE_CONFIG_LPCIP3511HS > 0U))
			#if !((defined FSL_FEATURE_SOC_USBPHY_COUNT) && (FSL_FEATURE_SOC_USBPHY_COUNT > 0U))
			USB_DeviceDisconnected();
			#endif
			#endif
			error = kStatus_USB_Success;
		}
		break;
		#endif

	case kUSB_DeviceEventSetConfiguration:
		if (0U == (*temp8)) {
			s_cdcVcom.attach               = 0;
			s_cdcVcom.currentConfiguration = 0U;
			error                          = kStatus_USB_Success;

		} else
			if (USB_CDC_VCOM_CONFIGURE_INDEX == (*temp8)) {
				s_cdcVcom.attach               = 1;
				s_cdcVcom.currentConfiguration = *temp8;
				error                          = kStatus_USB_Success;
				/* Schedule buffer for receive */
				USB_DeviceCdcAcmRecv(s_cdcVcom.cdcAcmHandle, USB_CDC_VCOM_BULK_OUT_ENDPOINT, s_currRecvBuf,
						     g_UsbDeviceCdcVcomDicEndpoints[1].maxPacketSize);

			} else {
				/* no action, return kStatus_USB_InvalidRequest */
			}

		break;

	case kUSB_DeviceEventSetInterface:
		if (0U != s_cdcVcom.attach) {
			uint8_t interface        = (uint8_t)((*temp16 & 0xFF00U) >> 0x08U);
			uint8_t alternateSetting = (uint8_t)(*temp16 & 0x00FFU);

			if (interface == USB_CDC_VCOM_COMM_INTERFACE_INDEX) {
				if (alternateSetting < USB_CDC_VCOM_COMM_INTERFACE_ALTERNATE_COUNT) {
					s_cdcVcom.currentInterfaceAlternateSetting[interface] = alternateSetting;
					error                                                 = kStatus_USB_Success;
				}

			} else
				if (interface == USB_CDC_VCOM_DATA_INTERFACE_INDEX) {
					if (alternateSetting < USB_CDC_VCOM_DATA_INTERFACE_ALTERNATE_COUNT) {
						s_cdcVcom.currentInterfaceAlternateSetting[interface] = alternateSetting;
						error                                                 = kStatus_USB_Success;
					}

				} else {
					/* no action, return kStatus_USB_InvalidRequest */
				}
		}

		break;

	case kUSB_DeviceEventGetConfiguration:
		if (NULL != param) {
			/* Get current configuration request */
			*temp8 = s_cdcVcom.currentConfiguration;
			error  = kStatus_USB_Success;
		}

		break;

	case kUSB_DeviceEventGetInterface:
		if (NULL != param) {
			/* Get current alternate setting of the interface request */
			uint8_t interface = (uint8_t)((*temp16 & 0xFF00U) >> 0x08U);

			if (interface < USB_CDC_VCOM_INTERFACE_COUNT) {
				*temp16 = (*temp16 & 0xFF00U) | s_cdcVcom.currentInterfaceAlternateSetting[interface];
				error   = kStatus_USB_Success;
			}
		}

		break;

	case kUSB_DeviceEventGetDeviceDescriptor:
		if (NULL != param) {
			error = USB_DeviceGetDeviceDescriptor(handle, (usb_device_get_device_descriptor_struct_t *)param);
		}

		break;

	case kUSB_DeviceEventGetConfigurationDescriptor:
		if (NULL != param) {
			error = USB_DeviceGetConfigurationDescriptor(handle,
				(usb_device_get_configuration_descriptor_struct_t *)param);
		}

		break;

	case kUSB_DeviceEventGetStringDescriptor:
		if (NULL != param) {
			/* Get device string descriptor request */
			error = USB_DeviceGetStringDescriptor(handle, (usb_device_get_string_descriptor_struct_t *)param);
		}

		break;

	default:
		/* no action, return kStatus_USB_InvalidRequest */
		break;
	}

	return error;
}

void CDC_VCOM_FreeRTOSEnterCritical(uint32_t *sr)
{
	*sr = DisableGlobalIRQ();
	__ASM("CPSID i");
}

void CDC_VCOM_FreeRTOSExitCritical(uint32_t sr)
{
	EnableGlobalIRQ(sr);
}

/*!
 * @brief Application initialization function.
 *
 * This function initializes the application.
 *
 * @return None.
 */
void USB_DeviceApplicationInit(void)
{
	USB_DeviceClockInit();
	#if (defined(FSL_FEATURE_SOC_SYSMPU_COUNT) && (FSL_FEATURE_SOC_SYSMPU_COUNT > 0U))
	SYSMPU_Enable(SYSMPU, 0);
	#endif /* FSL_FEATURE_SOC_SYSMPU_COUNT */

	s_cdcVcom.speed        = USB_SPEED_FULL;
	s_cdcVcom.attach       = 0;
	s_cdcVcom.cdcAcmHandle = (class_handle_t)NULL;
	s_cdcVcom.deviceHandle = NULL;

	if (kStatus_USB_Success != USB_DeviceClassInit(CONTROLLER_ID, &s_cdcAcmConfigList, &s_cdcVcom.deviceHandle)) {
		usb_echo("USB device init failed\r\n");

	} else {
		usb_echo("USB device CDC virtual com demo\r\n");
		s_cdcVcom.cdcAcmHandle = s_cdcAcmConfigList.config->classHandle;
	}

	USB_DeviceIsrEnable();

	/*Add one delay here to make the DP pull down long enough to allow host to detect the previous disconnection.*/
	SDK_DelayAtLeastUs(5000, SDK_DEVICE_MAXIMUM_CPU_CLOCK_FREQUENCY);
	USB_DeviceRun(s_cdcVcom.deviceHandle);
}

/*!
 * @brief USB task function.
 *
 * This function runs the task for USB device.
 *
 * @return None.
 */
#if USB_DEVICE_CONFIG_USE_TASK
void USB_DeviceTask(void *handle)
{
	while (1U) {
		USB_DeviceTaskFn(handle);
	}
}

#endif

void USB_OTG1_IRQHandler(void)
{
	USB_DeviceEhciIsrFunction(s_cdcVcom.deviceHandle);
}

void USB_OTG2_IRQHandler(void)
{
	USB_DeviceEhciIsrFunction(s_cdcVcom.deviceHandle);
}

void USB_DeviceClockInit(void)
{
	usb_phy_config_struct_t phyConfig = {
		BOARD_USB_PHY_D_CAL,
		BOARD_USB_PHY_TXCAL45DP,
		BOARD_USB_PHY_TXCAL45DM,
	};

	if (CONTROLLER_ID == kUSB_ControllerEhci0) {
		CLOCK_EnableUsbhs0PhyPllClock(kCLOCK_Usbphy480M, 480000000U);
		CLOCK_EnableUsbhs0Clock(kCLOCK_Usb480M, 480000000U);

	} else {
		CLOCK_EnableUsbhs1PhyPllClock(kCLOCK_Usbphy480M, 480000000U);
		CLOCK_EnableUsbhs1Clock(kCLOCK_Usb480M, 480000000U);
	}

	USB_EhciPhyInit(CONTROLLER_ID, BOARD_XTAL0_CLK_HZ, &phyConfig);
}

void USB_DeviceIsrEnable(void)
{
	uint8_t irqNumber;

	uint8_t usbDeviceEhciIrq[] = USBHS_IRQS;
	irqNumber                  = usbDeviceEhciIrq[CONTROLLER_ID - kUSB_ControllerEhci0];

	/* Install isr, set priority, and enable IRQ. */
	NVIC_SetPriority((IRQn_Type)irqNumber, USB_DEVICE_INTERRUPT_PRIORITY);
	EnableIRQ((IRQn_Type)irqNumber);
}

#if USB_DEVICE_CONFIG_USE_TASK
void USB_DeviceTaskFn(void *deviceHandle)
{
	USB_DeviceEhciTaskFunction(deviceHandle);
}

#endif