#ifndef __ENV_H__
#define __ENV_H__

#include "interface.h"
#include <map>

class Env : public mmg::Env
{
public:
	Env(const std::string &confpath);

	~Env();

	virtual std::string get_master_value(const std::string &key);

	virtual std::string get_server_value(const std::string &name, const std::string &key);

	virtual void get_server_names(const std::string &kind, std::vector<std::string> &names);

	virtual void get_server_kinds(std::vector<std::string> &kinds);

private:
	std::map<std::string, std::string> master_value_;
	std::map< std::string, std::map<std::string, std::string> > server_value_;
	std::map< std::string, std::vector<std::string> > server_names_;
	std::vector< std::string> server_kinds_;
};

#endif
