#include <vector>
#include <set>
#include <map>

namespace GTL
{
    template <class T>
    struct GraphNode
    {
    public:
        GraphNode() {};
        GraphNode(const T& data) : m_data(data) {};

    public:
        T m_data;
        std::vector<GraphNode<T> *> m_vecNeighborNodes;
    };

    template <class T>
    class Graph
    {
    public:
        static GraphNode<T> *Clone(GraphNode<T> *pRoot)
        {
            std::map<GraphNode<T> *, GraphNode<T> *> cloneMap;

            return Clone(pRoot, cloneMap);
        }

    private:
        // API definition: Clone the pNode and return the clone instance
        static GraphNode<T> *Clone(GraphNode<T>* pNode, std::map<GraphNode<T> *, GraphNode<T> *> &cloneMap)
        {
            if (nullptr == pNode)
            {
                return nullptr;
            }

            std::map<GraphNode<T> *, GraphNode<T> *>::iterator mapIter = cloneMap.find(pNode);
            if (mapIter != cloneMap.end())
            {
                return mapIter->second;
            }

            GraphNode<T> *pNewNode = new GraphNode<T>(pNode->m_data);
            // Need add the pNode in the map first
            // Otherwise, it may enter the infinite traverse in the following breath-first searching
            cloneMap[pNode] = pNewNode;
            for (std::vector<GraphNode<T> *> vecIter = pNode->m_vecNeighborNodes.begin(); 
                 vecIter != pNode->m_vecNeighborNodes.end(); 
                 ++vecIter)
            {
                pNewNode->m_vecNeighborNodes.push_back(Clone(*vecIter, cloneMap));
            }

            return pNewNode;
        }
    };
}