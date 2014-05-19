#ifndef __SVC_H__
#define __SVC_H__

#include <string>

namespace mmg {
	class RpcService;
	class Timer;
	class Env;
	class Sig;
}

namespace svc {

	int init(const std::string &name, const std::string &confpath);

	int run();

	int fini();

	mmg::RpcService * rpc_service();

	mmg::Timer * timer();

	mmg::Env * env();

	mmg::Sig * sig();
}

#endif // !__SVC_H__
