#include <iostream>
#include <fstream>
#include <string>

int main(int argc, char *argv[])
{
	std::string tmp;
	std::ofstream fout("tmp.txt");
	while (std::getline(std::cin, tmp))
	{
		if (tmp == "")
			continue;
		//std::cout << tmp << std::endl;
		fout << tmp << std::endl;
		if (tmp.find("query_ticket") != std::string::npos)
			std::cout << 1 << std::endl
					  << "HAPPY_TRAIN 中院 08-17 05:24 -> 下院 08-17 15:24 514 1000" << std::endl;
		else
			std::cout << 0 << std::endl;
	}
	return 0;
}