f2 = open('./input/box.in', 'r+')

def foo(x1,x2,x3,v) : 
    form = "{},{},{}|{},{},{}|{},{},{}|{},{},{}\n".format(x1[0],x1[1],x1[2],
                                                        x2[0],x2[1],x2[2],
                                                        x3[0],x3[1],x3[2],
                                                         v[0], v[0], v[0])
    return form


d = 8.0
e = 0.0
v = (0,0,0)

f2.write(foo((e,e,e),(e,e,d-e),(d-e,e,e),v))
f2.write(foo((d-e,e,e),(e,e,d-e),(d-e,e,d-e),v))

f2.write(foo((d-e,e,e),(d-e,d-e,e),(d-e,e,d-e),v))
f2.write(foo((d-e,d-e,e),(d-e,d-e,d-e),(d-e,e,d-e),v))

f2.write(foo((d-e,d-e,e),(e,d-e,e),(d-e,d-e,d-e),v))
f2.write(foo((d-e,d-e,d-e),(e,d-e,e),(e,d-e,d-e),v))

f2.write(foo((e,e,e),(e,d-e,e),(e,e,d-e),v))
f2.write(foo((e,e,d-e),(e,d-e,e),(e,d-e,d-e),v))

f2.write(foo((e,e,e),(d-e,e,e),(e,d-e,e),v))
f2.write(foo((d-e,e,e),(d-e,d-e,e),(e,d-e,e),v))