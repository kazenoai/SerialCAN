/**
 * @file SerlCan.c
 * @author Junfeng Shen (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2020-10-02
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#include "SerlCan.h"
#include "SerlCan_Intrpt.h"

/*Local Define Start*/
#define STRTBYTE_CNT_U08 (0xAAU)
#define ENDBYTE_CNT_U08   (0xBBU)
#define TISTAMPLEN_CNT_U08 (4U)
#define BITSSHIFTFORONEBYTE_CNT_U08 (8u)
#define ARBNLEN_CNT_U08 (4U)
#define TXLENFIXPART_CNT_U08 (11U) /*(start byte)+TISTAMPLEN_CNT_U08+ARBNLEN_CNT_U08+DLC+(end byte)*/
/*Local Define End*/
/*Local Type Define Start*/
typedef enum {
    SERLCAN_RXST_STRT,
    SERLCAN_RXST_TISTAMP,
    SERLCAN_RXST_DLC,
    SERLCAN_RXST_ARBNID,
    SERLCAN_RXST_DATA,
    SERLCAN_RXST_END
} SerlCan_RxSt;
/*Local Type Define End*/

/*Local Variable Declaration Start*/
SerlCan_RxSt RxSt_Cnt_M_enum;
uint8 RxCnt_Cnt_M_u08;
SerlCan_Arch RxMsg;
SerlCan_TxRingBuf TxRingBuf;
/*Local Variable Declaration End*/

/*Local Function Declaration Start*/

/*Local Function Declaration End*/

/*Function Start*/
/**
 * @brief Rx data from serial bus and process the data according serial can define.
 *        -----------------------------------------------------------------
 *        | Start Byte | Time Stamp |  DLC  | Arb ID | Payload | End Byte |
 *        |   1 byte   | 4 bytes    | 1 Byte| 4 bytes|0-8 bytes| 1 byte   |
 * 
 * @param Byte_Cnt_T_u08 The Data was received from serial bus.
 */
void SerlCan_SerlRx(uint8 Byte_Cnt_T_u08)
{
    switch(RxSt_Cnt_M_enum)
    {
        case SERLCAN_RXST_STRT:
            if (Byte_Cnt_T_u08 == STRTBYTE_CNT_U08)
            {
                RxSt_Cnt_M_enum = SERLCAN_RXST_TISTAMP;
                RxCnt_Cnt_M_u08 = 0;
                RxMsg.TiStamp_u32 = 0;
            }
        break;
        case SERLCAN_RXST_TISTAMP:
            RxMsg.TiStamp_u32 |= (uint32)Byte_Cnt_T_u08 << (RxCnt_Cnt_M_u08*BITSSHIFTFORONEBYTE_CNT_U08);
            RxCnt_Cnt_M_u08++;
            if (RxCnt_Cnt_M_u08 >= TISTAMPLEN_CNT_U08)
            {
                RxCnt_Cnt_M_u08 = 0;
                RxSt_Cnt_M_enum = SERLCAN_RXST_DLC;
            }
        break;
        case SERLCAN_RXST_DLC:
            RxMsg.Dlc_u08 = Byte_Cnt_T_u08;
            RxSt_Cnt_M_enum = SERLCAN_RXST_ARBNID;
            RxMsg.ArbnId_u32 = 0;
            if (RxMsg.Dlc_u08 > SERLCAN_DATALEN_CNT_U08) /*the data lenght of payload must be less 8*/
            {
                RxMsg.Dlc_u08 = 0;
                RxSt_Cnt_M_enum = SERLCAN_RXST_STRT;
            }
        break;
        case SERLCAN_RXST_ARBNID:
            RxMsg.ArbnId_u32 |= (uint32)Byte_Cnt_T_u08 << (RxCnt_Cnt_M_u08*BITSSHIFTFORONEBYTE_CNT_U08);
            RxCnt_Cnt_M_u08++;
            if (RxCnt_Cnt_M_u08 >= ARBNLEN_CNT_U08)
            {
                RxSt_Cnt_M_enum = SERLCAN_RXST_DATA;
                RxCnt_Cnt_M_u08 = 0;
            }
        break;
        case SERLCAN_RXST_DATA:
            if (RxCnt_Cnt_M_u08 < RxMsg.Dlc_u08)
            {
                RxMsg.Data_u08[RxCnt_Cnt_M_u08] = Byte_Cnt_T_u08;
                RxCnt_Cnt_M_u08++;
            }
            else
            {
                RxSt_Cnt_M_enum = SERLCAN_RXST_END;
            }
        break;
        case SERLCAN_RXST_END:
            if (Byte_Cnt_T_u08 == ENDBYTE_CNT_U08)
            {
                /*Send data to Can*/
                SerlCan_SendData(&RxMsg);
            }
            RxSt_Cnt_M_enum = SERLCAN_RXST_STRT;
        break;
        default:
            RxSt_Cnt_M_enum = SERLCAN_RXST_STRT;
        break;
    }
}

