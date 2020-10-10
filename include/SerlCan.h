/**
 * @file SerlCan.h
 * @author Junfeng Shen (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2020-10-03
 * 
 * @copyright Copyright (c) 2020
 * 
 */


#ifndef __SERLCAN_H__
#define __SERLCAN_H__
#include "StdTypeDef.h"

/*Define*/
#define SERLCAN_DATALEN_CNT_U08            (8u)
#define SERLCAN_MAXSERLTXLEN_CNT_U08       (19u)
#define SERLCAN_MAXRINGBUFLEN_CNT_U16      (128)
#define SERLCAN_MAXRINGBUFLENMASK_CNT_U16  (SERLCAN_MAXRINGBUFLEN_CNT_U16-1)
/*Type define*/
typedef struct 
{
    uint32 TiStamp_u32;
    uint8 Dlc_u08;
    uint32 ArbnId_u32;
    uint8 Data_u08[SERLCAN_DATALEN_CNT_U08];
} SerlCan_Arch;

 

typedef struct 
{
    uint8 Len_u08;
    uint8 Data_u08[SERLCAN_MAXSERLTXLEN_CNT_U08];
} SerlCan_SerlTxData;

typedef struct 
{
    uint16 WrPt_u16;
    uint16 ReadPt_u16;
    SerlCan_SerlTxData Data[SERLCAN_MAXRINGBUFLEN_CNT_U16];
} SerlCan_TxRingBuf;

extern SerlCan_TxRingBuf TxRingBuf;


/**
 * @brief 
 * 
 * @return uint16 
 */
static inline SerlCan_SerlTxData * SerlCan_GetTxData(void)
{
    return &(TxRingBuf.Data[TxRingBuf.ReadPt_u16 & SERLCAN_MAXRINGBUFLENMASK_CNT_U16]);
}

/**
 * @brief 
 * 
 * @return uint16 
 */
static inline void SerlCan_IncReadPt(void)
{
    TxRingBuf.ReadPt_u16++;
}
/**
 * @brief 
 * 
 * @return boolean 
 */
static inline boolean SerlCan_IsTxBufEmpty(void)
{
    return ((TxRingBuf.WrPt_u16&SERLCAN_MAXRINGBUFLENMASK_CNT_U16) == (TxRingBuf.ReadPt_u16&SERLCAN_MAXRINGBUFLENMASK_CNT_U16)?TRUE:FALSE);
}


extern void SerlCan_SerlRx(uint8 Byte_Cnt_T_u08);
extern uint16 SerlCan_GetTxReadPt(void);
extern void SerlCan_SerlTx(SerlCan_Arch CanBuff);


#endif