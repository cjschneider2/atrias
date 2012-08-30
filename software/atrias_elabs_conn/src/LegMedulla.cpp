#include "atrias_ecat_conn/LegMedulla.h"

namespace atrias {

namespace ecatConn {

LegMedulla::LegMedulla(uint8_t* inputs, uint8_t* outputs) :
            Medulla() {
	/* These are in the order of the PDO entries in the ESI XML file.
	 * I'd love cur_index to be a void * here, but C doesn't like doing pointer
	 * arithmetic w/ void pointers due to an unhelpful "feature" of the language
	 * (specifically, cur_index + 1 is equivalent to &(cur_index[1]) regardless
	 * of type...)
	 */
	uint8_t * cur_index = outputs;
	
	setPdoPointer(cur_index, command);
	setPdoPointer(cur_index, counter);
	setPdoPointer(cur_index, motorCurrent);
	
	cur_index           = inputs;
	
	setPdoPointer(cur_index, id);
	setPdoPointer(cur_index, state);
	setPdoPointer(cur_index, timingCounter);
	setPdoPointer(cur_index, errorFlags);
	setPdoPointer(cur_index, limitSwitch);
	setPdoPointer(cur_index, toeSensor);
	setPdoPointer(cur_index, motorEncoder);
	setPdoPointer(cur_index, motorEncoderTimestamp);
	setPdoPointer(cur_index, legEncoder);
	setPdoPointer(cur_index, legEncoderTimestamp);
	setPdoPointer(cur_index, incrementalEncoder);
	setPdoPointer(cur_index, incrementalEncoderTimestamp);
	setPdoPointer(cur_index, motorVoltage);
	setPdoPointer(cur_index, logicVoltage);
	setPdoPointer(cur_index, thermistor0);
	setPdoPointer(cur_index, thermistor1);
	setPdoPointer(cur_index, thermistor2);
	setPdoPointer(cur_index, thermistor3);
	setPdoPointer(cur_index, thermistor4);
	setPdoPointer(cur_index, thermistor5);
	setPdoPointer(cur_index, amp1MeasuredCurrent);
	setPdoPointer(cur_index, amp2MeasuredCurrent);
	
	motorEncoderValue                = (int64_t) *motorEncoder;
	motorEncoderTimestampValue       =           *motorEncoderTimestamp;
	legEncoderValue                  = (int64_t) *legEncoder;
	legEncoderTimestampValue         =           *legEncoderTimestamp;
	incrementalEncoderValue          =           *incrementalEncoder;
	incrementalEncoderTimestampValue =           *incrementalEncoderTimestamp;
	timingCounterValue               =           *timingCounter;
}

intptr_t LegMedulla::getInputsSize() {
	return MEDULLA_LEG_INPUTS_SIZE;
}

intptr_t LegMedulla::getOutputsSize() {
	return MEDULLA_LEG_OUTPUTS_SIZE;
}

void LegMedulla::processIncrementalEncoders(RTT::os::TimeService::nsecs deltaTime, atrias_msgs::robot_state& robotState) {
	// This compensates for wraparound.
	int16_t deltaPos = ((int32_t) *incrementalEncoder + (1 << 15) - incrementalEncoderValue) % (1 << 16) - (1 << 15);
	incrementalEncoderValue += deltaPos;
	
	// Let's take into account the timestamps, too.
	double adjustedTime = ((double) deltaTime) / SECOND_IN_NANOSECONDS +
	                      ((double) (*incrementalEncoderTimestamp - incrementalEncoderTimestampValue))
	                      / MEDULLA_TIMER_FREQ;
	
	incrementalEncoderTimestampValue = *incrementalEncoderTimestamp;
	
	switch (*id) {
		case MEDULLA_LEFT_LEG_A_ID:
			robotState.lLeg.halfA.rotorAngle += deltaPos * INC_ENC_RAD_PER_TICK;
			robotState.lLeg.halfA.rotorVelocity =
				((double) deltaPos) * INC_ENC_RAD_PER_TICK / adjustedTime;
			break;
			
		case MEDULLA_LEFT_LEG_B_ID:
			robotState.lLeg.halfB.rotorAngle += deltaPos * INC_ENC_RAD_PER_TICK;
			robotState.lLeg.halfB.rotorVelocity =
				((double) deltaPos) * INC_ENC_RAD_PER_TICK / adjustedTime;
			break;
			
		case MEDULLA_RIGHT_LEG_A_ID:
			robotState.rLeg.halfA.rotorAngle += deltaPos * INC_ENC_RAD_PER_TICK;
			robotState.rLeg.halfA.rotorVelocity =
				((double) deltaPos) * INC_ENC_RAD_PER_TICK / adjustedTime;
			break;
			
		case MEDULLA_RIGHT_LEG_B_ID:
			robotState.rLeg.halfB.rotorAngle += deltaPos * INC_ENC_RAD_PER_TICK;
			robotState.rLeg.halfB.rotorVelocity =
				((double) deltaPos) * INC_ENC_RAD_PER_TICK / adjustedTime;
			break;
	}
}

void LegMedulla::processReceiveData(atrias_msgs::robot_state& robot_state) {
	// If we don't have new data, don't run. It's pointless, and results in
	// NaN velocities.
	if (*timingCounter == timingCounterValue)
		return;
	// Calculate how much time has elapsed since the previous sensor readings.
	// Note: % isn't actually a modulo, hence the additional 256.
	RTT::os::TimeService::nsecs deltaTime =
		((((int16_t) *timingCounter) + 256 - ((int16_t) timingCounterValue)) % 256)
		* CONTROLLER_LOOP_PERIOD_NS;
	timingCounterValue = *timingCounter;
	
	processPositions(robot_state);
	processVelocities(deltaTime, robot_state);
	processIncrementalEncoders(deltaTime, robot_state);
	processThermistors(robot_state);
	processLimitSwitches();
	processVoltages(robot_state);
	processCurrents(robot_state);
	
	switch (*id) {
		case MEDULLA_LEFT_LEG_A_ID:
			robot_state.lLeg.halfA.medullaState = *state;
			break;
		case MEDULLA_LEFT_LEG_B_ID:
			robot_state.lLeg.halfB.medullaState = *state;
			break;
		case MEDULLA_RIGHT_LEG_A_ID:
			robot_state.rLeg.halfA.medullaState = *state;
			break;
		case MEDULLA_RIGHT_LEG_B_ID:
			robot_state.rLeg.halfB.medullaState = *state;
			break;
	}
}

int32_t LegMedulla::calcMotorCurrentOut(atrias_msgs::controller_output& controllerOutput) {
	// Don't command any amount of torque if we're not enabled.
	if (controllerOutput.command != medulla_state_run) return 0;
	
	// If the ID isn't recognized, command 0 torque.
	double torqueCmd = 0.0;
	
	switch(*id) {
		case MEDULLA_LEFT_LEG_A_ID:
			torqueCmd = controllerOutput.lLeg.motorCurrentA;
			break;
		case MEDULLA_LEFT_LEG_B_ID:
			torqueCmd = controllerOutput.lLeg.motorCurrentB;
			break;
		case MEDULLA_RIGHT_LEG_A_ID:
			torqueCmd = controllerOutput.rLeg.motorCurrentA;
			break;
		case MEDULLA_RIGHT_LEG_B_ID:
			torqueCmd = controllerOutput.rLeg.motorCurrentB;
			break;
	}
	
	return (int32_t) (((double) MTR_MAX_COUNT) * torqueCmd / MTR_MAX_TORQUE);
}

inline double LegMedulla::encTicksToRad(uint32_t ticks, uint32_t calib_val, double rad_per_tick, double calib_val_rad) {
	// Be careful of integer/double conversions here.
	return calib_val_rad + ((double) (((int64_t) ticks) - ((int64_t) calib_val))) * rad_per_tick;
}

void LegMedulla::processPositions(atrias_msgs::robot_state& robotState) {
	switch (*id) {
		case MEDULLA_LEFT_LEG_A_ID:
			robotState.lLeg.halfA.motorAngle =
				encTicksToRad(*motorEncoder, LEFT_TRAN_A_CALIB_VAL,  LEFT_TRAN_A_RAD_PER_CNT, LEG_A_CALIB_LOC);
			robotState.lLeg.halfA.legAngle   =
				encTicksToRad(*legEncoder,   LEFT_LEG_A_CALIB_VAL,   LEFT_LEG_A_RAD_PER_CNT,  LEG_A_CALIB_LOC);
			break;
		case MEDULLA_LEFT_LEG_B_ID:
			robotState.lLeg.halfB.motorAngle =
				encTicksToRad(*motorEncoder, LEFT_TRAN_B_CALIB_VAL,  LEFT_TRAN_B_RAD_PER_CNT, LEG_B_CALIB_LOC);
			robotState.lLeg.halfB.legAngle   =
				encTicksToRad(*legEncoder,   LEFT_LEG_B_CALIB_VAL,   LEFT_LEG_B_RAD_PER_CNT,  LEG_B_CALIB_LOC);
			break;
		case MEDULLA_RIGHT_LEG_A_ID:
			robotState.rLeg.halfA.motorAngle =
				encTicksToRad(*motorEncoder, RIGHT_TRAN_A_CALIB_VAL, RIGHT_TRAN_A_RAD_PER_CNT, LEG_A_CALIB_LOC);
			robotState.rLeg.halfA.legAngle   =
				encTicksToRad(*legEncoder,   RIGHT_LEG_A_CALIB_VAL,  RIGHT_LEG_A_RAD_PER_CNT,  LEG_A_CALIB_LOC);
			break;
		case MEDULLA_RIGHT_LEG_B_ID:
			robotState.rLeg.halfB.motorAngle =
				encTicksToRad(*motorEncoder, RIGHT_TRAN_B_CALIB_VAL, RIGHT_TRAN_B_RAD_PER_CNT, LEG_B_CALIB_LOC);
			robotState.rLeg.halfB.legAngle   =
				encTicksToRad(*legEncoder,   RIGHT_LEG_B_CALIB_VAL,  RIGHT_LEG_B_RAD_PER_CNT,  LEG_B_CALIB_LOC);
			break;
	}
}

void LegMedulla::processVelocities(RTT::os::TimeService::nsecs deltaTime, atrias_msgs::robot_state& robotState) {
	switch (*id) {
		case MEDULLA_LEFT_LEG_A_ID:
			// The division by 32 million translates timer ticks from the microcontroller into seconds
			robotState.lLeg.halfA.motorVelocity =
				((double) (((int64_t) *motorEncoder) - motorEncoderValue)) * LEFT_TRAN_A_RAD_PER_CNT  /
				(((double) deltaTime) / 1000000000.0 + ((double) (*motorEncoderTimestamp - motorEncoderTimestampValue)) / MEDULLA_TIMER_FREQ);
			robotState.lLeg.halfA.legVelocity   =
				((double) (((int64_t) *legEncoder)   - legEncoderValue))   * LEFT_LEG_A_RAD_PER_CNT   /
				(((double) deltaTime) / 1000000000.0 + ((double) (*legEncoderTimestamp   - legEncoderTimestampValue))   / MEDULLA_TIMER_FREQ);
			break;
		case MEDULLA_LEFT_LEG_B_ID:
			robotState.lLeg.halfB.motorVelocity =
				((double) (((int64_t) *motorEncoder) - motorEncoderValue)) * LEFT_TRAN_B_RAD_PER_CNT  /
				(((double) deltaTime) / 1000000000.0 + ((double) (*motorEncoderTimestamp - motorEncoderTimestampValue)) / MEDULLA_TIMER_FREQ);
			robotState.lLeg.halfB.legVelocity   =
				((double) (((int64_t) *legEncoder)   - legEncoderValue))   * LEFT_LEG_B_RAD_PER_CNT   /
				(((double) deltaTime) / 1000000000.0 + ((double) (*legEncoderTimestamp   -   legEncoderTimestampValue)) / MEDULLA_TIMER_FREQ);
			break;
		case MEDULLA_RIGHT_LEG_A_ID:
			robotState.rLeg.halfA.motorVelocity =
				((double) (((int64_t) *motorEncoder) - motorEncoderValue)) * RIGHT_TRAN_A_RAD_PER_CNT /
				(((double) deltaTime) / 1000000000.0 + ((double) (*motorEncoderTimestamp - motorEncoderTimestampValue)) / MEDULLA_TIMER_FREQ);
			robotState.rLeg.halfA.legVelocity   =
				((double) (((int64_t) *legEncoder)   - legEncoderValue))   * RIGHT_LEG_B_RAD_PER_CNT  /
				(((double) deltaTime) / 1000000000.0 + ((double) (*legEncoderTimestamp   - legEncoderTimestampValue))   / MEDULLA_TIMER_FREQ);
			break;
		case MEDULLA_RIGHT_LEG_B_ID:
			robotState.rLeg.halfB.motorVelocity =
				((double) (((int64_t) *motorEncoder) - motorEncoderValue)) * RIGHT_TRAN_B_RAD_PER_CNT /
				(((double) deltaTime) / 1000000000.0 + ((double) (*motorEncoderTimestamp - motorEncoderTimestampValue)) / MEDULLA_TIMER_FREQ);
			robotState.rLeg.halfB.legVelocity   =
				((double) (((int64_t) *legEncoder)   - legEncoderValue))   * RIGHT_LEG_B_RAD_PER_CNT  /
				(((double) deltaTime) / 1000000000.0 + ((double) (*legEncoderTimestamp   -   legEncoderTimestampValue)) / MEDULLA_TIMER_FREQ);
			break;
	}

	motorEncoderValue          = (int64_t) *motorEncoder;
	motorEncoderTimestampValue = *motorEncoderTimestamp;
	legEncoderValue            = (int64_t) *legEncoder;
	legEncoderTimestampValue   = *legEncoderTimestamp;
}

void LegMedulla::processThermistors(atrias_msgs::robot_state& robotState) {
	switch (*id) {
		case MEDULLA_LEFT_LEG_A_ID:
			robotState.lLeg.halfA.motorTherms[0] = processThermistorValue(*thermistor0);
			robotState.lLeg.halfA.motorTherms[1] = processThermistorValue(*thermistor1);
			robotState.lLeg.halfA.motorTherms[2] = processThermistorValue(*thermistor2);
			robotState.lLeg.halfA.motorTherms[3] = processThermistorValue(*thermistor3);
			robotState.lLeg.halfA.motorTherms[4] = processThermistorValue(*thermistor4);
			robotState.lLeg.halfA.motorTherms[5] = processThermistorValue(*thermistor5);
			break;
		case MEDULLA_LEFT_LEG_B_ID:
			robotState.lLeg.halfB.motorTherms[0] = processThermistorValue(*thermistor0);
			robotState.lLeg.halfB.motorTherms[1] = processThermistorValue(*thermistor1);
			robotState.lLeg.halfB.motorTherms[2] = processThermistorValue(*thermistor2);
			robotState.lLeg.halfB.motorTherms[3] = processThermistorValue(*thermistor3);
			robotState.lLeg.halfB.motorTherms[4] = processThermistorValue(*thermistor4);
			robotState.lLeg.halfB.motorTherms[5] = processThermistorValue(*thermistor5);
			break;
		case MEDULLA_RIGHT_LEG_A_ID:
			robotState.rLeg.halfA.motorTherms[0] = processThermistorValue(*thermistor0);
			robotState.rLeg.halfA.motorTherms[1] = processThermistorValue(*thermistor1);
			robotState.rLeg.halfA.motorTherms[2] = processThermistorValue(*thermistor2);
			robotState.rLeg.halfA.motorTherms[3] = processThermistorValue(*thermistor3);
			robotState.rLeg.halfA.motorTherms[4] = processThermistorValue(*thermistor4);
			robotState.rLeg.halfA.motorTherms[5] = processThermistorValue(*thermistor5);
			break;
		case MEDULLA_RIGHT_LEG_B_ID:
			robotState.rLeg.halfB.motorTherms[0] = processThermistorValue(*thermistor0);
			robotState.rLeg.halfB.motorTherms[1] = processThermistorValue(*thermistor1);
			robotState.rLeg.halfB.motorTherms[2] = processThermistorValue(*thermistor2);
			robotState.rLeg.halfB.motorTherms[3] = processThermistorValue(*thermistor3);
			robotState.rLeg.halfB.motorTherms[4] = processThermistorValue(*thermistor4);
			robotState.rLeg.halfB.motorTherms[5] = processThermistorValue(*thermistor5);
			break;
	}
}

void LegMedulla::processCurrents(atrias_msgs::robot_state& robotState) {
	double current1 = processAmplifierCurrent(*amp1MeasuredCurrent);
	double current2 = processAmplifierCurrent(*amp2MeasuredCurrent);
	
	switch (*id) {
		case MEDULLA_LEFT_LEG_A_ID:
			robotState.lLeg.halfA.amp1Current  = current1;
			robotState.lLeg.halfA.amp2Current  = current2;
			robotState.lLeg.halfA.motorCurrent = current1 + current2;
			break;
		case MEDULLA_LEFT_LEG_B_ID:
			robotState.lLeg.halfB.amp1Current  = current1;
			robotState.lLeg.halfB.amp2Current  = current2;
			robotState.lLeg.halfB.motorCurrent = current1 + current2;
			break;
		case MEDULLA_RIGHT_LEG_A_ID:
			robotState.rLeg.halfA.amp1Current  = current1;
			robotState.rLeg.halfA.amp2Current  = current2;
			robotState.rLeg.halfA.motorCurrent = current1 + current2;
			break;
		case MEDULLA_RIGHT_LEG_B_ID:
			robotState.rLeg.halfB.amp1Current  = current1;
			robotState.rLeg.halfB.amp2Current  = current2;
			robotState.rLeg.halfB.motorCurrent = current1 + current2;
			break;
	}
}

void LegMedulla::processLimitSwitches() {
	// TODO: figure out which limit switches are which and implement this.
}

void LegMedulla::processVoltages(atrias_msgs::robot_state& robotState) {
	switch (*id) {
		case MEDULLA_LEFT_LEG_A_ID:
			robotState.lLeg.halfA.motorVoltage = processADCValue(*motorVoltage) * 30.0;
			robotState.lLeg.halfA.logicVoltage = processADCValue(*logicVoltage) *  6.0;
			break;
		case MEDULLA_LEFT_LEG_B_ID:
			robotState.lLeg.halfB.motorVoltage = processADCValue(*motorVoltage) * 30.0;
			robotState.lLeg.halfB.logicVoltage = processADCValue(*logicVoltage) *  6.0;
			break;
		case MEDULLA_RIGHT_LEG_A_ID:
			robotState.rLeg.halfA.motorVoltage = processADCValue(*motorVoltage) * 30.0;
			robotState.rLeg.halfA.logicVoltage = processADCValue(*logicVoltage) *  6.0;
			break;
		case MEDULLA_RIGHT_LEG_B_ID:
			robotState.rLeg.halfB.motorVoltage = processADCValue(*motorVoltage) * 30.0;
			robotState.rLeg.halfB.logicVoltage = processADCValue(*logicVoltage) *  6.0;
			break;
	}
}

void LegMedulla::processTransmitData(atrias_msgs::controller_output& controller_output) {
	*counter      = ++local_counter;
	*command      = controller_output.command;
	*motorCurrent = calcMotorCurrentOut(controller_output);
}

uint8_t LegMedulla::getID() {
	return *id;
}

}

}
