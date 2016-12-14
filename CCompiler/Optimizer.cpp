#include "Optimizer.h"
#include "Dag.h"

namespace Yradex
{
	namespace CCompiler
	{
		void Optimizer::_optimize(const FunctionIdentifier & f)
		{
			PseudoTable::PseudoTableFunctionSwitcher sw(_pseudo_table, f);

			_common_subexpression_elimination(f);



			_pseudo_table.shrink_variable_set();

			Allocator allocator;
			allocator.allocate(_pseudo_table);
		}

		void Optimizer::_common_subexpression_elimination(const FunctionIdentifier &f)
		{
			Dag dag(_pseudo_table);
			dag.run_dag(f);
		}

		void Optimizer::optimize()
		{
			for (auto & fi : _pseudo_table.get_function_list())
			{
				_optimize(fi);
			}
		}
	}
}
