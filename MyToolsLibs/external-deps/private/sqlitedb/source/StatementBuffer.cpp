//
//  StatementBuffer.cpp
//

#include "StatementBuffer.h"
#include "sqlite/sqlite3.h"

CStatementBuffer::CStatementBuffer()
{
	m_buffer = 0;
}

CStatementBuffer::~CStatementBuffer()
{
	Clear();
}

void CStatementBuffer::Clear()
{
	if (m_buffer)
	{
		sqlite3_free(m_buffer);
		m_buffer = 0;
	}
}

const char* CStatementBuffer::Format(const char* format, ...)
{
	Clear();
	va_list va;
	va_start(va, format);
	m_buffer = sqlite3_vmprintf(format, va);
	va_end(va);
	return m_buffer;
}

const char* CStatementBuffer::FormatV(const char* format, va_list va)
{
	Clear();
	m_buffer = sqlite3_vmprintf(format, va);
	return m_buffer;
}