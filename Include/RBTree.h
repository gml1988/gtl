#pragma once

#include "Alloc.h"
#include "Pair.h"
#include "Gtl_Construct.h"
#include <assert.h>

namespace GTL
{
    enum class __RBTreeColor
    {
        Red = 0,
        Black = 1
    };

    template <class Value>
    struct __RBTreeNode
    {
        typedef __RBTreeColor           color_type;
        typedef __RBTreeNode<Value>     self;
        typedef __RBTreeNode<Value>*    node_pointer;

        __RBTreeNode() : fIsSentinel(false), color(color_type::Red), pLeftChild(nullptr), pRightChild(nullptr), pParent(nullptr)
        {

        }

        static bool IsNullOrSentinel(node_pointer pNode)
        {
            return (nullptr == pNode) || pNode->fIsSentinel;
        }

        static node_pointer Maximum(node_pointer pNode)
        {
            if (nullptr != pNode)
            {
                while (!IsNullOrSentinel(pNode->pRightChild))
                {
                    pNode = pNode->pRightChild;
                }
            }

            return pNode;
        }

        static node_pointer Minimum(node_pointer pNode)
        {
            if (!IsNullOrSentinel(pNode))
            {
                while (!IsNullOrSentinel(pNode->pLeftChild))
                {
                    pNode = pNode->pLeftChild;
                }
            }

            return pNode;
        }

        bool fIsSentinel;
        Value value;
        color_type color;
        node_pointer pLeftChild;
        node_pointer pRightChild;
        node_pointer pParent;
    };

    template <class Value, class Ref, class Ptr>
    struct __RBTreeIterator
    {
        typedef Value&                              reference;
        typedef __RBTreeNode<Value>                 tree_node;
        typedef __RBTreeNode<Value>*                node_pointer;
        typedef __RBTreeIterator<Value, Ref, Ptr>   self;

        __RBTreeIterator(node_pointer pNode) : pTreeNode(pNode)
        {
        
        }

        reference operator*()
        {
            return pTreeNode->value;
        }

        self& operator++()
        {
            Increment();
            return *this;
        }

        self operator++(int)
        {
            self iterTmp = *this;
            ++(*this);
            return iterTmp;
        }

        self& operator--()
        {
            Decrement();
            return *this;
        }

        self operator--(int)
        {
            self iterTmp = this;
            --(*this);
            return *this;
        }

        self& operator=(self const &other)
        {
            pTreeNode = other.pTreeNode;
            return *this;
        }

        bool operator==(self const &other) const
        {
            return pTreeNode == other.pTreeNode;
        }

        bool operator!=(self const &other) const
        {
            return !operator==(other);
        }

        void Increment()
        {
            // Go right for one step and then go left to the end
            if (!tree_node::IsNullOrSentinel(pTreeNode->pRightChild))
            {
                pTreeNode = pTreeNode->pRightChild;
                pTreeNode = tree_node::Minimum(pTreeNode);
            }
            else
            {
                node_pointer pNode = pTreeNode->pParent;
                while (!tree_node::IsNullOrSentinel(pNode) && (pTreeNode == pNode->pRightChild))
                {
                    pTreeNode = pNode;
                    pNode = pNode->pParent;
                }
                // The next one of the max node is the sentinel
                pTreeNode = pNode;
            }
        }

        void Decrement()
        {
            if (!tree_node::IsNullOrSentinel(pTreeNode->pLeftChild))
            {
                pTreeNode = pTreeNode->pLeftChild;
                pTreeNode = tree_node::Maximum(pTreeNode);
            }
            else
            {
                node_pointer pNode = pTreeNode->pParent;
                while (!tree_node::IsNullOrSentinel(pNode) && (pTreeNode == pNode->pLeftChild))
                {
                    pTreeNode = pNode;
                    pNode = pNode->pParent;
                }
                pTreeNode = pNode;
            }
        }

        node_pointer pTreeNode;
    };

    // An RB-Tree based Map
    template <class Key, class Value, class ExtractKey, class Compare, class Alloc = Allocator>
    class RBTree
    {
    public:
        typedef Key         key_type;
        typedef Value       value_type;
        typedef value_type& reference;
        typedef value_type* pointer;
        typedef ExtractKey  get_key;
        typedef size_t      size_type;
        typedef ptrdiff_t   difference_type;
        typedef __RBTreeIterator<Value, Value &, Value *>    iterator;

    public:
        RBTree(Compare const &comp = Compare()) : _nNodeCount(0), _comparer(comp)
        {
            _Initialize();
        }

        ~RBTree()
        {
            _Uninitialize();
        }
        
