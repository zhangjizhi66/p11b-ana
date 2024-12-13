// Do NOT change. Changes will be lost next time file is generated

#define R__DICTIONARY_FILENAME LinkDict
#define R__NO_DEPRECATION

/*******************************************************************/
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#define G__DICTIONARY
#include "RConfig.h"
#include "TClass.h"
#include "TDictAttributeMap.h"
#include "TInterpreter.h"
#include "TROOT.h"
#include "TBuffer.h"
#include "TMemberInspector.h"
#include "TInterpreter.h"
#include "TVirtualMutex.h"
#include "TError.h"

#ifndef G__ROOT
#define G__ROOT
#endif

#include "RtypesImp.h"
#include "TIsAProxy.h"
#include "TFileMergeInfo.h"
#include <algorithm>
#include "TCollectionProxyInfo.h"
/*******************************************************************/

#include "TDataMember.h"

// The generated code does not explicitly qualifies STL entities
namespace std {} using namespace std;

// Header files passed as explicit arguments
#include "DataAnalysis.hh"

// Header files passed via #pragma extra_include

namespace ROOT {
   static TClass *events_Dictionary();
   static void events_TClassManip(TClass*);
   static void *new_events(void *p = 0);
   static void *newArray_events(Long_t size, void *p);
   static void delete_events(void *p);
   static void deleteArray_events(void *p);
   static void destruct_events(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::events*)
   {
      ::events *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(::events));
      static ::ROOT::TGenericClassInfo 
         instance("events", "DataAnalysis.hh", 30,
                  typeid(::events), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &events_Dictionary, isa_proxy, 4,
                  sizeof(::events) );
      instance.SetNew(&new_events);
      instance.SetNewArray(&newArray_events);
      instance.SetDelete(&delete_events);
      instance.SetDeleteArray(&deleteArray_events);
      instance.SetDestructor(&destruct_events);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::events*)
   {
      return GenerateInitInstanceLocal((::events*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const ::events*)0x0); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *events_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const ::events*)0x0)->GetClass();
      events_TClassManip(theClass);
   return theClass;
   }

   static void events_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   // Wrappers around operator new
   static void *new_events(void *p) {
      return  p ? new(p) ::events : new ::events;
   }
   static void *newArray_events(Long_t nElements, void *p) {
      return p ? new(p) ::events[nElements] : new ::events[nElements];
   }
   // Wrapper around operator delete
   static void delete_events(void *p) {
      delete ((::events*)p);
   }
   static void deleteArray_events(void *p) {
      delete [] ((::events*)p);
   }
   static void destruct_events(void *p) {
      typedef ::events current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::events

namespace ROOT {
   static TClass *vectorlEeventsgR_Dictionary();
   static void vectorlEeventsgR_TClassManip(TClass*);
   static void *new_vectorlEeventsgR(void *p = 0);
   static void *newArray_vectorlEeventsgR(Long_t size, void *p);
   static void delete_vectorlEeventsgR(void *p);
   static void deleteArray_vectorlEeventsgR(void *p);
   static void destruct_vectorlEeventsgR(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const vector<events>*)
   {
      vector<events> *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(vector<events>));
      static ::ROOT::TGenericClassInfo 
         instance("vector<events>", -2, "vector", 214,
                  typeid(vector<events>), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &vectorlEeventsgR_Dictionary, isa_proxy, 4,
                  sizeof(vector<events>) );
      instance.SetNew(&new_vectorlEeventsgR);
      instance.SetNewArray(&newArray_vectorlEeventsgR);
      instance.SetDelete(&delete_vectorlEeventsgR);
      instance.SetDeleteArray(&deleteArray_vectorlEeventsgR);
      instance.SetDestructor(&destruct_vectorlEeventsgR);
      instance.AdoptCollectionProxyInfo(TCollectionProxyInfo::Generate(TCollectionProxyInfo::Pushback< vector<events> >()));

      ::ROOT::AddClassAlternate("vector<events>","std::vector<events, std::allocator<events> >");
      return &instance;
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const vector<events>*)0x0); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *vectorlEeventsgR_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const vector<events>*)0x0)->GetClass();
      vectorlEeventsgR_TClassManip(theClass);
   return theClass;
   }

   static void vectorlEeventsgR_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   // Wrappers around operator new
   static void *new_vectorlEeventsgR(void *p) {
      return  p ? ::new((::ROOT::Internal::TOperatorNewHelper*)p) vector<events> : new vector<events>;
   }
   static void *newArray_vectorlEeventsgR(Long_t nElements, void *p) {
      return p ? ::new((::ROOT::Internal::TOperatorNewHelper*)p) vector<events>[nElements] : new vector<events>[nElements];
   }
   // Wrapper around operator delete
   static void delete_vectorlEeventsgR(void *p) {
      delete ((vector<events>*)p);
   }
   static void deleteArray_vectorlEeventsgR(void *p) {
      delete [] ((vector<events>*)p);
   }
   static void destruct_vectorlEeventsgR(void *p) {
      typedef vector<events> current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class vector<events>

namespace {
  void TriggerDictionaryInitialization_LinkDict_Impl() {
    static const char* headers[] = {
"DataAnalysis.hh",
0
    };
    static const char* includePaths[] = {
"/opt/ROOT/root62006/include/",
"/wuhongyi1/zhangjizhi/p11B/ENN202406/5Draw_Li/",
0
    };
    static const char* fwdDeclCode = R"DICTFWDDCLS(
#line 1 "LinkDict dictionary forward declarations' payload"
#pragma clang diagnostic ignored "-Wkeyword-compat"
#pragma clang diagnostic ignored "-Wignored-attributes"
#pragma clang diagnostic ignored "-Wreturn-type-c-linkage"
extern int __Cling_Autoloading_Map;
struct __attribute__((annotate("$clingAutoload$DataAnalysis.hh")))  events;
namespace std{template <typename _Tp> class __attribute__((annotate("$clingAutoload$bits/allocator.h")))  __attribute__((annotate("$clingAutoload$string")))  allocator;
}
typedef struct events events __attribute__((annotate("$clingAutoload$DataAnalysis.hh"))) ;
)DICTFWDDCLS";
    static const char* payloadCode = R"DICTPAYLOAD(
#line 1 "LinkDict dictionary payload"


#define _BACKWARD_BACKWARD_WARNING_H
// Inline headers
#include "DataAnalysis.hh"

#undef  _BACKWARD_BACKWARD_WARNING_H
)DICTPAYLOAD";
    static const char* classesHeaders[] = {
"events", payloadCode, "@",
nullptr
};
    static bool isInitialized = false;
    if (!isInitialized) {
      TROOT::RegisterModule("LinkDict",
        headers, includePaths, payloadCode, fwdDeclCode,
        TriggerDictionaryInitialization_LinkDict_Impl, {}, classesHeaders, /*hasCxxModule*/false);
      isInitialized = true;
    }
  }
  static struct DictInit {
    DictInit() {
      TriggerDictionaryInitialization_LinkDict_Impl();
    }
  } __TheDictionaryInitializer;
}
void TriggerDictionaryInitialization_LinkDict() {
  TriggerDictionaryInitialization_LinkDict_Impl();
}
