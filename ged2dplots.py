import os
import struct
import numpy as np
from matplotlib import pyplot as plt

def shiftData(data,shift) :
    temp = list(data)
    for i in range(0,len(data)):
        temp[i] = temp[i] + shift
    return temp

def getData(datatype,time,parts) :
    fileDir = "./build/outputs/{}_t_{}".format(datatype,time)
    fileDir += "_part_"

    foo = dict()

    foo["xpos"] = []
    foo["ypos"] = []
    foo["data"] = []

    for i in range(0,parts):
        print(fileDir + str(i) + ".txt")
        f = open(os.path.expanduser(fileDir + str(i) + ".txt"), 'r')        
        for line in f:
            # print(line)
            splitted = line.split(",")         
            foo["xpos"].append(float(splitted[0]))
            foo["ypos"].append(float(splitted[2]))
            if len(splitted) == 4 :
            	foo["data"].append(float(splitted[3]))
        f.close()

    return foo

# def particlePlot(particleData,ax):

outputInterval = 20
parts = 4
pre = 0
dt = 0.0002
for t in range(0,3800,outputInterval):

    if t == 0:
        particleData = getData("particles",t,parts)
        start = shiftData(particleData["ypos"], 1.25)

        for i in range(50,int(0.5/(dt * outputInterval))):
            particleData["ypos"] = shiftData(start, - 0.5 * 10 * (i*dt*outputInterval) ** 2 )
            (fig, ax) = plt.subplots(figsize=(12, 12),nrows = 1,ncols = 1)      
            ax.set_xlim(0, 1)
            ax.set_ylim(0, 2)
            ax.set_aspect('equal')
            ax.scatter(particleData["xpos"],particleData["ypos"],color=(0.2,0.2,0.8),alpha=0.5,s=10,edgecolor= None)
            # plt.show()
            plt.savefig('./build/outputs/plots/{}.png'.format(int(pre + t/outputInterval)))
            plt.close()
            print(i*dt*outputInterval)
            pre += 1

    particleData = getData("particles",t,parts)
    (fig, ax) = plt.subplots(figsize=(12, 12),nrows = 1,ncols = 1)      
    ax.set_xlim(0, 1)
    ax.set_ylim(0, 2)
    ax.set_aspect('equal')
    ax.scatter(particleData["xpos"],particleData["ypos"],color=(0.2,0.2,0.8),alpha=0.5,s=10,edgecolor= None)
    # plt.show()
    plt.savefig('./build/outputs/plots/{}.png'.format(int(pre + t/outputInterval)))
    plt.close()
# gridData     = getData("grid",t,parts)
