#pragma once

class StringUtil
{
public:
	static void ReplaceAll(std::string& str, const std::string& from, const std::string& to) 
	{
		size_t start_pos = 0;
		while((start_pos = str.find(from, start_pos)) != std::string::npos) 
		{
			str.replace(start_pos, from.length(), to);
			start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
		}
	}
};

