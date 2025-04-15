# https://www.youtube.com/watch?v=oTTzNMHM05I

import heapq

profits = [10, 5, 15, 7, 6, 18, 3]
weights = [ 2, 3,  5, 7, 1,  4, 1]

n = len(weights)
capacity = 15

x = [0] * n
heap = []

for i in range(n):
    heapq.heappush(heap, (-profits[i]/weights[i], i))

while heap:
    q = heapq.heappop(heap)  
    if capacity >= weights[q[1]]:
        capacity -= weights[q[1]]
        x[q[1]] = 1
    else:
        x[q[1]] = capacity / weights[q[1]]
        break

profit = 0

for i, xi in enumerate(x):
    profit += (profits[i] * xi)
    print(i, profits[i], xi, (profits[i] * xi))

print(x)
print(profit)