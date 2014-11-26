//
//  SqliteTable.cpp
//  TKGame
//
//  Created by Hotman on 13-3-18.
//  Copyright (c) 2013年 Hotman. All rights reserved.
//

#include "SqliteTable.h"
#include "SqliteError.h"
#include "SqliteDatabase.h"

CSqliteTable::CSqliteTable()
{
	m_results = 0;
	m_rows = 0;
	m_cols = 0;
	m_currentRow = 0;
}

CSqliteTable::CSqliteTable( const CSqliteTable& table )
{
	m_results = table.m_results;
	const_cast<CSqliteTable&>(table).m_results = 0;
	m_rows = table.m_rows;
	m_cols = table.m_cols;
	m_currentRow = table.m_currentRow;
}

CSqliteTable::CSqliteTable( char** results, int rows, int cols )
{
	m_results = results;
	m_rows = rows;
	m_cols = cols;
	m_currentRow = 0;
}

CSqliteTable::~CSqliteTable()
{
    Finalize();
}

CSqliteTable& CSqliteTable::operator=( const CSqliteTable& table )
{
	if (this != &table)
	{
        Finalize();
		m_results = table.m_results;
		const_cast<CSqliteTable&>(table).m_results = 0;
		m_rows = table.m_rows;
		m_cols = table.m_cols;
		m_currentRow = table.m_currentRow;
	}
	return *this;
}

int CSqliteTable::FindColumnIndex( const char * columnName )
{
	CheckResults();
	if (columnName)
	{
		for (int columnIndex = 0; columnIndex < m_cols; columnIndex++)
		{
			if (strcmp(columnName, m_results[columnIndex]) == 0)
			{
				return columnIndex;
			}
		}
	}
    
	CSqliteDatabase::error(DATABASE_ERROR, DATABASE_ERRMSG_INVALID_NAME);
	return 0;
}

void CSqliteTable::Finalize()
{
	if (m_results)
	{
		sqlite3_free_table(m_results);
		m_results = 0;
	}
}

int CSqliteTable::GetColumnCount()
{
	CheckResults();
	return m_cols;
}

int CSqliteTable::GetRowCount()
{
	CheckResults();
	return m_rows;
}

const char * CSqliteTable::GetColumnName( int columnIndex )
{
	CheckResults();
	if (columnIndex < 0 || columnIndex > m_cols-1)
	{
		CSqliteDatabase::error(DATABASE_ERROR, DATABASE_ERRMSG_INVALID_INDEX);
	}
	return m_results[columnIndex];
}

const char * CSqliteTable::GetAsString( int columnIndex )
{
	if (columnIndex < 0 || columnIndex > m_cols-1)
	{
		CSqliteDatabase::error(DATABASE_ERROR, DATABASE_ERRMSG_INVALID_INDEX);
	}
	int nIndex = (m_currentRow*m_cols) + m_cols + columnIndex;
	const char* localValue = m_results[nIndex];
	return localValue;
}

const char * CSqliteTable::GetAsString( const char * columnName )
{
	int index = FindColumnIndex(columnName);
	return GetAsString(index);
}

int CSqliteTable::GetInt( int columnIndex, int nullValue /*= 0*/ )
{
	if (IsNull(columnIndex))
		return nullValue;
	else
		return (int) atol(GetAsString(columnIndex));
}

int CSqliteTable::GetInt( const char * columnName, int nullValue /*= 0*/ )
{
	if (IsNull(columnName))
		return nullValue;
	else
		return (int) atol(GetAsString(columnName));
}

inline int64 ConvertStringToint64(const char * str, int64 defValue /*=0*/)
{
	size_t n = strlen(str);
	size_t j = 0;
	int64 value = 0;
	bool negative = false;
    
	if (str[j] == '-')
	{
		negative = true;
		j++;
	}
    
	while (j < n)
	{
		if (str[j] < '0' || str[j] > '9')
		{
			return defValue;
		}
		value *= 10;
		value += (str[j] - '0');
		j++;
	}
    
	return negative ? -value : value;
}
int64 CSqliteTable::GetInt64( int columnIndex, int64 nullValue /*= 0*/ )
{
	if (IsNull(columnIndex))
	{
		return nullValue;
	}
	else
	{
		return ConvertStringToint64(GetAsString(columnIndex), nullValue);
	}
}

int64 CSqliteTable::GetInt64( const char * columnName, int64 nullValue /*= 0*/ )
{
	if (IsNull(columnName))
	{
		return nullValue;
	}
	else
	{
		return ConvertStringToint64(GetAsString(columnName), nullValue);
	}
}

