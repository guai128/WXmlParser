#include "XmlParser.h"
#include <iostream>

using namespace std;
using namespace WXML;

// traverse the xml tree
void traverse(const shared_ptr<XmlNode>& node, int depth = 0)
{
	if (node == nullptr) return;

	for (int i = 0; i < depth; i++) cout << "  ";
	cout << node->name << " : " << node->value;
	
	if (node->attributes.empty())
		cout << "\n";
	else
	{
		cout << " ( ";
		for (const auto& attr : node->attributes)
			cout << attr.first << " : " << attr.second << " ";
		cout << ")\n";
	}

	for (const auto& child : node->children)
		traverse(child, depth + 1);
}

int main()
{
	auto parser = XmlParser::getXmlParser();

	const string xml =
		"<?xml version=\"1.0\" encoding=\"UTF - 8\"?> \
		<site> \
		<name a = \"10\">RUNOOB</name> \
		<url>https://www.runoob.com</url> \
	<logo>runoob - logo.png</logo> \
		<desc>编程学习网站</desc> \
		</site>";

	auto context = parser->parse(xml);

	traverse(context.root());

	return 0;
}