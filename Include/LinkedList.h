namespace GTL
{
    template <class T>
    struct LinkedListNode
    {
        T m_data;
        LinkedListNode *m_pNext;
    };

    template <class T>
    class LinkedList
    {
    public:
        int Count() const
        {
            int nCount = 0;
            LinkedListNode<T> *pNode = m_pHead;

            while (pNode)
            {
                nCount++;
                pNode = pNode->m_pNext;
            }

            return nCount;
        }

        void AddFirst(LinkedListNode<T> *pNode)
        {
            if (nullptr == pNode)
            {
                return;
            }

            pNode->m_pNext = m_pHead;
            m_pHead = pNode;
        }

        void AddLast(LinkedListNode<T> *pNode)
        {
            if (nullptr == pNode)
            {
                return;
            }

            LinkedListNode<T> *pLastNode = Last();

            if (nullptr == pLastNode)
            {
                m_pHead->m_pNext = pNode;
            }
            else
            {
                pLastNode->m_pNext = pNode;
            }

            pNode->m_pNext = nullptr;
        }

        void RemoveFirst()
        {
            if (nullptr != m_pHead)
            {
                LinkedListNode<T> *pNodeToDelete = m_pHead;
                m_pHead = m_pHead->m_pNext;
                delete pNodeToDelete;
            }
        }

        void RemoveLast()
        {
            // Move to the last second node
        }

        LinkedListNode<T> *First()
        {
            return m_pHead;
        }

        LinkedListNode<T> *Last()
        {
            LinkedListNode<T> *pPreviousNode = nullptr;
            LinkedListNode<T> *pCurrentNode = m_pHead;

            while (pCurrentNode)
            {
                pPreviousNode = pCurrentNode;
                pCurrentNode = pCurrentNode->m_pNext;
            }

            return pPreviousNode;
        }

        // The head node is 0th node
        LinkedListNode<T> *FirstNth(int iIndex)
        {
            LinkedListNode<T> *pCurrentNode = m_pHead;

            while (iIndex > 0 && pCurrentNode)
            {
                pCurrentNode = pCurrentNode->m_pNext;
                iIndex--;
            }

            return pCurrentNode;
        }

        // The tail node is the last 0th node
        LinkedListNode<T> *LastNth(int iIndex)
        {
            LinkedListNode<T> *pPreviousNode = nullptr;
            LinkedListNode<T> *pCurrentNode = m_pHead;

            while (iIndex > 0 && pCurrentNode)
            {
                pCurrentNode = pCurrentNode->m_pNext;
                iIndex--;
            }

            while (pCurrentNode)
            {
                pPreviousNode = pCurrentNode;
                pCurrentNode = pCurrentNode->m_pNext;
            }

            return pPreviousNode;
        }

    private:
        LinkedListNode<T> *m_pHead;
    };
}