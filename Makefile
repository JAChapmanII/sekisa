OBJ=obj
LIB=lib
SRC=ex
BIN=bin

OBJS=${OBJ}/clock.o ${OBJ}/db.o ${OBJ}/err.o ${OBJ}/util.o ${OBJ}/web.o
OBJS+=${OBJ}/crypto.o

CXXFLAGS=-std=c++14 -I${LIB}
LDFLAGS=-lcurl -lsqlite3


# scrypt functionality
SCRYPT=scrypt-1.2.0
CFLAGS=-DHAVE_CONFIG_H -I${SCRYPT} -I${SCRYPT}/lib/util

LDFLAGS+=-lssl -lcrypto

CXXFLAGS+=-I${SCRYPT}
OBJS+=${OBJ}/scrypt-entropy.o ${OBJ}/scrypt-readpass.o ${OBJ}/scrypt-warnp.o
OBJS+=${OBJ}/scrypt-insecure_memzero.o ${OBJ}/scrypt-crypto_aesctr.o
OBJS+=${OBJ}/scrypt-crypto_entropy.o ${OBJ}/libcperciva_aesni_a-crypto_aes_aesni.o
OBJS+=${OBJ}/scrypt-crypto_aes.o ${OBJ}/scrypt-cpusupport_x86_aesni.o
OBJS+=${OBJ}/scrypt-cpusupport_x86_sse2.o ${OBJ}/scrypt-sha256.o
OBJS+=${OBJ}/scrypt-crypto_scrypt.o ${OBJ}/libscrypt_sse2_a-crypto_scrypt_smix_sse2.o
OBJS+=${OBJ}/scrypt-crypto_scrypt_smix.o ${OBJ}/scrypt-scryptenc.o
OBJS+=${OBJ}/scrypt-scryptenc_cpuperf.o ${OBJ}/scrypt-memlimit.o



main: dirs libsekisa.so web_ex

dirs:
	mkdir -p ${OBJ} ${BIN}

libsekisa.so: ${LIB}/*.cpp ${OBJS}
	${CXX} ${LDFLAGS} -shared ${OBJS} -o $@

web_ex: ${SRC}/web_ex.cpp ${OBJS}
	${CXX} ${CXXFLAGS} -o ${BIN}/$@ $^ ${LDFLAGS}

${OBJ}/%.o: ${LIB}/%.cpp ${SCRYPT}/config.h
	${CXX} ${CXXFLAGS} -fPIC -o $@ -c $<


# scrypt
${SCRYPT}.tgz:
	wget https://www.tarsnap.com/scrypt/scrypt-1.2.0.tgz
	# TODO; verify sha256 against sig

${SCRYPT}: ${SCRYPT}.tgz
	tar xf $<
	cd ${SCRYPT} && patch < ../scrypt-Makefile.in.patch

${SCRYPT}/config.h: ${SCRYPT}
	cd ${SCRYPT} && ./configure && make

${OBJ}/scrypt-%.o: ${SCRYPT}/config.h
	find ${SCRYPT} -name `basename "$@"` -print0 | xargs -0 -I{} cp {} ${OBJ}
${OBJ}/libcperciva%.o: ${SCRYPT}/config.h
	find ${SCRYPT} -name `basename "$@"` -print0 | xargs -0 -I{} cp {} ${OBJ}
${OBJ}/libscrypt%.o: ${SCRYPT}/config.h
	find ${SCRYPT} -name `basename "$@"` -print0 | xargs -0 -I{} cp {} ${OBJ}


clean:
	rm -f ${OBJ}/*.o ${BIN}/web_ex
	cd ${SCRYPT} && make clean && rm config.h

spotless: clean
	rm -fr ${SCRYPT}
	rm -fr ${SCRYPT}.tgz

