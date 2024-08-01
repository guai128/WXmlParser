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

	shared_ptr<XmlNode> rootNode;
	XmlParseContext context(rootNode);

	auto [version, encoding] = parseHead(head);
	context.version = version, context.encoding = encoding;

	// parse body
	auto body = xml.substr(head.size(), xml.size() - head.size());
	std::stack<string> node_stack;
	// get root node
	rootNode = 
	
	


	
	
	
	




	return XmlParseContext(shared_ptr<XmlNode>(nullptr));
}

bool XmlParser::checkHead(const string_view& xml) const
{
	if (xml.find("<?xml") != 0|| xml.find("?>") != xml.size() - 2)
		return false;

	return true;
}

std::pair<string_view, string_view> XmlParser::parseHead(const string_view& xml) const
{
	auto head = xml.substr(5, xml.size() - 7);
	auto pos = head.find("version");
	auto version = head.substr(pos + 8, head.find("\"", pos + 8) - pos - 8);
	pos = head.find("encoding");
	auto encoding = head.substr(pos + 9, head.find("\"", pos + 9) - pos - 9);

	return std::make_pair(version, encoding);
}
