# with open('meshes/land.obj', 'r') as infile:
#     x_values = []
#     y_values = []
#     for line in infile.readlines():
#         if line.startswith('v '):
#             words = line.split()
#             x_values.append(float(words[1]))
#             y_values.append(float(words[3]))

#     minx = min(x_values)
#     maxx = max(x_values)
#     miny = min(y_values)
#     maxy = max(y_values)

#     new_x_values = [(x - minx) / (maxx - minx) for x in x_values]
#     new_y_values = [(y - miny) / (maxy - miny) for y in y_values]

# with open('meshes/land.obj', 'r') as infile:
#     i = 0
#     for line in infile.readlines():
#         if line.startswith('vt '):
#             print('vt {} {}'.format(new_x_values[i], new_y_values[i]))
#             i += 1;
#         else:
#             print(line, end='')


# with open('meshes/land.obj', 'r') as infile:
#     x_values = []
#     y_values = []
#     for line in infile.readlines():
#         if line.startswith('vt '):
#             words = line.split()
#             x_values.append(float(words[1]))
#             y_values.append(float(words[2]))

#     minx = min(x_values)
#     maxx = max(x_values)
#     miny = min(y_values)
#     maxy = max(y_values)

#     new_x_values = [(x - minx) / (maxx - minx) for x in x_values]
#     new_y_values = [(y - miny) / (maxy - miny) for y in y_values]

# with open('meshes/land.obj', 'r') as infile:
#     i = 0
#     for line in infile.readlines():
#         if line.startswith('vt '):
#             print('vt {} {}'.format(new_x_values[i], new_y_values[i]))
#             i += 1;
#         else:
#             print(line, end='')

with open('meshes/land.obj', 'r') as infile:
    for line in infile.readlines():
        if line.startswith('v '):
            words = line.split()
            print('v {} {} {}'.format(float(words[1]), float(words[2]), float(words[3]) * -1))
        elif line.startswith('vn '):
            words = line.split()
            print('vn {} {} {}'.format(float(words[1]), float(words[2]), float(words[3]) * -1))
        else:
            print(line, end='')

