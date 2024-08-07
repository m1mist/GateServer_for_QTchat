#include "Config.h"
#include "global.h"

Section::~Section() { datas.clear(); }

Section::Section(const Section& other) {
	datas = other.datas;
}

Section& Section::operator=(const Section& other) {
	if (&other == this) {
		return *this;
	}
	this->datas = other.datas;
	return *this;
}

std::string Section::operator[](const std::string& key) {
	if (datas.find(key) == datas.end()) {
		return "";
	}
	return datas[key];
}

Config::Config() {
	boost::filesystem::path current_path = boost::filesystem::current_path();
	boost::filesystem::path config_path = current_path / "config.ini";
	std::cout << "config path:" << config_path << '\n';
	// ʹ��Boost.PropertyTree����ȡINI�ļ� 
	boost::property_tree::ptree pt;
	boost::property_tree::read_ini(config_path.string(), pt);

	// ����INI�ļ��е�����section  
	for (const auto& section_pair : pt) {
		const ::std::string& section_name = section_pair.first;
		const boost::property_tree::ptree& section_tree = section_pair.second;
		// ����ÿ��section�����������е�key-value��  
		Section section;
		for (const auto& k_v : section_tree) {
			const std::string& key = k_v.first;
			const std::string& value = k_v.second.get_value<std::string>();

			section.datas[key] = value;
		}
		config_map_[section_name] = section;
	}
	// ������е�section��key-value��  
	for (const auto& section_entry : config_map_) {
		const std::string& section_name = section_entry.first;
		Section section_config = section_entry.second;
		std::cout << "[" << section_name << "]" << '\n';
		for (const auto& key_value_pair : section_config.datas) {
			std::cout << key_value_pair.first << "=" << key_value_pair.second << '\n';
		}
	}

}

Config::Config(const Config& other) { config_map_ = other.config_map_; }

Config& Config::operator=(const Config& other) {
	if (&other == this) {
		return *this;
	}
	config_map_ = other.config_map_;
	return *this;
}

Config::~Config() {
	config_map_.clear();
}

Config& Config::Instance() {
	static Config cfg_mgr;
	return cfg_mgr;
}

Section Config::operator[](const std::string& section) {
	if (config_map_.find(section) == config_map_.end()) {
		return {};
	}
	return config_map_[section];
}
