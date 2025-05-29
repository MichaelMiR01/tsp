#ifndef _TCL
#include <tcl.h>
#endif
int TSP_debug(Tcl_Interp* interp, char const *format, ...) {
    /* Helper for Debugging*/
    char __buf [4096];
    va_list aptr;
    int ret;
    va_start(aptr, format);
    ret = vsnprintf(__buf, 4096, format, aptr);
    va_end(aptr);
    // compensate for surplus linebreaks, sincs puts will already write one
    if(ret<0) return EOF;
    if(ret>4096) ret=4096;
    if(__buf[ret-1]=='\n') __buf[ret-1]=0;
    Tcl_Interp* ip =  interp; 
    if (ip==NULL) Tcl_Panic("No interp found to call tcl routine!");
    mod_Tcl_errorCode=0;
    Tcl_Obj*  argObjvArray [2];
    Tcl_Obj* funcname = Tcl_NewStringObj("puts",-1);
    Tcl_IncrRefCount(funcname);
    argObjvArray[0] = funcname;
    Tcl_Obj* target_1 = Tcl_NewStringObj(__buf,-1);
    Tcl_IncrRefCount(target_1);
    argObjvArray[1] = target_1;
    int rs = Tcl_EvalObjv(ip, 2, argObjvArray, 0);
    if(funcname!=NULL) Tcl_DecrRefCount(funcname);
    if(target_1 != NULL) Tcl_DecrRefCount(target_1);
    if(rs !=TCL_OK) {
        Tcl_Eval (ip, "puts {Error evaluating TCL-Function puts}; puts $errorInfo; flush stdout;");
        return EOF;
    }
    Tcl_DoOneEvent(TCL_DONT_WAIT|TCL_ALL_EVENTS);
    return 1; 
}

/*********************************************************************************************/
/* convert to an int from a string */
TSP_REMOVABLE int
TSP_Util_lang_convert_int_string(Tcl_Interp* interp, Tcl_DString* sourceVarName, Tcl_WideInt* targetVarName) {
    int rc;
    Tcl_Obj* obj = Tcl_NewStringObj(Tcl_DStringValue(sourceVarName), Tcl_DStringLength(sourceVarName));
    Tcl_IncrRefCount(obj);
    rc = Tcl_GetWideIntFromObj(interp, obj, targetVarName);
    Tcl_DecrRefCount(obj);
    return rc;
}


/*********************************************************************************************/
/* convert to an int from a string const */
TSP_REMOVABLE int
TSP_Util_lang_convert_int_string_const(Tcl_Interp* interp, char* sourceVarName, Tcl_WideInt* targetVarName) {
    int rc;
    Tcl_Obj* obj = Tcl_NewStringObj(sourceVarName, -1);
    Tcl_IncrRefCount(obj);
    rc = Tcl_GetWideIntFromObj(interp, obj, targetVarName);
    Tcl_DecrRefCount(obj);
    return rc;
}


/*********************************************************************************************/
/* convert to a string from an int */
TSP_REMOVABLE Tcl_DString*
TSP_Util_lang_convert_string_int(Tcl_Interp* interp, Tcl_DString* targetVarName, Tcl_WideInt sourceVarName) {
    char str[500];
    char *format = "%" TCL_LL_MODIFIER "d";
    if (targetVarName != NULL) {
        Tcl_DStringSetLength(targetVarName, 0);
    } else {
        targetVarName = (Tcl_DString*) ckalloc(sizeof(Tcl_DString));;
        Tcl_DStringInit(targetVarName);
    }
    sprintf(str, format, sourceVarName);
    Tcl_DStringAppend(targetVarName, str, -1);
    return targetVarName;
}



/*********************************************************************************************/
/* convert to a string from a double */
TSP_REMOVABLE Tcl_DString*
TSP_Util_lang_convert_string_double(Tcl_Interp* interp, Tcl_DString* targetVarName, double sourceVarName) {
    char str[500];
    if (targetVarName != NULL) {
        Tcl_DStringSetLength(targetVarName, 0);
    } else {
        targetVarName = (Tcl_DString*) ckalloc(sizeof(Tcl_DString));;
        Tcl_DStringInit(targetVarName);
    }
    Tcl_PrintDouble(interp, sourceVarName, str);
    Tcl_DStringAppend(targetVarName, str, -1);
    return targetVarName;
}


