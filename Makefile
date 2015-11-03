OBJ=obj
LIB=lib
SRC=ex
BIN=bin

OBJS=${OBJ}/clock.o ${OBJ}/db.o ${OBJ}/err.o ${OBJ}/util.o ${OBJ}/web.o

CXXFLAGS=-std=c++14 -I${LIB}
LDFLAGS=-lcurl -lsqlite3

main: dirs libsekisa.so web_ex

dirs:
	mkdir -p ${OBJ} ${BIN}

libsekisa.so: ${LIB}/*.cpp
	${CXX} ${CXXFLAGS} -shared -fPIC $^ -o $@

web_ex: ${SRC}/web_ex.cpp ${OBJS}
	${CXX} ${CXXFLAGS} -o ${BIN}/$@ $^ ${LDFLAGS}

${OBJ}/%.o: ${LIB}/%.cpp
	${CXX} ${CXXFLAGS} -o $@ -c $^

clean:
	rm -f ${OBJ}/*.o ${BIN}/web_ex

