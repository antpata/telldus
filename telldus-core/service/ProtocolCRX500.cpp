// Sensor data parser for device with trade mark CRX500
// Seen also from other devices but actual chip maker is not identified.
//

#include "service/ProtocolCRX500.h"
#include <stdlib.h>
#include <iomanip>
#include <sstream>
#include <string>
#include "common/Strings.h"

std::string ProtocolCRX500::decodeData(const ControllerMessage &dataMsg) {
	std::string data = dataMsg.getParameter("data");
	if (data.length() < 8) {
		return "";
	}

    //Example data: 0BEA0ADF43
    // Humidity and temperature
    //    0-2     device constant bits
    //    3-7     Random boot code
    //    8       1 == batt OK
    //    9-11    CH. CH1=000 CH2=001 CH3=010
    //    12-23   temp in 1/10 C
    //    24-27   1111 signature
    //    28-35   Huminity %
    // Temperature only
    //    0-2     device constant bits.
    //    3-7     Random boot code
    //    8       1 == batt OK
    //    9-11    CH4=011
    //    12-15   1111 signature
    //    16-31   temp in 1/10 C
    //    32-35   ??? 0001 battery indication
    uint16_t id = (uint16_t)TelldusCore::hexTo64l(data.substr(0, 4));
    uint8_t channel = id&7;
    uint8_t dev_id = (id&0xE00)>>9;
    uint8_t rnd_id = (id&0x1F0)>>3;
    uint8_t batt_ok = (id&0x8)>>3;
    int16_t value = 0;
    std::stringstream retString;
    retString << "class:sensor;protocol:CRX500;id:" << static_cast<int>(rnd_id) << static_cast<int>(dev_id) << static_cast<int>(channel+1) << ";model:";

    if(channel == 3) {
        if (data[4] != 'F')
            return ""; // not our message

        value = (uint16_t)TelldusCore::hexTo64l(data.substr(5, 4));
        retString << "temperature;";

    } else {
        if (data[7] != 'F')
            return ""; // not out message

        uint8_t humidity = (uint8_t)TelldusCore::hexTo64l(data.substr(data.length()-2));
        // data validation
        // to block messafes where all bits are 1 and zero humidity
        if ((humidity == 0) || (humidity == 0xff))
            return "";

        if (humidity > 100)
            humidity = 100; //for buggy sensor otherwice we should reject the message

        value = (uint16_t)TelldusCore::hexTo64l(data.substr(4, 3));
        retString << "temperaturehumidity;humidity:" << static_cast<int>(humidity) << ";";
    }
    // extent signd to 16 bis
    if (value & 0x800) {
        value |= 0xF800; //sign extension
    }
    double temperature = value/10.0;

    if ( (temperature > 140.0) || (temperature < -45.0))
        return "";

    retString << "temp:" << std::fixed << std::setprecision(1) << temperature << ";";

	return retString.str();
}
