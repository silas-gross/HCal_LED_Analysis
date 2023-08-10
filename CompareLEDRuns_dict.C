// Do NOT change. Changes will be lost next time file is generated

#define R__DICTIONARY_FILENAME CompareLEDRuns_dict
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

// Header files passed as explicit arguments
#include "CompareLEDRuns.h"

// Header files passed via #pragma extra_include

// The generated code does not explicitly qualify STL entities
namespace std {} using namespace std;

namespace ROOT {
   static TClass *CompareLEDRuns_Dictionary();
   static void CompareLEDRuns_TClassManip(TClass*);
   static void *new_CompareLEDRuns(void *p = nullptr);
   static void *newArray_CompareLEDRuns(Long_t size, void *p);
   static void delete_CompareLEDRuns(void *p);
   static void deleteArray_CompareLEDRuns(void *p);
   static void destruct_CompareLEDRuns(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::CompareLEDRuns*)
   {
      ::CompareLEDRuns *ptr = nullptr;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(::CompareLEDRuns));
      static ::ROOT::TGenericClassInfo 
         instance("CompareLEDRuns", "CompareLEDRuns.h", 17,
                  typeid(::CompareLEDRuns), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &CompareLEDRuns_Dictionary, isa_proxy, 0,
                  sizeof(::CompareLEDRuns) );
      instance.SetNew(&new_CompareLEDRuns);
      instance.SetNewArray(&newArray_CompareLEDRuns);
      instance.SetDelete(&delete_CompareLEDRuns);
      instance.SetDeleteArray(&deleteArray_CompareLEDRuns);
      instance.SetDestructor(&destruct_CompareLEDRuns);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::CompareLEDRuns*)
   {
      return GenerateInitInstanceLocal((::CompareLEDRuns*)nullptr);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const ::CompareLEDRuns*)nullptr); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *CompareLEDRuns_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const ::CompareLEDRuns*)nullptr)->GetClass();
      CompareLEDRuns_TClassManip(theClass);
   return theClass;
   }

   static void CompareLEDRuns_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   // Wrappers around operator new
   static void *new_CompareLEDRuns(void *p) {
      return  p ? new(p) ::CompareLEDRuns : new ::CompareLEDRuns;
   }
   static void *newArray_CompareLEDRuns(Long_t nElements, void *p) {
      return p ? new(p) ::CompareLEDRuns[nElements] : new ::CompareLEDRuns[nElements];
   }
   // Wrapper around operator delete
   static void delete_CompareLEDRuns(void *p) {
      delete ((::CompareLEDRuns*)p);
   }
   static void deleteArray_CompareLEDRuns(void *p) {
      delete [] ((::CompareLEDRuns*)p);
   }
   static void destruct_CompareLEDRuns(void *p) {
      typedef ::CompareLEDRuns current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::CompareLEDRuns

namespace {
  void TriggerDictionaryInitialization_CompareLEDRuns_dict_Impl() {
    static const char* headers[] = {
"CompareLEDRuns.h",
nullptr
    };
    static const char* includePaths[] = {
"/cvmfs/sphenix.sdcc.bnl.gov/gcc-12.1.0/release/release_new/new.3/include",
"/cvmfs/sphenix.sdcc.bnl.gov/gcc-12.1.0/release/release_new/new.3/include",
"/cvmfs/sphenix.sdcc.bnl.gov/gcc-12.1.0/opt/sphenix/core/root-6.26.06.p01/include/",
"/gpfs/mnt/gpfs02/sphenix/user/sgross/HCal_LED_Analysis/",
nullptr
    };
    static const char* fwdDeclCode = R"DICTFWDDCLS(
#line 1 "CompareLEDRuns_dict dictionary forward declarations' payload"
#pragma clang diagnostic ignored "-Wkeyword-compat"
#pragma clang diagnostic ignored "-Wignored-attributes"
#pragma clang diagnostic ignored "-Wreturn-type-c-linkage"
extern int __Cling_AutoLoading_Map;
class __attribute__((annotate("$clingAutoload$CompareLEDRuns.h")))  CompareLEDRuns;
)DICTFWDDCLS";
    static const char* payloadCode = R"DICTPAYLOAD(
#line 1 "CompareLEDRuns_dict dictionary payload"


#define _BACKWARD_BACKWARD_WARNING_H
// Inline headers
#include "CompareLEDRuns.h"

#undef  _BACKWARD_BACKWARD_WARNING_H
)DICTPAYLOAD";
    static const char* classesHeaders[] = {
"CompareLEDRuns", payloadCode, "@",
nullptr
};
    static bool isInitialized = false;
    if (!isInitialized) {
      TROOT::RegisterModule("CompareLEDRuns_dict",
        headers, includePaths, payloadCode, fwdDeclCode,
        TriggerDictionaryInitialization_CompareLEDRuns_dict_Impl, {}, classesHeaders, /*hasCxxModule*/false);
      isInitialized = true;
    }
  }
  static struct DictInit {
    DictInit() {
      TriggerDictionaryInitialization_CompareLEDRuns_dict_Impl();
    }
  } __TheDictionaryInitializer;
}
void TriggerDictionaryInitialization_CompareLEDRuns_dict() {
  TriggerDictionaryInitialization_CompareLEDRuns_dict_Impl();
}
