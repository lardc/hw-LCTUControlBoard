#ifndef __CONSTRAINTS_H
#define __CONSTRAINTS_H

// Include
#include "stdinc.h"
#include "DataTable.h"
#include "Global.h"
#include "LowLevel.h"

//Definitions
#define COEF_P2_MIN					-1e-3
#define COEF_P2_MAX					1e-3
#define COEF_P2_DEF					0
//
#define COEF_P1_MIN					0.5f
#define COEF_P1_MAX					1.5f
#define COEF_P1_DEF					1
//
#define COEF_P0_MIN					INT16S_MIN
#define COEF_P0_MAX					INT16S_MAX
#define COEF_P0_DEF					0
//
#define COEF_K_MIN					1e-6f
#define COEF_K_MAX					1e+6f
#define COEF_K_USET_DEF				3751.0f
#define COEF_K_IMEAS_DEF			1.0f
#define COEF_K_UG_DEF				3751.0f
#define COEF_K_UBAT_DEF				1.0f
//
#define COEF_B_MIN					INT16S_MIN
#define COEF_B_MAX					INT16S_MAX
//
#define COEF_RSH_MIN				0
#define COEF_RSH_MAX				100e+6
#define COEF_RSH_CH0_DEF			2
#define COEF_RSH_CH1_DEF			22
#define COEF_RSH_CH2_DEF			220
#define COEF_RSH_CH3_DEF			2200
#define COEF_RSH_CH4_DEF			22000
//
#define U_ICES_MIN					500
#define U_ICES_MAX					7000 // в В
#define U_ICES_DEF					500
#define U_SELTTEST_MIN				1		// мВ
#define U_SELTTEST_DEF				7000
#define U_CAP_ACTIVATE_RSS_MIN		0
#define U_CAP_ACTIVATE_RSS_MAX		INT16U_MAX
#define U_CAP_ACTIVATE_RSS_DEF		3600
#define U_CAP_READY_MIN				0
#define U_CAP_READY_MAX				INT16U_MAX
#define U_CAP_READY_DEF				7200
//
#define U_ERR_COUNT_MIN				10		// в шт
#define U_ERR_COUNT_MAX				20
#define U_ERR_COUNT_DEF				15
#define I_ERR_COUNT_MIN				1		// в шт
#define I_ERR_COUNT_MAX				20
#define I_ERR_COUNT_DEF				5
//
#define I_ICES_MAX_MIN			10		// Макс. допустимый ток Ices, мА
#define I_ICES_MAX_MAX			300
#define I_ICES_MAX_DEF			100
//
#define COEF_I_CH0_MIN			0.1f	// В А — нижняя граница диапазона канала 0 (100...300 мА)
#define COEF_I_CH1_MIN			0.01f	// канал 1 (10...100 мА)
#define COEF_I_CH2_MIN			1e-3f	// канал 2 (1...10 мА)
#define COEF_I_CH3_MIN			100e-6f	// канал 3 (100...1000 мкА)
#define COEF_I_CH4_MIN			10e-6f	// канал 4 (10...100 мкА)
//
#define COEF_SWITCH_TIME_MIN		5		// в мс
#define COEF_SWITCH_TIME_ICES_MIN	20
#define PULSE_RISE_TIME_MIN			1
#define PULSE_RISE_TIME_DEF			1
#define PULSE_DURATION_MIN			1
#define PULSE_DURATION_DEF			100
#define DEACT_ROUT_DELAY_DEF		20
#define SYNC_DELAY_AFTER_FLAT_DEF	1
#define ST_PULSE_DURATION_DEF		100
#define COEF_FLATTOP_DURATION_DEF	10
//
#define COEF_BASE_VOLT_DEF		3 		// в В
//
#define R_ST_TESTLOAD_MIN			1
#define R_ST_TESTLOAD_7MOHM_DEF		7e6f	// 7 МОм
#define R_ST_TESTLOAD_700MOHM_DEF	700e6f	// 700 МОм
#define ST_CURRENT_ERR_THRESH_DEF	0.1f
//
#define NO						0
#define YES						1
#define COEF_THRESHOLD_MAX		1
//
// Types
typedef struct __TableItemConstraint
{
	float Min;
	float Max;
	float Default;
} TableItemConstraint;

// Variables
extern const TableItemConstraint NVConstraint[DATA_TABLE_NV_SIZE];
extern const TableItemConstraint VConstraint[DATA_TABLE_WP_START - DATA_TABLE_WR_START];

#endif // __CONSTRAINTS_H
