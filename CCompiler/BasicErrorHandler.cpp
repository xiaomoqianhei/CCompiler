#include "BasicErrorHandler.h"

namespace Yradex
{
	namespace CCompiler
	{
		const std::unordered_map<BasicErrorHandler::Level,
			BasicErrorHandler::string_type,
			EnumHasher<BasicErrorHandler::Level>>
			BasicErrorHandler::_level_map = {
				{ Level::debug,		"DEBUG" },
				{ Level::warning,	"WARNING" },
				{ Level::error,		"ERROR" },
				{ Level::fatal,		"FATAL" },
		};
		BasicErrorHandler::BasicErrorHandler()
		{
			auto iter = std::find_if(_level_map.cbegin(), _level_map.cend(),
				[](std::pair<Level, string_type> p) {return p.second == Config::error_level; });
			if (iter == _level_map.cend())
			{
				_level = Level::debug;
				return;
			}
			_level = iter->first;
		}
		void BasicErrorHandler::raise(Error error, Level level)
		{
			if (!should_raise(level))
			{
				return;
			}
			if (level == Level::error)
			{
				_error_count++;
			}
			std::cerr << _level_map.at(level) << ": " << error << std::endl;
			if (level == Level::fatal)
			{
				exit(0);
			}
		}
		void BasicErrorHandler::raise(Error error, std::string filename, size_t line_num, size_t char_num, Level level)
		{
			if (!should_raise(level))
			{
				return;
			}
			if (level == Level::error)
			{
				_error_count++;
			}
			std::cerr << _level_map.at(level) << ": "
				<< filename << "(" << line_num << ", " << char_num << "): "
				<< error << std::endl;
			if (level == Level::fatal)
			{
				exit(0);
			}
		}
		void BasicErrorHandler::raise(Error error, const string_type & info,
			std::string filename, size_t line_num, size_t char_num, Level level)
		{
			if (!should_raise(level))
			{
				return;
			}
			if (level == Level::error)
			{
				_error_count++;
			}
			std::cerr << _level_map.at(level) << ": "
				<< filename << "(" << line_num << ", " << char_num << "): "
				<< error << ": " << info << std::endl;
			if (level == Level::fatal)
			{
				exit(0);
			}
		}
		void BasicErrorHandler::debug(const string_type & info)
		{
			if (!should_raise(Level::debug))
			{
				return;
			}
			std::cerr << _level_map.at(Level::debug) << ": " << info << std::endl;
		}
	}
}
