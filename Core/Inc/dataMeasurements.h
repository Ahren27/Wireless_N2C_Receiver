/*
 * dataMeasurements.h
 *
 *  Created on: Jun 6, 2024
 *      Author: ahren
 */

#ifndef INC_DATAMEASUREMENTS_H_
#define INC_DATAMEASUREMENTS_H_

#include "stdint.h"

// Decode Raw Bytes Measurement into Global Variables
void split_data(uint8_t measurments1[], uint8_t measurments2[]);

// Uploads data to serial port
void print_data();

#endif /* INC_DATAMEASUREMENTS_H_ */
