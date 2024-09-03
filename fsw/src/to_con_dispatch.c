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

#include "cfe.h"

#include "to_con_app.h"
#include "to_con_dispatch.h"
#include "to_con_cmds.h"
#include "to_con_msg.h"
#include "to_con_eventids.h"
#include "to_con_msgids.h"


/* From Best Practices for using Software Bus

- Applications shall always use API functions to read or manipulate the
   Message Header.
- Applications should maintain a command counter and a command error counter
   in housekeeping telemetry.
- Applications should support a "No-operation" command and a "Reset Counters"
   command.
*/

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/*  TO_CON_ProcessGroundCommand() -- Process local message           */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void TO_CON_ProcessGroundCommand(const CFE_SB_Buffer_t *SBBufPtr)
{
    CFE_MSG_FcnCode_t CommandCode = 0;

    CFE_MSG_GetFcnCode(&SBBufPtr->Msg, &CommandCode);

    switch (CommandCode)
    {
        case TO_CON_NOOP_CC:
            TO_CON_NoopCmd((const TO_CON_NoopCmd_t *)SBBufPtr);
            break;

        case TO_CON_RESET_STATUS_CC:
            TO_CON_ResetCountersCmd((const TO_CON_ResetCountersCmd_t *)SBBufPtr);
            break;

        default:
            CFE_EVS_SendEvent(TO_CON_FNCODE_ERR_EID, CFE_EVS_EventType_ERROR,
                              "L%d TO: Invalid Function Code Rcvd In Ground Command 0x%x", __LINE__,
                              (unsigned int)CommandCode);
            ++TO_CON_Global.HkTlm.Payload.CommandErrorCounter;
    }
}


void TO_CON_TaskPipe(const CFE_SB_Buffer_t *SBBufPtr)
{
    CFE_SB_MsgId_t MsgId;

    CFE_MSG_GetMsgId(&SBBufPtr->Msg, &MsgId);

    /* For SB return statuses that imply a message: process it. */
    switch (CFE_SB_MsgIdToValue(MsgId))
    {
        case TO_CON_CMD_MID:
            TO_CON_ProcessGroundCommand(SBBufPtr);
            break;

        case TO_CON_SEND_HK_MID:
            TO_CON_SendHkCmd((const TO_CON_SendHkCmd_t *)SBBufPtr);
            break;

        default:
            CFE_EVS_SendEvent(TO_CON_MID_ERR_EID, CFE_EVS_EventType_ERROR, "L%d TO: Invalid Msg ID Rcvd 0x%x",
                              __LINE__, (unsigned int)CFE_SB_MsgIdToValue(MsgId));
            break;
    }
}
