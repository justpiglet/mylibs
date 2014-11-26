//
//  SqliteRecord.cpp
//

#include "SqliteError.h"
#include "sqlite/sqlite3.h"
#include "SqliteRecordSet.h"
#include "SqliteRecord.h"
#include "SqliteDatabase.h"

CSqliteRecord::CSqliteRecord()
:m_pData(NULL)
{
}
CSqliteRecord::CSqliteRecord(const CSqliteRecord & other)
:m_pData(NULL)
{
}

CSqliteRecord::CSqliteRecord(CSqliteRecordSet * pRecordSet)
:m_pData(NULL)
{
    InitWiht(pRecordSet);
}

CSqliteRecord& CSqliteRecord::operator=(const CSqliteRecord& other)
{
    Set(other);
    return *this;
}

CSqliteRecord::~CSqliteRecord()
{
    Clear();
}

void CSqliteRecord::InitWiht(CSqliteRecordSet * pRecordSet)
{
    m_pData = new Private;
    m_pData->m_pFields = (Field*)malloc(pRecordSet->GetColumnCount()*sizeof(Field));
    m_pData->m_columnCount = pRecordSet->GetColumnCount();
    
    int type;
    for (int i = 0; i < m_pData->m_columnCount; i++) {
        type = pRecordSet->GetColumnType(i);
        
        switch (type) {
            case SQLITE_TEXT:
            {
                m_pData->m_pFields[i].type = FieldType_Text;
                m_pData->m_pFields[i].text = strdup(pRecordSet->GetString(i));
                break;
            }
            case SQLITE_BLOB:
            case SQLITE_NULL:
            {
                m_pData->m_pFields[i].type = FieldType_Null;
                break;
            }
            case SQLITE_FLOAT:
            {
                m_pData->m_pFields[i].type = FieldType_Real;
                m_pData->m_pFields[i].r = pRecordSet->GetDouble(i);
                break;
            }
            case SQLITE_INTEGER:
            {
                m_pData->m_pFields[i].type = FieldType_Integer;
                m_pData->m_pFields[i].r = pRecordSet->GetInt64(i);
                break;
            }
            default:
                break;
        }
    }
}
void CSqliteRecord::Clear(Private *& pData)
{
    for (int i = 0; i < pData->m_columnCount; i++) {
        if(pData->m_pFields[i].type == FieldType_Text)
            free(pData->m_pFields[i].text);
    }
    free(pData->m_pFields);
    pData = NULL;
    
}
void CSqliteRecord::Clear()
{
    if(m_pData && m_pData->deref() == 0)
    {
        Clear(m_pData);
    }
    m_pData = NULL;
}

void CSqliteRecord::Set(const CSqliteRecord& other)
{
    Clear();
    m_pData = other.m_pData;
	if(m_pData)
		m_pData->ref();
}
int CSqliteRecord::GetFieldsCount()
{
    if(m_pData == NULL)
        return 0;
    
    return m_pData->m_columnCount;
}
CSqliteRecord::Field * CSqliteRecord::GetField(int  colIndex)
{
    if(colIndex >= GetFieldsCount())
        return NULL;
    
    return &m_pData->m_pFields[colIndex];
}
const char * CSqliteRecord::GetString(int  colIndex)
{
    Field * pField = GetField(colIndex);
    if(!pField || pField->type != FieldType_Text)
    {
        CSqliteDatabase::error(DATABASE_ERROR, DATABASE_ERRMSG_FIELD_ERROR);
        return NULL;
    }
    return pField->text;
}
int16   CSqliteRecord::GetShort(int  colIndex)
{
    Field * pField = GetField(colIndex);
    if(!pField || pField->type != FieldType_Integer)
    {
        CSqliteDatabase::error(DATABASE_ERROR, DATABASE_ERRMSG_FIELD_ERROR);
        return 0;
    }
    
    return (int16)pField->v;
}
uint16  CSqliteRecord::GetUShort(int  colIndex)
{
    Field * pField = GetField(colIndex);
    if(!pField || pField->type != FieldType_Integer)
    {
        CSqliteDatabase::error(DATABASE_ERROR, DATABASE_ERRMSG_FIELD_ERROR);
        return 0;
    }
    return (uint16)pField->v;
}
uint32  CSqliteRecord::GetUint(int  colIndex)
{
    Field * pField = GetField(colIndex);
    if(!pField || pField->type != FieldType_Integer)
    {
        CSqliteDatabase::error(DATABASE_ERROR, DATABASE_ERRMSG_FIELD_ERROR);
        return 0;
    }
    return (uint32)pField->v;
}

int32  CSqliteRecord::GetInt(int  colIndex)
{
    Field * pField = GetField(colIndex);
    if(!pField || pField->type != FieldType_Integer)
    {
        CSqliteDatabase::error(DATABASE_ERROR, DATABASE_ERRMSG_FIELD_ERROR);
        return 0;
    }
    return (int32)pField->v;
}
float CSqliteRecord::GetFloat(int  colIndex)
{
    Field * pField = GetField(colIndex);
    if(!pField || pField->type != FieldType_Real)
    {
        CSqliteDatabase::error(DATABASE_ERROR, DATABASE_ERRMSG_FIELD_ERROR);
        return 0.0f;
    }
    return (float)pField->r;
}
double  CSqliteRecord::GetDouble(int  colIndex)
{
    Field * pField = GetField(colIndex);
    if(!pField || pField->type != FieldType_Real)
    {
        CSqliteDatabase::error(DATABASE_ERROR, DATABASE_ERRMSG_FIELD_ERROR);
        return 0.0f;
    }
    return (double)pField->r;
}
uint64  CSqliteRecord::GetUint64(int  colIndex)
{
    Field * pField = GetField(colIndex);
    if(!pField || pField->type != FieldType_Integer)
    {
        CSqliteDatabase::error(DATABASE_ERROR, DATABASE_ERRMSG_FIELD_ERROR);
        return 0.0f;
    }
    return (uint64)pField->v;
}
int64  CSqliteRecord::GetInt64(int  colIndex)
{
    Field * pField = GetField(colIndex);
    if(!pField || pField->type != FieldType_Integer)
    {
        CSqliteDatabase::error(DATABASE_ERROR, DATABASE_ERRMSG_FIELD_ERROR);
        return 0.0f;
    }
    return pField->v;
}
