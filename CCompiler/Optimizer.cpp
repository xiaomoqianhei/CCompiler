#include "Optimizer.h"

namespace Yradex
{
	namespace CCompiler
	{
		void Optimizer::_optimize(const FunctionIdentifier & f)
		{
			PseudoTable::PseudoTableFunctionSwitcher sw(_pseudo_table, f);

			_calculate_constant_instruction();

			Allocator allocator;
			allocator.allocate(_pseudo_table);
		}

		void Optimizer::_calculate_constant_instruction()
		{
			auto list = _pseudo_table.get_instruction_list();
			for (auto & ins : list)
			{
				if (ins.argument_1->is_const() && ins.argument_2->is_const())
				{
					switch (ins.operator_)
					{
					case PseudoOperator::add:
						ins.operator_ = PseudoOperator::assign;
						ins.argument_1 = _pseudo_table.get_literal(ins.argument_1->get_value() + ins.argument_2->get_value());
						ins.argument_2 = Variable::null;
						break;
					case PseudoOperator::sub:
						ins.operator_ = PseudoOperator::assign;
						ins.argument_1 = _pseudo_table.get_literal(ins.argument_1->get_value() - ins.argument_2->get_value());
						ins.argument_2 = Variable::null;
						break;
					case PseudoOperator::mul:
						ins.operator_ = PseudoOperator::assign;
						ins.argument_1 = _pseudo_table.get_literal(ins.argument_1->get_value() * ins.argument_2->get_value());
						ins.argument_2 = Variable::null;
						break;
					case PseudoOperator::div:
						ins.operator_ = PseudoOperator::assign;
						ins.argument_1 = _pseudo_table.get_literal(ins.argument_1->get_value() / ins.argument_2->get_value());
						ins.argument_2 = Variable::null;
						break;
					case PseudoOperator::beq:
						if (ins.argument_1->get_value() != ins.argument_2->get_value())
						{
							ins.clear();
						}
						else
						{
							ins.operator_ = PseudoOperator::b;
							ins.argument_1 = ins.result;
							ins.argument_2 = Variable::null;
							ins.result = Variable::null;
						}
						break;
					case PseudoOperator::bne:
						if (ins.argument_1->get_value() == ins.argument_2->get_value())
						{
							ins.clear();
						}
						else
						{
							ins.operator_ = PseudoOperator::b;
							ins.argument_1 = ins.result;
							ins.argument_2 = Variable::null;
							ins.result = Variable::null;
						}
						break;
					case PseudoOperator::bltz:
						if (ins.argument_1->get_value() < ins.argument_2->get_value())
						{
							ins.clear();
						}
						else
						{
							ins.operator_ = PseudoOperator::b;
							ins.argument_1 = ins.result;
							ins.argument_2 = Variable::null;
							ins.result = Variable::null;
						}
						break;
					case PseudoOperator::blez:
						if (ins.argument_1->get_value() <= ins.argument_2->get_value())
						{
							ins.clear();
						}
						else
						{
							ins.operator_ = PseudoOperator::b;
							ins.argument_1 = ins.result;
							ins.argument_2 = Variable::null;
							ins.result = Variable::null;
						}
						break;
					case PseudoOperator::bgtz:
						if (ins.argument_1->get_value() > ins.argument_2->get_value())
						{
							ins.clear();
						}
						else
						{
							ins.operator_ = PseudoOperator::b;
							ins.argument_1 = ins.result;
							ins.argument_2 = Variable::null;
							ins.result = Variable::null;
						}
						break;
					case PseudoOperator::bgez:
						if (ins.argument_1->get_value() >= ins.argument_2->get_value())
						{
							ins.clear();
						}
						else
						{
							ins.operator_ = PseudoOperator::b;
							ins.argument_1 = ins.result;
							ins.argument_2 = Variable::null;
							ins.result = Variable::null;
						}
						break;
					default:
						break;
					}
				}
			}
			_pseudo_table.set_instruction_list(list);
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
