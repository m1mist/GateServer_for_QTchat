#include "Config.h"
#include "global.h"

Config::Config() {
	boost::filesystem::path current_path = boost::filesystem::current_path();
	boost::filesystem::path config_path = current_path / "config.ini";
	std::cout << "config path:" << config_path << '\n';
	// 使用Boost.PropertyTree来读取INI文件 
	boost::property_tree::ptree pt;
	boost::property_tree::read_ini(config_path.string(), pt);

	// 遍历INI文件中的所有section  
	for (const auto& section_pair : pt) {
		const ::std::string& section_name = section_pair.first;
		const boost::property_tree::ptree& section_tree = section_pair.second;
		// 对于每个section，遍历其所有的key-value对  
		Section section;
		for (const auto& k_v : section_tree) {
			const std::string& key = k_v.first;
			const std::string& value = k_v.second.get_value<std::string>();

			section.datas[key] = value;
		}
		config_map_[section_name] = section;
	}
	// 输出所有的section和key-value对  
	for (const auto& section_entry : config_map_) {
		const std::string& section_name = section_entry.first;
		Section section_config = section_entry.second;
		std::cout << "[" << section_name << "]" << '\n';
		for (const auto& key_value_pair : section_config.datas) {
			std::cout << key_value_pair.first << "=" << key_value_pair.second << '\n';
		}
	}

}
