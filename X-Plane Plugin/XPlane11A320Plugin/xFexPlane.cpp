#include "AsyncSerial.h"
#include "AsyncSerialCallback.h"
#include "XPLMDataAccess.h"
#include "XPLMDisplay.h"
#include "XPLMGraphics.h"
#include "XPLMProcessing.h"
#include "XPLMUtilities.h"
#include <string.h>
#include <string>
#include <vector>
#include <boost/algorithm/string.hpp>
#include "FexPlaneSerialProtocol.h"


static AsyncSerialCallback serial;


static XPLMWindowID gWindow = NULL;


static float gLastAltitude = 0.0f;
static float gLastHeading = 0.0f;
static float gLastAirspeed = 0.0f;
static float gLastVerticalSpeed = 0.0f;
static int gLastChange = 0;

static EncoderState gEncoderState = EncoderState::HEADING;


static XPLMDataRef gRefAltidude = NULL;
static XPLMDataRef gRefHeading = NULL;
static XPLMDataRef gRefAirspeed = NULL;
static XPLMDataRef gRefVerticalSpeed = NULL;


static void MyDrawWindowCallback(
	XPLMWindowID inWindowID,
	void* inRefcon);


static void MyHandleKeyCallback(
	XPLMWindowID inWindowID,
	char inKey,
	XPLMKeyFlags inFlags,
	char inVirtualKey,
	void* inRefcon,
	int losingFocus);


static int MyHandleMouseClickCallback(
	XPLMWindowID inWindowID,
	int x,
	int y,
	XPLMMouseStatus inMouse,
	void* inRefcon);


static float MyFlightLoopCallback(
	float inElapsedSinceLastCall,
	float inElapsedTimeSinceLastFlightLoop,
	int inCounter,
	void* inRefcon);


static void CBSerialReceived(const char* data, unsigned int len);


static void SetEncoderState(EncoderState state);


static void SendState();


static void UpdateStateValue(int change);

/*
* XPluginStart
*
* Our start routine registers our window and does any other initialization we
* must do.
*
*/
PLUGIN_API int XPluginStart(
	char* outName,
	char* outSig,
	char* outDesc)
{
	/* First we must fill in the passed in buffers to describe our
	* plugin to the plugin-system. */

	strcpy(outName, "HelloWorld");
	strcpy(outSig, "xplanesdk.examples.helloworld");
	strcpy(outDesc, "A plugin that makes a window.");

	/* Now we create a window.  We pass in a rectangle in left, top,
	* right, bottom screen coordinates.  We pass in three callbacks. */

	gWindow = XPLMCreateWindow(
		50, 600, 300, 200, /* Area of the window. */
		1, /* Start visible. */
		MyDrawWindowCallback, /* Callbacks */
		MyHandleKeyCallback,
		MyHandleMouseClickCallback,
		NULL); /* Refcon - not used. */

	/* We must return 1 to indicate successful initialization, otherwise we
	* will not be called back again. */

	//init datarefs
	gRefAltidude = XPLMFindDataRef("sim/cockpit2/autopilot/altitude_dial_ft");
	gRefAirspeed = XPLMFindDataRef("sim/cockpit2/autopilot/airspeed_dial_kts_mach");
	gRefHeading = XPLMFindDataRef("sim/cockpit2/autopilot/heading_dial_deg_mag_pilot");
	gRefVerticalSpeed = XPLMFindDataRef("sim/cockpit2/autopilot/vvi_dial_fpm");

	//init internal data
	gLastAltitude = XPLMGetDataf(gRefAltidude);
	gLastAirspeed = XPLMGetDataf(gRefAirspeed);
	gLastHeading = XPLMGetDataf(gRefHeading);
	gLastVerticalSpeed = XPLMGetDataf(gRefVerticalSpeed);

	/* Register our callback for once a second.  Positive intervals
	* are in seconds, negative are the negative of sim frames.  Zero
	* registers but does not schedule a callback for time. */
	XPLMRegisterFlightLoopCallback(MyFlightLoopCallback, 0.1, NULL);


	return 1;
}

/*
* XPluginStop
*
* Our cleanup routine deallocates our window.
*
*/
PLUGIN_API void XPluginStop(void)
{
	XPLMDestroyWindow(gWindow);
	/* Unregister the callback */
	XPLMUnregisterFlightLoopCallback(MyFlightLoopCallback, NULL);
}

