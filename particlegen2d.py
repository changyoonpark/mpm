from math import sqrt
from math import atan,pow
from math import cos,sin,tan,exp
from numpy import random
from matplotlib import pyplot as plt

class Vec2:

    def __init__ (self,x,y,tup = None):
        if tup is not None :
            self.x = tup[0]
            self.y = tup[1]
        else:
            self.x = x
            self.y = y

    def length(self):
        return sqrt(self.x*self.x+self.y*self.y)

    def dot(self,other):
        return self.x * other.x + self.y * other.y

    def dir(self):
        return self * (1./self.length())
        # return Vec2(self.x / self.length(), self.y / self.length())

    def __mul__ (self,other):
        return Vec2(self.x * other, self.y * other)

    def __rmul__ (self,other):
        return Vec2(self.x * other, self.y * other)

    def __truediv__(self,other):
        return Vec2(self.x / other, self.y / other)

    def __add__ (self,other):
        return Vec2(self.x + other.x, self.y + other.y)

    def __sub__ (self,other):
        return Vec2(self.x - other.x, self.y - other.y)
        
    def __neg__ (self):
        return Vec2(-self.x,-self.y)

    def __str__ (self):
        return "Vector : ({},{})".format(self.x, self.y)



template = "x:{},{},{} v:{},{},{} m:{},0,0\n"
text_file = open("./input/input.in", "w")

# h = 0.075
# h = 0.01
# h = 0.0125
# h = 0.025

r = 0.2
start = [0.5,0.01,0.05+0+r]

rho = 100
snowvolfrac = 0.2
totParticles = 5000
totmass = 3.141592 * (r*r) * rho * snowvolfrac
pos = []


class Elipse:


    def gaussianFlipperTester(self,pos):
        theta = atan( (pos[2] - self.center.y)/(pos[0] - self.center.x) )
        theta = theta + self.angle
        r = self.a*self.b/sqrt( (self.b * cos(theta))**2 + (self.a * sin(theta))**2 )
        d = ( Vec2(pos[0],pos[2]) - self.center ).length()
        # if d > r:
        #     return False

        x = r / d
        value = exp(-(x)**2) * 0.9
        if random.rand() < value: 
            return True
        else:
            return False

    def tester(self,pos):
        theta = atan( (pos[2] - self.center.y)/(pos[0] - self.center.x) )
        theta = theta + self.angle
        if (Vec2(pos[0],pos[2]) - self.center ).length() < self.a*self.b/sqrt( (self.b * cos(theta))**2 + (self.a * sin(theta))**2 ):
            return True
        else:
            return False

    def __init__(self,a,b,center,angle):
        self.a = a
        self.b = b
        self.center = center
        self.angle = angle




# def test(pos):
#     if sqrt( (posToAdd[0] - start[0]) * (posToAdd[0] - start[0]) +
#              (posToAdd[2] - start[2]) * (posToAdd[2] - start[2]) ) < r:

snowball = Elipse(r,r,Vec2(start[0],start[2]),3.14/2)

cracks = []

for i in range(0,0):
    posToAdd = (start[0] + 2 * (random.rand() - 0.5) * r, start[1], start[2] + 2 * (random.rand() - 0.5) * r)
 
    if snowball.tester(posToAdd):
        crackl = 0.1 * r * random.rand()
        crackw = crackl
        # crackl = 0.5 * r * random.rand()
        # crackw = 0.05 * r * random.rand()
        cracks.append(Elipse(crackl,crackw,Vec2(posToAdd[0],posToAdd[2]),3.141592 * random.rand()))


while len(pos) < totParticles:

    posToAdd = (start[0] + 2 * (random.rand() - 0.5) * r, start[1], start[2] + 2 * (random.rand() - 0.5) * r)

    foo = True
    if snowball.tester(posToAdd):
        for crack in cracks:
            # if (not crack.gaussianFlipperTester(posToAdd)):
            # if (crack.tester(posToAdd) ):
            if (crack.tester(posToAdd) and random.rand() > 0.4 ):
                foo = False
                break  
        if foo :    
            pos.append(posToAdd)
    # if snowball.gaussianFlipperTester(posToAdd):
    #     pos.append(posToAdd)
    print("adding particle... {}".format(len(pos)))

# for i in range(-3,4):
#     for j in range(-3,4):
#         for k in range(10,13):
#             # if ( sqrt((h*i)*(h*i) + (h*j)*(h*j) + (h*k)*(h*k)) < r ):
#             pos.append([start[0] + h * i, start[1] + h * j, start[2] + h * k])

pmass = totmass / len(pos);

xpos = [] 
ypos = []

for p in pos:
    # if p[2] > 2.3 :
    #     text_file.write(template.format(p[0],p[1],p[2],
    #                                     0,0,-10.,
    #                                     pmass))
    # else:
    #     text_file.write(template.format(p[0],p[1],p[2],
    #                                     0,0,0.,
    #                                     pmass))
    xpos.append(p[0])
    ypos.append(p[2])
    text_file.write(template.format(p[0],p[1],p[2],
                                    0.0,0.0,-5.0,
                                    pmass))

print("{} particles generated. total mass : {} kg".format(len(pos),totmass))


(fig, ax) = plt.subplots(figsize=(12, 12),nrows = 1,ncols = 1)      
ax.set_xlim(0.2, 0.8)
ax.set_ylim(0, 0.5)
ax.set_aspect('equal')
ax.scatter(xpos,ypos,color=(0.2,0.2,0.8),alpha=0.5,s=100,linewidth = 0,edgecolor= None)
plt.show()
text_file.close()