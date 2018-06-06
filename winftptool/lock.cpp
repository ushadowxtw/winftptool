#include "stdafx.h"
#include "lock.h"
#include <stdlib.h>
BaseLock::BaseLock()
{
#if defined(WIN32)
    m_mutex=CreateMutex(NULL,FALSE,NULL);
#else
     m_mutex= new pthread_mutex_t; 
    int flg=pthread_mutex_init(m_mutex,NULL);
    if(flg!=0)
    {
        abort();
    }

#endif

}


BaseLock & BaseLock::operator = (const BaseLock& RightSides)  
{  
    if (this == &RightSides) // 如果自己给自己赋值则直接返回  
    {
        return *this;
    }
    #if defined(WIN32)
        m_mutex=CreateMutex(NULL,FALSE,NULL);
    #else
        m_mutex= new pthread_mutex_t; 
        int flg=pthread_mutex_init(m_mutex,NULL);
        if(flg!=0)
        {
            abort();
        }
    #endif
}



BaseLock::BaseLock(const BaseLock& C)
{

#if defined(WIN32)
    m_mutex=CreateMutex(NULL,FALSE,NULL);
#else

     m_mutex= new pthread_mutex_t; 
    int flg=pthread_mutex_init(m_mutex,NULL);
    if(flg!=0)
    {
        abort();
    }
#endif

}


BaseLock::~BaseLock()
{
#if defined(WIN32)
    
#else
        if(m_mutex!=NULL)
        {
            pthread_mutex_destroy(m_mutex);
            delete m_mutex;
            m_mutex=NULL;
        }
#endif

}

void BaseLock::lock()
{
#if defined(WIN32)
    WaitForSingleObject(m_mutex,INFINITE);
#else
    pthread_mutex_lock (m_mutex);
#endif
}
void BaseLock::unlock()
{
#if defined(WIN32)
    ReleaseMutex(m_mutex);
#else
    pthread_mutex_unlock (m_mutex);
#endif
    
}


