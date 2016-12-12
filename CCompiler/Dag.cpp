#include "Dag.h"

namespace Yradex
{
	namespace CCompiler
	{
		void Dag::runDag(const FunctionIdentifier & f)
		{
			PseudoTable::PseudoTableFunctionSwitcher(_pseudo_table, f);
			std::list<PseudoInstruction> list;

			for (const auto &ins : _pseudo_table.get_current_instruction_list())
			{
				PseudoOperator op = PseudoOperator::nop;
				switch (op)
				{
				case PseudoOperator::add:
					break;
				case PseudoOperator::sub:
					break;
				case PseudoOperator::mul:
					break;
				case PseudoOperator::div:
					break;
				case PseudoOperator::b:
					break;
				case PseudoOperator::beq:
					break;
				case PseudoOperator::bne:
					break;
				case PseudoOperator::bltz:
					break;
				case PseudoOperator::blez:
					break;
				case PseudoOperator::bgtz:
					break;
				case PseudoOperator::bgez:
					break;
				case PseudoOperator::read:
					break;
				case PseudoOperator::print:
					break;
				case PseudoOperator::call:
					break;
				case PseudoOperator::ret:
					break;
				case PseudoOperator::label:
					break;
				case PseudoOperator::assign:
					break;
				case PseudoOperator::load:
					break;
				case PseudoOperator::store:
					break;
				case PseudoOperator::arg:
					break;
				case PseudoOperator::nop:
					break;
				default:
					break;
				}
			}

			// TODO switch list

		}
	}
}
