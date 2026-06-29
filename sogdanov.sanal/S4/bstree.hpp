#ifndef BSTREE_HPP
#define BSTREE_HPP

#include <algorithm>
#include <functional>
#include <stdexcept>

#include "bstiterators.hpp"

namespace sogdanov {

  template< class Key, class Value, class Compare = std::less< Key > >
  class BSTree {
  public:
    using node_ptr = Node< Key, Value > *;

    BSTree();
    BSTree(const BSTree< Key, Value, Compare > &other);
    ~BSTree();
    BSTree< Key, Value, Compare > &operator=(const BSTree< Key, Value, Compare > &other);
    bool empty() const;

    BSTIterator< Key, Value > begin();
    BSTIterator< Key, Value > end();
    BSTConstIterator< Key, Value > begin() const;
    BSTConstIterator< Key, Value > end() const;

    BSTIterator< Key, Value > find(const Key &k);
    void push(const Key &k, const Value &v);
    Value get(const Key &k);
    void drop(const Key &k);

    BSTConstIterator< Key, Value > rotateLeft(BSTConstIterator< Key, Value > it);
    BSTConstIterator< Key, Value > rotateRight(BSTConstIterator< Key, Value > it);
    BSTConstIterator< Key, Value > rotateLargeLeft(BSTConstIterator< Key, Value > it);
    BSTConstIterator< Key, Value > rotateLargeRight(BSTConstIterator< Key, Value > it);

    size_t height(BSTConstIterator< Key, Value > it) const;
    size_t height() const;

  private:
    node_ptr root;
    node_ptr fake_leaf;
    Compare comp;
    size_t tree_size;

    void swap(BSTree< Key, Value, Compare > &other) noexcept;
    void destroy(node_ptr n);
    node_ptr copyTree(const Node< Key, Value > *otherNode, node_ptr parent, node_ptr otherFake);
    size_t calculateHeight(node_ptr n) const;
  };

  template< class Key, class Value, class Compare >
  void BSTree< Key, Value, Compare >::swap(BSTree< Key, Value, Compare > &other) noexcept
  {
    std::swap(root, other.root);
    std::swap(fake_leaf, other.fake_leaf);
    std::swap(tree_size, other.tree_size);
    std::swap(comp, other.comp);
  }

  template< class Key, class Value, class Compare >
  void BSTree< Key, Value, Compare >::destroy(node_ptr n)
  {
    if (n != fake_leaf) {
      destroy(n->left);
      destroy(n->right);
      delete n;
    }
  }

  template< class Key, class Value, class Compare >
  typename BSTree< Key, Value, Compare >::node_ptr
  BSTree< Key, Value, Compare >::copyTree(const Node< Key, Value > *otherNode, node_ptr parent, node_ptr otherFake)
  {
    if (otherNode == otherFake) {
      return fake_leaf;
    }
    node_ptr newNode = new Node< Key, Value >(otherNode->data.first, otherNode->data.second, parent);
    try {
      newNode->left = fake_leaf;
      newNode->right = fake_leaf;
      newNode->left = copyTree(otherNode->left, newNode, otherFake);
      newNode->right = copyTree(otherNode->right, newNode, otherFake);
    } catch (...) {
      destroy(newNode);
      throw;
    }
    return newNode;
  }

  template< class Key, class Value, class Compare >
  size_t BSTree< Key, Value, Compare >::calculateHeight(node_ptr n) const
  {
    if (n == fake_leaf) {
      return 0;
    }
    return 1 + std::max(calculateHeight(n->left), calculateHeight(n->right));
  }

  template< class Key, class Value, class Compare >
  BSTree< Key, Value, Compare >::BSTree():
    tree_size(0)
  {
    fake_leaf = new Node< Key, Value >(Key(), Value(), nullptr);
    fake_leaf->left = fake_leaf;
    fake_leaf->right = fake_leaf;
    root = fake_leaf;
  }

  template< class Key, class Value, class Compare >
  BSTree< Key, Value, Compare >::BSTree(const BSTree< Key, Value, Compare > &other):
    comp(other.comp),
    tree_size(other.tree_size)
  {
    fake_leaf = new Node< Key, Value >(Key(), Value(), nullptr);
    fake_leaf->left = fake_leaf;
    fake_leaf->right = fake_leaf;
    try {
      root = copyTree(other.root, nullptr, other.fake_leaf);
    } catch (...) {
      delete fake_leaf;
      throw;
    }
  }

