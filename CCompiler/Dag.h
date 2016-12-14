#pragma once

#include "PseudoTable.h"

#include <set>

namespace Yradex
{
	namespace CCompiler
	{

		class DagNode
		{
		public:
			enum class Type
			{
				operator_,
				variable,
				const_,
			};

		public:
			int id;
			Type type;
			bool generated = false;

			PseudoOperator operator_;
			std::shared_ptr<Variable> variable = Variable::null();
			std::shared_ptr<Variable> result = Variable::null();

			int left = -1;
			int right = -1;
			std::set<int> parents;

		public:
			DagNode(int id_, Type type_)
				: id(id_), type(type_)
			{
			}
		};

		class Dag
		{
		public:
			using string_type = std::string;
			using char_type = typename string_type::value_type;
			using error_handler_type = BasicErrorHandler;

		private:
			PseudoTable &_pseudo_table;
			std::vector<DagNode> _node_vector;
			std::unordered_map<std::shared_ptr<Variable>, int> _variable_map;

		private:
			int _calculate_node_id(std::shared_ptr<Variable> variable);
			int _calculate_operator_node_id(PseudoOperator op, int left_arg_id, int right_arg_id);
			void _calculate_result_node(int operator_id, std::shared_ptr<Variable> result);
			void _remove_useless_instructions(std::list<PseudoInstruction> &list);
			void _generate_code_from_map(std::list<PseudoInstruction> &list);
			void _generate_code_from_node(std::list<PseudoInstruction>& list, DagNode & node);

			int __find_or_insert_operator_node(PseudoOperator op, int left_arg_id);
			int __find_or_insert_operator_node(PseudoOperator op, int left_arg_id, int right_arg_id, bool commutative);
			int __insert_operator_node(PseudoOperator op);
			int __insert_operator_node(PseudoOperator op, int left_arg_id);
			int __insert_operator_node(PseudoOperator op, int left_arg_id, int right_arg_id);

		public:
			Dag(PseudoTable &pseudo_table)
				:_pseudo_table(pseudo_table)
			{
			}

			void run_dag(const FunctionIdentifier &f);
		};
	}
}
