
class Node:
    def __init__(self, value):
        self.value = value
        self.right = None
        self.left = None
        self.height = 1

class AVL:
    
    def __init__(self):
        self.root = None 

    def height(self, node):
        if node is None:
            return 0
        return node.height
    
    def right_rotate(self, x):
        if x is None or x.left is None:  # Fix: Ensure `x.left` is not None
            return x
        y = x.left
        k = y.right
        y.right = x
        x.left = k
        x.height = 1 + max(self.height(x.left), self.height(x.right))
        y.height = 1 + max(self.height(y.left), self.height(y.right))
        return y
    
    def left_rotate(self, y):
        if y is None or y.right is None:  # Fix: Ensure `y.right` is not None
            return y
        x = y.right
        k = x.left
        x.left = y
        y.right = k
        x.height = 1 + max(self.height(x.left), self.height(x.right))
        y.height = 1 + max(self.height(y.left), self.height(y.right))
        return x

    def get_balance(self, node):
        if not node:
            return 0
        return self.height(node.left) - self.height(node.right)

    def __insert(self, parent, value):
        if parent is None:
            return Node(value)

        if parent.value < value:
            parent.right = self.__insert(parent.right, value)
        else:
            parent.left = self.__insert(parent.left, value)

        parent.height = 1 + max(self.height(parent.left), self.height(parent.right))

        balance = self.get_balance(parent)

        if balance > 1 and value < parent.left.value:
            return self.right_rotate(parent)

        if balance < -1 and value > parent.right.value:
            return self.left_rotate(parent)

        if balance > 1 and value > parent.left.value:
            parent.left = self.left_rotate(parent.left)
            return self.right_rotate(parent)

        if balance < -1 and value < parent.right.value:
            parent.right = self.right_rotate(parent.right)
            return self.left_rotate(parent)  
        
        return parent      

    def __find_min(self, parent):
        if parent.left is None:
            return parent
        return self.__find_min(parent.left)
    
    def find_min(self):
        return self.__find_min(self.root).value

    def insert(self, value):   
        self.root = self.__insert(self.root, value)

    def __delete(self, parent, value):
        if parent is None:
            return
        if parent.value < value:
            parent.right = self.__delete(parent.right, value)
        elif parent.value > value:
            parent.left = self.__delete(parent.left, value)
        else:
            if parent.left is None:
                return parent.right
            elif parent.right is None:
                return parent.left
            else:            
                inorder_successor = self.__find_min(parent.right)
                parent.value = inorder_successor.value
                parent.right = self.__delete(parent.right, inorder_successor.value)

        parent.height = 1 + max(self.height(parent.left), self.height(parent.right))
        balance = self.get_balance(parent)

        if balance > 1 and value < parent.left.value:
            return self.right_rotate(parent)
        if balance < -1 and value > parent.right.value:
            return self.left_rotate(parent)
        if balance > 1 and value > parent.left.value:
            parent.left = self.left_rotate(parent.left)
            return self.right_rotate(parent)
        if balance < -1 and value < parent.right.value:
            parent.right = self.right_rotate(parent.right)
            return self.left_rotate(parent)

        return parent 

    def delete(self, value):
        if self.root is not None:
            self.root = self.__delete(self.root, value)

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
    
avl = AVL()
avl.insert(100)
avl.insert(50)
avl.insert(150)
avl.insert(25)
avl.insert(200)
avl.insert(125)
avl.insert(28)
avl.insert(172)
avl.insert(15)
avl.insert(128)
avl.insert(121)
avl.insert(210)

print(f"inorder_traversal {avl.inorder_traversal()}")

print(f"find_min {avl.find_min()}")

avl.delete(128)
print(f"inorder_traversal {avl.inorder_traversal()}")

avl.delete(15)
print(f"inorder_traversal {avl.inorder_traversal()}")

print(f"find_min {avl.find_min()}")


avl = AVL()
for i in range(1, 50):
    avl.insert(i)

print(f"inorder_traversal (r2) {avl.inorder_traversal()}")
print(f"preorder_traversal (r2) {avl.preorder_traversal()}")
print(f"find_min {avl.find_min()}")

avl.delete(25)
print(f"inorder_traversal {avl.inorder_traversal()}")
print(f"find_min {avl.find_min()}")

avl.delete(1)
print(f"inorder_traversal {avl.inorder_traversal()}")
print(f"find_min {avl.find_min()}")

avl.delete(49)
print(f"inorder_traversal {avl.inorder_traversal()}")
print(f"find_min {avl.find_min()}")