from math import sqrt
from numpy import random

template = "x:{},{},{} v:{},{},{} m:{},0,0\n"
text_file = open("./input/input.in", "w")

start = [4,4,4]
h = 0.25
r = 3
rho = 400;
totmass = 3.141592 * 4./3. * (r*r*r) * rho
pos = []
noise = 0.25;
for i in range(-20,21):
    for j in range(-20,21):
        for k in range(-20,21):
            if ( sqrt((h*i)*(h*i) + (h*j)*(h*j) + (h*k)*(h*k)) < r ):
                pos.append([start[0] + h * (i + noise * (random.rand() - 1)), start[1] + h * (j + noise * (random.rand() - 1)), start[2] + h * (k + noise * (random.rand() - 1))])

# for i in range(-3,4):
#     for j in range(-3,4):
#         for k in range(10,13):
#             # if ( sqrt((h*i)*(h*i) + (h*j)*(h*j) + (h*k)*(h*k)) < r ):
#             pos.append([start[0] + h * i, start[1] + h * j, start[2] + h * k])

pmass = totmass / len(pos);

for p in pos:
    text_file.write(template.format(p[0],p[1],p[2],
                                    0,0,-10.,
                                    pmass))
text_file.close()