/*
* XPluginDisable
*
* We do not need to do anything when we are disabled, but we must provide the handler.
*
*/
PLUGIN_API void XPluginDisable(void)
{
	serial.close();
}

/*
* XPluginEnable.
*
* We don't do any enable-specific initialization, but we must return 1 to indicate
* that we may be enabled at this time.
*
*/
PLUGIN_API int XPluginEnable(void)
{
	int ret = 1;
	int baud = 9600;
	try
	{
		if (!serial.isOpen())
		{
			serial.clearCallback();
			serial.open("COM4", baud);
			serial.setCallback(CBSerialReceived);
		}
	}
	catch (std::exception& e)
	{
		XPLMDebugString("AsyncSerialCallback - Unable to Initialize\n");
		ret = -1;
	}
	return ret;
}

/*
* XPluginReceiveMessage
*
* We don't have to do anything in our receive message handler, but we must provide one.
*
*/
PLUGIN_API void XPluginReceiveMessage(
	XPLMPluginID inFromWho,
	int inMessage,
	void* inParam)
{
}

/*
* MyDrawingWindowCallback
*
* This callback does the work of drawing our window once per sim cycle each time
* it is needed.  It dynamically changes the text depending on the saved mouse
* status.  Note that we don't have to tell X-Plane to redraw us when our text
* changes; we are redrawn by the sim continuously.
*
*/
void MyDrawWindowCallback(
	XPLMWindowID inWindowID,
	void* inRefcon)
{
	int left, top, right, bottom;
	float color[] = {1.0, 1.0, 1.0}; /* RGB White */

	/* First we get the location of the window passed in to us. */
	XPLMGetWindowGeometry(inWindowID, &left, &top, &right, &bottom);

	/* We now use an XPLMGraphics routine to draw a translucent dark
	* rectangle that is our window's shape. */
	XPLMDrawTranslucentDarkBox(left, top, right, bottom);

	std::string sSpd = "SPD: " + std::to_string(gLastAirspeed);
	std::vector<char> writable(sSpd.begin(), sSpd.end());
	writable.push_back('\0');
	XPLMDrawString(color, left + 5, top - 20, &writable[0], NULL, xplmFont_Basic);

	std::string sHdg = "HDG: " + std::to_string(gLastHeading);
	std::vector<char> writable2(sHdg.begin(), sHdg.end());
	writable2.push_back('\0');
	XPLMDrawString(color, left + 5, top - 40, &writable2[0], NULL, xplmFont_Basic);

	std::string sAlt = "ALT: " + std::to_string(gLastAltitude);
	std::vector<char> writable3(sAlt.begin(), sAlt.end());
	writable3.push_back('\0');
	XPLMDrawString(color, left + 5, top - 60, &writable3[0], NULL, xplmFont_Basic);

	std::string sVs = "VS: " + std::to_string(gLastVerticalSpeed);
	std::vector<char> writable4(sVs.begin(), sVs.end());
	writable4.push_back('\0');
	XPLMDrawString(color, left + 5, top - 80, &writable4[0], NULL, xplmFont_Basic);
}

/*
* MyHandleKeyCallback
*
* Our key handling callback does nothing in this plugin.  This is ok;
* we simply don't use keyboard input.
*
*/
void MyHandleKeyCallback(
	XPLMWindowID inWindowID,
	char inKey,
	XPLMKeyFlags inFlags,
	char inVirtualKey,
	void* inRefcon,
	int losingFocus)
{
}

/*
* MyHandleMouseClickCallback
*
* Our mouse click callback toggles the status of our mouse variable
* as the mouse is clicked.  We then update our text on the next sim
* cycle.
*
*/
int MyHandleMouseClickCallback(
	XPLMWindowID inWindowID,
	int x,
	int y,
	XPLMMouseStatus inMouse,
	void* inRefcon)
{
	/* Returning 1 tells X-Plane that we 'accepted' the click; otherwise
	* it would be passed to the next window behind us.  If we accept
	* the click we get mouse moved and mouse up callbacks, if we don't
	* we do not get any more callbacks.  It is worth noting that we
	* will receive mouse moved and mouse up even if the mouse is dragged
	* out of our window's box as long as the click started in our window's
	* box. */
	return 0;
}

