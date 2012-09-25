////////////////////////////////////////////////////////////////////////////////
///@file    partition.h
///@short   Partition class for file systems on block devices
///@author  J.Nider
///@date    01-Jun-2007 01:30:19 PM
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

#ifndef _PARTITION__H
#define _PARTITION__H

#include <fs.h>

class IBlockDevice;

////////////////////////////////////////////////////////////////////////////////
///@date    01-Jun-2007 01:31:14 PM
///@brief   Partition class logically divides block devices into multiple regions
////////////////////////////////////////////////////////////////////////////////
class KPartition
{
public:
   static KPartition* New(IBlockDevice& device);
   bool Mount(const TFileSystemId& id);
   bool IsMounted() { return m_mounted; } ///< Has the file system on this partition been mounted
   const char* GetFirstDirEntry();
   const char* GetNextDirEntry();
   bool Read(unsigned int sector, unsigned int length, char* pBuffer);
   bool Write(unsigned int sector, unsigned int length, char* pBuffer);

private:
   KPartition(IBlockDevice& device);
   bool Init();

private:
   IBlockDevice* m_pDevice;   ///< physical device on which this partition resides
   bool m_mounted;            ///< has the file system been mounted on this partition
   KFileSystem* m_pFS;
};

#endif // _PARTITION__H
