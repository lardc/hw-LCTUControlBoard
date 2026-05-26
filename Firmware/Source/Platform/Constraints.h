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
#define COEF_K_MIN					0
#define COEF_K_MAX					INT16U_MAX
#define COEF_K_USET_DEF				0.08006f	// 1 / 12.49
#define COEF_K_IMEAS_DEF			0.383f		// 1 / 2.61
#define COEF_K_UG_DEF				10.989f		// 1 / 0.091
#define COEF_K_UPOT_DEF				3.5461f 	// 1 / 0.282
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
#define COEF_RSH_CH5_DEF			470000
#define COEF_RSH_CH6_DEF			47e+5
#define COEF_RSH_CH7_DEF			47e+6
//
#define SLEW_RATE_MIN				1.0f
//
#define U_RTH_MIN					110		// В мВ
#define U_RTH_MAX					1100
#define U_RTH_DEF					550
//
#define U_IGES_MIN					-30000		// В мВ
#define U_IGES_MAX					30000
#define U_IGES_DEF					5000
#define U_SELTTEST_DEF				5000
//
#define U_ERR_COUNT_MIN				10		// в шт
#define U_ERR_COUNT_MAX				20
#define U_ERR_COUNT_DEF				15
//
#define I_UGETH_MIN				5		// в мА
#define I_UGETH_MAX				500
#define I_UGETH_DEF				50
//
#define COEF_I_CH0_MIN			5e-2	// В А
#define COEF_I_CH1_MIN			5e-3
#define COEF_I_CH2_MIN			5e-4
#define COEF_I_CH3_MIN			5e-5
#define COEF_I_CH4_MIN			2e-6
#define COEF_I_CH5_MIN			2e-7
#define COEF_I_CH6_MIN			2e-8
#define COEF_I_CH7_MIN			5e-9
//
#define COEF_SWITCH_TIME_MIN		5		// в мс
#define COEF_SWITCH_TIME_IGES_MIN	20
#define REGULATOR_TIME_MIN			10
#define REGULATOR_TIME_MAX			10000
#define COEF_FLATTOP_DURATION_DEF	10
//
#define COEF_BASE_VOLT_DEF		3 		// в В
//
#define R_TESTLOAD_DEF			10000	// в Ом
#define R_TESTLOAD_MIN			1
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
