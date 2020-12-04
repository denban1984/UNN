#include "UploadTosite.h"
#include <iostream>
#include <locale>


int main(int argc, char* argv[])
{
	setlocale(LC_ALL, "Russian");


	std::vector<std::string> keys;
	keys.reserve(5);
	for (int i = 1; i < argc; ++i) { keys.emplace_back(argv[i]); }
	ParsingKeys BaseCl(keys);

	


	std::cin.get();

}
