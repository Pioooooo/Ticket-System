#include <iostream>
#include <fstream>
#include <algorithm>
#include <string>
#include <map>
#include <vector>

std::ofstream fout("tmp.txt");

class myTest
{
	std::map<std::vector<std::string>, std::string> dat;

public:
	myTest &addPattern(std::vector<std::string> &&patterns, std::string &&ret)
	{
		dat.insert(std::make_pair(std::move(patterns), std::move(ret)));
		return *this;
	}

	std::string exec(const std::string &str)
	{
		auto it = std::find_if(dat.begin(), dat.end(), [str](auto &patterns) {
			for (const auto &pattern : patterns.first)
				if (str.find(pattern) == std::string::npos)
					return false;
			return true;
		});
		if (it != dat.end())
		{
			std::cout << it->second << std::endl;
			return it->second;
		}
		std::cout << "0" << std::endl;
		return "0";
	}
};

int main(int argc, char *argv[])
{
	myTest test;
	test.addPattern({"query_ticket"}, "2\nHAPPY_TRAIN 中院 08-17 05:24 -> 下院 08-17 15:24 514 1000\nHAPPY_TRAIN 中院 08-17 05:24 -> 下院 08-17 15:24 514 1000")
		.addPattern({"query_transfer"}, "2\nHAPPY_TRAIN 中院 08-17 05:24 -> 下院 08-17 15:24 514 1000\nHAPPY_TRAIN 中院 08-17 05:24 -> 下院 08-17 15:24 514 1000")
		.addPattern({"query_profile"}, "Pioooooo 名字 mail@mail.com 3")
		.addPattern({"modify_profile"}, "Pioooooo1 名字一 mail1@mail1.com1 4")
		.addPattern({"query_train"}, "HAPPY_TRAIN G\n上院 xx-xx xx:xx -> 07-01 19:19 0 1000\n中院 07-02 05:19 -> 07-02 05:24 114 1000\n下院 07-02 15:24 -> xx-xx xx:xx 628 x")
		.addPattern({"buy_ticket", "-q true"}, "queue");

	std::string tmp;
	while (std::getline(std::cin, tmp))
	{
		if (tmp == "")
			continue;
		//std::cout << tmp << std::endl;

		fout << tmp << std::endl;
		test.exec(tmp);
		// if (tmp.find("query_ticket") != std::string::npos || tmp.find("query_transfer") != std::string::npos)
		// 	std::cout << 1 << std::endl
		// 			  << "HAPPY_TRAIN 中院 08-17 05:24 -> 下院 08-17 15:24 514 1000" << std::endl
		// 			  << "HAPPY_TRAIN 中院 08-17 05:24 -> 下院 08-17 15:24 514 1000" << std::endl;
		// else if (tmp.find("buy_ticket") != std::string::npos && tmp.find("-q true") != std::string::npos)
		// 	std::cout << "queue" << std::endl;
		// else
		// 	std::cout << 0 << std::endl;
	}
	return 0;
}