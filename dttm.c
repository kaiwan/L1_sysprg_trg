#include <stdio.h>
#include <unistd.h>

const char Date_Prj_str[]       = __DATE__;
const char Time_Prj_str[]       = __TIME__;

int main()
{
	printf("%s: last build: %s %s\n", __FILE__, Date_Prj_str, Time_Prj_str);
	return 0;
}
