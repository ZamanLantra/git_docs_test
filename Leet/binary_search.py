
def __binary_search(array, value, left, right):
    if right - left < 0:
        return False
    mid = (right - left) // 2 + left
    if array[mid] == value:
        return True    
    return __binary_search(array, value, left, mid - 1) or __binary_search(array, value, mid + 1, right)

def binary_search(array, value):
    return __binary_search(array, value, 0, len(array) - 1)


array = []
for i in range(1, 51):
    array.append(i)

print(binary_search(array, 100))
print(binary_search(array, 50))
print(binary_search(array, 1))
print(binary_search(array, 25))
print(binary_search(array, 0))