/**
 * @file SerlCan_Intrpt.c
 * @author Junfeng Shen (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2020-10-04
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#include "SerlCan_Intrpt.h"
#include "SerlCan.h"

#define UARTRXBUFSIZE_CNT_U08   ((uint16)1U)


uint8 UartRxBuf_Cnt_M_u08[UARTRXBUFSIZE_CNT_U08];


/**
 * @brief 
 * 
 * @param CanData 
 */
void SerlCan_SendData(SerlCan_Arch* CanData)
{
    CAN_TxHeaderTypeDef Header;
    uint8 Data_Cnt_T_u08[SERLCAN_DATALEN_CNT_U08];
    uint8 i;
    uint32 TxMailbox_Cnt_T_u32;

    Header.StdId = CanData->ArbnId_u32;
    Header.DLC = CanData->Dlc_u08;
    Header.ExtId = 0;
    Header.IDE = CAN_ID_STD;
    Header.RTR = 0;
    Header.TransmitGlobalTime = DISABLE;
    for (i = 0; i < SERLCAN_DATALEN_CNT_U08; i++)
    {
        Data_Cnt_T_u08[i] = CanData->Data_u08[i];
    }
    HAL_CAN_AddTxMessage(&hcan, &Header, Data_Cnt_T_u08, (uint32_t *)&TxMailbox_Cnt_T_u32);
}


/**
 * @brief 
 * 
 * @param huart 
 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    uint8 i;


    for (i=0; i < UARTRXBUFSIZE_CNT_U08; i++)
    {
        SerlCan_SerlRx(huart->pRxBuffPtr[i]);
    }
}
/**
 * @brief 
 * 
 * @param huart 
 */
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
    if (FALSE == SerlCan_IsTxBufEmpty())
    {
        SerlCan_SendDataToUart();
    }
}

/**
 * @brief 
 * 
 * @param hcan 
 */
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
    SerlCan_Arch CanRxData;
    CAN_RxHeaderTypeDef RxHeader;


    if (HAL_OK == HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &RxHeader, CanRxData.Data_u08))
    {

        CanRxData.ArbnId_u32 = (CAN_ID_STD == RxHeader.IDE) ? RxHeader.StdId : RxHeader.ExtId;
        CanRxData.Dlc_u08 = RxHeader.DLC;
        SerlCan_SerlTx(CanRxData);
    }
}
/**
 * @brief 
 * 
 * @param hcan 
 */
void HAL_CAN_RxFifo1MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
	SerlCan_Arch CanRxData;
    CAN_RxHeaderTypeDef RxHeader;


    if (HAL_OK == HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO1, &RxHeader, CanRxData.Data_u08))
    {

        CanRxData.ArbnId_u32 = (CAN_ID_STD == RxHeader.IDE) ? RxHeader.StdId : RxHeader.ExtId;
        CanRxData.Dlc_u08 = RxHeader.DLC;
        SerlCan_SerlTx(CanRxData);
    }
}

/**
 * @brief Starting Uart receiving.
 * 
 */
void SerlCan_StrtUartRx(void)
{
    HAL_UART_Receive_IT(&huart1, UartRxBuf_Cnt_M_u08, UARTRXBUFSIZE_CNT_U08);
}

/**
 * @brief 
 * 
 * @param pData 
 * @param Size_Cnt_T_u16 
 */
void SerlCan_SendDataToUart(void)
{
    SerlCan_SerlTxData *TxData;


    TxData = SerlCan_GetTxData();
    if (HAL_OK == HAL_UART_Transmit_IT(&huart1, TxData->Data_u08, TxData->Len_u08))
    {
        SerlCan_IncReadPt();
    }
}

