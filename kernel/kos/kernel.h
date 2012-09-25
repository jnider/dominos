////////////////////////////////////////////////////////////////////////////////
///@file    kernel.h
///@short   Kernel class
///@author  J.Nider
///@date    01-Jun-2007 12:29:01 PM
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

#ifndef _KERNEL__H
#define _KERNEL__H

//#include "bus/floppy/floppybus.h"
//#include "bus/pci/pcibus.h"
#include "istoragedevice.h"
#include "ibus.h"    // IBusObserver
#include "klist.h"

extern KPartition* g_pCurrentDevice;

/////////
///@author  J.Nider
///@date    01-Jun-2007 12:29:16 PM
///@brief   singleton class representing the kernel
/////////
class Kernel : public IBusObserver
{
public:
   Kernel();
   virtual ~Kernel() {};
   bool Init();

protected:
   // from IBusObserver
   void OnStorageDeviceFound(IStorageDevice& device);

private:
   //KFloppyBus* m_pFloppyBus;           ///< bus containing floppy controllers
   //KPCIBus* m_pPCIBus;                 ///< general usage PCI bus
   KPtrList<KPartition*> m_driveList;  ///< list of all discovered devices
};

#endif // _KERNEL__H
