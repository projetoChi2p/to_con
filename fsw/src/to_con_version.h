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
 *  The TO Console Application header file containing version information
 */
#ifndef TO_CON_VERSION_H
#define TO_CON_VERSION_H

/* Development Build Macro Definitions */
#define TO_CON_BUILD_NUMBER     0 /*!< Development Build: Number of commits since baseline */
#define TO_CON_BUILD_BASELINE   "equuleus-rc2" /*!< Development Build: git tag that is the base for the current development */
#define TO_CON_BUILD_DEV_CYCLE  "none" /**< @brief Development: Release name for current development cycle */
#define TO_CON_BUILD_CODENAME   "Experimental" /**< @brief: Development: Code name for the current build */

/*
 * Version Macros, see \ref cfsversions for definitions.
 */
#define TO_CON_MAJOR_VERSION 0  /*!< @brief Major version number */
#define TO_CON_MINOR_VERSION 1  /*!< @brief Minor version number */
#define TO_CON_REVISION      0 /*!< @brief Revision version number. Value of 0 indicates a development version.*/

/**
 * @brief Last official release.
 */
#define TO_CON_LAST_OFFICIAL "none"

/*!
 * @brief Mission revision.
 *
 * Reserved for mission use to denote patches/customizations as needed.
 * Values 1-254 are reserved for mission use to denote patches/customizations as needed. NOTE: Reserving 0 and 0xFF for
 * cFS open-source development use (pending resolution of nasa/cFS#440)
 */
#define TO_CON_MISSION_REV 0xFF

#define TO_CON_STR_HELPER(x) #x /*!< @brief Helper function to concatenate strings from integer macros */
#define TO_CON_STR(x)        TO_CON_STR_HELPER(x) /*!< @brief Helper function to concatenate strings from integer macros */

/*! @brief Development Build Version Number.
 * @details Baseline git tag + Number of commits since baseline. @n
 * See @ref cfsversions for format differences between development and release versions.
 */
#define TO_CON_VERSION TO_CON_BUILD_BASELINE "+dev" TO_CON_STR(TO_CON_BUILD_NUMBER)

/**
 * @brief Max Version String length.
 * 
 * Maximum length that a TO_CON version string can be.
 * 
 */
#define TO_CON_CFG_MAX_VERSION_STR_LEN 256

#endif
