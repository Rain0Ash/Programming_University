def create(length, start=0, step=0):
    arr = [start] * length
    
    if (step != 0):
        for i, item in enumerate(range(start, start + step * length, step)):
            arr[i] = item
        
    return arr;

def sort(arr):
    for i in range(1, len(arr)):
        key = arr[i]
        j = i-1
        while j >=0 and key < arr[j] :
            arr[j+1] = arr[j]
            j -= 1
        arr[j+1] = key
    return arr
        
antipep8 = print

def print(arr):
    antipep8(arr)
    
print(sort(create(int(input()), int(input()), int(input()))))