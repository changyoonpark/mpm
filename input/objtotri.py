f = open('bunnyobj', 'r+')
f2 = open('bunny.in', 'r+')

vertices = []
connect = []

for line in f:
	thisLine = line.split(" ")
	if thisLine[0] == "v" : 
		vertices.append((float(thisLine[1]) * 5,float(thisLine[2]) * 5,float(thisLine[3]) * 5))
	elif thisLine[0] == "f" : 
		connect.append((int(thisLine[1]),int(thisLine[2]),int(thisLine[3])))

for tri in connect:
	f2.write("{},{},{}|{},{},{}|{},{},{}|{},{},{}\n".format(
			vertices[tri[0]-1][0],vertices[tri[0]-1][1],vertices[tri[0]-1][2],
			vertices[tri[1]-1][0],vertices[tri[1]-1][1],vertices[tri[1]-1][2],
			vertices[tri[2]-1][0],vertices[tri[2]-1][1],vertices[tri[2]-1][2],
			0,0,0
			))

# print(connect)