// hdr.h
#ifndef __MYHDR__
#define __MYHDR__
// Func protos
int s2_a();
int s3_engine();
int s4_audio();

// External vars
extern int g;

#define SHOW_VAR(v) do { \
	printf("&var=%p var=%d\n", &v, v); \
} while(0)
#endif
