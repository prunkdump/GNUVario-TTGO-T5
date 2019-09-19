//
// definitions for the parsing table structure
//
#include <inttypes.h>

//
// Char types
//
#define whiteSpace       0x100
#define alpha            0x200
#define alphanum         0x300
#define quote            0x400
#define matchingquote    0x500
#define anychar          0x600   // must be one of these at the end of every state

//
// actions
//
#define donothing        0
#define incLTcount       1
#define decLTcount       2
#define storeifneeded    3
#define starttagname     4
#define addtotagname     5
#define removelasttag    6
#define checkremovelasttag 7
#define addtoattrname    8
#define setquotechar     9
#define gotattrvalue    10
#define error           11
#define initialise      12
#define clearattrname   13
#define addtoattrvalue  14
#define inctagcount     15
#define addtochktagname 16
#define cleardata       17
#define cleartagendname 18
#define cleartagname    19

//
// State Defines
//
#define Init               0
#define Init1              Init+3
#define TagStart           Init1+2
#define IgnoreToGT         TagStart+5
#define IgnoreTagToGT      IgnoreToGT+2
#define IgnoreTagToGTEnd   IgnoreTagToGT+3
#define TagEnd             IgnoreTagToGTEnd+2
#define TagName            TagEnd+3
#define InTag              TagName+5
#define InAttr             InTag+5
#define InAttrGetValue     InAttr+3
#define InAttrGetValue1    InAttrGetValue+6
#define InAttrGetValue2    InAttrGetValue1+2

struct parseTable {
  uint16_t   charToParse;
  uint8_t    actionNumber;
  uint8_t    actionNumber2;
  uint8_t    nextState;
};

const parseTable stateTable[] = {
/* 00 Init                */  {'<',           incLTcount,        starttagname,    TagStart},
/* 01 2                   */  {whiteSpace,    donothing,         donothing,       Init},
/* 02 3                   */  {anychar,       cleardata,         storeifneeded,   Init1},

/* 03 Init1               */  {'<',           incLTcount,        starttagname,    TagStart},
/* 04 2                   */  {anychar,       storeifneeded,     donothing,       Init1},

/* 05 TagStart            */  {'?',           cleartagname,      donothing,       IgnoreToGT},       // start of a tag name
/* 06 2                   */  {'!',           cleartagname,      donothing,       IgnoreToGT},       // start of a tag name
/* 07 3                   */  {'/',           cleartagendname,   donothing,       TagEnd},
/* 08 4                   */  {alpha,         addtotagname,      donothing,       TagName},
/* 09 5                   */  {anychar,       error,             initialise,      Init},

/* 10 IgnoreToGT          */  {'>',           decLTcount,        donothing,       Init},             // handle <? ... >
/* 11 2                   */  {anychar,       donothing,         donothing,       IgnoreToGT},

/* 12 IgnoreTagToGT       */  {'>',           decLTcount,        donothing,       Init},             // handle tag of form <name ....>
/* 13 2                   */  {'/',           donothing,         donothing,       IgnoreTagToGTEnd}, // handle tag of form <name ..../>
/* 14 3                   */  {anychar,       donothing,         donothing,       IgnoreTagToGT},

/* 15 IgnoreTagToGTEnd    */  {'>',           removelasttag,     decLTcount,      Init},             // handle tag of form <name ....>
/* 16 2                   */  {anychar,       donothing,         donothing,       IgnoreTagToGT},

/* 17 TagEnd              */  {alphanum,      addtochktagname,   donothing,       TagEnd},           // cope with </tagname>
/* 18 2                   */  {'>',           checkremovelasttag,decLTcount,      Init},
/* 19 3                   */  {anychar,       error,             initialise,      Init},

/* 20 TagName             */  {alphanum,      addtotagname,      donothing,       TagName},          // process <tag...>
/* 21 2                   */  {whiteSpace,    inctagcount,       clearattrname,   InTag},
/* 22 3                   */  {'>',           inctagcount,       decLTcount,      Init},
/* 23 4                   */  {'/',           inctagcount,       donothing,       IgnoreTagToGTEnd},
/* 24 5                   */  {anychar,       error,             initialise,      Init},

/* 25 InTag               */  {alpha,         addtoattrname,     donothing,       InAttr},           // cope with </tagname>
/* 26 2                   */  {whiteSpace,    clearattrname,     donothing,       InTag},
/* 27 3                   */  {'>',           decLTcount,        donothing,       Init},
/* 28 4                   */  {'/',           donothing,         donothing,       IgnoreTagToGTEnd},
/* 29 5                   */  {anychar,       error,             initialise,      Init},

/* 30 InAttr              */  {alphanum,      addtoattrname,     donothing,       InAttr},           // cope with </tagname>
/* 31 2                   */  {'=',           donothing,         donothing,       InAttrGetValue},
/* 32 3                   */  {anychar,       error,             initialise,      Init},

/* 33 InAttrGetValue      */  {quote,         setquotechar,      donothing,       InAttrGetValue1},          // process <tag...> 
/* 34 2                   */  {whiteSpace,    gotattrvalue,      clearattrname,   InTag},
/* 35 3                   */  {'/',           gotattrvalue,      donothing,       IgnoreTagToGTEnd},
/* 36 4                   */  {'>',           gotattrvalue,      decLTcount,      Init},
/* 37 5                   */  {alphanum,      addtoattrvalue,    donothing,       InAttrGetValue},
/* 38 6                   */  {anychar,       error,             initialise,      Init},

/* 39 InAttrGetValue1     */  {matchingquote, gotattrvalue,      donothing,       InAttrGetValue2},          // process <tag...>
/* 40 2                   */  {anychar,       addtoattrvalue,    donothing,       InAttrGetValue1},

/* 41 InAttrGetValue2     */  {whiteSpace,    clearattrname,     donothing,       InTag},          // process <tag...>
/* 42 2                   */  {'/',           donothing,         donothing,       IgnoreTagToGTEnd},
/* 43 3                   */  {'>',           decLTcount,        donothing,       Init},
/* 44 4                   */  {anychar,       error,             initialise,      Init}

};

