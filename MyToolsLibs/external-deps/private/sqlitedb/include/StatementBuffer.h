//
//  StatementBuffer.h
//

#ifndef _FHKJEF_FDS_CAN_StatementBuffer__
#define _FHKJEF_FDS_CAN_StatementBuffer__
#include<windows.h>

class CStatementBuffer
{
public:
    CStatementBuffer();
    
    ~CStatementBuffer();
    
    
    const char* Format(const char* format, ...);
    const char* FormatV(const char* format, va_list va);
    
    operator const char*() const { return m_buffer; }
    
    void Clear();
private:
    char* m_buffer;
};

#endif /* defined(__TKGame__StatementBuffer__) */
