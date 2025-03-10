data_set_file = open("testData.txt").read().split('\n')
workload_file = open('testQueries.txt').read().split('\n')

new_workload_file = open('testQueries_extended.txt', 'w')

# get max width and height
max_width, max_height= -1, -1
for line in data_set_file:
    if line != '':
        [min_x, min_y,  max_x, max_y] = [float(folan) for folan in line.split()]
        width, height = (max_x - min_x), (max_y - min_y)
        if width > max_width : max_width = width
        if height > max_height : max_height = height

print(max_width, max_height)

# extend the queries:
for line in workload_file:
    if line != '':
        [min_x, min_y, max_x, max_y] = [float(folan) for folan in line.split()]

        new_max_x, new_max_y = (max_x + max_width), (max_y + max_height)

        string = ""
        string = str(min_x) + " " + str(min_y) + " " + str(new_max_x) + " " + str(new_max_y) + "\n"
        new_workload_file.write(string)
new_workload_file.close()
