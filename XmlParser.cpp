#include "XmlParser.h"

using namespace WXML;

shared_ptr<XmlParser> XmlParser::_instance(nullptr);
std::once_flag XmlParser::_initFlag;

shared_ptr<XmlParser> XmlParser::getXmlParser()
{
	std::call_once(_initFlag, []() 
	{ 
		XmlParser::_instance = shared_ptr<XmlParser>(new XmlParser()); 
	});

	return XmlParser::_instance;
}

XmlParseContext XmlParser::parse(const string_view& xml)
{
	// parse head
	auto head = xml.substr(0, xml.find(">") + 1);
	if (!checkHead(head))
		throw std::runtime_error("Invalid head");
	auto [version, encoding] = parseHead(head);

	// parse body
	auto body = xml.substr(head.size(), xml.size() - head.size());
	std::stack<shared_ptr<XmlNode>> node_stack;
	size_t pos = 0;

	// get root node
	auto rootNode = parseElement(body, pos);
	node_stack.push(rootNode);

	XmlParseContext context(rootNode);
	context.version = version, context.encoding = encoding;

	while (!node_stack.empty())
	{
		while (body[pos] >= 0 && body[pos] <= 255 && isspace(body[pos])) pos++;
		if (body[pos] == '<')
		{
			if (body[pos + 1] == '/')
			{
				auto tempNode = parseElement(body, pos);
				if (tempNode->name != node_stack.top()->name)
					throw std::runtime_error("Invalid xml format");
				node_stack.pop();
			}
			else
			{
				auto tempNode = parseElement(body, pos);
				node_stack.top()->children.push_back(tempNode);
				tempNode->parent = node_stack.top();
				node_stack.push(tempNode);
			}
		}
		else
		{
			auto content = parseElementContent(body, pos);
			node_stack.top()->value = content;
		}
	}
	
	return context;
}

bool XmlParser::checkHead(const string_view& xml) const
{
	if (xml.find("<?xml") != 0|| xml.find("?>") != xml.size() - 2)
		return false;

	return true;
}

std::pair<string_view, string_view> XmlParser::parseHead(const string_view& xml) const
{
	auto pos = xml.find("\"");
	auto version = xml.substr(pos + 1, xml.find("\"", pos + 1) - pos - 1);
	pos = xml.find("encoding");
	pos = xml.find("\"", pos);
	auto encoding = xml.substr(pos + 1, xml.find("\"", pos + 1) - pos - 1);

	return std::make_pair(version, encoding);
}

shared_ptr<XmlNode> XmlParser::parseElement(const string_view& boby, size_t& pos)
{
	while (isspace(boby[pos])) pos++;
	auto node = std::make_shared<XmlNode>();
	if (boby.find("</", pos) == pos)
	{
		auto nextPos = boby.find(">", pos);
		auto name = boby.substr(pos + 2, nextPos - pos - 2);
		pos = nextPos + 1;

		node->name = name;
		return node;
	}

	auto state = TraverseCharState::GetName;
	string name, attr_name, attr_value;
	// parse name
	pos++;
	while (isspace(boby[pos])) pos++;
	while (!isspace(boby[pos]) && boby[pos] != '>') name.push_back(boby[pos++]);
	node->name = name;

	// parse attributes
	while (true)
	{
		if (boby[pos] == '>' && state == TraverseCharState::GetName)
		{
			pos++;
			break;
		}

		if (boby[pos] == '\"')
		{
			if (state == TraverseCharState::GetAttribute)
				state = TraverseCharState::InDoubleQuotation;
			else if (state == TraverseCharState::InDoubleQuotation)
			{
				state = TraverseCharState::GetName;
				node->attributes[attr_name] = attr_value;
				attr_name.clear(), attr_value.clear();
			}
			else
				throw std::runtime_error("Invalid xml format");
		}
		else if (boby[pos] == '\"')
		{
			if (state == TraverseCharState::GetAttribute)
				state = TraverseCharState::InDoubleQuotation;
			else if (state == TraverseCharState::InDoubleQuotation)
			{
				state = TraverseCharState::GetName;
				node->attributes[attr_name] = attr_value;
				attr_name.clear(), attr_value.clear();
			}
			else 
				throw std::runtime_error("Invalid xml format");
		}
		else if (boby[pos] == '=')
		{
			if (state == TraverseCharState::GetName)
				state = TraverseCharState::GetAttribute;
			else
				throw std::runtime_error("Invalid xml format");
		}
		else
		{
			if (state == TraverseCharState::GetName)
			{
				if (!isspace(boby[pos]))
					attr_name.push_back(boby[pos]);
			}
			else if (state == TraverseCharState::InDoubleQuotation || 
				state == TraverseCharState::InSingleQuotation)
				attr_value.push_back(boby[pos]);
		}
		pos++;
	}

	if (!attr_name.empty())
		node->attributes[attr_name] = attr_value;

	return node;
}

string XmlParser::parseElementContent(const string_view& boby, size_t& pos)
{
	string result;

	auto state = TraverseCharState::Normal;
	while (true)
	{
		if (boby[pos] == '<' && state == TraverseCharState::Normal)
			break;
		if (boby[pos] == '\'')
		{
			if (state == TraverseCharState::Normal)
				state = TraverseCharState::InSingleQuotation;
			else if (state == TraverseCharState::InSingleQuotation)
				state = TraverseCharState::Normal;
		}
		if (boby[pos] == '\"')
		{
			if (state == TraverseCharState::Normal)
				state = TraverseCharState::InDoubleQuotation;
			else if (state == TraverseCharState::InDoubleQuotation)
				state = TraverseCharState::Normal;
		}
		result.push_back(boby[pos++]);
	}

	return result;
}
