#include "env.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

Env::Env(const std::string &confpath)
{
	boost::property_tree::ptree pt;
	boost::property_tree::read_json(confpath + "/server.json", pt);
	boost::property_tree::ptree pt_master = pt.get_child("master");
	std::string host = pt_master.get<std::string>("host");
	master_value_["host"] = host;
	std::string port = pt_master.get<std::string>("port");
	master_value_["port"] = port;

	boost::property_tree::ptree pt_servers = pt.get_child("servers");
	for(boost::property_tree::ptree::iterator it = pt_servers.begin(); it != pt_servers.end(); ++it)  
	{
		std::string sname = (*it).first;
		server_kinds_.push_back(sname);
		boost::property_tree::ptree pt_ch = (*it).second;
		for (boost::property_tree::ptree::iterator jt = pt_ch.begin(); jt != pt_ch.end(); ++jt)
		{
			boost::property_tree::ptree pt_ch_ch = (*jt).second;
			std::string id = pt_ch_ch.get<std::string>("id");
			server_names_[sname].push_back(id);
			server_value_[id]["id"] = id;
			std::string host = pt_ch_ch.get<std::string>("host");
			server_value_[id]["host"] = host;
			std::string port = pt_ch_ch.get<std::string>("port");
			server_value_[id]["port"] = port;
		}
	}
}

Env::~Env()
{

}

std::string Env::get_master_value(const std::string &key)
{
	if (master_value_.find(key) != master_value_.end())
	{
		return master_value_[key];
	}
	return "";
}

std::string Env::get_server_value(const std::string &name, const std::string &key)
{
	if (name == "master")
	{
		if (master_value_.find(key) != master_value_.end())
		{
			return master_value_[key];
		}
	}
	else
	{
		if (server_value_.find(name) != server_value_.end())
		{
			if (server_value_[name].find(key) != server_value_[name].end())
			{
				return server_value_[name][key];
			}
		}
	}
	return "";
}

void Env::get_server_names(const std::string &kind, std::vector<std::string> &names)
{
	if (server_names_.find(kind) != server_names_.end())
	{
		names = server_names_[kind];
	}
}

void Env::get_server_kinds(std::vector<std::string> &kinds)
{
	kinds = server_kinds_;
}
