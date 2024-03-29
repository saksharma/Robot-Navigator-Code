//Final Code
#include "PC_FileIO.c" //custom library for file IO

//Function Prototypes
void turnLeft();
void turnRight();
bool obstaclePresent (tSensors ultrasonic);
void detectColour(tSensors port);
bool ultrasonicSensor(tSensors port, tSensors ultrasonic, int distance, int lineNum);
void turnGear (char direction, int lineNum);
bool touchSensor(int linenum, tSensors port);
void turnBack(int lineNum);

void turnLeft()
{
	//Sets the sound volume of the EV3 speaker to 75
	setSoundVolume(100);

	// Starts playing a soundfile on the EV3
	playSoundFile("Left");

	// Gives the file 2 seconds to play
	sleep(2000);
	resetGyro(S2);
	motor[motorA] = POWER;
	motor[motorD] = -POWER;
	while(getGyroDegrees(S2) > -90)
	{}
	motor[motorA] = motor[motorD] = 0;

}

void turnRight()
{
	//Sets the sound volume of the EV3 speaker to 75
	setSoundVolume(100);

	// Starts playing a soundfile on the EV3
	playSoundFile("Right");

	// Gives the file 2 seconds to play
	sleep(2000);
	resetGyro(S2);
	motor[motorA] = -POWER;
	motor[motorD] = POWER;
	while(getGyroDegrees(S2) < 90)
	{}
	motor[motorA] = motor[motorD] = 0;
}

bool obstaclePresent (tSensors ultrasonic)
{
	time1[T1] = 0;
	while (time1[T1] < 2000 && SensorValue[ultrasonic] < 50)
	{}

	if (time1[T1] >= 2000 && SensorValue[ultrasonic] < 50)
	{
		//Sets the sound volume of the EV3 speaker to 75
		setSoundVolume(100);

		// Starts playing a soundfile on the EV3
		playSoundFile("Stop");

		// Gives the file 2 seconds to play
		sleep(2000);
		return true; //there was an obstacle
	}
	return false;
}

void detectColour(tSensors port)
{
	while (SensorValue[S3] == (int)colorRed)
	{
		motor[motorA] = motor[motorD] = 0;
		wait1Msec(3000);
	}
	//Sets the sound volume of the EV3 speaker to 75
	setSoundVolume(100);

	// Starts playing a soundfile on the EV3
	playSoundFile("Forward");

	// Gives the file 2 seconds to play
	sleep(2000);
	motor[motorA] = motor[motorD] = 25;
}


bool ultrasonicSensor(tSensors port, tSensors ultrasonic, int distance, int lineNum)
{
	nMotorEncoder[motorD]=0;

	while(nMotorEncoder[motorD] < distance)
	{
		if (SensorValue[ultrasonic] < 50)
		{
			while(SensorValue[ultrasonic] < 50 && nMotorEncoder[motorD] < distance)
			{
				while(obstaclePresent(ultrasonic))
				{
					if (SensorValue[port]==(int)colorRed)
					{
						detectColour(port);
					}
					else
					{
						turnBack(lineNum);
						return false;
					}
				}
			}
		}
		else
			motor[motorA] = motor[motorD] = 25;
	}
	return true;
}

//This function turns the ultrasonic motor
void turnGear (char direction, int lineNum)
{
	int speed = 0;
	nMotorEncoder[motorB] = 0;

	if (direction == 'L')
	{
		speed = -25;
		motor[motorB] = speed;
	}

	else
	{
		speed = 25;
		motor[motorB] = speed;
	}

	while (abs(nMotorEncoder[motorB]) < 90)
	{}

	motor[motorB] = 0;

	wait1Msec(3000); //just some buffer time for a potential obstacle to appear

	bool yesObstacle = obstaclePresent(S4);

	if (yesObstacle)
	{
		motor[motorB] = -speed;
		while (fabs(nMotorEncoder[motorB]) < 90)
		{}
		motor[motorB] = 0;
		turnBack(lineNum);
	}

	else
	{
		nMotorEncoder[motorB] = 0;

		motor[motorB] = -speed;
		while (fabs(nMotorEncoder[motorB]) < 90)
		{}
		motor[motorB] = 0;

		if (direction == 'L')
		{
			turnLeft();
		}
		else
		{
			turnRight();
		}
	}
}

bool touchSensor(int linenum, tSensors port)
{
	if(SensorValue[port]==1)
	{
		turnBack(linenum);
		return false;
	}
	return true;
}

