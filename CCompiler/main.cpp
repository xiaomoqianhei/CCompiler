
#include "BasicErrorHandler.h"
#include "Parser.h"
#include "Optimizer.h"
#include "Generator.h"

#include <fstream>

using namespace Yradex::CCompiler;

int main()
{
	{
		std::ofstream stream("log.txt");
		stream.close();
	}

	std::cout << "enter file path: " << std::endl;
	std::string filename;
	//filename = "C:\\Users\\yradex\\Documents\\schoolWork\\3A\\compiler\\compiler_documents\\14061062_test.txt";
	std::cin >> filename;

	SymbolTable symbol_table;
	PseudoTable pseudo_table(symbol_table);

	{
		Parser parser(pseudo_table, filename);
		parser.parse_to_pseudo();
	}

	if (BasicErrorHandler::instance().get_error_count())
	{
		std::cerr << std::endl << "Error: " << BasicErrorHandler::instance().get_error_count() << std::endl;
		system("pause");
		return 0;
	}

	{
		Optimizer optimizer(pseudo_table);
		optimizer.optimize();
	}

	std::ofstream stream("asm.txt");

	{
		Generator generator(pseudo_table, stream);
		generator.generate();
	}

	std::cout << "output to asm.txt" << std::endl;

	system("pause");
}
