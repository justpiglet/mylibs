#include "tools.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"

char* _CANNP_NAME::files::ReadFile(const char* filePath)
{
	char * buffer = nullptr;
	if (!filePath)return buffer;

	FILE *fp = nullptr;
	if (fopen_s(&fp, filePath, "rb") == 0)
	{
		fseek(fp, 0, SEEK_END);
		int iSize = ftell(fp);
		fseek(fp, 0, SEEK_SET);
		buffer = (char*)malloc(iSize + 1);
		iSize = fread_s(buffer, iSize, sizeof(char), iSize, fp);
		fclose(fp);
		buffer[iSize] = '\0';
	}
	return buffer;
}

bool _CANNP_NAME::files::WriteFile(const char* filePath, const char* pbuffer)
{
	FILE* file = nullptr;
	if (fopen_s(&file, filePath, "wb") == 0)
	{
		fputs(pbuffer, file);
		fclose(file);
		return true;
	}
	else
		return false;
}
bool _CANNP_NAME::files::WriteFile(const char* filePath, const wchar_t* pbuffer)
{
	FILE* file = nullptr;
	if (fopen_s(&file, filePath, "wb") == 0)
	{
		fputws(pbuffer, file);
		fclose(file);
		return true;
	}
	else
		return false;
}

bool _CANNP_NAME::files::ReadFileJsonCpp(const char* filePath, Json::Value& value)
{
	char* pbuffer = ReadFile(filePath);
	if (pbuffer)
	{
		Json::Reader reader;
		if (reader.parse(pbuffer, value))
			return true;
	}

	return false;
}
bool _CANNP_NAME::files::WriteFileJsonCpp(const char* filePath, Json::Value& value)
{
	return WriteFile(filePath,value.toStyledString().c_str());
}



bool _CANNP_NAME::files::ReadFileRapidJson(const char* filePath, rapidjson::Document& document)
{
	char * buffer = ReadFile(filePath);
	if (!buffer)
		return false;

	const char* pError = document.Parse<rapidjson::kParseDefaultFlags>(buffer).GetParseError();
	delete buffer;
	if (pError)
		printf(pError);

	return pError == nullptr;
}

bool _CANNP_NAME::files::WriteFileRapidJson(const char* filePath, rapidjson::Document& document)
{
	rapidjson::StringBuffer  buffersss;
	rapidjson::Writer<rapidjson::StringBuffer> writer(buffersss);
	document.Accept(writer);

	return WriteFile(filePath, buffersss.GetString());
}