  template< class Key, class Value, class Compare >
  BSTree< Key, Value, Compare >::~BSTree()
  {
    destroy(root);
    delete fake_leaf;
  }

  template< class Key, class Value, class Compare >
  BSTree< Key, Value, Compare > &BSTree< Key, Value, Compare >::operator=(const BSTree< Key, Value, Compare > &other)
  {
    if (this != &other) {
      BSTree< Key, Value, Compare > tmp(other);
      swap(tmp);
    }
    return *this;
  }

  template< class Key, class Value, class Compare >
  bool BSTree< Key, Value, Compare >::empty() const
  {
    return root == fake_leaf;
  }

  template< class Key, class Value, class Compare >
  BSTIterator< Key, Value > BSTree< Key, Value, Compare >::begin()
  {
    node_ptr curr = root;
    while (curr != fake_leaf && curr->left != fake_leaf) {
      curr = curr->left;
    }
    return BSTIterator< Key, Value >(curr, fake_leaf);
  }

  template< class Key, class Value, class Compare >
  BSTIterator< Key, Value > BSTree< Key, Value, Compare >::end()
  {
    return BSTIterator< Key, Value >(fake_leaf, fake_leaf);
  }

  template< class Key, class Value, class Compare >
  BSTConstIterator< Key, Value > BSTree< Key, Value, Compare >::begin() const
  {
    const Node< Key, Value > *curr = root;
    while (curr != fake_leaf && curr->left != fake_leaf) {
      curr = curr->left;
    }
    return BSTConstIterator< Key, Value >(curr, fake_leaf);
  }

  template< class Key, class Value, class Compare >
  BSTConstIterator< Key, Value > BSTree< Key, Value, Compare >::end() const
  {
    return BSTConstIterator< Key, Value >(fake_leaf, fake_leaf);
  }

  template< class Key, class Value, class Compare >
  BSTIterator< Key, Value > BSTree< Key, Value, Compare >::find(const Key &k)
  {
    node_ptr curr = root;
    while (curr != fake_leaf) {
      if (comp(k, curr->data.first)) {
        curr = curr->left;
      } else if (comp(curr->data.first, k)) {
        curr = curr->right;
      } else {
        return BSTIterator< Key, Value >(curr, fake_leaf);
      }
    }
    return end();
  }

  template< class Key, class Value, class Compare >
  void BSTree< Key, Value, Compare >::push(const Key &k, const Value &v)
  {
    if (root == fake_leaf) {
      root = new Node< Key, Value >(k, v, nullptr);
      root->left = fake_leaf;
      root->right = fake_leaf;
      tree_size++;
      return;
    }
    node_ptr curr = root;
    node_ptr parent = nullptr;
    while (curr != fake_leaf) {
      parent = curr;
      if (comp(k, curr->data.first)) {
        curr = curr->left;
      } else if (comp(curr->data.first, k)) {
        curr = curr->right;
      } else {
        curr->data.second = v;
        return;
      }
    }
    node_ptr newNode = new Node< Key, Value >(k, v, parent);
    newNode->left = fake_leaf;
    newNode->right = fake_leaf;
    if (comp(k, parent->data.first)) {
      parent->left = newNode;
    } else {
      parent->right = newNode;
    }
    tree_size++;
  }

  template< class Key, class Value, class Compare >
  Value BSTree< Key, Value, Compare >::get(const Key &k)
  {
    BSTIterator< Key, Value > it = find(k);
    if (it != end()) {
      return (*it).second;
    }
    throw std::out_of_range("Key not found");
  }