        Pair<iterator, bool> InsertUnique(value_type const& obj)
        {
            return Pair<iterator, bool>(iterator(nullptr, true));
        }

        iterator InsertEqual(value_type const& obj)
        {
            node_pointer pNewNode = _CreateNode(obj);
            node_pointer pFirstToread = _Root();
            node_pointer pSecondToread = _pSentinelNode;

            while (!tree_node::IsNullOrSentinel(pFirstToread))
            {
                pSecondToread = pFirstToread;
                if (_comparer(obj, pFirstToread->value))
                {
                    pFirstToread = pFirstToread->pLeftChild;
                }
                else
                {
                    pFirstToread = pFirstToread->pRightChild;
                }
            }

            pNewNode->pParent = pSecondToread;
            pNewNode->pLeftChild = _pSentinelNode;
            pNewNode->pRightChild = _pSentinelNode;
            // The root is nullptr
            if (tree_node::IsNullOrSentinel(pSecondToread))
            {
                _pSentinelNode->pParent = pNewNode;
                pNewNode->color = color_type::Black;
            }
            else
            {
                if (_comparer(obj, pSecondToread->value))
                {
                    pSecondToread->pLeftChild = pNewNode;
                }
                else
                {
                    pSecondToread->pRightChild = pNewNode;
                }
                pNewNode->color = color_type::Red;
                _RBTreeInsertFixUp(pNewNode);
            }

            ++_nNodeCount;

            return iterator(pNewNode);
        }

        iterator Erase(iterator &iter)
        {
            node_pointer pNode = iter.pTreeNode;
            node_pointer pNodeToDelete = nullptr;
            node_pointer pNodeToChangeChild = _pSentinelNode;
            node_pointer pNodeToChangeParent = _pSentinelNode;

            if (tree_node::IsNullOrSentinel(pNode))
            {
                return iterator(nullptr);
            }

            if (tree_node::IsNullOrSentinel(pNode->pLeftChild) || tree_node::IsNullOrSentinel(pNode->pRightChild))
            {
                pNodeToDelete = pNode;
            }
            else
            {
                iter++;
                pNodeToDelete = iter.pTreeNode;
            }

            pNodeToChangeChild = pNodeToDelete->pParent;
            if (!tree_node::IsNullOrSentinel(pNodeToDelete->pLeftChild))
            {
                pNodeToChangeParent = pNodeToDelete->pLeftChild;
            }
            else if(!tree_node::IsNullOrSentinel(pNodeToDelete->pRightChild))
            {
                pNodeToChangeParent = pNodeToDelete->pRightChild;
            }

            if (pNodeToDelete == _Root())
            {
                if (!tree_node::IsNullOrSentinel(pNodeToChangeParent))
                {
                    _Root() = _pSentinelNode;
                }
                else
                {
                    _Root() = pNodeToChangeParent;
                    pNodeToChangeParent->pParent = _pSentinelNode;
                }
            }
            else
            {
                if (pNodeToDelete == pNodeToChangeChild->pLeftChild)
                {
                    pNodeToChangeChild->pLeftChild = pNodeToChangeParent;
                }
                else if(pNodeToDelete == pNodeToChangeChild->pRightChild)
                {
                    pNodeToChangeChild->pRightChild = pNodeToChangeParent;
                }
                pNodeToChangeParent = pNodeToChangeChild;
            }

            if (pNodeToDelete != pNode)
            {
                pNode->value = pNodeToDelete->value;
            }

            if (color_type::Black == pNodeToDelete->color)
            {
                _RBTreeDeleteFixUp(pNodeToChangeParent);
            }

            _DestroyNode(pNodeToDelete);

            --_nNodeCount;

            return iterator(nullptr);
        }

        iterator Find(value_type const& obj)
        {
            node_pointer pNode = _Root();
            while (!tree_node::IsNullOrSentinel(pNode))
            {
                if (obj == pNode->value)
                {
                    break;
                }
                else if (_comparer(obj, pNode->value))
                {
                    pNode = pNode->pLeftChild;
                }
                else
                {
                    pNode = pNode->pRightChild;
                }
            }

            return pNode;
        }

        iterator Begin()
        {
            node_pointer pMinNode = tree_node::Minimum(_Root());
            return iterator(pMinNode);
        }

        iterator End()
        {
            return iterator(_pSentinelNode);
        }

        size_type Size() const
        {
            return _nNodeCount;
        }

    protected:
        typedef __RBTreeColor                       color_type;
        typedef __RBTreeNode<Value>                 tree_node;
        typedef tree_node*                          node_pointer;
        typedef Simple_Allocator<tree_node, Alloc>  node_allocator;

