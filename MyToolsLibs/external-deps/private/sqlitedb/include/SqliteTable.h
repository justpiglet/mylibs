//
//  SqliteTable.h
//

#ifndef _FHKJEF_FDS_CAN_SqliteTable__
#define _FHKJEF_FDS_CAN_SqliteTable__

#include "SqliteDatabasePro.h"

class CSqliteTable {
private:
	int m_cols;
	int m_rows;
	int m_currentRow;
	char** m_results;
public:
    CSqliteTable();
	CSqliteTable(const CSqliteTable& table);
	CSqliteTable(char** results, int rows, int cols);
	virtual ~CSqliteTable();
    
	CSqliteTable& operator=(const CSqliteTable& table);
	int GetColumnCount();
	int GetRowCount();
	int FindColumnIndex(const char * columnName);
	const char * GetColumnName(int columnIndex);
	const char * GetAsString(int columnIndex);
	const char * GetAsString(const char * columnName);
	int GetInt(int columnIndex, int nullValue = 0);
	int GetInt(const char * columnName, int nullValue = 0);
	int64 GetInt64(int columnIndex, int64 nullValue = 0);
	int64 GetInt64(const char * columnName, int64 nullValue = 0);
	double GetDouble(int columnIndex, double nullValue = 0.0);
	double GetDouble(const char *  columnName, double nullValue = 0.0);
	const char * GetString(int columnIndex, const char *  nullValue = NULL);
	const char * GetString(const char * columnName, const char *  nullValue = NULL);
	bool GetBool(int columnIndex);
	bool GetBool(const char *  columnName);
	bool IsNull(int columnIndex);
	bool IsNull(const char *  columnName);
	void SetRow(int row);
	void Finalize();
	bool IsOk();
    
private:
    void CheckResults();

};

#endif /* defined(__TKGame__SqliteTable__) */