/*********************************************************************************************/
/* convert to a string from a var */
TSP_REMOVABLE Tcl_DString*
TSP_Util_lang_convert_string_var(Tcl_DString* targetVarName, Tcl_Obj* sourceVarName) {
    char* str;
    int len;
    if (targetVarName != NULL) {
        Tcl_DStringSetLength(targetVarName, 0);
    } else {
        targetVarName = (Tcl_DString*) ckalloc(sizeof(Tcl_DString));;
        Tcl_DStringInit(targetVarName);
    }
    str = Tcl_GetStringFromObj(sourceVarName, &len);
    Tcl_DStringAppend(targetVarName, str, len);
    return targetVarName;
}


/*********************************************************************************************/
/* get a string from an int */
/* string must be used immediately */
TSP_REMOVABLE Tcl_DString*
TSP_Util_lang_get_string_int(Tcl_WideInt sourceVarName) {
    static int doInit = 1;
    static Tcl_DString ds;
    Tcl_DString* dsPtr;
    if (doInit) {
        Tcl_DStringInit(&ds);
        doInit = 0;
    } else {
        Tcl_DStringSetLength(&ds, 0);
    }
    dsPtr = &ds;
    dsPtr = TSP_Util_lang_convert_string_int(NULL, dsPtr, sourceVarName);
    return dsPtr;
}


/*********************************************************************************************/
/* get a string from an double */
/* string must be used immediately */
TSP_REMOVABLE Tcl_DString*
TSP_Util_lang_get_string_double(double sourceVarName) {
    static int doInit = 1;
    static Tcl_DString ds;
    if (doInit) {
        Tcl_DStringInit(&ds);
        doInit = 0;
    } else {
        Tcl_DStringSetLength(&ds, 0);
    }
    TSP_Util_lang_convert_string_double(NULL, &ds, sourceVarName);
    return &ds;
}

/*********************************************************************************************/
/* get a string from a var */
/* string must be used immediately */
TSP_REMOVABLE Tcl_DString*
TSP_Util_lang_get_string_var(Tcl_Obj* sourceVarName) {
    static int doInit = 1;
    static Tcl_DString ds;
    int len;
    char* str;
    if (doInit) {
        Tcl_DStringInit(&ds);
        doInit = 0;
    } else {
        Tcl_DStringSetLength(&ds, 0);
    }
    str = Tcl_GetStringFromObj(sourceVarName, &len);
    Tcl_DStringAppend(&ds, str, len);
    return &ds;
}


/*********************************************************************************************/
/* assign a var from a boolean */
TSP_REMOVABLE Tcl_Obj*
TSP_Util_lang_assign_var_boolean(Tcl_Obj* targetVarName, int sourceVarName) {
    if (targetVarName != NULL) {
        Tcl_DecrRefCount(targetVarName);
    }
    targetVarName = Tcl_NewBooleanObj(sourceVarName);
    Tcl_IncrRefCount(targetVarName);
    return targetVarName;
}


/*********************************************************************************************/
/* assign a var from an int */
TSP_REMOVABLE Tcl_Obj*
TSP_Util_lang_assign_var_int(Tcl_Obj* targetVarName, Tcl_WideInt sourceVarName) {
    if (targetVarName != NULL) {
        Tcl_DecrRefCount(targetVarName);
    }
    targetVarName = Tcl_NewWideIntObj(sourceVarName);
    Tcl_IncrRefCount(targetVarName);
    return targetVarName;
}


/*********************************************************************************************/
/* assign a var from an double */
TSP_REMOVABLE Tcl_Obj*
TSP_Util_lang_assign_var_double(Tcl_Obj* targetVarName, double sourceVarName) {
    if (targetVarName != NULL) {
        Tcl_DecrRefCount(targetVarName);
    }
    targetVarName = Tcl_NewDoubleObj(sourceVarName);
    Tcl_IncrRefCount(targetVarName);
    return targetVarName;
}


/*********************************************************************************************/
/* assign a var from an string */
TSP_REMOVABLE Tcl_Obj*
TSP_Util_lang_assign_var_string(Tcl_Obj* targetVarName, Tcl_DString* sourceVarName) {
    if (targetVarName != NULL) {
        Tcl_DecrRefCount(targetVarName);
    }
    targetVarName = Tcl_NewStringObj(Tcl_DStringValue(sourceVarName), Tcl_DStringLength(sourceVarName));
    Tcl_IncrRefCount(targetVarName);
    return targetVarName;
}