    protected:
        void _Initialize()
        {
            _pSentinelNode = _CreateNode();
            _pSentinelNode->fIsSentinel = true;
            _pSentinelNode->color = color_type::Black;
            _pSentinelNode->pParent = _pSentinelNode;
            _pSentinelNode->pLeftChild = _pSentinelNode;
            _pSentinelNode->pRightChild = _pSentinelNode;
        }

        void _Uninitialize()
        {
            _DestroyNode(_pSentinelNode);
        }

        node_pointer _CreateNode()
        {
            node_pointer pNode = node_allocator::Allocate();
            Construct(pNode);
            return pNode;
        }

        node_pointer _CreateNode(value_type const& obj)
        {
            node_pointer pNode = _CreateNode();
            Construct(&(pNode->value), obj);
            return pNode;
        }

        void _DestroyNode(node_pointer pNode)
        {
            Destroy(&(pNode->value));
            node_allocator::Deallocate(pNode);
        }

        node_pointer &_Root()
        {
            return _pSentinelNode->pParent;
        }

        void _RBTreeInsertFixUp(node_pointer pNode)
        {
            assert(color_type::Red == pNode->color);
            while (color_type::Red == pNode->pParent->color)
            {
                node_pointer pGrandParentNode = pNode->pParent->pParent;
                node_pointer pParentNode = pNode->pParent;
                if (pParentNode == pGrandParentNode->pLeftChild)
                {
                    node_pointer pUncleNode = pGrandParentNode->pRightChild;
                    // Case 1: The uncle is red
                    if (color_type::Red == pUncleNode->color)
                    {
                        pParentNode->color = color_type::Black;
                        pUncleNode->color = color_type::Black;
                        pGrandParentNode->color = color_type::Red;
                        pNode = pGrandParentNode;
                    }
                    else if (color_type::Black == pUncleNode->color)
                    {
                        // Case 2: The uncle is black, and z is rightchild
                        if (pNode == pParentNode->pRightChild)
                        {
                            _TreeRotateLeft(pParentNode);
                        }

                        // Case 3: The uncle is black, and z is leftchild
                        pNode->color = color_type::Black;
                        pNode->pParent->color = color_type::Red;
                        _TreeRotateRight(pNode->pParent);
                        break;
                    }
                }
                else if(pParentNode == pGrandParentNode->pRightChild)
                {
                    node_pointer pUncleNode = pGrandParentNode->pLeftChild;
                    if (color_type::Red == pUncleNode->color)
                    {
                        pParentNode->color = color_type::Black;
                        pUncleNode->color = color_type::Black;
                        pGrandParentNode->color = color_type::Red;
                        pNode = pGrandParentNode;
                    }
                    else if(color_type::Black == pUncleNode->color)
                    {
                        if (pNode == pParentNode->pLeftChild)
                        {
                            _TreeRotateRight(pParentNode);
                        }

                        pNode->color = color_type::Black;
                        pNode->pParent->color = color_type::Red;
                        _TreeRotateLeft(pNode->pParent);
                        break;
                    }
                }
            }
        }

