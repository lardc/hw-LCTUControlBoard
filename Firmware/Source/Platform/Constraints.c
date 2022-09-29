﻿// ----------------------------------------
// Global definitions
// ----------------------------------------

// Header
#include "Constraints.h"

// Constants
//
const TableItemConstraint Constraint[DATA_TABLE_WP_START] = {
		{COEF_K_MIN, COEF_K_MAX, COEF_K_DEF},										// 0
		{OFFSET_MIN, OFFSET_MAX, OFFSET_DEF},										// 1
		{COEF_P2_MIN, COEF_P2_MAX, COEF_P2_DEF},									// 2
		{COEF_P1_MIN, COEF_P1_MAX, COEF_P1_DEF},									// 3
		{COEF_P0_MIN, COEF_P0_MAX, COEF_P0_DEF},									// 4
		{COEF_K_MIN, COEF_K_MAX, COEF_K_DEF},										// 5
		{OFFSET_MIN, OFFSET_MAX, OFFSET_DEF},										// 6
		{COEF_P2_MIN, COEF_P2_MAX, COEF_P2_DEF},									// 7
		{COEF_P1_MIN, COEF_P1_MAX, COEF_P1_DEF},									// 8
		{COEF_P0_MIN, COEF_P0_MAX, COEF_P0_DEF},									// 9
		{COEF_K_MIN, COEF_K_MAX, COEF_K_DEF},										// 10
		{OFFSET_MIN, OFFSET_MAX, OFFSET_DEF},										// 11
		{COEF_P2_MIN, COEF_P2_MAX, COEF_P2_DEF},									// 12
		{COEF_P1_MIN, COEF_P1_MAX, COEF_P1_DEF},									// 13
		{COEF_P0_MIN, COEF_P0_MAX, COEF_P0_DEF},									// 14
		{COEF_K_MIN, COEF_K_MAX, COEF_K_DEF},										// 15
		{OFFSET_MIN, OFFSET_MAX, OFFSET_DEF},										// 16
		{REGULATOR_KP_MIN, REGULATOR_KP_MAX, REGULATOR_KP_DEF},						// 17
		{REGULATOR_KI_MIN, REGULATOR_KI_MAX, REGULATOR_KI_DEF},						// 18
		{0, 0, 0},																	// 19
		{0, 0, 0},																	// 20
		{0, 0, 0},																	// 21
		{0, 0, 0},																	// 22
		{0, 0, 0},																	// 23
		{0, 0, 0},																	// 24
		{0, 0, 0},																	// 25
		{0, 0, 0},																	// 26
		{0, 0, 0},																	// 27
		{0, 0, 0},																	// 28
		{0, 0, 0},																	// 29
		{FRONT_WIDTH_MIN, FRONT_WIDTH_MAX, FRONT_WIDTH_DEF},						// 30
		{ALOWED_ERROR_MIN, ALOWED_ERROR_MAX, ALOWED_ERROR_DEF},						// 31
		{0, INT16U_MAX, 0},															// 32
		{AFTER_PULSE_PAUSE_MIN, AFTER_PULSE_PAUSE_MAX, AFTER_PULSE_PAUSE_DEF},		// 33
		{0, INT16U_MAX, 0},															// 34
		{FOLLOWING_ERR_CNT_MIN, FOLLOWING_ERR_CNT_MAX, FOLLOWING_ERR_CNT_DEF},		// 35
		{NO, YES, YES},																// 36
		{PS_FIRST_START_TIME_MIN, PS_FIRST_START_TIME_MAX, PS_FIRST_START_TIME_DEF},// 37
		{PS_PREPARE_TIME_MIN, PS_PREPARE_TIME_MAX, PS_PREPARE_TIME_DEF},			// 38
		{VOLTAGE_THRESHOLD_MIN, VOLTAGE_THRESHOLD_MAX, VOLTAGE_THRESHOLD_DEF},		// 39
		{NO, YES, YES},																// 40
		{FAN_PERIOD_MIN, FAN_PERIOD_MAX, FAN_PERIOD_DEF},							// 41
		{FAN_TIME_MIN, FAN_TIME_MAX, FAN_TIME_DEF},									// 42
		{PAU_DELAY_SYNC_MIN, PAU_DELAY_SYNC_MAX, PAU_DELAY_SYNC_DEF},				// 43
		{NO, YES, YES},																// 44
		{NO, YES, YES},																// 45
		{NO, YES, YES},																// 46
		{NO, YES, YES},																// 47
		{0, PAU_SYNC_STEP_MAX, PAU_SYNC_STEP_DEF},									// 48
		{PAU_SYNC_CNT_MIN, PAU_SYNC_CNT_MAX, PAU_SYNC_CNT_DEF},						// 49
		{PAU_SYNC_WIDTH_MIN, PAU_SYNC_WIDTH_MAX, PAU_SYNC_WIDTH_DEF},				// 50
		{NO, YES, YES},																// 51
		{0, 0, 0},																	// 52
		{0, 0, 0},																	// 53
		{0, 0, 0},																	// 54
		{0, 0, 0},																	// 55
		{0, 0, 0},																	// 56
		{0, 0, 0},																	// 57
		{0, 0, 0},																	// 58
		{0, 0, 0},																	// 59
		{0, 0, 0},																	// 60
		{0, 0, 0},																	// 61
		{0, 0, 0},																	// 62
		{0, 0, 0},																	// 63
		{0, 0, 0},																	// 64
		{0, 0, 0},																	// 65
		{0, 0, 0},																	// 66
		{0, 0, 0},																	// 67
		{0, 0, 0},																	// 68
		{0, 0, 0},																	// 69
		{0, 0, 0},																	// 70
		{0, 0, 0},																	// 71
		{0, 0, 0},																	// 72
		{0, 0, 0},																	// 73
		{0, 0, 0},																	// 74
		{0, 0, 0},																	// 75
		{0, 0, 0},																	// 76
		{0, 0, 0},																	// 77
		{0, 0, 0},																	// 78
		{0, 0, 0},																	// 79
		{0, 0, 0},																	// 80
		{0, 0, 0},																	// 81
		{0, 0, 0},																	// 82
		{0, 0, 0},																	// 83
		{0, 0, 0},																	// 84
		{0, 0, 0},																	// 85
		{0, 0, 0},																	// 86
		{0, 0, 0},																	// 87
		{0, 0, 0},																	// 88
		{0, 0, 0},																	// 89
		{0, 0, 0},																	// 90
		{0, 0, 0},																	// 91
		{0, 0, 0},																	// 92
		{0, 0, 0},																	// 93
		{0, 0, 0},																	// 94
		{0, 0, 0},																	// 95
		{0, 0, 0},																	// 96
		{0, 0, 0},																	// 97
		{0, 0, 0},																	// 98
		{0, 0, 0},																	// 99
		{0, 0, 0},																	// 100
		{0, 0, 0},																	// 101
		{0, 0, 0},																	// 102
		{0, 0, 0},																	// 103
		{0, 0, 0},																	// 104
		{0, 0, 0},																	// 105
		{0, 0, 0},																	// 106
		{0, 0, 0},																	// 107
		{0, 0, 0},																	// 108
		{0, 0, 0},																	// 109
		{0, 0, 0},																	// 110
		{0, 0, 0},																	// 111
		{0, 0, 0},																	// 112
		{0, 0, 0},																	// 113
		{0, 0, 0},																	// 114
		{0, 0, 0},																	// 115
		{0, 0, 0},																	// 116
		{0, 0, 0},																	// 117
		{0, 0, 0},																	// 118
		{0, 0, 0},																	// 119
		{0, 0, 0},																	// 120
		{0, 0, 0},																	// 121
		{0, 0, 0},																	// 122
		{0, 0, 0},																	// 123
		{0, 0, 0},																	// 124
		{0, 0, 0},																	// 125
		{0, 0, 0},																	// 126
		{0, 0, 0},																	// 127
		{VOLTAGE_SETPOINT_MIN, VOLTAGE_SETPOINT_MAX, 0},							// 128
		{PULSE_WIDTH_MIN, PULSE_WIDTH_MAX, PULSE_WIDTH_DEF},						// 129
		{0, 0, 0},																	// 130
		{0, 0, 0},																	// 131
		{0, 0, 0},																	// 132
		{0, 0, 0},																	// 133
		{0, 0, 0},																	// 134
		{0, 0, 0},																	// 135
		{0, 0, 0},																	// 136
		{0, 0, 0},																	// 137
		{0, 0, 0},																	// 138
		{0, 0, 0},																	// 139
		{0, 0, 0},																	// 140
		{0, 0, 0},																	// 141
		{0, 0, 0},																	// 142
		{0, 0, 0},																	// 143
		{0, 0, 0},																	// 144
		{0, 0, 0},																	// 145
		{0, 0, 0},																	// 146
		{0, 0, 0},																	// 147
		{0, 0, 0},																	// 148
		{0, 0, 0},																	// 149
		{0, INT16U_MAX, 0},															// 150
		{0, INT16U_MAX, 0},															// 151
		{0, 0, 0},																	// 152
		{0, 0, 0},																	// 153
		{0, 0, 0},																	// 154
		{0, 0, 0},																	// 155
		{0, 0, 0},																	// 156
		{0, 0, 0},																	// 157
		{0, 0, 0},																	// 158
		{0, 0, 0},																	// 159
		{0, 0, 0},																	// 160
		{0, 0, 0},																	// 161
		{0, 0, 0},																	// 162
		{0, 0, 0},																	// 163
		{0, 0, 0},																	// 164
		{0, 0, 0},																	// 165
		{0, 0, 0},																	// 166
		{0, 0, 0},																	// 167
		{0, 0, 0},																	// 168
		{0, 0, 0},																	// 169
		{0, 0, 0},																	// 170
		{0, 0, 0},																	// 171
		{0, 0, 0},																	// 172
		{0, 0, 0},																	// 173
		{0, 0, 0},																	// 174
		{0, 0, 0},																	// 175
		{0, 0, 0},																	// 176
		{0, 0, 0},																	// 177
		{0, 0, 0},																	// 178
		{0, 0, 0},																	// 179
		{0, 0, 0},																	// 180
		{0, 0, 0},																	// 181
		{0, 0, 0},																	// 182
		{0, 0, 0},																	// 183
		{0, 0, 0},																	// 184
		{0, 0, 0},																	// 185
		{0, 0, 0},																	// 186
		{0, 0, 0},																	// 187
		{0, 0, 0},																	// 188
		{0, 0, 0},																	// 189
		{0, 0, 0},																	// 190
		{0, 0, 0}																	// 191
};
