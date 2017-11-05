#pragma once
#include <sstream>
#include <iomanip>
#include <cmath>
#include <algorithm>

enum class EncoderState
{
	HEADING,
	AIRSPEED,
	ALTITUDE,
	VERTICALSPEED
};


//########################################################################################
//AltitudeUpdateCMD
//########################################################################################
class AltitudeUpdateCMD
{
public:
	AltitudeUpdateCMD();
	static char header;
	static const int DATALENGTH = 6;
	char data[DATALENGTH];
	void fromFloat(float value);
	std::string toString();
};

char AltitudeUpdateCMD::header = 'A';

inline AltitudeUpdateCMD::AltitudeUpdateCMD()
{
	for (int i = 0; i < DATALENGTH; i++)
	{
		data[i] = '0';
	}
}

inline void AltitudeUpdateCMD::fromFloat(float value)
{
	std::stringstream stream;
	stream << std::setw(DATALENGTH) << std::setfill(' ') << std::right << std::fixed << std::setprecision(0) << value;
	std::string s = stream.str();
	for (int i = 0; i < DATALENGTH; i++)
	{
		data[i] = s[i];
	}
}

inline std::string AltitudeUpdateCMD::toString()
{
	std::string s;
	s += header;
	s.append(data, DATALENGTH);
	s += '\n';
	return s;
}

//########################################################################################
//HeadingUpdateCMD
//########################################################################################
class HeadingUpdateCMD
{
public:
	HeadingUpdateCMD();
	static char header;
	static const int DATALENGTH = 3;
	char data[DATALENGTH];
	void fromFloat(float value);
	std::string toString();

};

char HeadingUpdateCMD::header = 'H';

inline HeadingUpdateCMD::HeadingUpdateCMD()
{
	for (int i = 0; i < DATALENGTH; i++)
	{
		data[i] = '0';
	}
}

inline void HeadingUpdateCMD::fromFloat(float value)
{
	std::stringstream stream;
	stream << std::setw(DATALENGTH) << std::setfill('0') << std::right << std::fixed << std::setprecision(0) << value;
	std::string s = stream.str();
	for (int i = 0; i < DATALENGTH; i++)
	{
		data[i] = s[i];
	}
}

inline std::string HeadingUpdateCMD::toString()
{
	std::string s;
	s += header;
	s.append(data, DATALENGTH);
	s += '\n';
	return s;
}

//########################################################################################
//VerticalSpeedUpdateCMD
//########################################################################################
class VerticalSpeedUpdateCMD
{
public:
	VerticalSpeedUpdateCMD();
	static char header;
	static const int DATALENGTH = 5;
	char data[DATALENGTH];
	void fromFloat(float value);
	std::string toString();
};

char VerticalSpeedUpdateCMD::header = 'V';

inline VerticalSpeedUpdateCMD::VerticalSpeedUpdateCMD()
{
	for (int i = 0; i < DATALENGTH; i++)
	{
		data[i] = '0';
	}
}

inline void VerticalSpeedUpdateCMD::fromFloat(float value)
{
	std::stringstream stream;
	stream << std::setw(DATALENGTH) << std::setfill(' ') << std::right << std::fixed << std::setprecision(0) << value;
	std::string s = stream.str();
	for (int i = 0; i < DATALENGTH; i++)
	{
		data[i] = s[i];
	}
}

inline std::string VerticalSpeedUpdateCMD::toString()
{
	std::string s;
	s += header;
	s.append(data, DATALENGTH);
	s += '\n';
	return s;
}

//########################################################################################
//AirspeedUpdateCMD
//########################################################################################
class AirspeedUpdateCMD
{
public:
	AirspeedUpdateCMD();
	static char header;
	static const int DATALENGTH = 6;
	char data[DATALENGTH];
	void fromFloat(float value);
	std::string toString();
};

char AirspeedUpdateCMD::header = 'S';

inline AirspeedUpdateCMD::AirspeedUpdateCMD()
{
	for (int i = 0; i < DATALENGTH; i++)
	{
		data[i] = '0';
	}
}

inline void AirspeedUpdateCMD::fromFloat(float value)
{
	std::stringstream stream;
	stream << std::setw(DATALENGTH) << std::setfill(' ') << std::right << std::fixed << std::setprecision(2) << value;
	std::string s = stream.str();
	for (int i = 0; i < DATALENGTH; i++)
	{
		data[i] = s[i];
	}
}

inline std::string AirspeedUpdateCMD::toString()
{
	std::string s;
	s += header;
	s.append(data, DATALENGTH);
	s += '\n';
	return s;
}