        void _RBTreeDeleteFixUp(node_pointer pNode)
        {
            while (pNode != _Root() && color_type::Black == pNode->color)
            {
                node_pointer pParentNode = pNode->pParent;
                if (pNode == pParentNode->pLeftChild)
                {
                    node_pointer pBrotherNode = pParentNode->pRightChild;
                    // Case 1: the brother of pNode is red
                    if (color_type::Red == pBrotherNode->color)
                    {
                        Swap(pParentNode->color, pBrotherNode->color);
                        _TreeRotateLeft(pParentNode);
                    }
                    else if (color_type::Black == pBrotherNode->color)
                    {
                        // Case 2: The brother is black, the two children of brother is both black
                        if (color_type::Black == pBrotherNode->pLeftChild->color &&
                            color_type::Black == pBrotherNode->pRightChild->color)
                        {
                            pBrotherNode->color = color_type::Red;
                            pNode = pParentNode;                            
                        }
                        else
                        {
                            // Case 3: Brother is black, the inner child is red, the outer child is black
                            // Swift to Case 4
                            if (color_type::Red == pBrotherNode->pLeftChild->color &&
                                color_type::Black == pBrotherNode->pRightChild->color)
                            {
                                pBrotherNode->pLeftChild->color = color_type::Black;
                                pBrotherNode->color = color_type::Red;
                                pBrotherNode = _TreeRotateLeft(pBrotherNode);
                            }
                            
                            // Case 4: Brother is black, the outer child is red
                            Swap(pParentNode->color, pBrotherNode->color);
                            pBrotherNode->pRightChild->color = color_type::Black;
                            _TreeRotateLeft(pParentNode);
                            pNode = _Root();
                        }
                    }
                }
                else if(pNode == pParentNode->pRightChild)
                {
                    node_pointer pBrotherNode = pParentNode->pLeftChild;
                    // Case 1: the brother of pNode is red
                    if (color_type::Red == pBrotherNode->color)
                    {
                        Swap(pParentNode->color, pBrotherNode->color);
                        _TreeRotateRight(pParentNode);
                    }
                    else if (color_type::Black == pBrotherNode->color)
                    {
                        // Case 2: The brother is black, the two children of brother is both black
                        if (color_type::Black == pBrotherNode->pLeftChild->color &&
                            color_type::Black == pBrotherNode->pRightChild->color)
                        {
                            pBrotherNode->color = color_type::Black;
                            pNode = pParentNode;
                        }
                        else
                        {
                            // Case 3: Brother is black, the inner child is red, the outer child is black
                            // Swift to Case 4
                            if (color_type::Red == pBrotherNode->pRightChild->color &&
                                color_type::Black == pBrotherNode->pLeftChild->color)
                            {
                                pBrotherNode->pRightChild->color = color_type::Black;
                                pBrotherNode->color = color_type::Red;
                                pBrotherNode = _TreeRotateLeft(pBrotherNode);
                            }

                            // Case 4: Brother is black, the outer child is red
                            Swap(pParentNode->color, pBrotherNode->color);
                            pBrotherNode->pLeftChild->color = color_type::Black;
                            _TreeRotateRight(pParentNode);
                            pNode = _Root();
                        }
                    }
                }
            }

            pNode->color = color_type::Black;
        }

        // In param: the root node of the sub-tree
        // return: the new root node of the sub-tree
        node_pointer _TreeRotateRight(node_pointer pNode)
        {
            node_pointer pNewSubTreeRootNode = pNode->pLeftChild;
            // The connection between the rightchild of the pNewSubTreeRootNode and pNode
            pNode->pLeftChild = pNewSubTreeRootNode->pRightChild;
            if (nullptr != pNewSubTreeRootNode->pRightChild)
            {
                pNewSubTreeRootNode->pRightChild->pParent = pNode;
            }

            // The connnection between pNewSubTreeRootNode and parent node of pNode
            node_pointer pHigherLevelNode = pNode->pParent;
            if (pNode == _Root())
            {
                // There is no left child or right child, just the parent pointer of the _pSentinel
                _Root() = pNewSubTreeRootNode;
            }
            else
            {
                if (pNode == pHigherLevelNode->pLeftChild)
                {
                    pHigherLevelNode->pLeftChild = pNewSubTreeRootNode;
                }
                else
                {
                    pHigherLevelNode->pRightChild = pNewSubTreeRootNode;
                }
            }
            pNewSubTreeRootNode->pParent = pHigherLevelNode;

            // The connection between pNewSubTreeRootNode and pNode
            pNewSubTreeRootNode->pRightChild = pNode;
            pNode->pParent = pNewSubTreeRootNode;

            return pNewSubTreeRootNode;
        }

        node_pointer _TreeRotateLeft(node_pointer pNode)
        {
            node_pointer pNewSubTreeRootNode = pNode->pRightChild;
            pNode->pRightChild = pNewSubTreeRootNode->pLeftChild;
            if (nullptr != pNewSubTreeRootNode->pLeftChild)
            {
                pNewSubTreeRootNode->pLeftChild->pParent = pNode;
            }

            // The connnection between pNewSubTreeRootNode and parent node of pNode
            node_pointer pHigherLevelNode = pNode->pParent;
            if (pNode == _Root())
            {
                // There is no left child or right child, just the parent pointer of the _pSentinel
                _Root() = pNewSubTreeRootNode;
            }
            else
            {
                if (pNode == pHigherLevelNode->pLeftChild)
                {
                    pHigherLevelNode->pLeftChild = pNewSubTreeRootNode;
                }
                else
                {
                    pHigherLevelNode->pRightChild = pNewSubTreeRootNode;
                }
            }
            pNewSubTreeRootNode->pParent = pHigherLevelNode;

            // The connection between pNewSubTreeRootNode and pNode
            pNewSubTreeRootNode->pLeftChild = pNode;
            pNode->pParent = pNewSubTreeRootNode;
            
            return pNewSubTreeRootNode;
        }

    protected:
        node_pointer    _pSentinelNode;
        size_type       _nNodeCount;
        Compare         _comparer;
    };
}