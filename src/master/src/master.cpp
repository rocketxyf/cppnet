#include "svc.h"
#include <vector>
#include "interface.h"
#include <ace/Process_Manager.h>
#include <ace/Signal.h>

int ACE_TMAIN(int argc, ACE_TCHAR *argv[])
{
	if (argc < 2)
	{
		return 0;
	}
	svc::init("master", argv[1]);

	std::vector<ACE_Process *> processes;
	std::vector<std::string> kinds;
	svc::env()->get_server_kinds(kinds);
	for (int i = 0; i < kinds.size(); ++i)
	{
		std::vector<std::string> names;
		svc::env()->get_server_names(kinds[i], names);
		for (int j = 0; j < names.size(); ++j)
		{
			ACE_Process_Options options;
			std::string cmd = kinds[i] + " " + names[j] + " " + argv[1];
			options.command_line(cmd.c_str());
			ACE_Process *process = new ACE_Process();
			process->spawn(options);
			processes.push_back(process);
		}
	}

	svc::run();

	for (int i = 0; i < processes.size(); ++i)
	{
		processes[i]->kill();
	}

	for (int i = 0; i < processes.size(); ++i)
	{
		processes[i]->wait();
		delete processes[i];
	}

	svc::fini();

	return 0;
}
