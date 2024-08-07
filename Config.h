#pragma once
#include "global.h"
struct Section {
	Section() = default;
	~Section();

	Section(const Section& other);
	Section& operator=(const Section& other);

	std::string operator[](const std::string& key);

	std::map<std::string, std::string> datas;
};
class Config {
public:
	Config(const Config& other);
	Config& operator=(const Config& other);
	~Config();

	static Config& Instance();
	Section operator[](const std::string& section);

private:
	Config();
	std::map<std::string, Section> config_map_;
};

