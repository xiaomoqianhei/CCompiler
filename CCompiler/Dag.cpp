#include "Dag.h"

namespace Yradex
{
	namespace CCompiler
	{
		int Dag::_get_node_id(std::shared_ptr<Variable> variable)
		{
			if (variable == Variable::null)
			{
				return -1;
			}

			auto iter = _variable_map.find(variable);
			if (iter != _variable_map.cend())
			{
				return iter->second;
			}

			switch (variable->get_variable_type())
			{
			case Variable::Type::const_variable:
			{
				int id = _node_vector.size();
				DagNode node(id, DagNode::Type::const_);
				node.value = variable->get_value();
				_node_vector.push_back(node);
				return id;
			}
			case Variable::Type::variable:
			case Variable::Type::array:
			{
				int id = _node_vector.size();
				DagNode node(id, DagNode::Type::variable);
				node.variable = variable;
				_node_vector.push_back(node);
				return id;
			}
			case Variable::Type::label:
			default:
				assert(false);
				break;
			}
			return -1;
		}
		int Dag::_get_operator_id(PseudoOperator op, int left, int right)
		{
			switch (op)
			{
			case PseudoOperator::add:
			{
				if (_node_vector[left].type == DagNode::Type::const_ && _node_vector[right].type == DagNode::Type::const_)
				{
					// TODO
				}
				auto iter = std::find_if(_node_vector.cbegin(), _node_vector.cend(), 
					[op, left, right](const DagNode &node) {
					return node.operator_ == op 
						&& ((node.left == left && node.right == right) || (node.left == right && node.right == left)); 
				});

				break;
			}
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
			return 0;
		}
		void Dag::run_dag(const FunctionIdentifier & f)
		{
			PseudoTable::PseudoTableFunctionSwitcher(_pseudo_table, f);
			std::list<PseudoInstruction> list;

			for (const auto &ins : _pseudo_table.get_current_instruction_list())
			{
				int left_arg_id = _get_node_id(ins.get_left_argument());
				int right_arg_id = _get_node_id(ins.get_right_argument());
				int operator_id = _get_operator_id(ins.get_operator(), left_arg_id, right_arg_id);

				if (PseudoOperatorUtility::is_end_of_basic_block(ins.get_operator()))
				{

				}
					// TODO switch list
			}
		}
	}
}
