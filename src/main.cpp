#include <core/application.hpp>

/*
* ============= ENTRY POINT =============
*/
int main(int argc, char** argv)
{
	if (!Application::Run())
	{
		#ifdef SHOW_CONSOLE
		printf("Something went wrong when launching Cloud.\n");
		#endif

		return 1;
	}
}