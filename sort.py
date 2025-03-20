
def bubble(lst):
    for i in range (len(lst)-1):
        for j in range(len(lst)-1-i):
            if lst[j] > lst[j+1]:
                lst[j], lst[j+1] = lst[j+1], lst[j]
    return lst

def insertion(lst):
    for i in range (len(lst)):
        j=i
        while j > 0 and lst[j-1] > lst[j]:
            lst[j], lst[j-1] = lst[j-1], lst[j]
            j -= 1
    return lst

def merge(lst1, lst2):
    l1 = 0
    l2 = 0
    newlist = []
    while l1 < len(lst1) and l2 < len(lst2):
        if lst1[l1] < lst2[l2]:
            newlist.append(lst1[l1])
            l1 += 1
        else:
            newlist.append(lst2[l2])
            l2 += 1
    newlist.extend(lst1[l1:])
    newlist.extend(lst2[l2:])
    return newlist

def mergeSort(lst):
    n=len(lst)
    if n == 1:
        return lst
    mid = len(lst) // 2
    arr1 = mergeSort(lst[:mid])
    arr2 = mergeSort(lst[mid:])
    return merge(arr1, arr2)

def findMinIdx(lst, left, right):
    if left == right:
        return left
    k = findMinIdx(lst, left+1, right)
    return left if lst[left] < lst[k] else k

def selectionSortRec(lst, idx = 0):
    n = len(lst)-1
    if idx == n:
        return
    minIdx = findMinIdx(lst, idx, n)
    if (minIdx != idx):
        lst[idx], lst[minIdx] = lst[minIdx], lst[idx]
    selectionSortRec(lst, idx+1)

def selectionSort(lst):
    for i in range(len(lst)-1):
        min_idx = i
        for j in range(i,len(lst)):
            if lst[j] < lst[min_idx]:
                min_idx = j
        if i != min_idx:
            lst[min_idx], lst[i] = lst[i], lst[min_idx]
    
    return lst
    
ll = [2,6,5,3,8,7,1,0]
print(f'insertion sort {insertion(ll.copy())}')
print(ll)

print(f'merge sort {mergeSort(ll.copy())}')
print(ll)

lst = ll.copy()
selectionSortRec(lst)
print(f'recursive selection sort {lst}')
print(ll)

print(f'selection sort {selectionSort(ll.copy())}')
print(ll)



