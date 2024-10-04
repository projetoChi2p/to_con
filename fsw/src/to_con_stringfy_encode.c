/************************************************************************
 * NASA Docket No. GSC-18,719-1, and identified as “core Flight System: Bootes”
 *
 * Copyright (c) 2020 United States Government as represented by the
 * Administrator of the National Aeronautics and Space Administration.
 * All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License. You may obtain
 * a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 ************************************************************************/

/**
 * \file
 *  This file contains the source code for the TO Console application
 */

#include "cfe_config.h"
#include "cfe_sb.h"
#include "cfe_msg.h"

#include "to_con_app.h"
#include "to_con_eventids.h"
#include "to_con_encode.h"



//#include "cfe_tbl_filedef.h" /* Required to obtain the CFE_TBL_FILEDEF macro definition */
//#include "cfe_sb_api_typedefs.h"
//#include "to_con_tbl.h"
#include "cfe_msgids.h"

/*
** Add the proper include file for the message IDs below
*/
#include "to_con_msgids.h"

#ifdef HAVE_MXM_APP
#include "mxm_app_msgids.h"
#include "mxm_app_msgstruct.h"
#endif

#ifdef HAVE_HUFF_APP
#include "huff_app_msgids.h"
#include "huff_app_msgstruct.h"
#endif

#ifdef HAVE_CI_LAB
#include "ci_lab_msgids.h"
#endif




#define MAX_TO_TEXT_PAYLOAD_BYTES 128
#define MAX_TO_MSG_NAME_BYTES 32
#define MAX_TO_MSG_TEXT_BYTES 128

/*
 * --------------------------------------------
 * This implements an "encoder" that simply outputs the same pointer that was passed in.
 * This matches the traditional TO behavior where the "C" struct is passed directly to the socket.
 *
 * The only thing this needs to do get the real size of the output datagram, which should be
 * the size stored in the CFE message header.
 * --------------------------------------------
 */
