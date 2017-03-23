#include "queue.h"

#pragma config(Sensor, S1,     touchSensor,    sensorEV3_Touch)
#pragma config(Sensor, S2,     gyroSensor,     sensorEV3_Gyro)
#pragma config(Sensor, S3,     colorSensor,    sensorEV3_Color, modeEV3Color_Color)
#pragma config(Sensor, S4,     sonarSensor,    sensorEV3_Ultrasonic)
#pragma config(Motor,  motorA,          armMotor,      tmotorEV3_Large, PIDControl, encoder)
#pragma config(Motor,  motorB,          leftMotor,     tmotorEV3_Large, PIDControl, driveLeft, encoder)
#pragma config(Motor,  motorC,          rightMotor,    tmotorEV3_Large, PIDControl, driveRight, encoder)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

#define TOLERANCE 15

Queue path;

int r, g, b;
int begin_degree;

int getDegrees(short gyro_sensor) {
	int gyroReading = getGyroDegrees(gyro_sensor);
	if (gyroReading > 360) {
		gyroReading %= 360;
	} else if (gyroReading < 0) {
		gyroReading %= 360;
		gyroReading += 360;
	}
	return gyroReading;
}

bool isReverse(int orig, int curr_deg) {
	int diff = curr_deg - orig;
	if (diff < 0)
		diff += 360;
	return (diff >= 180 - TOLERANCE && diff <= 180 + TOLERANCE);
}

bool isColor(short color_sensor) {
	return (getColorName(color_sensor) == colorBlue ||
			getColorName(color_sensor) == colorGreen ||
			getColorName(color_sensor) == colorRed ||
			getColorName(color_sensor) == colorYellow);
}

void reverse() {
	int curr_deg = getDegrees(S2);
	while (!isReverse(curr_deg, getDegrees(S2))) {
		setMotorSpeed(motorB, -50);
		setMotorSpeed(motorC, 50);
	}
}

void lineFollow() {
	// Line following
	if (getColorName(S3) == colorWhite) {
		setMotorSpeed(motorB, 0);
		setMotorSpeed(motorC, 50);
	} else {
		setMotorSpeed(motorB, 50);
		setMotorSpeed(motorC, 20);
	}

	// Confirm color reading
	for (int i = 1; i <= 30; ++i) {
		if (isColor(S3)) {
			setMotorSpeed(motorB, 20);
			setMotorSpeed(motorC, 50);
			wait(5, milliseconds);
		}
	}
}

void turnInNode() {
	bool found_neighbor;
	do {
		int orig_deg, curr_deg, deg_diff;

		// Center robot in green
		do {
			setMotorSpeed(motorB, 40);
			setMotorSpeed(motorC, 50);
			if (getColorName(S3) != colorGreen) {
				setMotorSpeed(motorB, 20);
				setMotorSpeed(motorC, 50);
				wait(0.2, seconds);
			}
		} while (getColorName(S3) == colorGreen);

		// Turn
		orig_deg = getDegrees(S2);
		do {
			setMotorSpeed(motorB, 0);
			setMotorSpeed(motorC, 20);
		} while (getColorName(S3) != colorGreen && getColorName(S3) != colorWhite);
		do {
			setMotorSpeed(motorB, 0);
			setMotorSpeed(motorC, 20);

			curr_deg = getDegrees(S2);
			deg_diff = orig_deg - curr_deg;
			if (deg_diff < 0)
				deg_diff += 360;
			found_neighbor = deg_diff <= 135 + TOLERANCE;
		} while (getColorName(S3) != colorBlack && found_neighbor);

		// If no neighbor to the left
		if (getColorName(S3) != colorBlack) {
			// Correct orientation
			do {
				setMotorSpeed(motorB, -30);
				setMotorSpeed(motorC, 30);
			} while (!isReverse((orig_deg + 270) % 360, getDegrees(S2)));
			do {
				setMotorSpeed(motorB, 40);
				setMotorSpeed(motorC, 50);
				if (getColorName(S3) == colorGreen) {
					setMotorSpeed(motorB, 20);
					setMotorSpeed(motorC, 50);
					wait(0.15, seconds);
				}
			} while (getColorName(S3) != colorGreen);
		}
	} while (!found_neighbor);
}

void victoryDance() {
	displayTextLine(5, "Fire Found");
	for (int i = 0; i < 4; ++i) {
		reverse();
		stopAllMotors();
		setMotorSpeed(motorA, 50 * -1 * (i % 2));
		wait(0.25);
	}
}

