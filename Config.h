#pragma once

#include "encrypt.h"
#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <unordered_set>
#include <unordered_map>
#include <regex>
#include <algorithm>

class Config {
public:
	static Config& inst() {
		static Config instance;
		return instance;
	}

	std::vector<std::string> getAllKeys() const {
		std::vector<std::string> v;
		for (const auto& p : _map) {
			v.push_back(p.first);
		}
		return v;
	}

	std::string get(const std::string & key) const {
		auto i = _map.find(key);
		return (i != _map.end()) ? i->second : "";
	}

	void preProcessFile(const std::string& cfgPath) {
		std::stringstream iss;
		{
			std::ifstream infile(cfgPath.c_str());
			if (!infile) {
				std::cerr << "file open error" << std::endl;
				return;
			}
			iss << infile.rdbuf();
			infile.close();
		}

		std::string input = iss.str();
		
		std::unordered_set<std::string> visited;
		std::stringstream oss;
		std::string line, key, value;
		while (std::getline(iss, line)) {
			if (analyseLine(line, key, value)) {
				if (visited.find(key) == visited.end()) {
					if (key.find(pattern) == std::string::npos) {
						oss << line << '\n';
					}
					else {
						if (key.find(pattern + "Hash") == std::string::npos && value!="") { // Real Password input
							oss << key << "=\n";
							std::string hashed = std::regex_replace(key, std::regex(pattern), pattern + "Hash");
							oss << hashed << "=" << encrypt(value, encryptKey) << '\n';
							visited.insert(hashed);
						}
						else {
							oss << line << '\n';
						}
					}
				}
			}
			else {
				oss << line << '\n';
			}
		}

		std::string output = oss.str();

		if (input != output) {
			std::ofstream outfile(cfgPath.c_str(), std::ios::trunc);
			outfile << output;
			outfile.close();
		}
	}

	void parseFile(const std::string & cfgPath) {
		if (pattern != "") {
			preProcessFile(cfgPath);
		}
		std::unordered_map<std::string, std::string> temp;
		
		_map.clear();
		std::ifstream infile(cfgPath.c_str());
		if (!infile) {
			std::cerr << "file open error" << std::endl;
			return;
		}
		std::string line, key, value;
		while (std::getline(infile, line)) {
			if (analyseLine(line, key, value)) {
				_map[key] = value;
				if (pattern != "") {
					if (key.find(pattern) != std::string::npos) {
						if (key.find(pattern + "Hash") == std::string::npos) {
							temp[key] = std::regex_replace(key, std::regex(pattern), pattern + "Hash");
						}
					}
				}
			}
		}

		if (pattern != "") {
			for (auto& pair : temp) {
				_map[pair.first] = decrypt(_map[pair.second], encryptKey);
			}
		}
	}

	void setEncryptKeyToPattern(const std::string& encryptKey, const std::string& pattern) {
		this->encryptKey = encryptKey;
		this->pattern = pattern;
	}

	// https://stackoverflow.com/questions/5888022/split-string-by-single-spaces
	static std::vector<std::string> split(const std::string &txt, char ch)
	{
		size_t pos = txt.find(ch);
		size_t initialPos = 0;
		std::vector<std::string> strs;

		// Decompose statement
		while (pos != std::string::npos) {
			strs.push_back(txt.substr(initialPos, pos - initialPos));
			initialPos = pos + 1;

			pos = txt.find(ch, initialPos);
		}

		// Add the last one
		strs.push_back(txt.substr(initialPos, min(pos, txt.size()) - initialPos + 1));

		return strs;
	}

private:

	Config() {}
	static const char COMMENT_CHAR = '#';
	static bool isSpace(char c) {
		if (' ' == c || '\t' == c)
			return true;
		return false;
	}
	static void trim(std::string & str) {
		if (str.empty()) {
			return;
		}
		long i = 0;
		for (; i < str.size(); ++i) {
			if (!isSpace(str[i])) {
				break;
			}
		}
		if (i == str.size()) { // 全部是空白字符串
			str = "";
			return;
		}

		long start_pos = i;

		for (i = str.size() - 1; i >= 0; --i) {
			if (!isSpace(str[i])) {
				break;
			}
		}
		long end_pos = i;

		str = str.substr(start_pos, end_pos - start_pos + 1);
	}
	static bool analyseLine(const std::string & line, std::string & key, std::string & value) {
		if (line.empty())
			return false;
		int start_pos = 0, end_pos = line.size() - 1, pos;
		if ((pos = line.find(COMMENT_CHAR)) != -1) {
			if (0 == pos) {  // 行的第一个字符就是注释字符
				return false;
			}
		}
		std::string new_line = line.substr(start_pos, start_pos + 1 - end_pos);  // 预处理，删除注释部分

		if ((pos = new_line.find('=')) == -1)
			return false;  // 没有=号

		key = new_line.substr(0, pos);
		value = new_line.substr(pos + 1, end_pos + 1 - (pos + 1));

		trim(key);
		if (key.empty()) {
			return false;
		}
		trim(value);
		return true;
	}

	std::string encryptKey;
	std::string pattern;
	std::unordered_map<std::string, std::string> _map;

};

