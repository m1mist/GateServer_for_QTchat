#pragma once
#include "global.h"
struct Section {
	Section();
	~Section() { data.clear(); };

	Section(const Section& other) {
		data = other.data;
	}

	Section& operator=(const Section& other) {
		if (&other == this) {
			return *this;
		}
		this->data = other.data;
	}

	std::map<std::string, std::string> data;
	std::string operator[](const std::string& key) {
		if (data.find(key) == data.end()) {
			return "";
		}
		return data[key];
	}
};
class Config {

};