float MyFlightLoopCallback(
	float inElapsedSinceLastCall,
	float inElapsedTimeSinceLastFlightLoop,
	int inCounter,
	void* inRefcon)
{
	if (gLastChange != 0);
	{
		UpdateStateValue(gLastChange);
		gLastChange = 0;
	}

	//read data
	float newAltitude = XPLMGetDataf(gRefAltidude);
	float newAirspeed = XPLMGetDataf(gRefAirspeed);
	float newHeading = XPLMGetDataf(gRefHeading);
	float newVerticalSpeed = XPLMGetDataf(gRefVerticalSpeed);

	if (newAirspeed != gLastAirspeed)
	{
		gLastAirspeed = newAirspeed;
		if (gEncoderState == EncoderState::AIRSPEED)
		{
			SendState();
		}
	}
	if (newHeading != gLastHeading)
	{
		gLastHeading = newHeading;
		if (gEncoderState == EncoderState::HEADING)
		{
			SendState();
		}
	}
	if (newAltitude != gLastAltitude)
	{
		gLastAltitude = newAltitude;
		if (gEncoderState == EncoderState::ALTITUDE)
		{
			SendState();
		}
	}
	if (newVerticalSpeed != gLastVerticalSpeed)
	{
		gLastVerticalSpeed = newVerticalSpeed;
		if (gEncoderState == EncoderState::VERTICALSPEED)
		{
			SendState();
		}
	}
	
	/* Return 1.0 to indicate that we want to be called again in 1 second. */
	return 0.1;
}

void CBSerialReceived(const char* data, unsigned int len)
{
	std::string msg(data, len);
	std::vector<std::string> strs;
	boost::split(strs, msg, boost::is_any_of("\n\r"));
	for (std::string token : strs)
	{
		if (token.length() > 1)
		{
			if (token[0] == 'M')
			{
				if (token[1] == 'A')
				{
					SetEncoderState(EncoderState::ALTITUDE);
				}
				else if (token[1] == 'S')
				{
					SetEncoderState(EncoderState::AIRSPEED);
				}
				else if (token[1] == 'V')
				{
					SetEncoderState(EncoderState::VERTICALSPEED);
				}
				else if (token[1] == 'H')
				{
					SetEncoderState(EncoderState::HEADING);
				}
			}
			else if (token[0] == 'E')
			{
				try
				{
					gLastChange += std::stoi(token.substr(1, token.size() - 1));
					
				}
				catch (...)
				{
				}
			}
			
		}
	}
}

void SetEncoderState(EncoderState state)
{
	gEncoderState = state;
	SendState();
}

void SendState()
{
	switch (gEncoderState)
	{
	case EncoderState::HEADING:
		if (serial.isOpen() && !serial.errorStatus())
		{
			HeadingUpdateCMD cmd;
			cmd.fromFloat(gLastHeading);
			serial.writeString(cmd.toString());
		}
		break;
	case EncoderState::AIRSPEED:
		if (serial.isOpen() && !serial.errorStatus())
		{
			AirspeedUpdateCMD cmd;
			cmd.fromFloat(gLastAirspeed);
			serial.writeString(cmd.toString());
		}
		break;
	case EncoderState::ALTITUDE:
		if (serial.isOpen() && !serial.errorStatus())
		{
			AltitudeUpdateCMD cmd;
			cmd.fromFloat(gLastAltitude);
			serial.writeString(cmd.toString());
		}
		break;
	case EncoderState::VERTICALSPEED:
		if (serial.isOpen() && !serial.errorStatus())
		{
			VerticalSpeedUpdateCMD cmd;
			cmd.fromFloat(gLastVerticalSpeed);
			serial.writeString(cmd.toString());
		}
		break;
	default:
		break;
	}
}


void UpdateStateValue(int change)
{
	switch (gEncoderState)
	{
	case EncoderState::HEADING:
		XPLMSetDataf(gRefHeading, gLastHeading + change);
		break;
	case EncoderState::AIRSPEED:
		XPLMSetDataf(gRefAirspeed, gLastAirspeed + change);
		break;
	case EncoderState::ALTITUDE:
		XPLMSetDataf(gRefAltidude, gLastAltitude + change*10);
		break;
	case EncoderState::VERTICALSPEED:
		XPLMSetDataf(gRefVerticalSpeed, gLastVerticalSpeed + change*100);
		break;
	default: break;
	}
}
