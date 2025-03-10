import sys

# usage dataset workload outputfile

data_set_file = open(sys.argv[1]).read().split('\n')
workload_file = open(sys.argv[2]).read().split('\n')
new_workload_file = open(sys.argv[3], 'w')

max_width, max_height= -1, -1
for line in data_set_file:
    if line != '':
        [min_x, min_y,  max_x, max_y] = [float(folan) for folan in line.split()]
        width, height = (max_x - min_x), (max_y - min_y)
        if width > max_width : max_width = width
        if height > max_height : max_height = height
        #if depth > max_depth: max_depth = depth

#max_width, max_height, max_depth = 1, 1, 1

print(max_width, max_height)

# extend the queries:
for line in workload_file:
    if line != '':
        [min_x, min_y, max_x, max_y] = [float(folan) for folan in line.split()]

        # new_max_x, new_max_y = (max_x + max_width), (max_y + max_height)
        new_min_x, new_min_y = (min_x - max_width), (min_y - max_height)
        if new_min_x < 0:
                new_min_x = 0
        if new_min_y < 0:
                new_min_y = 0

        string = ""
        string = str(new_min_x) + " " + str(new_min_y) + " " + str(max_x) + " " + str(max_y) + "\n"
        new_workload_file.write(string)
new_workload_file.close()


