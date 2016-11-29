
#include "BasicErrorHandler.h"
#include "Parser.h"
#include "Optimizer.h"
#include "Generator.h"

using namespace Yradex::CCompiler;

int main(int argc, char **argv)
{
	if (argc != 2)
	{
		std::cerr << "usage: CCompiler.exe filepath" << std::endl;
		return 0;
	}
	std::string filename = argv[1];

	SymbolTable symbol_table;
	PseudoTable pseudo_table(symbol_table);

	Parser parser(pseudo_table, filename);
	parser.parse_to_pseudo();

	if (BasicErrorHandler::instance().get_error_count())
	{
		std::cerr << std::endl << "Error: " << BasicErrorHandler::instance().get_error_count() << std::endl;
		system("pause");
		return 0;
	}

	Optimizer optimizer(pseudo_table);
	optimizer.optimize();

	Generator generator(pseudo_table, std::cout);
	generator.generate();

	system("pause");
}
