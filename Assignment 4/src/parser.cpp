#include "includes/parser.h"

using namespace std;

//Gumbo parser clean text function
string Parser::cleantext(GumboNode* node) {
	if (node->type == GUMBO_NODE_TEXT) {
		return string(node->v.text.text);
	} 
	else if (node->type == GUMBO_NODE_ELEMENT &&
	node->v.element.tag != GUMBO_TAG_SCRIPT &&
	node->v.element.tag != GUMBO_TAG_STYLE) {
		string contents = "";
		GumboVector* children = &node->v.element.children;
		for (unsigned int i = 0; i < children->length; ++i) {
			const string text = cleantext((GumboNode*) children->data[i]);
			if (i != 0 && !text.empty()) {
				contents.append(" ");
			}
			contents.append(text);
		}
		return contents;
	} else {
		return "";
	}
}

string Parser::getCleanText(string contents){
	GumboOutput* output = gumbo_parse(contents.c_str());
	string textOriginal = cleantext(output->root);
	gumbo_destroy_output(&kGumboDefaultOptions, output);

	//Converts the text to an utf16 string resultant from the parser
	//and removes non-latin caracters, punctuations and marking caracters
	wstring_convert<codecvt_utf8_utf16<wchar_t>> converter;
	wstring tmpText = converter.from_bytes(textOriginal);

	bool flag = false;
	wstring tmpCleanText;
	for (int i = 0; i < (int)tmpText.size(); i++){
		if ((tmpText[i] >= 65 && tmpText[i] <= 90) ||   //Basic Latin (A,..., Z)
			(tmpText[i] >= 97 && tmpText[i] <= 122) ||  //Basic Latin (a,..., z)
			(tmpText[i] >= 48 && tmpText[i] <= 57) ||   //Basic Latin (0,..., 9)
			(tmpText[i] >= 192 && tmpText[i] <= 255) || //Additional Latin Letters
			(tmpText[i] >= 256 && tmpText[i] <= 383) || //Extendent Latin A
			(tmpText[i] >= 384 && tmpText[i] <= 591) ){ //Extendent Latin B
			tmpCleanText += tmpText[i];
			flag = true;
		}
		//if there's no space separating the last substring from the current one
		//a space is added between them  
		else if (flag){
			tmpCleanText += ' ';
			flag = false;
		}
	}

	//Converts back the utf16 text to utf8 
	string cleanText = converter.to_bytes(tmpCleanText);

	return cleanText;
}