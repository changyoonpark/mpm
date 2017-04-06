from math import sqrt
from numpy import random

template = "x:{},{},{} v:{},{},{} m:{},0,0\n"
text_file = open("./input/input.in", "w")

start = [4,4,3.5]
h = 0.5
r = 2.5
rho = 400;
# totmass = 3.141592 * 4./3. * (r*r*r) * rho
totmass = (h * 10) * (h * 10) * (h * 10) * rho 
# totmass = 3.14 * (2.5) * (2.5) * (h * 8) * rho
pos = []
noise = 0.05;
for i in range(-5,6):
    for j in range(-5,6):
        for k in range(-5,6):
            # if ( sqrt((h*i)*(h*i) + (h*k)*(h*k)) < r ):
            pos.append([start[0] + h * (i + noise * (random.rand() - 1)), start[1] + h * (j + noise * (random.rand() - 1)), start[2] + h * (k + noise * (random.rand() - 1))])

# for i in range(-3,4):
#     for j in range(-3,4):
#         for k in range(10,13):
#             # if ( sqrt((h*i)*(h*i) + (h*j)*(h*j) + (h*k)*(h*k)) < r ):
#             pos.append([start[0] + h * i, start[1] + h * j, start[2] + h * k])

pmass = totmass / len(pos);

for p in pos:
    # if p[2] > 2.3 :
    #     text_file.write(template.format(p[0],p[1],p[2],
    #                                     0,0,-10.,
    #                                     pmass))
    # else:
    #     text_file.write(template.format(p[0],p[1],p[2],
    #                                     0,0,0.,
    #                                     pmass))
    text_file.write(template.format(p[0],p[1],p[2],
                                    0,0,0.,
                                    pmass))

print("{} particles generated".format(len(pos)))

text_file.close()