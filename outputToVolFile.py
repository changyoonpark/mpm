import os
import struct
import numpy as np
f = open(os.path.expanduser('~/Desktop/outputs/grid_t_0.txt'), 'r')


domainSize = (1.0,1.0,1.0)
maxDensity = 0.0
h = 0.025
densityDict = dict()
nx = int(domainSize[0]/h)+1
ny = int(domainSize[1]/h)+1
nz = int(domainSize[2]/h)+1

densityList = [None]*(nx * ny * nz)

for line in f:
    splitted = line.split(' ')
    density = float(splitted[1])

    if density > maxDensity :
        maxDensity = density

    positionString = splitted[0].split(',')
    position = (int(float(positionString[0]) / h),
                int(float(positionString[1]) / h),
                int(float(positionString[2]) / h))
    densityDict[position] = density
foo = np.zeros((41,41),dtype = float)
for k in range(0, nz):
    for j in range(0, ny):
        for i in range(0, nx):
            dens = densityDict.get((i, j, k))
            if dens is not None:
                dens = dens / maxDensity
                densityList[(nx * ny) * k + nx * j + i] = dens
            else:
                dens = float(0)
                densityList[(nx * ny) * k + nx * j + i] = dens

            if k == nz // 2:
                foo[i,j] = dens
                # print("{},{} : {}".format(i,j,dens))

print(foo[18:24,18:24])
fout = open(os.path.expanduser('~/Desktop/outputs/grid_t_0.vol'), 'wb')

buf = struct.pack('%sf' % len(densityList), *densityList)
string = b'VOL'
string += (3).to_bytes(1, byteorder='little')
string += (1).to_bytes(4, byteorder='little')
string += (nx).to_bytes(4, byteorder='little')
string += (ny).to_bytes(4, byteorder='little')
string += (nz).to_bytes(4, byteorder='little')
string += (1).to_bytes(4, byteorder='little')
string += struct.pack('f', 0.0)
string += struct.pack('f', 0.0)
string += struct.pack('f', 0.0)
string += struct.pack('f', domainSize[0])
string += struct.pack('f', domainSize[1])
string += struct.pack('f', domainSize[2])

fout.write(string)
fout.write(buf)
print("to mitsuba VOL format conversion done")

