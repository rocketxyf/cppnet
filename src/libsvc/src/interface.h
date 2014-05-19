#ifndef __INTERFACE_H__
#define __INTERFACE_H__

#include <string>
#include <boost/function.hpp>
#include <google/protobuf/message.h>
#include <ace/Time_Value.h>
#include <ace/Event_Handler.h>

typedef boost::function<void (const std::string &data)> ResponseFunc;
typedef boost::function<int(const ACE_Time_Value & cur_time)> Callback;

#define BEGIN_MESSAGE_MAP \
	virtual void dispath_push_handle(int opcode, const std::string &data) { \
	switch(opcode) {

#define MESSAGE_HANDLER(opcode, handler) \
	case opcode: \
		handler(data); \
	break;

#define END_MESSAGE_MAP \
} \
}

#define BEGIN_REQUEST_MAP \
	virtual google::protobuf::Message * dispath_req_handle(int opcode, const std::string &data) { \
	switch(opcode) {

#define REQUEST_HANDLER(opcode, handler) \
	case opcode: \
		return handler(data); \
	break;

#define END_REQUEST_MAP \
} \
	return 0; \
}

namespace mmg {

	class RpcInterface
	{
	public:
		virtual void dispath_push_handle(int opcode, const std::string &data) = 0;

		virtual google::protobuf::Message * dispath_req_handle(int opcode, const std::string &data) = 0;
	};

	class RpcService
	{
	public:
		virtual void set_rif(mmg::RpcInterface *rif) = 0;

		virtual void request(const std::string &name, int opcode, google::protobuf::Message *msg, ResponseFunc func) = 0;

		virtual void push(const std::string &name, int opcode, google::protobuf::Message *msg) = 0;

		virtual void response(const std::string &name, int id, google::protobuf::Message *msg, int error_code = 0, const std::string &error_text = "") = 0;
	};

	class Timer
	{
	public:
		virtual int schedule(Callback task, int expire, const std::string &name) = 0;

		virtual int cancel(int expiry_id) = 0;

		virtual uint64_t now(void) = 0;

		virtual int weekday(void) = 0;

		virtual int day(void) = 0;

		virtual int month(void) = 0;

		virtual bool trigger_time(uint64_t old_time, int hour, int minute) = 0;

		virtual bool trigger_week_time(uint64_t old_time) = 0;

		virtual bool trigger_month_time(uint64_t old_time) = 0;

		virtual int run_day(uint64_t old_time) = 0;
	};

	class Env
	{
	public:
		virtual std::string get_master_value(const std::string &key) = 0;

		virtual std::string get_server_value(const std::string &name, const std::string &key) = 0;

		virtual void get_server_names(const std::string &kind, std::vector<std::string> &names) = 0;

		virtual void get_server_kinds(std::vector<std::string> &kinds) = 0;
	};

	class Sig
	{
	public:
		virtual void set_sig_handle(ACE_Event_Handler *handler) = 0;
	};
}

#endif // !__RPC_INTERFACE_H__
