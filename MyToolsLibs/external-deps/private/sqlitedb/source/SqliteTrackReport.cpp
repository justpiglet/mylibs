//
//  SqliteTrackReport.cpp
//
#include "SqliteTrackReport.h"

CSqliteTrackReport * CSqliteTrackReport::g_pSelf = NULL;

CSqliteTrackReport::CSqliteTrackReport()
{
    
}

CSqliteTrackReport::~CSqliteTrackReport()
{
    
}

void CSqliteTrackReport::AddItem(CSqliteRecordSet * ptr,const char * lpszPath)
{
    TrackNodeMap::iterator itr = m_TrackNodeMap.find(ptr);
    if(itr != m_TrackNodeMap.end())
    {
        printf("CSqliteTrackReport::AddItem HAS EXIST %s",lpszPath);
        return;
    }
    
    TrackNode * pNode = new TrackNode;
    pNode->m_ptr = ptr;
    pNode->m_path = lpszPath;
    
    m_TrackNodeMap.insert(std::make_pair(ptr, pNode));
}
void CSqliteTrackReport::RemoveItem(CSqliteRecordSet * ptr)
{
    TrackNodeMap::iterator itr = m_TrackNodeMap.find(ptr);
    if(itr != m_TrackNodeMap.end())
    {
        delete itr->second;
        m_TrackNodeMap.erase(itr);
    }
}

void CSqliteTrackReport::ReportTrack()
{
    printf("CSqliteTrackReport: begin \n");
    TrackNode * pNode = NULL;
    TrackNodeMap::iterator itr = m_TrackNodeMap.begin(),itr_end = m_TrackNodeMap.end();
    for (; itr != itr_end; itr++) {
        pNode = itr->second;
        //CCAssert(pNode, "pNode is nil");
        printf("CSqliteTrackReport: SQL --- %s \n",pNode->m_path.c_str());
    }
    printf("CSqliteTrackReport: end \n");
}