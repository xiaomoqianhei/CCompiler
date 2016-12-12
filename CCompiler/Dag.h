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
			PseudoOperator operator_;
			std::shared_ptr<Variable> variable;
		};

		class Dag
		{
		public:
			using string_type = std::string;
			using char_type = typename string_type::value_type;
			using error_handler_type = BasicErrorHandler;

		private:
			PseudoTable &_pseudo_table;
			std::unordered_map<int, DagNode> _node_map;
			std::unordered_map<std::shared_ptr<Variable>, int> _variable_map;

		public:
			Dag(PseudoTable &pseudo_table)
				:_pseudo_table(pseudo_table)
			{
			}

			void runDag(const FunctionIdentifier &f);
		};
	}
}
