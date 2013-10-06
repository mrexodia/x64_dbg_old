#include "instruction.h"
#include "argument.h"
#include "variable.h"
#include "console.h"
#include "value.h"
#include "command.h"

CMDRESULT cbBadCmd(const char* cmd)
{
    uint value=0;
    int valsize=0;
    bool isvar=false;
    bool hexonly=false;
    if(valfromstring(cmd, &value, &valsize, &isvar, false, &hexonly)) //dump variable/value/register/etc
    {
        //printf("[DEBUG] valsize: %d\n", valsize);
        if(valsize)
            valsize*=2;
        else
            valsize=1;
        char format_str[deflen]="";
        if(isvar)// and *cmd!='.' and *cmd!='x') //prevent stupid 0=0 stuff
        {
            if(value>15 and !hexonly)
            {
                sprintf(format_str, "%%s=%%.%d"fext"X (%%"fext"ud)\n", valsize);
                printf(format_str, cmd, value, value);
            }
            else
            {
                sprintf(format_str, "%%s=%%.%d"fext"X\n", valsize);
                printf(format_str, cmd, value);
            }
        }
        else
        {
            if(value>15 and !hexonly)
            {
                sprintf(format_str, "%%.%d"fext"X (%%"fext"ud)\n", valsize);
                printf(format_str, value, value);
            }
            else
            {
                sprintf(format_str, "%%.%d"fext"X\n", valsize);
                printf(format_str, value);
            }
        }
    }
    else //unknown command
    {
        printf("unknown command/expression: \"%s\"\n", cmd);
        return STATUS_ERROR;
    }
    return STATUS_CONTINUE;
}

CMDRESULT cbInstrVar(const char* cmd)
{
    char arg1[deflen]="";
    char arg2[deflen]="";
    if(!argget(cmd, arg1, 0, false)) //var name
        return STATUS_ERROR;
    argget(cmd, arg2, 1, true); //var value (optional)
    uint value=0;
    int add=0;
    if(*arg1=='$')
        add++;
    if(valfromstring(arg1+add, &value, 0, 0, true, 0))
    {
        printf("invalid variable name \"%s\"\n", arg1);
        return STATUS_ERROR;
    }
    if(!valfromstring(arg2, &value, 0, 0, false, 0))
    {
        printf("invalid value \"%s\"\n", arg2);
        return STATUS_ERROR;
    }
    if(!varnew(arg1, value, VAR_USER))
    {
        printf("error creating variable \"%s\"\n", arg1);
        return STATUS_ERROR;
    }
    else
    {
        if(value>15)
            printf("%s=%"fext"X (%"fext"ud)\n", arg1, value, value);
        else
            printf("%s=%"fext"X\n", arg1, value);
    }
    return STATUS_CONTINUE;
}

CMDRESULT cbInstrVarDel(const char* cmd)
{
    char arg1[deflen]="";
    if(!argget(cmd, arg1, 0, false)) //var name
        return STATUS_ERROR;
    if(!vardel(arg1, false))
        printf("could not delete variable \"%s\"\n", arg1);
    else
        printf("deleted variable \"%s\"\n", arg1);
    return STATUS_CONTINUE;
}

CMDRESULT cbInstrMov(const char* cmd)
{
    char arg1[deflen]="";
    char arg2[deflen]="";
    if(!argget(cmd, arg1, 0, false)) //dest name
        return STATUS_ERROR;
    if(!argget(cmd, arg2, 1, false)) //src name
        return STATUS_ERROR;
    uint set_value=0;
    if(!valfromstring(arg2, &set_value, 0, 0, false, 0))
    {
        printf("invalid src \"%s\"\n", arg2);
        return STATUS_ERROR;
    }
    bool isvar=false;
    uint temp;
    valfromstring(arg1, &temp, 0, &isvar, true, 0);
    if(!isvar or !valtostring(arg1, &set_value, false))
    {
        uint value;
        if(valfromstring(arg1, &value, 0, 0, true, 0))
        {
            printf("invalid dest \"%s\"\n", arg1);
            return STATUS_ERROR;
        }
        varnew(arg1, set_value, VAR_USER);
    }
    cbBadCmd(arg1);
    return STATUS_CONTINUE;
}

CMDRESULT cbInstrVarList(const char* cmd)
{
    char arg1[deflen]="";
    argget(cmd, arg1, 0, true);
    int filter=0;
    if(!strcasecmp(arg1, "USER"))
        filter=VAR_USER;
    else if(!strcasecmp(arg1, "READONLY"))
        filter=VAR_READONLY;
    else if(!strcasecmp(arg1, "SYSTEM"))
        filter=VAR_SYSTEM;
    VAR* cur=vargetptr();
    if(!cur or !cur->name)
    {
        cputs("no variables");
        return STATUS_CONTINUE;
    }

    bool bNext=true;
    while(bNext)
    {
        char name[deflen]="";
        strcpy(name, cur->name);
        int len=strlen(name);
        for(int i=0; i<len; i++)
            if(name[i]==1)
                name[i]='/';
        uint value=(uint)cur->value.value;
        if(cur->type!=VAR_HIDDEN)
        {
            if(filter)
            {
                if(cur->type==filter)
                {
                    if(value>15)
                        cprintf("%s=%"fext"X (%"fext"ud)\n", name, value, value);
                    else
                        cprintf("%s=%"fext"X\n", name, value);
                }
            }
            else
            {
                if(value>15)
                    cprintf("%s=%"fext"X (%"fext"ud)\n", name, value, value);
                else
                    cprintf("%s=%"fext"X\n", name, value);
            }
        }
        cur=cur->next;
        if(!cur)
            bNext=false;
    }
    return STATUS_CONTINUE;
}

CMDRESULT cbInstrChd(const char* cmd)
{
    char arg1[deflen]="";
    if(!argget(cmd, arg1, 0, false))
        return STATUS_ERROR;
    if(!DirExists(arg1))
    {
        cputs("directory doesn't exist");
        return STATUS_ERROR;
    }
    SetCurrentDirectoryA(arg1);
    cputs("current directory changed!");
    return STATUS_CONTINUE;
}