#include "boolean.h"
#include "stackt.h"

#pragma config(Sensor, S1,     touchSensor,    sensorEV3_Touch)
#pragma config(Sensor, S2,     gyroSensor,     sensorEV3_Gyro)
#pragma config(Sensor, S3,     colorSensor,    sensorEV3_Color, modeEV3Color_Color)
#pragma config(Sensor, S4,     sonarSensor,    sensorEV3_Ultrasonic)
#pragma config(Motor,  motorA,          armMotor,      tmotorEV3_Large, PIDControl, encoder)
#pragma config(Motor,  motorB,          leftMotor,     tmotorEV3_Large, PIDControl, driveLeft, encoder)
#pragma config(Motor,  motorC,          rightMotor,    tmotorEV3_Large, PIDControl, driveRight, encoder)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

#define TOLERANCE	15

Stack path;

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

boolean isReverse(int orig, int curr_deg) {
	int diff = curr_deg - orig;
	if (diff < 0)
		diff += 360;
	return (diff >= 180 - TOLERANCE && diff <= 180 + TOLERANCE);
}

boolean isColor(short color_sensor) {
	return (getColorName(color_sensor) == colorBlue ||
			getColorName(color_sensor) == colorGreen ||
			getColorName(color_sensor) == colorRed ||
			getColorName(color_sensor) == colorYellow);
}

void lineFollow() {
	if (getColorName(S3) == colorWhite) {
		setMotorSpeed(motorB, 0);
		setMotorSpeed(motorC, 50);
	} else {
		setMotorSpeed(motorB, 50);
		setMotorSpeed(motorC, 20);
	}
	for (int i = 1; i <= 15; ++i) {
		if (isColor(S3)) {
			setMotorSpeed(motorB, 20);
			setMotorSpeed(motorC, 50);
			wait(10, milliseconds);
		}
	}
}

