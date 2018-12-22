#include <string.h>
#include <stdlib.h>

class Actor {
    char* m_name;
public:
 Actor (const char* name)
 {
     m_name = (char*) malloc(strlen(name)+1);
     strcpy(m_name,name);
 }
 ~Actor () 
 {
     free(m_name);
 }
 const char* GetName ()
 {
     return m_name;
 }
};
