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
 * @file
 *   Define TO Console Application header file
 */

#ifndef TO_CON_APP_H
#define TO_CON_APP_H

#include "common_types.h"
#include "osapi.h"
#include "cfe.h"

#include "to_con_mission_cfg.h"
#include "to_con_platform_cfg.h"
#include "to_con_cmds.h"
#include "to_con_dispatch.h"
#include "to_con_msg.h"
#include "to_con_tbl.h"

/************************************************************************
** Type Definitions
*************************************************************************/

/**
 * CI global data structure
 */
typedef struct
{
    CFE_SB_PipeId_t Tlm_pipe;
    CFE_SB_PipeId_t Cmd_pipe;

    TO_CON_HkTlm_t        HkTlm;
    TO_CON_DataTypesTlm_t DataTypesTlm;

    TO_CON_Subs_t *  SubsTblPtr;
    CFE_TBL_Handle_t SubsTblHandle;

    osal_id_t        TimeBaseId;
} TO_CON_GlobalData_t;

/************************************************************************
 * Function Prototypes
 ************************************************************************/

void  TO_CON_AppMain(void);
void  TO_CON_openTLM(void);
int32 TO_CON_init(void);
void  TO_CON_process_commands(void);
void  TO_CON_forward_telemetry(void);

/******************************************************************************/

/* Global State Object */
extern TO_CON_GlobalData_t TO_CON_Global;

#endif
