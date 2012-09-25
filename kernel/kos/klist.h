////////////////////////////////////////////////////////////////////////////////
/// @file      klist.h
/// @short  template implementation of linked list
/// @author J.Nider
/// @date   25-Jul-2008 08:43:53 AM
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

#ifndef _KLIST__H
#define _KLIST__H

////////////////////////////////////////////////////////////////////////////////
/// @author J.Nider
/// @date   25-Jul-2008 08:44:02 AM
/// @brief  linked list of pointer to object
////////////////////////////////////////////////////////////////////////////////
struct __node_t;

typedef struct __node_t
{
   struct __node_t* pNext;
   _T* pData;
} __node_t;

typedef struct klist
{
   __node_t* m_pFirst;
   __node_t* m_pLast;
   int m_count;
};

klist_init(klist* list)
{
   m_pFirst = 0;
   m_pLast = 0;
   m_count = 0;
}
////////////////////////////////////////////////////////////////////////////////
/// @author J.Nider
/// @date   25-Jul-2008 08:44:58 AM
/// @brief  empty the list; delete all nodes
////////////////////////////////////////////////////////////////////////////////
void klist_Clear(klist* l)
{
   while (l->m_pFirst)
   {
      __node_t* pNode = l->m_pFirst;
      l->m_pFirst = l->m_pFirst->pNext;
      delete pNode;
   }
   l->m_count = 0;
}

////////////////////////////////////////////////////////////////////////////////
///@date 25-Jul-2008 08:44:38 AM
///@brief   add node to the end of the list
////////////////////////////////////////////////////////////////////////////////
  void PushBack(_T* _p)
   {
      __node_t* pNode = new __node_t;
      pNode->pData = _p;
      pNode->pNext = 0;

      __node_t* temp = m_pFirst;
      if (!m_pFirst)
      {
         m_pFirst = pNode;
      }
      else
      {
         while (temp->pNext)
         {
            temp = temp->pNext;
         }
      }

      temp->pNext = pNode;
      m_count++;
   }

////////////////////////////////////////////////////////////////////////////////
/// @date   30/10/2009
/// @brief  insert an existing node at a particular position
/// @param  index position to insert the node
/// @param  _p pointer to node to be inserted
/// @return false if insertion point was not found
////////////////////////////////////////////////////////////////////////////////
   bool Insert(unsigned int index, _T* _p)
   {
      if (index < m_count)
      {
         __node_t* pTemp = m_pFirst;
         __node_t* pNode = new __node_t;
         pNode->pData = _p;

         // find insertion point
         for (unsigned int i=0; i < index; i++)
            pTemp = pTemp->pNext;

         pNode->pNext = pTemp->pNext;
         pTemp->pNext = pNode;
         m_count++;
         return true;
      }
      return false;
   }

////////////////////////////////////////////////////////////////////////////////
/// @date   18/04/2010
/// @brief  delete a node from this position
/// @param  index position to delete the node
/// @return false if insertion point was not found
////////////////////////////////////////////////////////////////////////////////

   bool Delete(unsigned int index)
   {
      if (index < m_count)
      {
         __node_t* pPrev;
         __node_t* pTemp = m_pFirst;
         // JKN: this can be optimized to updating a single pointer
         // while going through the list by stopping at index-1
         for (unsigned int i=0; i < index; i++)
         {
            pPrev = pTemp;
            pTemp = pTemp->pNext;
         }

         pPrev->pNext = pTemp->pNext;
         delete pTemp;
         m_count--;
      }
      return true;
   }

////////////////////////////////////////////////////////////////////////////////
///@author  J.Nider
///@date 25-Jul-2008 08:45:25 AM
///@brief   remove a node from the front of the list
////////////////////////////////////////////////////////////////////////////////
  _T* PopFront()
   {
      _T* pData = 0;
      if (m_pFirst)
      {
         pData = m_pFirst->pData;
         delete m_pFirst;
         m_pFirst = m_pFirst->pNext;
         m_count--;
      }

      return pData;
   }

////////////////////////////////////////////////////////////////////////////////
///@author  J.Nider
///@date 25-Jul-2008 08:45:38 AM
///@brief   returns number of items in the list
////////////////////////////////////////////////////////////////////////////////
  unsigned int Count() { return m_count; }

////////////////////////////////////////////////////////////////////////////////
///@author  J.Nider
///@date 25-Jul-2008 08:46:05 AM
///@brief   allows array access into the list
////////////////////////////////////////////////////////////////////////////////
  _T* operator [](unsigned int index)
   {
      if (index < m_count)
      {
         __node_t* pTemp = m_pFirst;
         for (unsigned int i=0; i < index; i++)
            pTemp = pTemp->pNext;

         return pTemp->pData;
      }

      return 0;
   }

protected:
   __node_t* m_pFirst;        ///< head node; null if list is empty
   unsigned int m_count;      ///< count of the number of nodes in the list
};

template<class _T> class KList
{
   struct __node_t
   {
      __node_t* pNext;
      _T data;
   };

public:
   KList() : m_pFirst(0), m_count(0) {}
   ~KList() { Clear(); };

   void Clear()
   {
      while (m_pFirst)
      {
         __node_t* pNode = m_pFirst;
         m_pFirst = m_pFirst->pNext;
         delete pNode;
      }
      m_count = 0;
   }

   void PushBack(_T _p)
   {
      __node_t* pNode = new __node_t;
      pNode->data = _p;
      pNode->pNext = 0;

      __node_t* temp = m_pFirst;
      if (!m_pFirst)
      {
         m_pFirst = pNode;
      }
      else
      {
         while (temp->pNext)
         {
            temp = temp->pNext;
         }
      }

      temp->pNext = pNode;
      m_count++;
   }

   _T PopFront()
   {
      _T data;
      if (m_pFirst)
      {
         data = m_pFirst->data;
         delete m_pFirst;
         m_pFirst = m_pFirst->pNext;
         m_count--;
      }

      return data;
   }

   unsigned int Count() { return m_count; }

protected:
   __node_t* m_pFirst;        ///< head node; null if list is empty
   unsigned int m_count;      ///< count of the number of nodes in the list
};


#endif // _KLIST__H
