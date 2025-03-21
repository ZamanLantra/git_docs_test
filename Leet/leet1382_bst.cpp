#include <vector>
#include <numeric>
#include <memory>
#include <iostream>

template<typename T>
class Node {
public:
    Node(const T val) : value(val) {}
    T value;
    std::shared_ptr<Node> left, right;
};

template<typename T>
class BST {
public:
    BST() {}
    ~BST() {}

    void insert(const T value);
    void inorder_traversal(std::vector<T>& vec) {
        __inorder_traversal(root, vec);
    }
    void preorder_traversal(std::vector<T>& vec) {
        __preorder_traversal(root, vec);
    }
    void postorder_traversal(std::vector<T>& vec) {
        __postorder_traversal(root, vec);
    }
    void balance_tree() {
        __balance_tree(root);
    }
    int depth() {
        return __depth(root);
    }
private:
    void __insert(std::shared_ptr<Node<T>> parent, const T value);
    void __inorder_traversal(std::shared_ptr<Node<T>> parent, std::vector<T>& vec);
    void __preorder_traversal(std::shared_ptr<Node<T>> parent, std::vector<T>& vec);
    void __postorder_traversal(std::shared_ptr<Node<T>> parent, std::vector<T>& vec);
    void __balance_tree(std::shared_ptr<Node<T>>& parent);
    void __balance(std::shared_ptr<Node<T>>& parent, std::vector<T> order, int left, int right);
    int __depth(std::shared_ptr<Node<T>>& parent);
    std::shared_ptr<Node<T>> root;
};

template<typename T>
void BST<T>::insert(const T value) {
    if (!root.get())
        root = std::make_shared<Node<T>>(value);
    else
        __insert(root, value);
}

template<typename T>
void BST<T>::__insert(std::shared_ptr<Node<T>> parent, const T value) {
    if (parent->value > value) {
        if (!parent->left.get())
            parent->left = std::make_shared<Node<T>>(value);
        else 
            __insert(parent->left, value);
    }
    else {
        if (!parent->right.get())
            parent->right = std::make_shared<Node<T>>(value);
        else 
            __insert(parent->right, value);
    }
}

template<typename T>
void BST<T>::__inorder_traversal(std::shared_ptr<Node<T>> parent, std::vector<T>& vec) {
    if (parent->left.get())
        __inorder_traversal(parent->left, vec);
    vec.push_back(parent->value);
    if (parent->right.get())
        __inorder_traversal(parent->right, vec);
}

template<typename T>
void BST<T>::__preorder_traversal(std::shared_ptr<Node<T>> parent, std::vector<T>& vec) {
    vec.push_back(parent->value);
    if (parent->left.get())
        __preorder_traversal(parent->left, vec);
    if (parent->right.get())
        __preorder_traversal(parent->right, vec);
}

template<typename T>
void BST<T>::__postorder_traversal(std::shared_ptr<Node<T>> parent, std::vector<T>& vec) {
    if (parent->left.get())
        __postorder_traversal(parent->left, vec);
    if (parent->right.get())
        __postorder_traversal(parent->right, vec);
    vec.push_back(parent->value);
}

template<typename T>
void BST<T>::__balance(std::shared_ptr<Node<T>>& parent, std::vector<T> order, int left, int right) {
    const int mid = (right - left) / 2 + left;
    parent = std::make_shared<Node<T>>(order[mid]);
    if ((mid - 1) - left >= 0)
        __balance(parent->left, order, left, mid-1);
    if (right - (mid + 1) >= 0)
        __balance(parent->right, order, mid+1, right);
}

template<typename T>
void BST<T>::__balance_tree(std::shared_ptr<Node<T>>& parent) {
    std::vector<T> inorder;
    inorder_traversal(inorder);
    __balance(root, inorder, 0, (int)inorder.size()-1);
}

template <typename T>
int BST<T>::__depth(std::shared_ptr<Node<T>> &parent)
{
    int d1 = 0, d2 = 0;
    if (parent->left.get())
        d1 = __depth(parent->left);
    if (parent->right.get())
        d2 = __depth(parent->right);
    int d = (d1 > d2) ? d1 : d2;
    return 1 + d;
}

int main() {

    constexpr int tree_size = 50;
    std::vector<int> nodeValues(tree_size);
    std::iota(nodeValues.begin(), nodeValues.end(), 1);

    BST<int> bst;
    for (auto i : nodeValues)
        bst.insert(i);
    
    std::vector<int> preorder;
    bst.preorder_traversal(preorder);

    std::cout << "After preorder_traversal (before balance): ";
    for (auto i : preorder)
        std::cout << i << ", ";
    std::cout << std::endl;

    std::cout << "BST depth (before balance): " << bst.depth() << std::endl;

    bst.balance_tree();

    preorder.clear();
    bst.preorder_traversal(preorder);

    std::cout << "After preorder_traversal (after balance): ";
    for (auto i : preorder)
        std::cout << i << ", ";
    std::cout << std::endl;

    std::cout << "BST depth (after balance): " << bst.depth() << std::endl;
}
