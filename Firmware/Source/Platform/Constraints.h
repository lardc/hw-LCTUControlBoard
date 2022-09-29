#ifndef __CONSTRAINTS_H
#define __CONSTRAINTS_H

// Include
#include "stdinc.h"
#include "DataTable.h"
#include "Global.h"

//Definitions
//
#define PULSE_WIDTH_MIN				1			// (мс)
#define PULSE_WIDTH_MAX				100			// (мс)
#define PULSE_WIDTH_DEF				20			// (мс)
//
#define FRONT_WIDTH_MIN				1			// (мс)
#define FRONT_WIDTH_MAX				10			// (мс)
#define FRONT_WIDTH_DEF				5			// (мс)
//
#define ALOWED_ERROR_MIN			0			// (%)
#define ALOWED_ERROR_MAX			1000		// (%)
#define ALOWED_ERROR_DEF			30			// (%)
//
#define FOLLOWING_ERR_CNT_MIN		0
#define FOLLOWING_ERR_CNT_MAX		1000
#define FOLLOWING_ERR_CNT_DEF		200
//
#define AFTER_PULSE_PAUSE_MIN		1000		// (мс)
#define AFTER_PULSE_PAUSE_MAX		5000		// (мс)
#define AFTER_PULSE_PAUSE_DEF		2000		// (мс)
//
#define COEF_K_MIN					-INT16S_MAX
#define COEF_K_MAX					INT16S_MAX
#define COEF_K_DEF					1
//
#define OFFSET_MIN					-INT16S_MAX
#define OFFSET_MAX					INT16S_MAX
#define OFFSET_DEF					0
//
#define COEF_P2_MIN					-INT16S_MAX
#define COEF_P2_MAX					INT16S_MAX
#define COEF_P2_DEF					0
//
#define COEF_P1_MIN					-INT16S_MAX
#define COEF_P1_MAX					INT16S_MAX
#define COEF_P1_DEF					1
//
#define COEF_P0_MIN					-INT16S_MAX
#define COEF_P0_MAX					INT16U_MAX
#define COEF_P0_DEF					0
//
#define REGULATOR_KP_MIN			0
#define REGULATOR_KP_MAX			INT16U_MAX
#define REGULATOR_KP_DEF			0
//
#define REGULATOR_KI_MIN			0
#define REGULATOR_KI_MAX			INT16U_MAX
#define REGULATOR_KI_DEF			0
//
#define NO							0
#define YES							1
//
#define VOLTAGE_SETPOINT_MIN		200						// В
#define VOLTAGE_SETPOINT_MAX		LCTU_VOLTAGE_MAX		// В
//
#define PS_FIRST_START_TIME_MIN		500						// мс
#define PS_FIRST_START_TIME_MAX		20000					// мс
#define PS_FIRST_START_TIME_DEF		10000					// мс

#define PS_PREPARE_TIME_MIN			10						// мс
#define PS_PREPARE_TIME_MAX			500						// мс
#define PS_PREPARE_TIME_DEF			50						// мс
//
#define VOLTAGE_THRESHOLD_MIN		VOLTAGE_SETPOINT_MIN	// В
#define VOLTAGE_THRESHOLD_MAX		LCTU_VOLTAGE_MAX		// В
#define VOLTAGE_THRESHOLD_DEF		1300					// В
//
#define FAN_PERIOD_MIN				60						// c
#define FAN_PERIOD_MAX				300						// c
#define FAN_PERIOD_DEF				60						// c

#define FAN_TIME_MIN				10						// c
#define FAN_TIME_MAX				200						// c
#define FAN_TIME_DEF				40						// c
//
#define PAU_DELAY_SYNC_MIN			0						// в тиках
#define PAU_DELAY_SYNC_MAX			50						// в тиках
#define PAU_DELAY_SYNC_DEF			10						// в тиках
//
#define PAU_SYNC_STEP_MAX			50						// в тиках
#define PAU_SYNC_STEP_DEF			0						// в тиках
//
#define PAU_SYNC_CNT_MIN			1						//
#define PAU_SYNC_CNT_MAX			100						//
#define PAU_SYNC_CNT_DEF			10						//
//
#define PAU_SYNC_WIDTH_MIN			3						// мкс
#define PAU_SYNC_WIDTH_MAX			10						// мкс
#define PAU_SYNC_WIDTH_DEF			5						// мкс

// Types
typedef struct __TableItemConstraint
{
	float Min;
	float Max;
	float Default;
} TableItemConstraint;

// Variables
extern const TableItemConstraint Constraint[];

#endif // __CONSTRAINTS_H
