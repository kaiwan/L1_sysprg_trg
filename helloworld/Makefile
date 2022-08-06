ALL := hello armhello
all: ${ALL}

CL := gcc
OUT := hello
ifeq ($(ARCH),arm)
  @echo "Building for ARM"
  CL := ${CROSS_COMPILE}gcc
  OUT := armhello
endif

hello: helloworld.c
	${CL} helloworld.c -o ${OUT} -Wall -Os
clean:
	rm -f ${OUT}
