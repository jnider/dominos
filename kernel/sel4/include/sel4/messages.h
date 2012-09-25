/*
 * Copyright (C) 2010 National ICT Australia Limited (NICTA)
 *
 * For non-commercial use only. For license terms, see the file
 * LICENSE-OKL-NICTA.pdf in the top-level directory.
 */

#ifndef __SEL4_MESSAGES_H__
#define __SEL4_MESSAGES_H__

#include <sel4/types.h>
#include <sel4/functions.h>

/*************
 * Accessing fields from the in the tag 
 */
static inline seL4_Word seL4_Get_MSGLabel(void)
{
  return seL4_GetIPCBuffer() -> tag.label;
}

static inline seL4_Word seL4_Get_MSGLength(void)
{
  return seL4_GetIPCBuffer() -> tag.length;
}

static inline seL4_Word seL4_ExtraCaps(void)
{
  return seL4_GetIPCBuffer() -> tag.extraCaps;
}

static inline seL4_Word seL4_isBlocking(void)
{
  return 1; //seL4_GetIPCBuffer() -> tag.blockingSend;
}

static inline seL4_MessageInfo seL4_Get_MSGTag(void)
{
  return seL4_GetIPCBuffer() -> tag;
}

/* read and write words into the message buffer */
static inline seL4_Word seL4_MsgWord(seL4_Word *buf, seL4_Word index)
{
  return buf[index]; /* we do not have an offset. Tag is kept 
		      * seperately */
}

static inline void seL4_MsgPutWord(seL4_Word *buf, seL4_Word index, 
				   seL4_Word val)
{
  buf[index] = val;
}


/**********
 * Modify tags
 */
static inline seL4_MessageInfo seL4_CreateTag(seL4_Word len, 
					      seL4_Word exCap,
					      seL4_Word block,
					      seL4_Word label)
{
  seL4_MessageInfo tmp;

  tmp.length       = len;
  tmp.extraCaps    = exCap;
  tmp.label        = label;
  
  return tmp;
}

static inline void seL4_SetTag(seL4_MessageInfo newtag)
{
  seL4_GetIPCBuffer() -> tag = newtag;
}

/****************
 * values from a given tag 
 */
static inline seL4_Word seL4_Label_From(seL4_MessageInfo tag)
{
  return tag.label;
}

static inline seL4_Word seL4_Length_From(seL4_MessageInfo tag)
{
  return tag.length;
}

static inline void seL4_MsgStore(seL4_MessageInfo tag, seL4_Word *buf)
{
  int i;
  for(i=0; i < seL4_Length_From(tag); i++)
    buf[i] = seL4_GetMR(i);
}

static inline void seL4_ClearMSGTag(void)
{
  seL4_SetTag(seL4_CreateTag(0,0,0,0));
}

/***********
 * This contains information about the kernel generated messages
 */
#include <sel4/arch/pfIPC.h>   /* format of pagefault ipc */
#include <sel4/arch/exIPC.h>   /* format of exception ipc */ 


#endif /* ! ifndef __SEL4_MESSAGES_H__ */
