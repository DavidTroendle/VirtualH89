/// \file SerialPortDevice.cpp
///
/// \date Apr 23, 2009
/// \author Mark Garlanger
///

#include "SerialPortDevice.h"

#include "INS8250.h"
#include "logger.h"


SerialPortDevice::SerialPortDevice(): port_m(0)
{

}

SerialPortDevice::~SerialPortDevice()
{

}

void
SerialPortDevice::attachPort(INS8250* port)
{
    port_m = port;
}

bool
SerialPortDevice::sendReady()
{
    return port_m != nullptr && port_m->receiveReady();
}

bool
SerialPortDevice::sendData(BYTE data)
{
    if (port_m)
    {
        port_m->receiveData(data);
        return true;
    }

    debugss(ssSerial, WARNING, "port_m is NULL\n");

    return false;
}
