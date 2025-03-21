
class Node:
    def __init__(self, value):
        self.value = value
        self.right = None
        self.left = None

class BST:
    
    def __init__(self):
        self.root = None

    def __insert(self, parent, value):
        if parent.value > value:
            if parent.left is None:
                parent.left = Node(value)
            else:
                self.__insert(parent.left, value)
        else:
            if parent.right is None:
                parent.right = Node(value)
            else:
                self.__insert(parent.right, value)

    def insert(self, value):
        if self.root is None:
            self.root = Node(value)
            return      
        else:
            self.__insert(self.root, value)
    
    def __inorder_traversal(self, parent, lst):
        if parent is None:
            return
        self.__inorder_traversal(parent.left, lst)
        lst.append(parent.value) 
        self.__inorder_traversal(parent.right, lst)

    def inorder_traversal(self):
        lst = []
        self.__inorder_traversal(self.root, lst)
        return lst

    def __preorder_traversal(self, parent, lst):
        if parent is None:
            return
        lst.append(parent.value) 
        self.__preorder_traversal(parent.left, lst)
        self.__preorder_traversal(parent.right, lst)

    def preorder_traversal(self):
        lst = []
        self.__preorder_traversal(self.root, lst)
        return lst

    def __postorder_traversal(self, parent, lst):
        if parent is None:
            return
        self.__postorder_traversal(parent.left, lst)
        self.__postorder_traversal(parent.right, lst)
        lst.append(parent.value) 

    def postorder_traversal(self):
        lst = []
        self.__postorder_traversal(self.root, lst)
        return lst    

    def __balance(self, order, left, right):
        if right - left < 0:
            return
        mid = (right - left) // 2 + left
        node = Node(order[mid])
        node.right = self.__balance(order, (mid+1), right)
        node.left = self.__balance(order, left, (mid-1))
        return node

    def balance(self):
        order = self.inorder_traversal()
        self.root = self.__balance(order, 0, len(order)-1)

bst = BST()
bst.insert(100)
bst.insert(50)
bst.insert(150)
bst.insert(25)
bst.insert(200)
bst.insert(125)
bst.insert(28)
bst.insert(172)
bst.insert(15)
bst.insert(128)
bst.insert(121)
bst.insert(210)

print("inorder_traversal")
print(bst.inorder_traversal())



bst = BST()
for i in range(1, 51):
    bst.insert(i)

print("preorder_traversal (before balance)")
print(bst.preorder_traversal())

bst.balance()

print("preorder_traversal (after balance)")
print(bst.preorder_traversal())