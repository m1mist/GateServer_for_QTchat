#pragma once
#include "global.h"
struct Section {
	Section() {};
	~Section() { datas.clear(); };

	Section(const Section& other) {
		datas = other.datas;
	}

	Section& operator=(const Section& other) {
		if (&other == this) {
			return *this;
		}
		this->datas = other.datas;
	}

	std::string operator[](const std::string& key) {
		if (datas.find(key) == datas.end()) {
			return "";
		}
		return datas[key];
	}

	std::map<std::string, std::string> datas;
};
class Config {
public:
	Config();

	Config(const Config& other) { config_map_ = other.config_map_; };

	Config& operator=(const Config& other) {
		if (&other == this) {
			return *this;
		}
		config_map_ = other.config_map_;
	};

	~Config() {
		config_map_.clear();
	}

	Section operator[](const std::string& section) {
		if (config_map_.find(section) == config_map_.end()) {
			return Section();
		}
		return config_map_[section];
	};
private:

	std::map<std::string, Section> config_map_;
};

