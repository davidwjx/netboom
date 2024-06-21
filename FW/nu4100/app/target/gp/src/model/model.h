#ifndef MODEL_ENTRY
#define MODEL_ENTRY

typedef void (*MODELG_entryT) (void);

#define DEFINE_SET_MODEL_ENTRY(entry, model) \
   void __attribute__ ((noinline)) MODELG_setEntry_ ## model(void){\
      entry();\
   }


ERRG_codeE MODELG_initializeModel(void);
void MODELG_doModelEntry(void);
ERRG_codeE MODELG_readModel(INU_DEFSG_moduleTypeE *model);

#endif //MODEL_ENTRY
