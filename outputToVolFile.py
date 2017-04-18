import os
import struct
import numpy as np

def getVolumeFile(fileDir,outputVolFileDir):
    parts = 16
    domainSize = (1.0,1.0,1.0)
    maxDensity = 0.0
    h = 0.025
    densityDict = dict()
    nx = int(domainSize[0]/h)+1
    ny = int(domainSize[1]/h)+1
    nz = int(domainSize[2]/h)+1
    densityList = [None]*(nx * ny * nz)

    for part in range(0,parts):
        f = open(os.path.expanduser(fileDir)+"_part_{}.txt".format(part), 'r')
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
        f.close()


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



    fout = open(os.path.expanduser(outputVolFileDir), 'wb')

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

    # f = open(os.path.expanduser('./build/outputs/particle_t_704.txt'), 'r')
    # fparticle = open(os.path.expanduser('./build/outputs/mitsuba_particle_t_704.txt'), 'w+')
    # form = "<shape type=\"sphere\">\
    #         <point name=\"center\" x=\"{}\" y=\"{}\" z=\"{}\"/>\
    #         <float name=\"radius\" value=\"{}\"/>\
    #         <bsdf type=\"diffuse\">\
    #         <rgb name=\"diffuseReflectance\" value=\"0.4, 0.5, 0.6\"/>\
    #         </bsdf>\
    #         </shape>\n"

    # for line in f:
    #     splitted = line.split(",")
    #     fparticle.write(form.format(float(splitted[0]),float(splitted[1]),float(splitted[2]),0.1 * h * np.random.rand() ))

    print("to mitsuba VOL format conversion done")



for t in range(0,600):
    name = "./build/outputs/grid_t_{}".format(t)
    volName = "snowball.vol".format(t) 
    getVolumeFile(name,volName)
    os.system("mitsuba snowball.xml ./outputframes/snowball_t_{}.png".format(t))