static double SQLite3AtoF(const char *z)
{
	int sign = 1;
	long double v1 = 0.0;
	int nSignificant = 0;
	while (isspace(*(unsigned char*)z))
	{
		++z;
	}
	if (*z == '-')
	{
		sign = -1;
		++z;
	}
	else if (*z == '+')
	{
		++z;
	}
	while (*z == '0')
	{
		++z;
	}
	while (isdigit(*(unsigned char*)z))
	{
		v1 = v1*10.0 + (*z - '0');
		++z;
		++nSignificant;
	}
	if (*z == '.')
	{
		long double divisor = 1.0;
		++z;
		if (nSignificant == 0)
		{
			while (*z == '0')
			{
				divisor *= 10.0;
				++z;
			}
		}
		while (isdigit(*(unsigned char*)z))
		{
			if (nSignificant < 18)
			{
				v1 = v1*10.0 + (*z - '0');
				divisor *= 10.0;
				++nSignificant;
			}
			++z;
		}
		v1 /= divisor;
	}
	if (*z=='e' || *z=='E')
	{
		int esign = 1;
		int eval = 0;
		long double scale = 1.0;
		++z;
		if (*z == '-')
		{
			esign = -1;
			++z;
		}
		else if (*z == '+')
		{
			++z;
		}
		while (isdigit(*(unsigned char*)z))
		{
			eval = eval*10 + *z - '0';
			++z;
		}
		while (eval >= 64) { scale *= 1.0e+64; eval -= 64; }
		while (eval >= 16) { scale *= 1.0e+16; eval -= 16; }
		while (eval >=  4) { scale *= 1.0e+4;  eval -= 4; }
		while (eval >=  1) { scale *= 1.0e+1;  eval -= 1; }
		if (esign < 0)
		{
			v1 /= scale;
		}
		else
		{
			v1 *= scale;
		}
	}
	return (double) ((sign < 0) ? -v1 : v1);
}
double CSqliteTable::GetDouble( int columnIndex, double nullValue /*= 0.0*/ )
{
	if (IsNull(columnIndex))
	{
		return nullValue;
	}
	else
	{
		if (columnIndex < 0 || columnIndex > m_cols-1)
		{
			CSqliteDatabase::error(DATABASE_ERROR, DATABASE_ERRMSG_INVALID_INDEX);
		}
		int nIndex = (m_currentRow*m_cols) + m_cols + columnIndex;
		return SQLite3AtoF(m_results[nIndex]);
	}
}

double CSqliteTable::GetDouble( const char * columnName, double nullValue /*= 0.0*/ )
{
	int index = FindColumnIndex(columnName);
	return GetDouble(index, nullValue);
}

const char * CSqliteTable::GetString( int columnIndex, const char * nullValue /*= NULL*/ )
{
	if (IsNull(columnIndex))
	{
		return nullValue;
	}
	else
	{
		return GetAsString(columnIndex);
	}
}

const char * CSqliteTable::GetString( const char * columnName, const char * nullValue /*= NULL*/ )
{
	if (IsNull(columnName))
	{
		return nullValue;
	}
	else
	{
		return GetAsString(columnName);
	}
}

bool CSqliteTable::GetBool( int columnIndex )
{
	return GetInt(columnIndex) != 0;
}

bool CSqliteTable::GetBool( const char * columnName )
{
	int columnIndex = FindColumnIndex(columnName);
	return GetBool(columnIndex);
}

bool CSqliteTable::IsNull( int columnIndex )
{
	CheckResults();
    
	if (columnIndex < 0 || columnIndex > m_cols-1)
	{
		CSqliteDatabase::error(DATABASE_ERROR, DATABASE_ERRMSG_INVALID_INDEX);
	}
    
	int index = (m_currentRow*m_cols) + m_cols + columnIndex;
	const char* localValue = m_results[index];
	return (localValue == 0);
}

bool CSqliteTable::IsNull( const char * columnName )
{
	int index = FindColumnIndex(columnName);
	return IsNull(index);
}

void CSqliteTable::SetRow( int row )
{
	CheckResults();
    
	if (row < 0 || row > m_rows-1)
	{
		CSqliteDatabase::error(DATABASE_ERROR, DATABASE_ERRMSG_INVALID_ROW);
	}
    
	m_currentRow = row;
}

bool CSqliteTable::IsOk()
{
	return (m_results != 0);
}

void CSqliteTable::CheckResults()
{
	if (m_results == 0)
	{
		CSqliteDatabase::error(DATABASE_ERROR, DATABASE_ERRMSG_NORESULT);
	}
}