
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




ll = [2,6,5,3,8,7,1,0]
print(insertion(ll))

print(mergeSort(ll))
