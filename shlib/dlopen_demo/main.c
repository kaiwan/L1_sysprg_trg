#include <stdio.h>
#include <dlfcn.h>

int main(void)
{
	void *handle;
	// Define a function pointer matching the signature: void function(const char*)
	void (*my_function)(const char *);
	char *error;

	// 1. Open the private shared library
	// We use "./" to explicitly tell it to look in the current working directory
	handle = dlopen("./libmyplugin.so", RTLD_LAZY);
	if (!handle) {
		fprintf(stderr, "Error loading library: %s\n", dlerror());
		return 1;
	}
	// 2. Clear any existing error, then locate the function symbol
	dlerror();
	my_function = (void (*)(const char *))dlsym(handle, "print_message");

	if ((error = dlerror()) != NULL) {
		fprintf(stderr, "Error locating symbol: %s\n", error);
		dlclose(handle);
		return 1;
	}
	// 3. Call the loaded function
	my_function("Developer");

	// 4. Clean up and close the library
	dlclose(handle);
	return 0;
}
