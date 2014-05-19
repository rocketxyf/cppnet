#ifndef __RPC_SERVICE_H__
#define __RPC_SERVICE_H__

#include <map>
#include <string>
#include <list>
#include "interface.h"

class RpcClient;
class RpcServer;

namespace google
{
	namespace protobuf
	{
		class Message;
	}
}

namespace rpcproto
{
	class rpc;
}

class RpcService : public mmg::RpcService
{
public:
	RpcService(const std::string &name);

	~RpcService();

	void set_rif(mmg::RpcInterface *rif);

	void request(const std::string &name, int opcode, google::protobuf::Message *msg, ResponseFunc func);

	void push(const std::string &name, int opcode, google::protobuf::Message *msg);

	void response(const std::string &name, int id, google::protobuf::Message *msg, int error_code = 0, const std::string &error_text = "");

protected:
	void send(const std::string &name, rpcproto::rpc *msg);

	int dispacth(const ACE_Time_Value &tv);

private:
	int request_id_;
	std::string name_;
	std::map<std::string, RpcClient *> rpc_client_map_;
	RpcServer *rpc_server_;
	mmg::RpcInterface *rif_;
	std::map<int, ResponseFunc> response_func_map_;
	int timer_;
};

#endif // !__RPC_SERVICE_H__
