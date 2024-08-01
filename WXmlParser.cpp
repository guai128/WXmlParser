#include "XmlParser.h"

using namespace WXML;

int main()
{
	auto parser = XmlParser::getXmlParser();

	const string xml =
		"<?xml version=\"1.0\" encoding=\"UTF - 8\"?> \
		<site> \
		<name>RUNOOB</name> \
		<url>https://www.runoob.com</url> \
	<logo>runoob - logo.png</logo> \
		<desc>编程学习网站</desc> \
		</site>";

	auto context = parser->parse(xml);





	return 0;
}