#pragma once

#include "PseudoTable.h"

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

			PseudoOperator operator_;
			std::shared_ptr<Variable> variable;
			int value;

			int left = -1;
			int right = -1;
			std::vector<int> parents;

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
			int _get_node_id(std::shared_ptr<Variable> variable);
			int _get_operator_id(PseudoOperator op, int left, int right);

		public:
			Dag(PseudoTable &pseudo_table)
				:_pseudo_table(pseudo_table)
			{
			}

			void run_dag(const FunctionIdentifier &f);
		};
	}
}
