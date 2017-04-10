import os
import struct
import numpy as np
f = open(os.path.expanduser('./build/outputs/grid_t_704.txt'), 'r')


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
    position = (int((float(positionString[0]) + h * 0.0001) / h),
                int((float(positionString[1]) + h * 0.0001) / h),
                int((float(positionString[2]) + h * 0.0001) / h))
    densityDict[position] = density

foo = np.zeros((41,41),dtype = float)

for k in range(0, nz):
    for j in range(0, ny):
        for i in range(0, nx):
            dens = densityDict.get((i, j, k))
            if dens is not None:
                dens = dens / maxDensity
                # if dens < 0.5:
                    # dens = float(0);
                densityList[(nx * ny) * k + nx * j + i] = dens * dens * dens * dens
            else:
                dens = float(0)
                densityList[(nx * ny) * k + nx * j + i] = dens

            if k == nz // 2:
                foo[i,j] = dens
                # print("{},{} : {}".format(i,j,dens))


fout = open(os.path.expanduser('./build/outputs/snowball.vol'), 'wb')

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

f = open(os.path.expanduser('./build/outputs/particle_t_704.txt'), 'r')
fparticle = open(os.path.expanduser('./build/outputs/mitsuba_particle_t_704.txt'), 'w+')
form = "<shape type=\"sphere\">\
		<point name=\"center\" x=\"{}\" y=\"{}\" z=\"{}\"/>\
		<float name=\"radius\" value=\"{}\"/>\
        <bsdf type=\"diffuse\">\
        <rgb name=\"diffuseReflectance\" value=\"0.4, 0.5, 0.6\"/>\
		</bsdf>\
    	</shape>\n"

for line in f:
    splitted = line.split(",")
    fparticle.write(form.format(float(splitted[0]),float(splitted[1]),float(splitted[2]),0.1 * h * np.random.rand() ))

print("to mitsuba VOL format conversion done")

