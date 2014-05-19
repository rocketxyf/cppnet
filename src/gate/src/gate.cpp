#include "interface.h"
#include "svc.h"

int ACE_TMAIN(int argc, ACE_TCHAR *argv[])
{
	if (argc < 3)
	{
		return 0;
	}
	svc::init(argv[1], argv[2]);

	svc::run();

	svc::fini();
	return 0;
}
