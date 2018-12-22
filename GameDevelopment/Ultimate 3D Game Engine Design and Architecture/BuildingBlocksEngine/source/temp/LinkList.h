


#ifndef _BBE_LINKLIST_H_
#define _BBE_LINKLIST_H_


template<class T>
class CLinkListNode
{
   public:
      T m_data;
      CLinkListNode *m_next;
};


template<class T>
class CLinkList
{
   public:
      CLinkList();
      ~CLinkList();
      void MoveToStart();
      void MoveForward();
      void Push_Data(T newData);
      void Pop_Data();
      void Append_Data(T newData);
      
      int GetTotalNodes();
      T GetData();
   
   private:
      int m_totalNodes;
      CLinkListNode<T> *m_root;
      CLinkListNode<T> *m_currentNode;
};


template<class T>
CLinkList<T>::CLinkList() : m_totalNodes(0), m_root(0), m_currentNode(0)
{
}


template<class T>
CLinkList<T>::~CLinkList()
{
   while(m_root) Pop_Data();
}


template<class T>
void CLinkList<T>::MoveToStart()
{
   m_currentNode = m_root;
}


template<class T>
void CLinkList<T>::MoveForward()
{
   if((m_currentNode != NULL) && (m_currentNode->m_next != NULL))
      m_currentNode = m_currentNode->m_next;
}


template<class T>
void CLinkList<T>::Push_Data(T newData)
{
   CLinkListNode<T> *node;

   if(m_currentNode)
      {
         node = new CLinkListNode<T>;
         node->m_data = newData;
         node->m_next = m_currentNode->m_next;
         m_currentNode->m_next = node;
         m_totalNodes++;
      }
   else
      {
         Append_Data(newData);
      }
}


template<class T>
void CLinkList<T>::Pop_Data()
{
   CLinkListNode<T> *lastNode;
   if(m_currentNode == NULL) return;

   if(m_root == m_currentNode)
      {
         m_currentNode = m_root->m_next;

         delete m_root;
         m_root = m_currentNode;
      }
   else
      {
         lastNode = m_root;

         while(lastNode->m_next != m_currentNode)
            lastNode = lastNode->m_next;

         m_currentNode = m_currentNode->m_next;
         delete lastNode->m_next;
         lastNode->m_next = m_currentNode;
         m_currentNode = lastNode;
      }

   m_totalNodes = (m_totalNodes == 0 ? m_totalNodes : m_totalNodes - 1);
}


template<class T>
void CLinkList<T>::Append_Data(T newData)
{
   CLinkListNode<T> *node = new CLinkListNode<T>;
   CLinkListNode<T> *tempRoot = m_root;

   node->m_data = newData;
   node->m_next = NULL;

   if(tempRoot != NULL)
      {
         while(tempRoot->m_next != NULL)
            tempRoot = tempRoot->m_next;

         tempRoot->m_next = node;
      }
   else
      {
         m_root = node;
         m_currentNode = m_root;
      }

   m_totalNodes++;
}
      

template<class T>
int CLinkList<T>::GetTotalNodes()
{
   return m_totalNodes;
}


template<class T>
T CLinkList<T>::GetData()
{
   return m_currentNode->m_data;
}

#endif