//
//  SqliteRecord.h
//

#ifndef _FHKJEF_FDS_CAN_SqliteRecord__
#define _FHKJEF_FDS_CAN_SqliteRecord__

#include "SqliteDatabasePro.h"

class CSqliteRecordSet;
/*
#define SQLITE_INTEGER  1
#define SQLITE_FLOAT    2
#define SQLITE_BLOB     4
#define SQLITE_NULL     5
#ifdef SQLITE_TEXT
# undef SQLITE_TEXT
#else
# define SQLITE_TEXT     3
#endif
#define SQLITE3_TEXT     3
8*/
class CSqliteRecord {
    enum FieldType
    {
        FieldType_Null = 0,
        FieldType_Text,
        FieldType_Blob,
        FieldType_Real,
        FieldType_Integer,
        
    };
    struct Field
    {
        Field()
        :type(FieldType_Text),ptr(NULL)
        {
            
        }
        FieldType type;
        union
        {
            int64   v;  //
            double  r;  // REAL
            char   * text;
            void * ptr;
        };
    };
    struct Private
    {
        Private()
        :m_pFields(NULL)
        ,m_columnCount(0)
        ,m_uRef(1)
        {
            
        }
        Field * m_pFields;
        int     m_columnCount;
        unsigned int m_uRef;
        
        unsigned int deref()
        {
            return --m_uRef;
        }
        unsigned int ref()
        {
            return ++m_uRef;
        }
    };

    Private * m_pData;
public:
    CSqliteRecord();
	CSqliteRecord(const CSqliteRecord & other);
	CSqliteRecord(CSqliteRecordSet * pRecordSet);
    
	CSqliteRecord& operator=(const CSqliteRecord& other);
    
    ~CSqliteRecord();
    
    void InitWiht(CSqliteRecordSet * pRecordSet);
    void Clear();
    void Set(const CSqliteRecord& other);
    int GetFieldsCount();
    
    const char * GetString(int  colIndex);
    int16   GetShort(int  colIndex);
    uint16  GetUShort(int  colIndex);
    uint32  GetUint(int  colIndex);
    int32  GetInt(int  colIndex);
    float   GetFloat(int  colIndex);
    double  GetDouble(int  colIndex);
    uint64  GetUint64(int  colIndex);
    int64  GetInt64(int  colIndex);
    
private:
    void Clear(Private *& pData);
    Field * GetField(int  colIndex);
};

#endif /* defined(__TKGame__SqliteRecord__) */