/*********************************************************************************************/
/* assign a var from an string const */
TSP_REMOVABLE Tcl_Obj*
TSP_Util_lang_assign_var_string_const(Tcl_Obj* targetVarName, char* sourceVarName) {
    if (targetVarName != NULL) {
        Tcl_DecrRefCount(targetVarName);
    }
    targetVarName = Tcl_NewStringObj(sourceVarName, -1);
    Tcl_IncrRefCount(targetVarName);
    return targetVarName;
}


/*********************************************************************************************/
/* assign a var from a var */
TSP_REMOVABLE Tcl_Obj*
TSP_Util_lang_assign_var_var(Tcl_Obj* targetVarName, Tcl_Obj* sourceVarName) {
    if (targetVarName != NULL) {
        Tcl_DecrRefCount(targetVarName);
    }

     targetVarName = Tcl_DuplicateObj(sourceVarName);  
    //targetVarName = sourceVarName;

    Tcl_IncrRefCount(targetVarName);
    return targetVarName;
}


/*********************************************************************************************/
/* assign an array & element from a var */

TSP_REMOVABLE int
TSP_Util_lang_assign_array_var(Tcl_Interp* interp, Tcl_Obj* targetArrayVar, Tcl_Obj* targetIdxVar, Tcl_Obj* var) {
    Tcl_Obj* obj;
    if (var==NULL) {return TCL_ERROR;};
    //debug(interp,"TSP_Util_lang_assign_array_var %p %p %p\n",targetArrayVar,targetIdxVar,var);
    obj = Tcl_ObjSetVar2(interp, targetArrayVar, targetIdxVar, var, TCL_LEAVE_ERR_MSG);
    if (obj == NULL) {
        return TCL_ERROR;
    } else {
        return TCL_OK;
    }
}

/*********************************************************************************************/
/* compare a Tcl_DString to a const string. use negative length to find first null in string2 */
TSP_REMOVABLE int
TSP_Util_string_compare_const(Tcl_DString* s1, char* string2, int length2) {
    char* string1;
    int length1;
    int length;
    int match;

    string1 = Tcl_DStringValue(s1);
    length1 = Tcl_DStringLength(s1);

    length1 = Tcl_NumUtfChars(string1, length1);
    length2 = Tcl_NumUtfChars(string2, length2);
    
    length = (length1 < length2) ? length1 : length2;
    match = Tcl_UtfNcmp(string1, string2, (unsigned) length);
    if (match == 0) {
        match = length1 - length2;
    }
    return match;
}


/*********************************************************************************************/
/* compare two Tcl_DStrings */
TSP_REMOVABLE int
TSP_Util_string_compare(Tcl_DString* s1, Tcl_DString* s2) {
    char* string2;
    int length2;

    return TSP_Util_string_compare_const(s1, Tcl_DStringValue(s2), Tcl_DStringLength(s2));
}


/* create Dstring from Char in tmpVar */ 
Tcl_DString* TSP_UTIL_dstring_from_char (char* str, Tcl_DString* tmpvar) {
    Tcl_DStringSetLength(tmpvar, 0);
    Tcl_DStringAppend(tmpvar, str, -1);
    return tmpvar;
}



/*********************************************************************************************/
/* create a constant string obj                                                              */
TSP_REMOVABLE Tcl_Obj*
TSP_Util_const_string(char* str) {
    Tcl_Obj* constObj;
    constObj = Tcl_NewStringObj(str, -1);
    Tcl_IncrRefCount(constObj);
    return constObj;
}

/*********************************************************************************************/
/* create a constant wide int obj                                                            */
TSP_REMOVABLE Tcl_Obj*
TSP_Util_const_int(Tcl_WideInt i) {
    Tcl_Obj* constObj;
    constObj = Tcl_NewWideIntObj(i);
    Tcl_IncrRefCount(constObj);
    return constObj;
}

/*********************************************************************************************/
/* create a constant double obj                                                              */
TSP_REMOVABLE Tcl_Obj*
TSP_Util_const_double(double d) {
    Tcl_Obj* constObj;
    constObj = Tcl_NewDoubleObj(d);
    Tcl_IncrRefCount(constObj);
    return constObj;
}