CFE_Status_t TO_CON_StringfyOutputMessage(const CFE_SB_Buffer_t *SourceBuffer, const char **DestBufferOut,
                                        size_t *DestSizeOut)
{
    uint32_t                    MsgIdValue;
    static char                 TextBuffer[MAX_TO_TEXT_PAYLOAD_BYTES];
    char                        MessageName[MAX_TO_MSG_NAME_BYTES];
    char                        MessageText[MAX_TO_MSG_TEXT_BYTES];
    OS_time_t                   LocalTime;
    int64                       NowTimeMillis;
    size_t                      ActualLength;
    size_t                      ExpectedLength;
    CFE_SB_MsgId_t              MsgId = CFE_SB_INVALID_MSG_ID;
    int32                       status;

#ifdef HAVE_MXM_APP
    const MXM_APP_ResultTlm_t*  MxmAppResultTlmPtr;
#endif
#ifdef HAVE_HUFF_APP
    const HUFF_APP_ResultTlm_t* HuffAppResultTlmPtr;
#endif

    MessageText[0] = '\0';

    CFE_MSG_GetMsgId(&SourceBuffer->Msg, &MsgId);
    MsgIdValue = CFE_SB_MsgIdToValue(MsgId);

    switch (MsgIdValue) {
        case TO_CON_HK_TLM_MID:
            strncpy ( MessageName, "TO_HK", MAX_TO_MSG_NAME_BYTES);
            break;
        case CFE_ES_HK_TLM_MID:
            strncpy ( MessageName, "ES_HK", MAX_TO_MSG_NAME_BYTES);
            break;
        case CFE_EVS_HK_TLM_MID:
            strncpy ( MessageName, "EVS_HK", MAX_TO_MSG_NAME_BYTES);
            break;
        case CFE_SB_HK_TLM_MID:
            strncpy ( MessageName, "SB_HK", MAX_TO_MSG_NAME_BYTES);
            break;
        case CFE_TBL_HK_TLM_MID:
            strncpy ( MessageName, "TBL_HK", MAX_TO_MSG_NAME_BYTES);
            break;
        case CFE_TIME_HK_TLM_MID:
            strncpy ( MessageName, "TIME_HK", MAX_TO_MSG_NAME_BYTES);
            break;
        case CFE_TIME_DIAG_TLM_MID:
            strncpy ( MessageName, "TIME_DIAG", MAX_TO_MSG_NAME_BYTES);
            break;
        case CFE_SB_STATS_TLM_MID:
            strncpy ( MessageName, "SB_STATS", MAX_TO_MSG_NAME_BYTES);
            break;
        case CFE_TBL_REG_TLM_MID:
            strncpy ( MessageName, "TBL_REG", MAX_TO_MSG_NAME_BYTES);
            break;
        case CFE_EVS_LONG_EVENT_MSG_MID:
            strncpy ( MessageName, "EVS_LONG_EVENT", MAX_TO_MSG_NAME_BYTES);
            break;
        case CFE_ES_APP_TLM_MID:
            strncpy ( MessageName, "ES_APP", MAX_TO_MSG_NAME_BYTES);
            break;
        case CFE_ES_MEMSTATS_TLM_MID:
            strncpy ( MessageName, "ES_MEMSTATS", MAX_TO_MSG_NAME_BYTES);
            break;
#ifdef HAVE_MXM_APP
        case MXM_APP_HK_TLM_MID:
            strncpy ( MessageName, "MXM_HK", MAX_TO_MSG_NAME_BYTES);
            break;
        case MXM_APP_RES_TLM_MID:
            strncpy ( MessageName, "MXM_RES", MAX_TO_MSG_NAME_BYTES);
            ExpectedLength = sizeof(MXM_APP_ResultTlm_t);
            ActualLength = 0;
            CFE_MSG_GetSize(&SourceBuffer->Msg, &ActualLength);
            if (ActualLength == ExpectedLength)
            {
                MxmAppResultTlmPtr = (const MXM_APP_ResultTlm_t *)SourceBuffer;
                status = CFE_SB_MessageStringGet(
                    MessageText,
                    MxmAppResultTlmPtr->Payload.ResultStr,
                    NULL, 
                    sizeof(MessageText),
                    sizeof(MxmAppResultTlmPtr->Payload.ResultStr));
                if (status == CFE_SB_BAD_ARGUMENT) {
                    CFE_EVS_SendEvent(TO_CON_ENCODE_ERR_EID, CFE_EVS_EventType_ERROR,
                            "Failed retrieve message content: 0x%08lx", (unsigned long)status);
                }
            }
            else
            {
                CFE_EVS_SendEvent(TO_CON_ENCODE_ERR_EID, CFE_EVS_EventType_ERROR,
                          "Invalid Msg length: ID = 0x%X,  Len = %u, Expected = %u",
                          (unsigned int)MsgIdValue, 
                          (unsigned int)ActualLength,
                          (unsigned int)ExpectedLength);
            }
            break;
#endif
#ifdef HAVE_HUFF_APP
        case HUFF_APP_HK_TLM_MID:
            strncpy ( MessageName, "HUFF_HK", MAX_TO_MSG_NAME_BYTES);
            break;
        case HUFF_APP_RES_TLM_MID:
            strncpy ( MessageName, "HUFF_RES", MAX_TO_MSG_NAME_BYTES);
            ExpectedLength = sizeof(HUFF_APP_ResultTlm_t);
            ActualLength = 0;
            CFE_MSG_GetSize(&SourceBuffer->Msg, &ActualLength);
            if (ActualLength == ExpectedLength)
            {
                HuffAppResultTlmPtr = (const HUFF_APP_ResultTlm_t *)SourceBuffer;
                status = CFE_SB_MessageStringGet(
                    MessageText,
                    HuffAppResultTlmPtr->Payload.ResultStr,
                    NULL, 
                    sizeof(MessageText),
                    sizeof(HuffAppResultTlmPtr->Payload.ResultStr));
                if (status == CFE_SB_BAD_ARGUMENT) {
                    CFE_EVS_SendEvent(TO_CON_ENCODE_ERR_EID, CFE_EVS_EventType_ERROR,
                            "Failed retrieve message content: 0x%08lx", (unsigned long)status);
                }
            }
            else
            {
                CFE_EVS_SendEvent(TO_CON_ENCODE_ERR_EID, CFE_EVS_EventType_ERROR,
                          "Invalid Msg length: ID = 0x%X,  Len = %u, Expected = %u",
                          (unsigned int)MsgIdValue, 
                          (unsigned int)ActualLength,
                          (unsigned int)ExpectedLength);
            }
            break;
#endif
        default:
            strncpy ( MessageName, "unknown", MAX_TO_MSG_NAME_BYTES );
    }
    MessageName[MAX_TO_MSG_NAME_BYTES-1] = '\0';

    memset(&LocalTime, 0, sizeof(LocalTime));
    CFE_PSP_GetTime(&LocalTime);
    NowTimeMillis = OS_TimeGetTotalMilliseconds(LocalTime);

    //ResultStatus = CFE_MSG_GetSize(&SourceBuffer->Msg, &SourceBufferSize);
    snprintf(TextBuffer, MAX_TO_TEXT_PAYLOAD_BYTES, "%lu %04lx %s %s",
        (unsigned long)NowTimeMillis,
        MsgIdValue,
        MessageName,
        MessageText
    );
    TextBuffer[MAX_TO_TEXT_PAYLOAD_BYTES-1] = '\0';

    *DestBufferOut = TextBuffer;
    *DestSizeOut   = strlen(TextBuffer);

    return CFE_SUCCESS;
}
