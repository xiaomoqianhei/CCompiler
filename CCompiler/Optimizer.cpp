#include "Optimizer.h"
#include "Dag.h"

namespace Yradex
{
	namespace CCompiler
	{
		void Optimizer::_optimize(const FunctionIdentifier & f)
		{
			PseudoTable::PseudoTableFunctionSwitcher sw(_pseudo_table, f);

			//_calculate_constant_instruction();
			//_common_subexpression_elimination(f);



			_pseudo_table.shrink_variable_set();

			Allocator allocator;
			allocator.allocate(_pseudo_table);
		}

		//void Optimizer::_calculate_constant_instruction()
		//{
		//	auto list = _pseudo_table.get_current_instruction_list();
		//	for (auto & ins : list)
		//	{
		//		if (ins._left_argument->is_const() && ins._right_argument->is_const())
		//		{
		//			switch (ins._operator_)
		//			{
		//			case PseudoOperator::add:
		//				ins._operator_ = PseudoOperator::assign;
		//				ins._left_argument = _pseudo_table.get_literal(ins._left_argument->get_value() + ins._right_argument->get_value());
		//				ins._right_argument = Variable::null;
		//				break;
		//			case PseudoOperator::sub:
		//				ins._operator_ = PseudoOperator::assign;
		//				ins._left_argument = _pseudo_table.get_literal(ins._left_argument->get_value() - ins._right_argument->get_value());
		//				ins._right_argument = Variable::null;
		//				break;
		//			case PseudoOperator::mul:
		//				ins._operator_ = PseudoOperator::assign;
		//				ins._left_argument = _pseudo_table.get_literal(ins._left_argument->get_value() * ins._right_argument->get_value());
		//				ins._right_argument = Variable::null;
		//				break;
		//			case PseudoOperator::div:
		//				ins._operator_ = PseudoOperator::assign;
		//				ins._left_argument = _pseudo_table.get_literal(ins._left_argument->get_value() / ins._right_argument->get_value());
		//				ins._right_argument = Variable::null;
		//				break;
		//			case PseudoOperator::beq:
		//				if (ins._left_argument->get_value() != ins._right_argument->get_value())
		//				{
		//					ins.clear();
		//				}
		//				else
		//				{
		//					ins._operator_ = PseudoOperator::b;
		//					ins._left_argument = ins._result;
		//					ins._right_argument = Variable::null;
		//					ins._result = Variable::null;
		//				}
		//				break;
		//			case PseudoOperator::bne:
		//				if (ins._left_argument->get_value() == ins._right_argument->get_value())
		//				{
		//					ins.clear();
		//				}
		//				else
		//				{
		//					ins._operator_ = PseudoOperator::b;
		//					ins._left_argument = ins._result;
		//					ins._right_argument = Variable::null;
		//					ins._result = Variable::null;
		//				}
		//				break;
		//			case PseudoOperator::bltz:
		//				if (ins._left_argument->get_value() < ins._right_argument->get_value())
		//				{
		//					ins.clear();
		//				}
		//				else
		//				{
		//					ins._operator_ = PseudoOperator::b;
		//					ins._left_argument = ins._result;
		//					ins._right_argument = Variable::null;
		//					ins._result = Variable::null;
		//				}
		//				break;
		//			case PseudoOperator::blez:
		//				if (ins._left_argument->get_value() <= ins._right_argument->get_value())
		//				{
		//					ins.clear();
		//				}
		//				else
		//				{
		//					ins._operator_ = PseudoOperator::b;
		//					ins._left_argument = ins._result;
		//					ins._right_argument = Variable::null;
		//					ins._result = Variable::null;
		//				}
		//				break;
		//			case PseudoOperator::bgtz:
		//				if (ins._left_argument->get_value() > ins._right_argument->get_value())
		//				{
		//					ins.clear();
		//				}
		//				else
		//				{
		//					ins._operator_ = PseudoOperator::b;
		//					ins._left_argument = ins._result;
		//					ins._right_argument = Variable::null;
		//					ins._result = Variable::null;
		//				}
		//				break;
		//			case PseudoOperator::bgez:
		//				if (ins._left_argument->get_value() >= ins._right_argument->get_value())
		//				{
		//					ins.clear();
		//				}
		//				else
		//				{
		//					ins._operator_ = PseudoOperator::b;
		//					ins._left_argument = ins._result;
		//					ins._right_argument = Variable::null;
		//					ins._result = Variable::null;
		//				}
		//				break;
		//			default:
		//				break;
		//			}
		//		}
		//	}
		//	_pseudo_table.set_instruction_list(list);
		//}

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
