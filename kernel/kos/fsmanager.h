////////////////////////////////////////////////////////////////////////////////
/// @file   fsmanager.h
/// @short  file system manager class
/// @author J.Nider
/// @date   30/10/2009
////////////////////////////////////////////////////////////////////////////////
/// Allows registration and initialization of a file system.  File systems can
/// be added at run-time to mount new types without having to install or reboot.
////////////////////////////////////////////////////////////////////////////////

#ifndef _FSMANAGER__H
#define _FSMANAGER__H

#include "fs.h"
#include "klist.h"
#include "partition.h"

class KFileSystemManager;

struct fs_node_t
{
   TFileSystemId id;
   FnFSCreator* pFnCreator;
};

////
///@class  KFileSystemManager
///@author J.Nider
///@date   03/02/2007
///@brief  Manages the file systems and creates a new instance when required
////
class KFileSystemManager
{
public:
   static KFileSystemManager* Instance();
   KFileSystem* Create(TFileSystemId id, KPartition& device); ///< creates the appropriate file system based on requested id for the specified device
   TFileSystemId Identify(char* pBuffer);  ///< identifies the file system based on buffer contents
   bool Register(TFileSystemId id, FnFSCreator* pFnCreator); ///< registers a function to identify a filesystem from a buffer
   bool Unregister(TFileSystemId id); ///< unregisters a filesystem

private:
   KFileSystemManager();

protected:
   KPtrList<fs_node_t> m_list;
   static KFileSystemManager* m_pInstance;
};

#endif // _FSMANAGER__H
