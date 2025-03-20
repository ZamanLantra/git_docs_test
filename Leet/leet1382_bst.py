import math

class BST:
    def __init__(self, value):
        self.value = value
        self.left = None
        self.right = None
    
    def insert(self, value):
        if value < self.value:
            if self.left is None:
                self.left = BST(value)
            else:
                self.left.insert(value)
        else:
            if self.right is None:
                self.right = BST(value)
            else:
                self.right.insert(value)
    
    def __inorder_traversal(self, values):
        if self.left is not None:
            self.left.__inorder_traversal(values)
        values.append(self.value)
        if self.right is not None:
            self.right.__inorder_traversal(values)

    def inorder_traversal(self):
        values = []
        self.__inorder_traversal(values)
        return values

    def __preorder_traversal(self, values):
        values.append(self.value)
        if self.left is not None:
            self.left.__preorder_traversal(values)
        if self.right is not None:
            self.right.__preorder_traversal(values)

    def preorder_traversal(self):
        values = []
        self.__preorder_traversal(values)
        return values
    
    def __postorder_traversal(self, values):     
        if self.left is not None:
            self.left.__postorder_traversal(values)
        if self.right is not None:
            self.right.__postorder_traversal(values)
        values.append(self.value)

    def postorder_traversal(self):     
        values = []
        self.__postorder_traversal(values)
        return values

    def find(self, value):
        if value < self.value:
            if self.left is None:
                return False
            else:
                self.left.find(value)
        elif value > self.value:
            if self.right is None:
                return False  
            else:
                self.right.find(value)
        else:
            return True
    
    def __balance(self, tree, lst):
        if len(lst) == 0:
            return
        elif len(lst) == 1:
            tree.insert(lst[0])
        elif len(lst) == 2:
            tree.insert(lst[0])
            tree.insert(lst[1])
        else:
            middle = len(lst)//2
            tree.insert(lst[middle])
            tree.__balance(tree, lst[:middle])
            tree.__balance(tree, lst[middle+1:])

    def balance(self):     
        lst = self.inorder_traversal()
        middle = len(lst)//2
        bst = BST(lst[middle])
        del lst[middle]                     # why? had to do because BST constructor takes one value parameter
        bst.__balance(bst, lst[:middle])
        bst.__balance(bst, lst[middle:])
        return bst

    def maxdepth(self):
        m1 = self.left.maxdepth() if self.left is not None else 0
        m2 = self.right.maxdepth() if self.right is not None else 0
        return 1 + max(m1, m2)
        
bst = BST(100)
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

print("preorder_traversal")
print(bst.preorder_traversal())

print("postorder_traversal")
print(bst.postorder_traversal())

bst = BST(1)
for i in range(2, 51):
    bst.insert(i)
print(f"preorder_traversal -- unbalancesd maxdepth {bst.maxdepth()} preorder_traversal -- {bst.preorder_traversal()}")

balanced = bst.balance()
print(f"preorder_traversal -- balanced maxdepth {balanced.maxdepth()} preorder_traversal -- {balanced.preorder_traversal()}")
