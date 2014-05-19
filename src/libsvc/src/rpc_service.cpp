#include "rpc_service.h"
#include "rpc_client.h"
#include "rpc_server.h"
#include <google/protobuf/message.h>
#include <boost/bind.hpp>
#include "rpc.pb.h"
#include "svc.h"

RpcService::RpcService(const std::string &name)
	: request_id_(0)
	, name_(name)
	, rpc_server_(0)
	, rif_(0)
{
	
	std::string host = svc::env()->get_server_value(name, "host");
	std::string port = svc::env()->get_server_value(name, "port");
	if (host == "" || port == "")
	{
		return;
	}
	std::string addr = "tcp://" + host + ":" + port;
	rpc_server_ = new RpcServer(name, addr);
	timer_ = svc::timer()->schedule(boost::bind(&RpcService::dispacth, this, _1), 30, "rpc_service");

}

RpcService::~RpcService()
{
	if (timer_ != -1)
	{
		svc::timer()->cancel(timer_);
		timer_ = -1;
	}
	if (rpc_server_)
	{
		delete rpc_server_;
	}
	for (std::map<std::string, RpcClient *>::iterator it = rpc_client_map_.begin(); it != rpc_client_map_.end(); ++it)
	{
		RpcClient *rpc_client_ = (*it).second;
		delete rpc_client_;
	}
	rpc_client_map_.clear();
}

void RpcService::set_rif(mmg::RpcInterface *rif)
{
	rif_ = rif;
}

void RpcService::request(const std::string &name, int opcode, google::protobuf::Message *msg, ResponseFunc func)
{
	rpcproto::rpc rpc;
	rpc.set_type(rpcproto::REQUESST);
	rpcproto::request *req = rpc.mutable_req();
	req->set_name(name_);
	req->set_id(request_id_);
	req->set_opcode(opcode);
	if (msg)
	{
		std::string s;
		msg->SerializeToString(&s);
		req->set_msg(s);
	}

	send(name, &rpc);

	response_func_map_[request_id_] = func;
	request_id_++;
}

void RpcService::push(const std::string &name, int opcode, google::protobuf::Message *msg)
{
	rpcproto::rpc rpc;
	rpc.set_type(rpcproto::PUSH);
	rpcproto::request *req = rpc.mutable_req();
	req->set_name(name_);
	req->set_id(0);
	req->set_opcode(opcode);
	if (msg)
	{
		std::string s;
		msg->SerializeToString(&s);
		req->set_msg(s);
	};

	send(name, &rpc);
}

void RpcService::response(const std::string &name, int id, google::protobuf::Message *msg, int error_code, const std::string &error_text)
{
	rpcproto::rpc rpc;
	rpc.set_type(rpcproto::RESPONSE);
	rpcproto::response *rep = rpc.mutable_rep();
	rep->set_name(name_);
	rep->set_id(id);
	if (msg)
	{
		std::string s;
		msg->SerializeToString(&s);
		rep->set_msg(s);
	}
	if (error_code)
	{
		rpcproto::error *err = rep->mutable_error();
		err->set_code(error_code);
		err->set_text(error_text);
	}

	send(name, &rpc);
}

void RpcService::send(const std::string &name, rpcproto::rpc *msg)
{
	RpcClient *rc = 0;
	std::map<std::string, RpcClient *>::iterator it = rpc_client_map_.find(name);
	if (it == rpc_client_map_.end())
	{
		std::string host = svc::env()->get_server_value(name, "host");
		std::string port = svc::env()->get_server_value(name, "port");
		if (host == "" || port == "")
		{
			return;
		}
		std::string addr = "tcp://" + host + ":" + port;
		rc = new RpcClient(name, addr);
		rpc_client_map_[name] = rc;
	}
	else
	{
		rc = (*it).second;
	}
	std::string s;
	msg->SerializeToString(&s);
	rc->putq(s);
}

int RpcService::dispacth(const ACE_Time_Value &tv)
{
	std::list<std::string> msgs;
	rpc_server_->gutq(msgs);

	if (!rif_)
	{
		return 0;
	}

	for (std::list<std::string>::iterator it = msgs.begin(); it != msgs.end(); ++it)
	{
		std::string data = *it;
		rpcproto::rpc rpc;
		rpc.ParseFromString(data);
		if (rpc.type() == rpcproto::REQUESST)
		{
			const rpcproto::request req = rpc.req();
			google::protobuf::Message *msg = rif_->dispath_req_handle(req.opcode(), req.msg());
			response(req.name(), req.id(), msg);
		}
		else if (rpc.type() == rpcproto::PUSH)
		{
			const rpcproto::request req = rpc.req();
			rif_->dispath_push_handle(req.opcode(), req.msg());
		}
		else if (rpc.type() == rpcproto::RESPONSE)
		{
			const rpcproto::response rep = rpc.rep();
			std::map<int, ResponseFunc>::iterator it = response_func_map_.find(rep.id());
			if (it != response_func_map_.end())
			{
				ResponseFunc f = (*it).second;
				f(rep.msg());
				response_func_map_.erase(it);
			}
		}
	}
	return 0;
}
