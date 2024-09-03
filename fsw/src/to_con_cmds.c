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
#include "to_con_cmds.h"
#include "to_con_msg.h"
#include "to_con_eventids.h"
#include "to_con_msgids.h"


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* TO_CON_Noop() -- Noop Handler                                   */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
CFE_Status_t TO_CON_NoopCmd(const TO_CON_NoopCmd_t *data)
{
    CFE_EVS_SendEvent(TO_CON_NOOP_INF_EID, CFE_EVS_EventType_INFORMATION, "No-op command");
    ++TO_CON_Global.HkTlm.Payload.CommandCounter;
    return CFE_SUCCESS;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* TO_CON_ResetCounters() -- Reset counters                        */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
CFE_Status_t TO_CON_ResetCountersCmd(const TO_CON_ResetCountersCmd_t *data)
{
    TO_CON_Global.HkTlm.Payload.CommandErrorCounter = 0;
    TO_CON_Global.HkTlm.Payload.CommandCounter      = 0;
    return CFE_SUCCESS;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* TO_CON_SendHousekeeping() -- HK status                          */
/* Does not increment CommandCounter                               */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
CFE_Status_t TO_CON_SendHkCmd(const TO_CON_SendHkCmd_t *data)
{
    CFE_SB_TimeStampMsg(CFE_MSG_PTR(TO_CON_Global.HkTlm.TelemetryHeader));
    CFE_SB_TransmitMsg(CFE_MSG_PTR(TO_CON_Global.HkTlm.TelemetryHeader), true);
    return CFE_SUCCESS;
}
