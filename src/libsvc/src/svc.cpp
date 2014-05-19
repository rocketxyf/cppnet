#include "svc.h"
#include "interface.h"
#include "rpc_service.h"
#include "timer.h"
#include "env.h"
#include "sig.h"
#include <ace/Reactor.h>

namespace svc {

	RpcService *rs_;
	Timer *timer_;
	Env *env_;
	Sig *sig_;
	std::string name_;

	int init(const std::string &name, const std::string &confpath)
	{
		name_ = name;
		env_ = new Env(confpath);
		timer_ = new Timer();
		rs_ = new RpcService(name);
		sig_ = new Sig();

		printf("%s init\n", name_.c_str());

		return 0;
	}

	int run()
	{
		ACE_Reactor::instance()->run_reactor_event_loop();
		return 0;
	}

	int fini()
	{
		delete sig_;
		delete rs_;
		delete timer_;
		delete env_;

		printf("%s fini\n", name_.c_str());
		return 0;
	}

	mmg::RpcService * rpc_service()
	{
		return rs_;
	}

	mmg::Timer * timer()
	{
		return timer_;
	}

	mmg::Env * env()
	{
		return env_;
	}

	mmg::Sig * sig()
	{
		return sig_;
	}
}