void SerlCan_SerlTx(SerlCan_Arch CanBuff)
{
    uint16 *WrPt_Cnt_T_Ptr;
    uint8 i;


    WrPt_Cnt_T_Ptr = &(TxRingBuf.WrPt_u16);
    TxRingBuf.Data[*WrPt_Cnt_T_Ptr&SERLCAN_MAXRINGBUFLENMASK_CNT_U16].Data_u08[0] = STRTBYTE_CNT_U08;
    TxRingBuf.Data[*WrPt_Cnt_T_Ptr&SERLCAN_MAXRINGBUFLENMASK_CNT_U16].Data_u08[1] = (uint8)(CanBuff.TiStamp_u32&0xFFU);
    TxRingBuf.Data[*WrPt_Cnt_T_Ptr&SERLCAN_MAXRINGBUFLENMASK_CNT_U16].Data_u08[2] = (uint8)((CanBuff.TiStamp_u32>>8)&0xFFU);
    TxRingBuf.Data[*WrPt_Cnt_T_Ptr&SERLCAN_MAXRINGBUFLENMASK_CNT_U16].Data_u08[3] = (uint8)((CanBuff.TiStamp_u32>>16)&0xFFU);
    TxRingBuf.Data[*WrPt_Cnt_T_Ptr&SERLCAN_MAXRINGBUFLENMASK_CNT_U16].Data_u08[4] = (uint8)((CanBuff.TiStamp_u32>>24)&0xFFU);
    TxRingBuf.Data[*WrPt_Cnt_T_Ptr&SERLCAN_MAXRINGBUFLENMASK_CNT_U16].Data_u08[5] = CanBuff.Dlc_u08;
    TxRingBuf.Data[*WrPt_Cnt_T_Ptr&SERLCAN_MAXRINGBUFLENMASK_CNT_U16].Data_u08[6] = (uint8)(CanBuff.ArbnId_u32&0xFFU);
    TxRingBuf.Data[*WrPt_Cnt_T_Ptr&SERLCAN_MAXRINGBUFLENMASK_CNT_U16].Data_u08[7] = (uint8)((CanBuff.ArbnId_u32>>8)&0xFFU);
    TxRingBuf.Data[*WrPt_Cnt_T_Ptr&SERLCAN_MAXRINGBUFLENMASK_CNT_U16].Data_u08[8] = (uint8)((CanBuff.ArbnId_u32>>16)&0xFFU);
    TxRingBuf.Data[*WrPt_Cnt_T_Ptr&SERLCAN_MAXRINGBUFLENMASK_CNT_U16].Data_u08[9] = (uint8)((CanBuff.ArbnId_u32>>24)&0xFFU);
    for (i = 10; i < 10+CanBuff.Dlc_u08; i++)
    {
        TxRingBuf.Data[*WrPt_Cnt_T_Ptr&SERLCAN_MAXRINGBUFLENMASK_CNT_U16].Data_u08[i] = CanBuff.Data_u08[i];
    }
    TxRingBuf.Data[*WrPt_Cnt_T_Ptr&SERLCAN_MAXRINGBUFLENMASK_CNT_U16].Data_u08[i] = ENDBYTE_CNT_U08;
    (*WrPt_Cnt_T_Ptr)++;
    SerlCan_SendDataToUart();
}



/*Function End*/