bool solveMaze() {
	displayTextLine(1, "Searching for Fire");
	while (1) {
		while (!isColor(S3))
			lineFollow();

		if (getColorName(S3) == colorYellow || (getColorHue(S3) >= 30 && getColorHue(S3) <= 60)) {
			getColorRawRGB(S3, r, g, b);
			writeDebugStreamLine("Yellow: %d %d %d, %d", r, g, b, getColorHue(S3));

			victoryDance();
			reverse();
			return true;
		} else if (getColorName(S3) == colorBlue || (getColorHue(S3) >= 160 && getColorHue(S3) <= 270)) {
			getColorRawRGB(S3, r, g, b);
			writeDebugStreamLine("Blue: %d %d %d, %d", r, g, b, getColorHue(S3));

			return false;
		} else if (getColorName(S3) == colorRed) {
			getColorRawRGB(S3, r, g, b);
			writeDebugStreamLine("Red: %d %d %d, %d", r, g, b, getColorHue(S3));

			// Backtrack
			reverse();
			while (getColorName(S3) != colorGreen)
				lineFollow();
			return false;
			break;
		} else if (getColorName(S3) == colorGreen) {
			// Correct orientation
			do {
				setMotorSpeed(motorB, 20);
				setMotorSpeed(motorC, -20);
			} while (getDegrees(S2) % 45 >= TOLERANCE / 2 && getDegrees(S2) % 45 <= 45 - TOLERANCE / 2);

			// Record current node
			node curr_node;
			curr_node.num = 0;
			curr_node.degree = getDegrees(S2);

			getColorRawRGB(S3, r, g, b);
			writeDebugStreamLine("Green: %d %d %d, %d | %d", r, g, b, getColorHue(S3), curr_node.degree);
			if (IsEmpty(path)) {
				begin_degree = curr_node.degree;
				writeDebugStreamLine("Begin degree: %d", begin_degree);
			}

			// Explore neighbors
			bool has_neighbors_left;
			do {
				// Turn
				turnInNode();

				// Correct orientation
				do {
					setMotorSpeed(motorB, 20);
					setMotorSpeed(motorC, -20);
				} while (getDegrees(S2) % 45 >= TOLERANCE / 2 && getDegrees(S2) % 45 <= 45 - TOLERANCE / 2);

				has_neighbors_left = !isReverse(curr_node.degree, getDegrees(S2));
				if (has_neighbors_left) {
					// Explore neighbor
					curr_node.num++;

					// Display stuff
					string curr_path_string = "   ", temp;
					int i;
					for (i = 1; i <= Tail(path); ++i) {
						StringFormat(temp, "%d ", path.T[i].num);
						strcat(curr_path_string, temp);
					}
					StringFormat(temp, "%d", curr_node.num);
					strcat(curr_path_string, temp);
					writeDebugStreamLine("%s", curr_path_string);
					displayTextLine(3,curr_path_string);

					// Slightly move to right to correct orientation
					setMotorSpeed(motorB, 50);
					setMotorSpeed(motorC, 0);
					sleep(0.5);

					// Recurse
					Add(&path, curr_node);
					if (solveMaze()) return true;
					Del(&path, &curr_node);
				}
			} while (has_neighbors_left);

			// Backtrack
			for (int i = 1; i <= Tail(path); ++i)
				writeDebugStream("%d - ", path.T[i].num);
			writeDebugStreamLine("Back | %d", getDegrees(S2));
			do {
				setMotorSpeed(motorB, 50);
				setMotorSpeed(motorC, 50);
			} while (getColorName(S3) == colorGreen);
			while (!isColor(S3))
				lineFollow();
			return false;
		}
	}
}

void backHome() {
	writeDebugStreamLine("Back Home");
	displayTextLine(1, "Back Home");
	node curr_node;

	do {
		// Display stuff
		string curr_path_string = "  ", temp;
		int i;
		for (i = 1; i <= Tail(path); ++i) {
			StringFormat(temp, "%d ", path.T[i].num);
			strcat(curr_path_string, temp);
		}
		StringFormat(temp, "%d", InfoHead(path).num);
		strcat(curr_path_string, temp);
		writeDebugStreamLine("%s", curr_path_string);
		displayTextLine(3,curr_path_string);

		while (getColorName(S3) != colorGreen && getColorName(S3) != colorBlue)
			lineFollow();

		do {
			setMotorSpeed(motorB, 50);
			setMotorSpeed(motorC, 50);
		} while (getColorName(S3) == colorGreen);
		do {
			setMotorSpeed(motorB, 20);
			setMotorSpeed(motorC, -20);
		} while (!isReverse(InfoHead(path).degree, getDegrees(S2)));
		writeDebugStreamLine("%d", getDegrees(S2));
		do {
			setMotorSpeed(motorB, 50);
			setMotorSpeed(motorC, 50);
		} while (isColor(S3));

		Del(&path, &curr_node);
	} while (!IsEmpty(path));

	do {
		setMotorSpeed(motorB, 20);
		setMotorSpeed(motorC, -20);
	} while (!isReverse(begin_degree, getDegrees(S2)));
	do {
		setMotorSpeed(motorB, 50);
		setMotorSpeed(motorC, 50);
	} while (isColor(S3));
	while (getColorName(S3) != colorGreen && !(getColorName(S3) == colorBlue || (getColorHue(S3) >= 160 && getColorHue(S3) <= 270)))
		lineFollow();
}

task seeHue() {
	while (1) {
		//writeDebugStreamLine("Hue: %d", getColorHue(S3));
		wait(0.2,seconds);
	}
}

task main() {
	clearDebugStream();
	CreateEmpty(&path);
	resetGyro(S2);

	StartTask(seeHue);

	do {
		setMotorSpeed(motorB, 100);
		setMotorSpeed(motorC, 100);
	} while (getColorName(S3) != colorBlue);

	getColorRawRGB(S3, r, g, b);
	writeDebugStreamLine("Blue: %d %d %d", r, g, b);

	do {
		setMotorSpeed(motorB, 100);
		setMotorSpeed(motorC, 100);
	} while (getColorName(S3) != colorBlack);


	if (solveMaze()) {
		stopAllMotors();
		wait(1, seconds);

		backHome();
	}

	switch (getColorName(S3)) {
		case colorGreen:
			writeDebugStreamLine("End: Green"); break;
		case colorYellow:
			writeDebugStreamLine("End: Yellow"); break;
		case colorRed:
			writeDebugStreamLine("End: Red"); break;
		case colorBlue:
			writeDebugStreamLine("End: Blue"); break;
		case colorBlack:
			writeDebugStreamLine("End: Black"); break;
		case colorWhite:
			writeDebugStreamLine("End: White"); break;
		default:
			writeDebugStreamLine("End: ???"); break;
	}
}
