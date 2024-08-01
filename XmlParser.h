#pragma once
#include <string>
#include <memory>
#include <vector>
#include <mutex>
#include <map>
#include <stack>

namespace WXML
{
	using std::string;
	using std::shared_ptr;
	using std::vector;
	using std::string_view;
	using std::map;

	struct XmlNode
	{
		string name, value, decription; // first version not support decription
		map<string, string> attributes;

		shared_ptr<XmlNode> parent { nullptr };
		vector<shared_ptr<XmlNode>> children;
	};

	class XmlParseContext
	{
	public:
		explicit XmlParseContext(shared_ptr<XmlNode> root)
			: _root(root)
		{ }

	public:
		string version, encoding;

	private:
		shared_ptr<XmlNode> _root;
	};

	class XmlParser
	{
	// construct function
	public:
		static shared_ptr<XmlParser> getXmlParser();
	private:
		XmlParser() = default;
		XmlParser(const XmlParser&) = delete;
		XmlParser& operator=(const XmlParser&) = delete;

	// tool function
	public:
		XmlParseContext parse(const string_view& xml);
	private:
		enum class TraverseCharState
		{
			Normal, // normal state
			InSingleQuotation, // meet single quotation
			InDoubleQuotation, // meet double quotation
		};

	private:
		// check if the head is valid
		bool checkHead(const string_view& xml) const;
		std::pair<string_view, string_view> parseHead(const string_view& xml) const;

		// parse single element
		shared_ptr<XmlNode> parseElement(const string_view& boby, size_t& pos);
	private:
		static std::once_flag _initFlag;
		static shared_ptr<XmlParser> _instance;
	};
}