/// \file DiskController.h
///
/// \date Feb 8, 2016
/// \author Douglas Miller
///

#ifndef DISKCONTROLLER_H_
#define DISKCONTROLLER_H_

#include "h89Types.h"
#include "IODevice.h"
#include "GenericDiskDrive.h"
#include <vector>
#include <string>

/// \todo - determine if interrupt level for the device should be here, or if we subclass
///         this to a IOIntrDevice.
///        I'm thinking subclass it with InterruptDevice.

///
/// \brief  Abstract Disk Controller I/O device .
///
/// Base class for all Disk Controller I/O devices.
///
class DiskController : public IODevice
{
  public:
    ///
    ///   \param base Base address for the I/O device
    ///   \param numPorts The number of addresses used by the device.
    ///
    DiskController(BYTE base, BYTE numPorts);
    virtual ~DiskController();

    // Return list of all connected disk drives.
    virtual std::vector<GenericDiskDrive *> getDiskDrives() = 0;

    // Return the nmemonic identifier for this device.
    // For example "H17" or "MMS77316".
    virtual std::string getDeviceName() = 0;

    // Return the disk drive associated with identifer,
    // in the form of <deviceName>"-"<driveNumber>.
    // Drive numbers start at "1".
    virtual GenericDiskDrive *findDrive(std::string ident) = 0;

    // Return the "standard" name for disk drive 'index' (0-n).
    // This is the same name as will match in findDrive().
    // Default if not overriden is getDeviceName()+"-"+<index+1>.
    virtual std::string getDriveName(int index);

  protected:

  private:
    /// Hide default constructor.
    DiskController();

};

#endif // DISKCONTROLLER_H_