  template< class Key, class Value, class Compare >
  void BSTree< Key, Value, Compare >::drop(const Key &k)
  {
    BSTIterator< Key, Value > it = find(k);
    if (it == end()) {
      throw std::out_of_range("Key not found");
    }

    node_ptr node = it.node;
    if (node->left == fake_leaf && node->right == fake_leaf) {
      if (node->parent != nullptr) {
        if (node->parent->left == node) {
          node->parent->left = fake_leaf;
        } else {
          node->parent->right = fake_leaf;
        }
      } else {
        root = fake_leaf;
      }
      delete node;
    } else if (node->left == fake_leaf || node->right == fake_leaf) {
      node_ptr child = (node->left != fake_leaf) ? node->left : node->right;
      child->parent = node->parent;
      if (node->parent != nullptr) {
        if (node->parent->left == node) {
          node->parent->left = child;
        } else {
          node->parent->right = child;
        }
      } else {
        root = child;
      }
      delete node;
    } else {
      node_ptr succ = node->right;
      while (succ->left != fake_leaf) {
        succ = succ->left;
      }
      node->data = succ->data;
      node_ptr child = succ->right;
      if (succ->parent->left == succ) {
        succ->parent->left = child;
      } else {
        succ->parent->right = child;
      }
      if (child != fake_leaf) {
        child->parent = succ->parent;
      }
      delete succ;
    }
    tree_size--;
  }

  template< class Key, class Value, class Compare >
  BSTConstIterator< Key, Value > BSTree< Key, Value, Compare >::rotateLeft(BSTConstIterator< Key, Value > it)
  {
    node_ptr node = const_cast< node_ptr >(it.node);
    if (node == fake_leaf || node->parent == nullptr || node->parent->right != node) {
      return it;
    }

    node_ptr parent = node->parent;
    node_ptr grand = parent->parent;

    parent->right = node->left;
    if (node->left != fake_leaf) {
      node->left->parent = parent;
    }

    node->left = parent;
    parent->parent = node;

    node->parent = grand;
    if (grand != nullptr) {
      if (grand->left == parent) {
        grand->left = node;
      } else {
        grand->right = node;
      }
    } else {
      root = node;
    }
    return BSTConstIterator< Key, Value >(node, fake_leaf);
  }

  template< class Key, class Value, class Compare >
  BSTConstIterator< Key, Value > BSTree< Key, Value, Compare >::rotateRight(BSTConstIterator< Key, Value > it)
  {
    node_ptr node = const_cast< node_ptr >(it.node);
    if (node == fake_leaf || node->parent == nullptr || node->parent->left != node) {
      return it;
    }

    node_ptr parent = node->parent;
    node_ptr grand = parent->parent;

    parent->left = node->right;
    if (node->right != fake_leaf) {
      node->right->parent = parent;
    }

    node->right = parent;
    parent->parent = node;

    node->parent = grand;
    if (grand != nullptr) {
      if (grand->left == parent) {
        grand->left = node;
      } else {
        grand->right = node;
      }
    } else {
      root = node;
    }
    return BSTConstIterator< Key, Value >(node, fake_leaf);
  }

  template< class Key, class Value, class Compare >
  BSTConstIterator< Key, Value > BSTree< Key, Value, Compare >::rotateLargeLeft(BSTConstIterator< Key, Value > it)
  {
    node_ptr node = const_cast< node_ptr >(it.node);
    if (node == fake_leaf || node->parent == nullptr) {
      return it;
    }
    rotateRight(BSTConstIterator< Key, Value >(node, fake_leaf));
    return rotateLeft(BSTConstIterator< Key, Value >(node, fake_leaf));
  }

  template< class Key, class Value, class Compare >
  BSTConstIterator< Key, Value > BSTree< Key, Value, Compare >::rotateLargeRight(BSTConstIterator< Key, Value > it)
  {
    node_ptr node = const_cast< node_ptr >(it.node);
    if (node == fake_leaf || node->parent == nullptr) {
      return it;
    }
    rotateLeft(BSTConstIterator< Key, Value >(node, fake_leaf));
    return rotateRight(BSTConstIterator< Key, Value >(node, fake_leaf));
  }

  template< class Key, class Value, class Compare >
  size_t BSTree< Key, Value, Compare >::height(BSTConstIterator< Key, Value > it) const
  {
    return calculateHeight(const_cast< node_ptr >(it.node));
  }

  template< class Key, class Value, class Compare >
  size_t BSTree< Key, Value, Compare >::height() const
  {
    return calculateHeight(root);
  }

}

#endif
