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
#include "cfe_config.h"

#include "to_con_app.h"
#include "to_con_encode.h"
#include "to_con_eventids.h"
#include "to_con_msgids.h"
#include "to_con_perfids.h"
#include "to_con_version.h"
#include "to_con_msg.h"
#include "to_con_tbl.h"

/*
** TO Global Data Section
*/
TO_CON_GlobalData_t TO_CON_Global;

extern TO_CON_Subs_t TO_CON_Subs;

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                   */
/* TO_CON_AppMain() -- Application entry point and main process loop */
/*                                                                   */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void TO_CON_AppMain(void)
{
    uint32       RunStatus = CFE_ES_RunStatus_APP_RUN;
    CFE_Status_t status;

    CFE_ES_PerfLogEntry(TO_CON_MAIN_TASK_PERF_ID);

    status = TO_CON_init();

    if (status != CFE_SUCCESS)
    {
        return;
    }

    /*
    ** TO RunLoop
    */
    while (CFE_ES_RunLoop(&RunStatus) == true)
    {
        CFE_ES_PerfLogExit(TO_CON_MAIN_TASK_PERF_ID);

        OS_TaskDelay(TO_CON_TASK_MSEC);

        CFE_ES_PerfLogEntry(TO_CON_MAIN_TASK_PERF_ID);

        TO_CON_forward_telemetry();

        TO_CON_process_commands();
    }

    CFE_ES_ExitApp(RunStatus);
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* TO_CON_init() -- TO initialization                              */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
CFE_Status_t TO_CON_init(void)
{
    CFE_Status_t  status;
    char          PipeName[OS_MAX_API_NAME];
    uint16        PipeDepth;
    uint16        i;
    char          ToTlmPipeName[OS_MAX_API_NAME];
    uint16        ToTlmPipeDepth;
    void *        TblPtr;
    TO_CON_Sub_t *SubEntry;
    char          VersionString[TO_CON_CFG_MAX_VERSION_STR_LEN];
    osal_id_t     TimeBaseId = OS_OBJECT_ID_UNDEFINED;
    int32         OsStatus;

    PipeDepth      = TO_CON_CMD_PIPE_DEPTH;
    strcpy(PipeName, "TO_CON_CMD_PIPE");
    ToTlmPipeDepth = TO_CON_TLM_PIPE_DEPTH;
    strcpy(ToTlmPipeName, "TO_CON_TLM_PIPE");

    /*
    ** Register with EVS
    */
    status = CFE_EVS_Register(NULL, 0, CFE_EVS_EventFilter_BINARY);
    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("TO_CON: Error registering for Event Services, RC = 0x%08X\n", (unsigned int)status);
        return status;
    }

    /* The underlying timebase object should have been created by the PSP */
    OsStatus = OS_TimeBaseGetIdByName(&TimeBaseId, "cFS-Master");
    if (OsStatus != OS_SUCCESS)
    {
        CFE_ES_WriteToSysLog("%s: OS_TimeBaseGetIdByName failed:RC=%ld\n", __func__, (long)OsStatus);
        return CFE_STATUS_EXTERNAL_RESOURCE_FAIL;
    }
    TO_CON_Global.TimeBaseId = TimeBaseId;

    /*
    ** Initialize housekeeping packet (clear user data area)...
    */
    CFE_MSG_Init(CFE_MSG_PTR(TO_CON_Global.HkTlm.TelemetryHeader), CFE_SB_ValueToMsgId(TO_CON_HK_TLM_MID),
                 sizeof(TO_CON_Global.HkTlm));

    status =
        CFE_TBL_Register(&TO_CON_Global.SubsTblHandle, "TO_CON_Subs", sizeof(TO_CON_Subs_t), CFE_TBL_OPT_DEFAULT, NULL);

    if (status != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(TO_CON_TBL_ERR_EID, CFE_EVS_EventType_ERROR, "L%d TO Can't register table status %i",
                          __LINE__, (int)status);
        return status;
    }

    //status = CFE_TBL_Load(TO_CON_Global.SubsTblHandle, CFE_TBL_SRC_FILE, "/cf/to_con_sub.tbl");
    status = CFE_TBL_Load(TO_CON_Global.SubsTblHandle, CFE_TBL_SRC_ADDRESS, &TO_CON_Subs);

    if (status != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(TO_CON_TBL_ERR_EID, CFE_EVS_EventType_ERROR, "L%d TO Can't load table status %i", __LINE__,
                          (int)status);
        return status;
    }

    status = CFE_TBL_GetAddress((void **)&TblPtr, TO_CON_Global.SubsTblHandle);

    if (status != CFE_SUCCESS && status != CFE_TBL_INFO_UPDATED)
    {
        CFE_EVS_SendEvent(TO_CON_TBL_ERR_EID, CFE_EVS_EventType_ERROR, "L%d TO Can't get table addr status %i",
                          __LINE__, (int)status);
        return status;
    }

    TO_CON_Global.SubsTblPtr = TblPtr; /* Save returned address */

    /* Subscribe to my commands */
    status = CFE_SB_CreatePipe(&TO_CON_Global.Cmd_pipe, PipeDepth, PipeName);
    if (status == CFE_SUCCESS)
    {
        CFE_SB_Subscribe(CFE_SB_ValueToMsgId(TO_CON_SEND_HK_MID), TO_CON_Global.Cmd_pipe);
    }
    else
        CFE_EVS_SendEvent(TO_CON_CR_PIPE_ERR_EID, CFE_EVS_EventType_ERROR, "L%d TO Can't create cmd pipe status %i",
                          __LINE__, (int)status);

    /* Create TO TLM pipe */
    status = CFE_SB_CreatePipe(&TO_CON_Global.Tlm_pipe, ToTlmPipeDepth, ToTlmPipeName);
    if (status != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(TO_CON_TLMPIPE_ERR_EID, CFE_EVS_EventType_ERROR, "L%d TO Can't create Tlm pipe status %i",
                          __LINE__, (int)status);
    }

    /* Subscriptions for TLM pipe*/
    SubEntry = TO_CON_Global.SubsTblPtr->Subs;
    for (i = 0; i < TO_CON_MAX_SUBSCRIPTIONS; i++)
    {
        if (!CFE_SB_IsValidMsgId(SubEntry->Stream))
        {
            /* Only process until invalid MsgId is found*/
            break;
        }

        status = CFE_SB_SubscribeEx(SubEntry->Stream, TO_CON_Global.Tlm_pipe, SubEntry->Flags, SubEntry->BufLimit);
        if (status != CFE_SUCCESS)
        {
            CFE_EVS_SendEvent(TO_CON_SUBSCRIBE_ERR_EID, CFE_EVS_EventType_ERROR,
                              "L%d TO Can't subscribe to stream 0x%x status %i", __LINE__,
                              (unsigned int)CFE_SB_MsgIdToValue(SubEntry->Stream), (int)status);
        }

        ++SubEntry;
    }

    CFE_Config_GetVersionString(VersionString, TO_CON_CFG_MAX_VERSION_STR_LEN, "TO Console",
                          TO_CON_VERSION, TO_CON_BUILD_CODENAME, TO_CON_LAST_OFFICIAL);

    CFE_EVS_SendEvent(TO_CON_INIT_INF_EID, CFE_EVS_EventType_INFORMATION,
                      "TO Console Initialized. %s.", VersionString);

    return CFE_SUCCESS;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* TO_CON_process_commands() -- Process command pipe message       */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void TO_CON_process_commands(void)
{
    CFE_SB_Buffer_t *SBBufPtr;
    CFE_Status_t     Status;

    /* Exit command processing loop if no message received. */
    while (1)
    {
        Status = CFE_SB_ReceiveBuffer(&SBBufPtr, TO_CON_Global.Cmd_pipe, CFE_SB_POLL);
        if (Status != CFE_SUCCESS)
        {
            break;
        }

        TO_CON_TaskPipe(SBBufPtr);
    }
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* TO_CON_forward_telemetry() -- Forward telemetry                 */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void TO_CON_forward_telemetry(void)
{
    CFE_Status_t     CfeStatus;
    CFE_SB_Buffer_t *SBBufPtr;
    const char *     TextBufPtr;
    size_t           TextBufSize;
    uint32           PktCount = 0;

    do
    {
        CfeStatus = CFE_SB_ReceiveBuffer(&SBBufPtr, TO_CON_Global.Tlm_pipe, TO_CON_TLM_PIPE_TIMEOUT);

        if (CfeStatus == CFE_SUCCESS)
        {
            CFE_ES_PerfLogEntry(TO_CON_SOCKET_SEND_PERF_ID);

            CfeStatus = TO_CON_StringfyOutputMessage(SBBufPtr, &TextBufPtr, &TextBufSize);

            if (CfeStatus != CFE_SUCCESS)
            {
                CFE_EVS_SendEvent(TO_CON_ENCODE_ERR_EID, CFE_EVS_EventType_ERROR, "Error packing output: %d\n",
                                    (int)CfeStatus);
            }
            else
            {
                OS_printf("%s\n", TextBufPtr);
            }

            CFE_ES_PerfLogExit(TO_CON_SOCKET_SEND_PERF_ID);
        }
        /* If CFE_SB_status != CFE_SUCCESS, then no packet was received from CFE_SB_ReceiveBuffer() */

        PktCount++;
    } while (CfeStatus == CFE_SUCCESS && PktCount < TO_CON_MAX_TLM_PKTS);
}

/************************/
/*  End of File Comment */
/************************/
