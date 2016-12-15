#include "Dag.h"

#include <iterator>
#include <fstream>

namespace Yradex
{
	namespace CCompiler
	{
		int Dag::_calculate_node_id(std::shared_ptr<Variable> variable)
		{
			if (variable == Variable::null())
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
				node.variable = variable;
				_node_vector.push_back(node);
				_variable_map.insert(std::make_pair(variable, id));
				return id;
			}
			case Variable::Type::variable:
			case Variable::Type::array:
			case Variable::Type::label:
			case Variable::Type::string:
			{
				int id = _node_vector.size();
				DagNode node(id, DagNode::Type::variable);
				node.variable = variable;
				_node_vector.push_back(node);
				_variable_map.insert(std::make_pair(variable, id));
				return id;
			}
			default:
				break;
			}
			return -1;
		}
		int Dag::_calculate_operator_node_id(PseudoOperator op, int left_arg_id, int right_arg_id)
		{
			switch (op)
			{
			case PseudoOperator::add:
			{
				DagNode &left_arg_node = _node_vector[left_arg_id];
				DagNode &right_arg_node = _node_vector[right_arg_id];

				// if both are const calculate it
				if (left_arg_node.type == DagNode::Type::const_ && right_arg_node.type == DagNode::Type::const_)
				{
					return _calculate_node_id(_pseudo_table.get_literal(
						left_arg_node.variable->get_value() + right_arg_node.variable->get_value()));
				}

				return __find_or_insert_operator_node(op, left_arg_id, right_arg_id, true);
			}
			case PseudoOperator::sub:
			{
				DagNode &left_arg_node = _node_vector[left_arg_id];
				DagNode &right_arg_node = _node_vector[right_arg_id];

				// if both are const calculate it
				if (left_arg_node.type == DagNode::Type::const_ && right_arg_node.type == DagNode::Type::const_)
				{
					return _calculate_node_id(_pseudo_table.get_literal(
						left_arg_node.variable->get_value() - right_arg_node.variable->get_value()));
				}

				return __find_or_insert_operator_node(op, left_arg_id, right_arg_id, false);
			}
			case PseudoOperator::mul:
			{
				DagNode &left_arg_node = _node_vector[left_arg_id];
				DagNode &right_arg_node = _node_vector[right_arg_id];

				// if both are const calculate it
				if (left_arg_node.type == DagNode::Type::const_ && right_arg_node.type == DagNode::Type::const_)
				{
					return _calculate_node_id(_pseudo_table.get_literal(
						left_arg_node.variable->get_value() * right_arg_node.variable->get_value()));
				}

				return __find_or_insert_operator_node(op, left_arg_id, right_arg_id, true);
			}
			case PseudoOperator::div:
			{
				DagNode &left_arg_node = _node_vector[left_arg_id];
				DagNode &right_arg_node = _node_vector[right_arg_id];

				// if both are const calculate it
				if (left_arg_node.type == DagNode::Type::const_ && right_arg_node.type == DagNode::Type::const_)
				{
					if (right_arg_node.variable->get_value() == 0)
					{
						error_handler_type::instance().raise(Error::devide_by_zero,
							"in function " + _pseudo_table.get_current_function_identifier().get_name(),
							error_handler_type::Level::error);
						return _calculate_node_id(_pseudo_table.get_literal(1));
					}
					else
					{
						return _calculate_node_id(_pseudo_table.get_literal(
							left_arg_node.variable->get_value() / right_arg_node.variable->get_value()));
					}
				}

				return __find_or_insert_operator_node(op, left_arg_id, right_arg_id, false);
			}
			case PseudoOperator::b:
			{
				return __insert_operator_node(PseudoOperator::b);
			}
			case PseudoOperator::beq:
			{
				DagNode &left_arg_node = _node_vector[left_arg_id];
				DagNode &right_arg_node = _node_vector[right_arg_id];

				// if both are const calculate it
				if (left_arg_node.type == DagNode::Type::const_ && right_arg_node.type == DagNode::Type::const_)
				{
					if (left_arg_node.variable->get_value() == right_arg_node.variable->get_value())
					{
						return __insert_operator_node(PseudoOperator::b);
					}
					else
					{
						return -1;
					}
				}

				return __insert_operator_node(op, left_arg_id, right_arg_id);
			}
			case PseudoOperator::bne:
			{
				DagNode &left_arg_node = _node_vector[left_arg_id];
				DagNode &right_arg_node = _node_vector[right_arg_id];

				// if both are const calculate it
				if (left_arg_node.type == DagNode::Type::const_ && right_arg_node.type == DagNode::Type::const_)
				{
					if (left_arg_node.variable->get_value() != right_arg_node.variable->get_value())
					{
						return __insert_operator_node(PseudoOperator::b);
					}
					else
					{
						return -1;
					}
				}

				return __insert_operator_node(op, left_arg_id, right_arg_id);
			}
			case PseudoOperator::bltz:
			{
				DagNode &left_arg_node = _node_vector[left_arg_id];

				// if both are const calculate it
				if (left_arg_node.type == DagNode::Type::const_)
				{
					if (left_arg_node.variable->get_value() < 0)
					{
						return __insert_operator_node(PseudoOperator::b);
					}
					else
					{
						return -1;
					}
				}

				return __insert_operator_node(op, left_arg_id);
			}
			case PseudoOperator::blez:
			{
				DagNode &left_arg_node = _node_vector[left_arg_id];

				// if both are const calculate it
				if (left_arg_node.type == DagNode::Type::const_)
				{
					if (left_arg_node.variable->get_value() <= 0)
					{
						return __insert_operator_node(PseudoOperator::b);
					}
					else
					{
						return -1;
					}
				}

				return __insert_operator_node(op, left_arg_id);
			}
			case PseudoOperator::bgtz:
			{
				DagNode &left_arg_node = _node_vector[left_arg_id];

				// if both are const calculate it
				if (left_arg_node.type == DagNode::Type::const_)
				{
					if (left_arg_node.variable->get_value() > 0)
					{
						return __insert_operator_node(PseudoOperator::b);
					}
					else
					{
						return -1;
					}
				}

				return __insert_operator_node(op, left_arg_id);
			}
			case PseudoOperator::bgez:
			{
				DagNode &left_arg_node = _node_vector[left_arg_id];

				// if both are const calculate it
				if (left_arg_node.type == DagNode::Type::const_)
				{
					if (left_arg_node.variable->get_value() >= 0)
					{
						return __insert_operator_node(PseudoOperator::b);
					}
					else
					{
						return -1;
					}
				}

				return __insert_operator_node(op, left_arg_id);
			}
			case PseudoOperator::read:
				return __insert_operator_node(op);
			case PseudoOperator::print:
				return __insert_operator_node(op, left_arg_id, right_arg_id);
			case PseudoOperator::call:
				return __insert_operator_node(op, left_arg_id);
			case PseudoOperator::ret:
				return __insert_operator_node(op, left_arg_id);
			case PseudoOperator::label:
				return __insert_operator_node(op, left_arg_id);
			case PseudoOperator::assign:
				return left_arg_id;
			case PseudoOperator::load:
				return __find_or_insert_operator_node(op, left_arg_id, right_arg_id, false);
			case PseudoOperator::store:
				return __insert_operator_node(op, left_arg_id, right_arg_id);
			case PseudoOperator::arg:
				return __insert_operator_node(op, left_arg_id);
			case PseudoOperator::nop:
				break;
			default:
				break;
			}
			return -1;
		}
		void Dag::_calculate_result_node(int operator_id, std::shared_ptr<Variable> result)
		{
			assert(operator_id != -1);

			if (result == Variable::null())
			{
				return;
			}

			_node_vector[operator_id].result = result;

			if (PseudoOperatorUtility::writes_result_in_basic_block(_node_vector[operator_id].operator_))
			{
				_variable_map.insert_or_assign(result, operator_id);
			}
		}
		void Dag::_remove_useless_instructions(std::list<PseudoInstruction>& list)
		{
			int this_count = list.size();
			int last_count = 0;
			while (this_count != last_count)
			{
				for (auto iter = list.begin(); iter != list.end();)
				{
					if ((!PseudoOperatorUtility::has_side_effect(iter->get_operator()))
						&& iter->get_result()->get_ref() == 1
						&& iter->get_result()->get_function() != FunctionIdentifier::global)
					{
						iter = list.erase(iter);
					}
					else
					{
						++iter;
					}
				}
				last_count = this_count;
				this_count = list.size();
			}
		}
		void Dag::_generate_code_from_map(std::list<PseudoInstruction>& list)
		{
			for (auto &node : _node_vector)
			{
				if (node.parents.empty() || PseudoOperatorUtility::has_side_effect(node.operator_))
				{
					_generate_code_from_node(list, node);
				}
			}
		}
		void Dag::_generate_code_from_node(std::list<PseudoInstruction>& list, DagNode & node)
		{
			if (node.generated)
			{
				return;
			}
			node.generated = true;

			if (node.type != DagNode::Type::operator_)
			{
				for (const auto &p : _variable_map)
				{
					if (p.second == node.id
						&& p.first->get_variable_type() == Variable::Type::variable
						&& p.first != node.variable)
					{
						// FIX check it
						// copy result
						if (p.first->get_variable_type() == Variable::Type::variable)
						{
							auto temp = _pseudo_table.declear_temp_variable(p.first->get_type());
							list.push_back(PseudoInstruction(PseudoOperator::assign, p.first, Variable::null(), temp));

							for (auto &n : _node_vector)
							{
								if (n.variable == p.first)
								{
									n.variable = temp;
								}
							}
						}
						list.push_back(PseudoInstruction(PseudoOperator::assign, node.variable, Variable::null(), p.first));
					}
				}
			}
			else
			{
				std::shared_ptr<Variable> left_arg = Variable::null();
				std::shared_ptr<Variable> right_arg = Variable::null();

				// get arguments
				if (node.left != -1)
				{
					_generate_code_from_node(list, _node_vector[node.left]);
				}
				if (node.right != -1)
				{
					_generate_code_from_node(list, _node_vector[node.right]);
					right_arg = _node_vector[node.right].variable;
				}
				if (node.left != -1)
				{
					left_arg = _node_vector[node.left].variable;
				}

				// get result
				std::shared_ptr<Variable> result = node.result;

				if (PseudoOperatorUtility::writes_result_in_basic_block(node.operator_))
				{
					auto iter = std::find_if(_variable_map.cbegin(), _variable_map.cend(),
						[&node](const std::pair<std::shared_ptr<Variable>, int> &p) {
						return p.second == node.id && !p.first->is_temp();
					});

					if (iter == _variable_map.cend())
					{
						result = _pseudo_table.declear_temp_variable(Symbol::int_symbol);
					}
					else
					{
						result = iter->first;
					}
				}

				// FIX check it
				// copy result
				if (result->get_variable_type() == Variable::Type::variable)
				{
					auto temp = _pseudo_table.declear_temp_variable(result->get_type());
					list.push_back(PseudoInstruction(PseudoOperator::assign, result, Variable::null(), temp));

					for (auto &n : _node_vector)
					{
						if (n.variable == result)
						{
							n.variable = temp;
						}
					}
				}


				// generate code
				node.variable = result;
				list.push_back(PseudoInstruction(node.operator_, left_arg, right_arg, result));

				// assign to all
				for (const auto &p : _variable_map)
				{
					if (p.second == node.id && p.first != result
						&& p.first->get_variable_type() == Variable::Type::variable)
					{
						// FIX check it
						// copy result
						if (p.first->get_variable_type() == Variable::Type::variable)
						{
							auto temp = _pseudo_table.declear_temp_variable(p.first->get_type());
							list.push_back(PseudoInstruction(PseudoOperator::assign, p.first, Variable::null(), temp));

							for (auto &n : _node_vector)
							{
								if (n.variable == p.first)
								{
									n.variable = temp;
								}
							}
						}

						list.push_back(PseudoInstruction(PseudoOperator::assign, node.variable, Variable::null(), p.first));
					}
				}
			}
		}
		int Dag::__find_or_insert_operator_node(PseudoOperator op, int left_arg_id)
		{
			DagNode &left_arg_node = _node_vector[left_arg_id];

			// find this node
			auto &intersection = left_arg_node.parents;

			auto &node_vector = _node_vector;
			auto iter = std::find_if(intersection.cbegin(), intersection.cend(),
				[op, &node_vector](int id) { return node_vector[id].operator_ == op; }
			);

			// found, return its id
			if (iter != intersection.cend())
			{
				return *iter;
			}

			// not found, create it
			return __insert_operator_node(op, left_arg_id);
		}
		int Dag::__find_or_insert_operator_node(PseudoOperator op, int left_arg_id, int right_arg_id, bool commutative)
		{
			DagNode &left_arg_node = _node_vector[left_arg_id];
			DagNode &right_arg_node = _node_vector[right_arg_id];

			if (commutative)
			{
				// find this node
				std::vector<int> intersection;

				std::set_intersection(left_arg_node.parents.cbegin(), left_arg_node.parents.cend(),
					right_arg_node.parents.cbegin(), right_arg_node.parents.cend(),
					std::back_inserter(intersection));

				auto &node_vector = _node_vector;
				auto iter = std::find_if(intersection.cbegin(), intersection.cend(),
					[op, &node_vector](int id) { return node_vector[id].operator_ == op; }
				);

				// found, return its id
				if (iter != intersection.cend())
				{
					return *iter;
				}
			}
			else
			{
				// find this node
				std::vector<int> intersection;

				std::set_intersection(left_arg_node.parents.cbegin(), left_arg_node.parents.cend(),
					right_arg_node.parents.cbegin(), right_arg_node.parents.cend(),
					std::back_inserter(intersection));

				auto &node_vector = _node_vector;
				auto iter = std::find_if(intersection.cbegin(), intersection.cend(),
					[op, left_arg_id, right_arg_id, &node_vector](int id) {
					return node_vector[id].operator_ == op
						&& node_vector[id].left == left_arg_id
						&& node_vector[id].right == right_arg_id;
				});

				// found, return its id
				if (iter != intersection.cend())
				{
					return *iter;
				}
			}

			// not found, create it
			return __insert_operator_node(op, left_arg_id, right_arg_id);
		}
		int Dag::__insert_operator_node(PseudoOperator op)
		{
			int id = _node_vector.size();
			DagNode node(id, DagNode::Type::operator_);
			node.operator_ = op;
			_node_vector.push_back(node);
			return id;
		}
		int Dag::__insert_operator_node(PseudoOperator op, int left_arg_id)
		{
			int id = _node_vector.size();
			DagNode node(id, DagNode::Type::operator_);
			node.operator_ = op;
			if (left_arg_id != -1)
			{
				node.left = left_arg_id;
				_node_vector[left_arg_id].parents.insert(id);
			}
			_node_vector.push_back(node);
			return id;
		}
		int Dag::__insert_operator_node(PseudoOperator op, int left_arg_id, int right_arg_id)
		{
			int id = _node_vector.size();
			DagNode node(id, DagNode::Type::operator_);
			node.operator_ = op;
			if (left_arg_id != -1)
			{
				node.left = left_arg_id;
				_node_vector[left_arg_id].parents.insert(id);
			}
			if (right_arg_id != -1)
			{
				node.right = right_arg_id;
				_node_vector[right_arg_id].parents.insert(id);
			}
			_node_vector.push_back(node);
			return id;
		}
		void Dag::run_dag(const FunctionIdentifier & f)
		{
			PseudoTable::PseudoTableFunctionSwitcher(_pseudo_table, f);
			std::list<PseudoInstruction> new_ins_list;

			auto original_ins_list = _pseudo_table.set_instruction_list(std::list<PseudoInstruction>());
			auto last_iter = original_ins_list.begin();
			for (auto iter = original_ins_list.begin(); iter != original_ins_list.end();)
			{
				auto &ins = *iter;

				// pre-check
				if (ins.get_operator() == PseudoOperator::label && ins.get_left_argument()->get_ref() == 1)
				{
					iter = original_ins_list.erase(iter);
					continue;
				}

				int left_arg_id = _calculate_node_id(ins.get_left_argument());
				int right_arg_id = _calculate_node_id(ins.get_right_argument());
				int operator_id = _calculate_operator_node_id(ins.get_operator(), left_arg_id, right_arg_id);

				if (operator_id == -1)
				{
					iter = original_ins_list.erase(iter);
					continue;
				}

				_calculate_result_node(operator_id, ins.get_result());

				++iter;
				if (PseudoOperatorUtility::is_end_of_basic_block(ins.get_operator()) && operator_id != -1)
				{
					auto &last_node = _node_vector[operator_id];
					if (last_node.operator_ == PseudoOperator::b)
					{
						while (iter->get_operator() != PseudoOperator::label)
						{
							iter = original_ins_list.erase(iter);
						}
						if (last_node.result == iter->get_left_argument())
						{
							iter = original_ins_list.erase(--iter);
							iter = original_ins_list.erase(iter);
							_node_vector.clear();
							_variable_map.clear();
							iter = last_iter;
							continue;
						}
					}

					last_iter = iter;
					_generate_code_from_map(new_ins_list);
					
					_variable_map.clear();
					_node_vector.clear();
				}
			}

			original_ins_list.clear();
			_remove_useless_instructions(new_ins_list);
			
			// TEST
			std::ofstream stream("log.txt", std::ios::app);

			stream << std::endl << std::endl << f.get_name() << std::endl;
			for (auto &i : new_ins_list)
			{
				stream << i << std::endl;
			}

			_pseudo_table.set_instruction_list(new_ins_list);

		}
	}
}