boolean solveMaze() {
	while (1) {
		while (!isColor(S3))
			lineFollow();

		if (getColorName(S3) == colorYellow) {
			getColorRawRGB(S3, r, g, b);
			writeDebugStreamLine("Yellow: %d %d %d", r, g, b);
			/*writeDebugStreamLine("Yellow");
			setMotorSpeed(motorB,25);
			setMotorSpeed(motorC,0);
			sleep(0.5);*/

			int curr_deg = getDegrees(S2);
			while (!isReverse(curr_deg, getDegrees(S2))) {
				setMotorSpeed(motorB, 50);
				setMotorSpeed(motorC, -50);
			}
			return true;
			//return solveMaze();
		} else if (getColorName(S3) == colorBlue) {
			getColorRawRGB(S3, r, g, b);
			writeDebugStreamLine("Blue: %d %d %d", r, g, b);
			return false;
		} else if (getColorName(S3) == colorRed) {
			getColorRawRGB(S3, r, g, b);
			writeDebugStreamLine("Red: %d %d %d", r, g, b);
			// Backtrack
			int curr_deg = getDegrees(S2);
			while (!isReverse(curr_deg, getDegrees(S2))) {
				setMotorSpeed(motorB, 50);
				setMotorSpeed(motorC, -50);
			}
			while (getColorName(S3) != colorGreen) {
				lineFollow();
			}
			return false;
		} else if (getColorName(S3) == colorGreen) {
			do {
				setMotorSpeed(motorB, 20);
				setMotorSpeed(motorC, -20);
			} while (getDegrees(S2) % 45 >= TOLERANCE / 2 && getDegrees(S2) % 45 <= 45 - TOLERANCE / 2);
			
			// Node
			node curr_node;
			curr_node.num = 0;
			curr_node.degree = getDegrees(S2);

			if (IsEmpty(path)) {
				begin_degree = curr_node.degree;
				writeDebugStreamLine("Begin degree: %d", begin_degree);
			}

			//writeDebugStreamLine("Green: %d", curr_node.degree);
			getColorRawRGB(S3, r, g, b);
			writeDebugStreamLine("Green: %d %d %d | %d", r, g, b, curr_node.degree);

			// Explore neighbors
			bool has_neighbors_left;
			do {
				// Turn
				boolean found_neighbor;
				do {
					int deg1, deg2, ddeg;
					do {
						setMotorSpeed(motorB, 40);
						setMotorSpeed(motorC, 50);
						if (getColorName(S3) != colorGreen) {
							setMotorSpeed(motorB, 20);
							setMotorSpeed(motorC, 50);
							wait(0.2, seconds);
						}
					} while (getColorName(S3) == colorGreen);
					deg1 = getDegrees(S2);
					do {
						setMotorSpeed(motorB, 0);
						setMotorSpeed(motorC, 20);
					} while (getColorName(S3) != colorGreen && getColorName(S3) != colorWhite);
					do {
						setMotorSpeed(motorB, 0);
						setMotorSpeed(motorC, 20);

						deg2 = getDegrees(S2);
						ddeg = deg1 - deg2;
						if (ddeg < 0)
							ddeg += 360;
						found_neighbor = ddeg <= 135 + TOLERANCE;
					} while (getColorName(S3) != colorBlack && found_neighbor);

					if (getColorName(S3) != colorBlack) {
						do {
							setMotorSpeed(motorB, -30);
							setMotorSpeed(motorC, 30);
						} while (!isReverse(deg2 + 45, getDegrees(S2)));
						do {
							setMotorSpeed(motorB, 20);
							setMotorSpeed(motorC, 20);
						} while (getColorName(S3) != colorGreen);
					}
				} while (!found_neighbor);

				do {
					setMotorSpeed(motorB, 20);
					setMotorSpeed(motorC, -20);
				} while (getDegrees(S2) % 45 >= TOLERANCE / 2 && getDegrees(S2) % 45 <= 45 - TOLERANCE / 2);

				has_neighbors_left = !isReverse(curr_node.degree, getDegrees(S2));
				if (has_neighbors_left) {
					/*do {
						setMotorSpeed(motorB, 50);
						setMotorSpeed(motorC, 50);
						if (getColorName(S3) != colorGreen) {
							setMotorSpeed(motorB, 20);
							setMotorSpeed(motorC, 50);
							wait(0.2, seconds);
						}
					} while (getColorName(S3) == colorGreen);
					do {
						setMotorSpeed(motorB, 0);
						setMotorSpeed(motorC, 20);
					} while (getColorName(S3) != colorGreen && getColorName(S3) != colorWhite);
					do {
						setMotorSpeed(motorB, 0);
						setMotorSpeed(motorC, 20);
					} while (getColorName(S3) != colorBlack);*/
					/*do {
						setMotorSpeed(motorB, 30);
						setMotorSpeed(motorC, 50);
					} while (getColorName(S3) != colorWhite);*/
					setMotorSpeed(motorB, 50);
					setMotorSpeed(motorC, 0);
					sleep(0.5);


					// Explore neighbor
					curr_node.num++;

					for (int i = 1; i <= Top(path); ++i)
						writeDebugStream("%d - ", path.T[i].num);
					writeDebugStreamLine("%d", curr_node.num);

					Push(&path, curr_node);
					if (solveMaze()) return true;

					Pop(&path, &curr_node);
				}
			} while (has_neighbors_left);

			// Backtrack
			for (int i = 1; i <= Top(path); ++i)
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
	node curr_node;

	do {
		for (int i = 1; i < Top(path); ++i)
			writeDebugStream("%d - ", path.T[i].num);
		writeDebugStreamLine("%d", InfoTop(path).num);

		while (getColorName(S3) != colorGreen && getColorName(S3) != colorBlue)
			lineFollow();

		do {
			setMotorSpeed(motorB, 50);
			setMotorSpeed(motorC, 50);
		} while (getColorName(S3) == colorGreen);
		do {
			setMotorSpeed(motorB, 20);
			setMotorSpeed(motorC, -20);
		} while (!isReverse(InfoTop(path).degree, getDegrees(S2)));
		writeDebugStreamLine("%d", getDegrees(S2));
		do {
			setMotorSpeed(motorB, 50);
			setMotorSpeed(motorC, 50);
		} while (isColor(S3));

		Pop(&path, &curr_node);
	} while (!IsEmpty(path));

	do {
		setMotorSpeed(motorB, 20);
		setMotorSpeed(motorC, -20);
	} while (!isReverse(begin_degree, getDegrees(S2)));
	do {
		setMotorSpeed(motorB, 50);
		setMotorSpeed(motorC, 50);
	} while (isColor(S3));
	while (getColorName(S3) != colorGreen && getColorName(S3) != colorBlue)
		lineFollow();
}


task main() {
	clearDebugStream();
	CreateEmpty(&path);
	resetGyro(S2);

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