void turnBack(int lineNum)
{
	motor[motorA] = motor[motorD] = 0;
	long	myMotorEncoder = nMotorEncoder[motorA];
	int line = lineNum;
	TFileHandle directions;
	bool fileOkay = openReadPC(directions, "directions.txt");

	//Sets the sound volume of the EV3 speaker to 75
	setSoundVolume(100);

	// Starts playing a soundfile on the EV3
	playSoundFile("Backwards");

	// Gives the file 2 seconds to play
	sleep(2000);
	if (fileOkay)
	{
		//set the array assuming no more than 15 directions
		int distArray[15] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
		char directArray[15] = {'a','a','a','a','a','a','a','a','a','a','a','a','a','a','a'};

		for (int count = 0; count < line; count ++)
		{
			char direct;
			int distance = 1;
			readIntPC(directions, distance);
			readCharPC(directions, direct);

			distArray[count] = distance;
			directArray[count] = direct;
		}

		//turns around 180Deg
		resetGyro(S2);
		motor[motorA] = -25;
		motor[motorD] = 25;
		while(getGyroDegrees(S2) < 180)
		{}
		motor[motorA] = motor[motorD] = 0;

		nMotorEncoder[motorA] = 0;
		motor[motorA] = motor[motorD] = 25;
		while (nMotorEncoder[motorA] < myMotorEncoder)
		{}
		motor[motorA] = motor[motorD] = 0;

		for (int count = (line - 1); count >= 0; count --)
		{
			if (directArray[count] == 'L')
			{
				turnRight(); //ie turn in the opposite direction
			}
			else if (directArray[count] == 'R')
			{
				turnLeft();
			}

			motor[motorA] = motor[motorD] =0;
			int encLimit = (distArray[count] * 360 / (2 * PI * 4.00));
			nMotorEncoder[motorA] = 0;
			motor[motorA] = motor[motorD] = 25;
			while(nMotorEncoder[motorA] < encLimit)
			{}

			motor[motorA] = motor[motorD] = 0;
			wait1Msec(3000);
		}
	}
	//Sets the sound volume of the EV3 speaker to 75
	setSoundVolume(100);

	// Starts playing a soundfile on the EV3
	playSoundFile("T-rex roar");

	// Gives the file 2 seconds to play
	sleep(2000);
}


task main()
{
	const int POWER = 25;

	//Configure Sensors
	SensorType[S3] = sensorEV3_Color;
	wait1Msec(50);
	SensorMode[S3] = modeEV3Color_Color;
	wait1Msec(50);
	SensorType[S2] = sensorEV3_Gyro;
	wait1Msec(50);
	SensorMode[S2] = modeEV3Gyro_RateAndAngle;
	wait1Msec(50);
	SensorType[S4] = sensorEV3_Ultrasonic;
	SensorType[S1] = sensorEV3_Touch;

	TFileHandle directions;
	bool fileOkay = openReadPC(directions, "directions.txt");
	int	distance = 0, lineNum = 0;
	bool continueRead = true; //make the turnback function a bool
	//Sets the sound volume of the EV3 speaker to 75
	setSoundVolume(100);

	// Starts playing a soundfile on the EV3
	playSoundFile("Start");

	// Gives the file 2 seconds to play
	sleep(2000);
	while(readIntPC(directions, distance) && continueRead)
	{
		char turn;
		int ENC_LIMIT = 0;
		ENC_LIMIT = (distance * 180 / ( PI * 4.00))/2;
		nMotorEncoder[motorA] = 0;
		motor[motorA] = POWER;
		motor[motorD] = POWER;

		while(nMotorEncoder[motorA] < ENC_LIMIT && SensorValue[S1] != 1 && SensorValue[S4] > 50)
		{}

		if (nMotorEncoder[motorA] > ENC_LIMIT || SensorValue[S1] == 1)
		{
			motor[motorA] = motor[motorD] = 0;
			continueRead = touchSensor(lineNum, S1);
		}
		else
		{
			continueRead = ultrasonicSensor(S3, S4, ENC_LIMIT, lineNum);
		}

		if (continueRead)
		{
			motor[motorA] = motor[motorD] = 0;

			readCharPC(directions, turn);

			turnGear(turn, lineNum);
			lineNum++;
		}
	}

	motor[motorA] = motor[motorD] = 0;
	//Sets the sound volume of the EV3 speaker to 75
	setSoundVolume(100);

	// Starts playing a soundfile on the EV3
	playSoundFile("Fanfare");

	// Gives the file 2 seconds to play
	sleep(2000);
}
