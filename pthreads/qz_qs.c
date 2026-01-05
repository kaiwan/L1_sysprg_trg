/*------------------- Prg 1 -------------------------*/
void * libfunc(void *msg)
{
	long thrdnum=(void *)msg, i=3, j=7;
	// Note: no static/ global vars accessed at all here

	i += 5;
	j -= 5;
	// ...
	pthread_exit((void *)0);
}
int main(void)
{
	long t;
	// ...
	for (t = 0; t < NUMTHRDS; t++) {
		if (pthread_create(&threads[t], NULL, PrintStuff, (void *)t)) {
			// handle the error ...
		}
	}
	// ...
}

/*------------------- Prg 2 -------------------------*/
static long i=3, j=7;
void * libfunc(void *msg)
{
	long thrdnum=(void *)msg;

	i += 5;
	j -= 5;
	// ...
	pthread_exit((void *)0);
}
int main(void)
{
	long t;
	// ...
	for (t = 0; t < NUMTHRDS; t++) {
		if (pthread_create(&threads[t], NULL, PrintStuff, (void *)t)) {
			// handle the error ...
		}
	}
	// ...
}
