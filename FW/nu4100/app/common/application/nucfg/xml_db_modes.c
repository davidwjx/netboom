/*
xml database module
*/
#include <inttypes.h>
#include "inu_common.h"
#include "xml_db_modes.h"
#include "xml_db.h"
#include "sxmlc.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void(*nodeCbT)(XMLNode *node, void *arg);

/*
   walk sub-tree recursively invoking the cb
   post-order walk (cb is called as the recursion returns from all children)
*/
static void XMLModes_walkNodes(XMLNode* node, nodeCbT cb, void *arg)
{
   int i;

   for (i = 0; i < node->n_children; i++)
   {
      XMLModes_walkNodes(node->children[i], cb, arg);
   }
   cb(node, arg);
   //printf("%s\n", node->tag);
}

/*
   walk the xml buffer invoking the cb
*/
static ERRG_codeE XMLModes_walkXml(XMLDoc *docp, char *buf, nodeCbT cb, void *arg)
{
   ERRG_codeE retval = XMLDB__RET_SUCCESS;
   int ret = 0;

   //parse xml buffer and set values in hash table

   ret = XMLDoc_parse_buffer_DOM(buf, NULL, docp);
   if (ret)
   {
      if (docp->i_root >= 0)
      {
         XMLModes_walkNodes(docp->nodes[docp->i_root], cb, arg);
      }
      else
      {
         retval = XMLDB__ERR_INVALID_ARGS;
      }
   }
   else
   {
      retval = XMLDB__ERR_UNEXPECTED;
   }

   return retval;
}

static int XMLModes_getAttributeValue(XMLNode* node, char* attName, const char** val)
{
    return XMLNode_get_attribute_with_default(node, attName, val, "-1");
}

typedef struct
{
   UINT32 blockAdd;
   UINT16 regOffset;
   UINT8 startBitOffset;
   UINT8 width;
} XMLModesDB_addressT;

typedef struct
{
   UINT32 val; //text converted from xml
   UINT32 resetVal;
   XMLModesDB_addressT regFieldParams;
   UINT16 flags;//dirty,empty
} XMLModesDB_valueT;

typedef struct _XMLModesDB_dbT
{
   XMLModesDB_valueT valueTbl[XMLDB_NUM_PATHS_E];
   char pathbuf[XMLDB_MAX_PATH_LEN];//work area for load and save functions
} XMLModesDB_dbT;

#define XMLDB_EMPTY_BIT  (0)
#define XMLDB_DIRTY_BIT  (1)
#define XMLDB_CLR_EMPTY(f)  (f &= ~(1<<XMLDB_EMPTY_BIT))
#define XMLDB_SET_DIRTY(f)  (f |= (1<<XMLDB_DIRTY_BIT))
#define XMLDB_SET_EMPTY(f)  (f |= (1<<XMLDB_EMPTY_BIT))

/*
   Convert text to u32.
   Return 0 on success, -1 on error: empty string, conversion error or larger than max u32.
*/
static int text2u32(const char *text, UINT32 *value)
{
   int ret = -1;
   char *endptr;
   uintmax_t intval;

   if ((!text) || (strlen(text) == 0))
      return -1; //no string or empty string

   //input string is not empty, so if conversion is OK endptr 
   //should point to its eos . If not then either part or all of the string is invalid.
   intval = strtoumax(text, &endptr, 0);
   if (*endptr == '\0')
   {
      //now verify does not exceed u32 maximum
      if (intval <= (UINT32)(~0))
      {
         *value = (UINT32)intval;
         ret = 0;
      }
   }
   return ret;
}


static void readXmlValFromDoc(XMLNode* node, void *arg)
{
   int ret;
   XMLModesDB_valueT *valp;
   UINT32 newval;
   XMLModesDB_dbT *dbp = (XMLModesDB_dbT *)arg;
   const char* userId = NULL;
   const char* value = NULL;
   INT32 userIdInt;

   //if there is no user_id - it is no field, do nothing.
   ret = XMLModes_getAttributeValue(node, "USER_ID",&userId);//It is assumed that each field has a user_ID
   userIdInt = atoi(userId);
   if ((ret == true) && (userIdInt!=-1) && (userIdInt < XMLDB_NUM_PATHS_EXT_E))
   {
      ret = XMLModes_getAttributeValue(node, "VALUE", &value);
      if (ret != true)
      {
          free((void *)userId);
          return;
      }
      if (strcmp(value, "-1") != 0)
      {
         valp = &dbp->valueTbl[XMLDB_extendedToRegularEnumConvert(userIdInt)];

         if (XMLDB_isConst(valp->flags))
         {
            LOGG_PRINT(LOG_WARN_E, NULL, "Ignore write value %s to userid %d, value is const %d\n",value,userIdInt,valp->val);
            free((void *)userId); free((void *)value);
            return;
         }
         
         ret = text2u32(value, &newval);

         if (ret >= 0)
         {
            valp->val = newval;
            XMLDB_CLR_EMPTY(valp->flags);
            XMLDB_SET_DIRTY(valp->flags);
         }
         else //empty string
         {
            //sometimes users put empty values in the XML for convenience.
            //it shouldn't overwrite the boot xml if the field was set there
         }
      }
      free((void*)userId); free((void*)value);
   }
}


static void readDpeFuncValFromDoc(XMLNode* node, void *arg)
{
   XMLModesG_dpeFuncT *dpeFuncP =(XMLModesG_dpeFuncT *)arg;
   const char* dpeValue = NULL;
   const char* readName = NULL;
   double dpeInt;
   int ret;

   if (strcmp((char*)node->tag, "AGGREGATION_SETUP") == 0)//since the cb invocation, the function tag appears at the end of function fields
   {
      dpeFuncP->numOfAggFunc++;//count how much this function appears
      if (dpeFuncP->numOfAggFunc < MAX_DPE_FUNCT_INST)
      {
         memcpy(&dpeFuncP->aggregationSetup[dpeFuncP->numOfAggFunc], &dpeFuncP->aggregationSetup[0], sizeof(XMLModesG_aggregationSetupParamsT));//start with default params.aggregationSetup[0] is the default
      }
   }

   if (strcmp((char*)node->tag, "OPTIMIZATION_SETUP") == 0)//since the cb invocation, the function tag appears at the end of function fields
   {
      dpeFuncP->numOfOptimizationFunc++;//count how much this function appears
      if (dpeFuncP->numOfOptimizationFunc < MAX_DPE_FUNCT_INST)
      {
         memcpy(&dpeFuncP->optimizationSetup[dpeFuncP->numOfOptimizationFunc], &dpeFuncP->optimizationSetup[0], sizeof(XMLModesG_optimizationSetupParamsT));//start with default params.optimizationSetup[0] is the default
      }
   }

   if (strcmp((char*)node->tag, "UNITE_SETUP") == 0)//since the cb invocation, the function tag appears at the end of function fields
   {
      dpeFuncP->numOfUniteFunc++;//count how much this function appears
      if (dpeFuncP->numOfUniteFunc < MAX_DPE_FUNCT_INST)
      {
         memcpy(&dpeFuncP->uniteSetup[dpeFuncP->numOfUniteFunc], &dpeFuncP->uniteSetup[0], sizeof(XMLModesG_uniteSetupParamsT));//start with default params.uniteSetup[0] is the default
      }
   }

   if (strcmp((char*)node->tag, "SMALL_FILTER_SETUP") == 0)//since the cb invocation, the function tag appears at the end of function fields
   {
      dpeFuncP->numOfSmallFilterFunc++;//count how much this function appears
      if (dpeFuncP->numOfSmallFilterFunc < MAX_DPE_FUNCT_INST)
      {
         memcpy(&dpeFuncP->smallFilterSetup[dpeFuncP->numOfSmallFilterFunc], &dpeFuncP->smallFilterSetup[0], sizeof(XMLModesG_smallFilterSetupParamsT));//start with default params.smallFilterFunc[0] is the default
      }
   }

   if (strcmp((char*)node->tag, "BLOB_SETUP") == 0)//since the cb invocation, the function tag appears at the end of function fields
   {
      dpeFuncP->numOfBlobFunc++;//count how much this function appears
      if (dpeFuncP->numOfBlobFunc < MAX_DPE_FUNCT_INST)
      {
         memcpy(&dpeFuncP->blobSetup[dpeFuncP->numOfBlobFunc], &dpeFuncP->blobSetup[0], sizeof(XMLModesG_blobSetupParamsT));//start with default params.blobFunc[0] is the default
      }
   }

   if (strcmp((char*)node->tag, "EDGE_DETECT_SETUP") == 0)//since the cb invocation, the function tag appears at the end of function fields
   {
      dpeFuncP->numOfEdgeDetectFunc++;//count how much this function appears
      if (dpeFuncP->numOfEdgeDetectFunc < MAX_DPE_FUNCT_INST)
      {
         memcpy(&dpeFuncP->edgeDetectSetup[dpeFuncP->numOfEdgeDetectFunc], &dpeFuncP->edgeDetectSetup[0], sizeof(XMLModesG_edgeDetectSetupParamsT));//start with default params.edgeDetectSetupFunc[0] is the default
      }
   }

   if (strcmp((char*)node->tag, "INFO_MAP_SETUP") == 0)//since the cb invocation, the function tag appears at the end of function fields
   {
      dpeFuncP->numOfInfoMapFunc++;//count how much this function appears
      if (dpeFuncP->numOfInfoMapFunc < MAX_DPE_FUNCT_INST)
      {
         memcpy(&dpeFuncP->infoMapSetup[dpeFuncP->numOfInfoMapFunc], &dpeFuncP->infoMapSetup[0], sizeof(XMLModesG_infoMapParamsT));//start with default params.infoMapSetup[0] is the default
      }
   }

   if (strcmp((char*)node->tag, "SUBPIXEL_SETUP") == 0)//since the cb invocation, the function tag appears at the end of function fields
   {
      dpeFuncP->numOfSubpixelFunc++;//count how much this function appears
      if (dpeFuncP->numOfSubpixelFunc < MAX_DPE_FUNCT_INST)
      {
         memcpy(&dpeFuncP->subpixelSetup[dpeFuncP->numOfSubpixelFunc], &dpeFuncP->subpixelSetup[0], sizeof(XMLModesG_subpixelSetupParamsT));//start with default params.subpixelSetupFunc[0] is the default
      }
   }

   if (strcmp((char*)node->tag, "PLANE_FIT_SETUP") == 0)//since the cb invocation, the function tag appears at the end of function fields
   {
      dpeFuncP->numOfPlaneFitFunc++;//count how much this function appears
      if (dpeFuncP->numOfPlaneFitFunc < MAX_DPE_FUNCT_INST)
      {
         memcpy(&dpeFuncP->planeFitSetup[dpeFuncP->numOfPlaneFitFunc], &dpeFuncP->planeFitSetup[0], sizeof(XMLModesG_planeFitSetupParamsT));//start with default params.planeFitSetupFunc[0] is the default
      }
   }

   if (strcmp((char*)node->tag, "CONFIDENCE_SETUP") == 0)//since the cb invocation, the function tag appears at the end of function fields
   {
      dpeFuncP->numOfConfidenceFunc++;//count how much this function appears
      if (dpeFuncP->numOfConfidenceFunc < MAX_DPE_FUNCT_INST)
      {
         memcpy(&dpeFuncP->confidenceSetup[dpeFuncP->numOfConfidenceFunc], &dpeFuncP->confidenceSetup[0], sizeof(XMLModesG_confidenceSetupParamsT));//start with default params.confidenceSetup[0] is the default
      }
   }

   if (strcmp((char*)node->tag, "CONFIDENCE_BINS_SETUP") == 0)//since the cb invocation, the function tag appears at the end of function fields
   {
      dpeFuncP->numOfConfidenceBinsFunc++;//count how much this function appears
      if (dpeFuncP->numOfConfidenceBinsFunc < MAX_DPE_FUNCT_INST)
      {
         memcpy(&dpeFuncP->confidenceBinsSetup[dpeFuncP->numOfConfidenceBinsFunc], &dpeFuncP->confidenceBinsSetup[0], sizeof(XMLModesG_confidenceBinsSetupParamsT));//start with default params.confidenceBinsSetup[0] is the default
      }
   }

   if (strcmp((char*)node->tag, "MULTI_RES_FULL_IMAGE_SETUP") == 0)//since the cb invocation, the function tag appears at the end of function fields
   {
      dpeFuncP->numOfMultiResFullImageFunc++;//count how much this function appears
      if (dpeFuncP->numOfMultiResFullImageFunc < MAX_DPE_FUNCT_HYBRID_INST)
      {
         memcpy(&dpeFuncP->multiResFullImageSetup[dpeFuncP->numOfMultiResFullImageFunc], &dpeFuncP->multiResFullImageSetup[0], sizeof(XMLModesG_multiResFullImageSetupParamsT));//start with default params.multiResFullImageSetup[0] is the default
      }
   }

   if (strcmp((char*)node->tag, "MIN_DISPARITY_SETUP") == 0)//since the cb invocation, the function tag appears at the end of function fields
   {
      dpeFuncP->numOfminDisparityFunc++;//count how much this function appears
      if (dpeFuncP->numOfminDisparityFunc < MAX_DPE_FUNCT_HYBRID_INST)
      {
         memcpy(&dpeFuncP->minDisparitySetup[dpeFuncP->numOfminDisparityFunc], &dpeFuncP->minDisparitySetup[0], sizeof(XMLModesG_minDisparitySetupParamsT));//start with default params.minDisparitySetup[0] is the default
      }
   }

   ret = XMLModes_getAttributeValue(node, "VALUE",&dpeValue);
   dpeInt = atof(dpeValue);
   free((void *)dpeValue);
   if (dpeInt!=-1)
   {
      if (strcmp((char*)node->father->tag, "HYBRID") == 0)
      {
         XMLModes_getAttributeValue(node, "NAME",&readName);
         if (strcmp(readName, "en") == 0)
         {
            dpeFuncP->hybridFuncParams.enable = (UINT32)dpeInt;
         }
         else if (strcmp(readName, "p1DownSampleFactor") == 0)
         {
            dpeFuncP->hybridFuncParams.p1DownSampleFactor = dpeInt;
         }
         else if (strcmp(readName, "rightEdge") == 0)
         {
            dpeFuncP->hybridFuncParams.rightEdge = (UINT32)dpeInt;
         }
         free((void*)readName);
      }

      if (strcmp((char*)node->father->tag, "MinDisp") == 0)
      {
         XMLModes_getAttributeValue(node, "NAME",&readName);
         if (strcmp(readName, "en") == 0)
         {
            dpeFuncP->minDispFuncParams.enable = (UINT32)dpeInt;
         }
         else if (strcmp(readName, "minDisp") == 0)
         {
            dpeFuncP->minDispFuncParams.minDisp = (UINT32)dpeInt;
         }
         free((void*)readName);
      }
      else if (strcmp((char*)node->father->tag, "D2D") == 0)
      {
         XMLModes_getAttributeValue(node, "NAME", &readName);
         if (strcmp(readName, "depth_scale_factor_host") == 0)
         {
            dpeFuncP->depthScaleFactor.depthScaleFactorHost = (UINT32)dpeInt;
         }
         else if (strcmp(readName, "depth_scale_factor_cva") == 0)
         {
            dpeFuncP->depthScaleFactor.depthScaleFactorCva = (UINT32)dpeInt;
         }
         free((void*)readName);
      }
      else if (strcmp((char*)node->father->tag, "AGGREGATION_SETUP") == 0)
      {
         XMLModes_getAttributeValue(node, "NAME", &readName);
         if (strcmp(readName, "enable") == 0)
         {
            dpeFuncP->aggregationSetup[dpeFuncP->numOfAggFunc].enable = (UINT32)dpeInt;
         }
         if (strcmp(readName, "SAD_census_ratio") == 0)
         {
            dpeFuncP->aggregationSetup[dpeFuncP->numOfAggFunc].SAD_census_ratio = (float)dpeInt;
         }
         else if (strcmp(readName, "agg_edge_mode") == 0)
         {
            dpeFuncP->aggregationSetup[dpeFuncP->numOfAggFunc].agg_edge_mode = (int)dpeInt;
         }
         else if (strcmp(readName, "agg_win_horizontal") == 0)
         {
            dpeFuncP->aggregationSetup[dpeFuncP->numOfAggFunc].agg_win_horizontal = (int)dpeInt;
         }
         else if (strcmp(readName, "agg_win_vertical") == 0)
         {
            dpeFuncP->aggregationSetup[dpeFuncP->numOfAggFunc].agg_win_vertical = (int)dpeInt;
         }
         else if (strcmp(readName, "cost_clip_high") == 0)
         {
            dpeFuncP->aggregationSetup[dpeFuncP->numOfAggFunc].cost_clip_high = (int)dpeInt;
         }
         else if (strcmp(readName, "cost_clip_low") == 0)
         {
            dpeFuncP->aggregationSetup[dpeFuncP->numOfAggFunc].cost_clip_low = (int)dpeInt;
         }
         else if (strcmp(readName, "census_factor") == 0)
         {
            dpeFuncP->aggregationSetup[dpeFuncP->numOfAggFunc].census_factor = (int)dpeInt;
         }
         else if (strcmp(readName, "p0_p1_mode") == 0)
         {
            dpeFuncP->aggregationSetup[dpeFuncP->numOfAggFunc].p0_p1_mode = (UINT32)dpeInt;
         }
         free((void*)readName);
      }
      else if (strcmp((char*)node->father->tag, "OPTIMIZATION_SETUP") == 0)
      {
         XMLModes_getAttributeValue(node, "NAME", &readName);
         if (strcmp(readName, "enable") == 0)
         {
            dpeFuncP->optimizationSetup[dpeFuncP->numOfOptimizationFunc].enable = (UINT32)dpeInt;
         }
         if (strcmp(readName, "opt_en") == 0)
         {
            dpeFuncP->optimizationSetup[dpeFuncP->numOfOptimizationFunc].opt_en = (UINT32)dpeInt;
         }
         else if (strcmp(readName, "opt_edge_mask_en") == 0)
         {
            dpeFuncP->optimizationSetup[dpeFuncP->numOfOptimizationFunc].opt_edge_mask_en = (UINT32)dpeInt;
         }
         else if (strcmp(readName, "p1") == 0)
         {
            dpeFuncP->optimizationSetup[dpeFuncP->numOfOptimizationFunc].p1 = (UINT32)dpeInt;
         }
         else if (strcmp(readName, "p2") == 0)
         {
            dpeFuncP->optimizationSetup[dpeFuncP->numOfOptimizationFunc].p2 = (UINT32)dpeInt;
         }
         else if (strcmp(readName, "p3") == 0)
         {
            dpeFuncP->optimizationSetup[dpeFuncP->numOfOptimizationFunc].p3 = (UINT32)dpeInt;
         }
         else if (strcmp(readName, "p0_p1_mode") == 0)
         {
            dpeFuncP->optimizationSetup[dpeFuncP->numOfOptimizationFunc].p0_p1_mode = (UINT32)dpeInt;
         }
         free((void*)readName);
     }
      else if (strcmp((char*)node->father->tag, "UNITE_SETUP") == 0)
      {
         XMLModes_getAttributeValue(node, "NAME", &readName);
         if (strcmp(readName, "enable") == 0)
         {
            dpeFuncP->uniteSetup[dpeFuncP->numOfUniteFunc].enable = (UINT32)dpeInt;
         }
         if (strcmp(readName, "unite_threshold") == 0)
         {
            dpeFuncP->uniteSetup[dpeFuncP->numOfUniteFunc].unite_threshold = (UINT32)dpeInt;
         }
         else if (strcmp(readName, "unite_side_left") == 0)
         {
            dpeFuncP->uniteSetup[dpeFuncP->numOfUniteFunc].unite_side_left = (UINT32)dpeInt;
         }
         else if (strcmp(readName, "p0_p1_mode") == 0)
         {
            dpeFuncP->uniteSetup[dpeFuncP->numOfUniteFunc].p0_p1_mode = (UINT32)dpeInt;
         }
         free((void*)readName);
      }
      else if (strcmp((char*)node->father->tag, "SMALL_FILTER_SETUP") == 0)
      {
         XMLModes_getAttributeValue(node, "NAME", &readName);
         if (strcmp(readName, "enable") == 0)
         {
            dpeFuncP->smallFilterSetup[dpeFuncP->numOfSmallFilterFunc].enable = (UINT32)dpeInt;
         }
         else if (strcmp(readName, "small_filter_en") == 0)
         {
            dpeFuncP->smallFilterSetup[dpeFuncP->numOfSmallFilterFunc].small_filter_en = (UINT32)dpeInt;
         }
         else if (strcmp(readName, "unite_threshold") == 0)
         {
            dpeFuncP->smallFilterSetup[dpeFuncP->numOfSmallFilterFunc].unite_threshold = (UINT32)dpeInt;
         }
         else if (strcmp(readName, "disp_threshold") == 0)
         {
            dpeFuncP->smallFilterSetup[dpeFuncP->numOfSmallFilterFunc].disp_threshold = (UINT32)dpeInt;
         }
         else if (strcmp(readName, "min_edges_count") == 0)
         {
            dpeFuncP->smallFilterSetup[dpeFuncP->numOfSmallFilterFunc].min_edges_count = (UINT32)dpeInt;
         }
         else if (strcmp(readName, "max_edges_count") == 0)
         {
            dpeFuncP->smallFilterSetup[dpeFuncP->numOfSmallFilterFunc].max_edges_count = (UINT32)dpeInt;
         }
         else if (strcmp(readName, "max_win_size") == 0)
         {
            dpeFuncP->smallFilterSetup[dpeFuncP->numOfSmallFilterFunc].max_win_size = (UINT32)dpeInt;
         }
         else if (strcmp(readName, "dil_win_size") == 0)
         {
            dpeFuncP->smallFilterSetup[dpeFuncP->numOfSmallFilterFunc].dil_win_size = (UINT32)dpeInt;
         }
         else if (strcmp(readName, "cleaning_disp_threshold") == 0)
         {
            dpeFuncP->smallFilterSetup[dpeFuncP->numOfSmallFilterFunc].cleaning_disp_threshold = (UINT32)dpeInt;
         }
         else if (strcmp(readName, "clean_flt_size") == 0)
         {
            dpeFuncP->smallFilterSetup[dpeFuncP->numOfSmallFilterFunc].clean_flt_size = (UINT32)dpeInt;
         }
         else if (strcmp(readName, "clean_flt_en") == 0)
         {
            dpeFuncP->smallFilterSetup[dpeFuncP->numOfSmallFilterFunc].clean_flt_en = (UINT32)dpeInt;
         }
         else if (strcmp(readName, "p0_p1_mode") == 0)
         {
            dpeFuncP->smallFilterSetup[dpeFuncP->numOfSmallFilterFunc].p0_p1_mode = (UINT32)dpeInt;
         }
         free((void*)readName);
      }
      else if (strcmp((char*)node->father->tag, "BLOB_SETUP") == 0)
      {
         XMLModes_getAttributeValue(node, "NAME", &readName);
         if (strcmp(readName, "enable") == 0)
         {
            dpeFuncP->blobSetup[dpeFuncP->numOfBlobFunc].enable = (UINT32)dpeInt;
         }
         else if (strcmp(readName, "blob_en") == 0)
         {
            dpeFuncP->blobSetup[dpeFuncP->numOfBlobFunc].blob_en = (UINT32)dpeInt;
         }
         else if (strcmp(readName, "blob_max_size") == 0)
         {
            dpeFuncP->blobSetup[dpeFuncP->numOfBlobFunc].blob_max_size = (UINT32)dpeInt;
         }
         else if (strcmp(readName, "blob_disp_diff") == 0)
         {
            dpeFuncP->blobSetup[dpeFuncP->numOfBlobFunc].blob_disp_diff = (UINT32)dpeInt;
         }
         else if (strcmp(readName, "blob_buffer_height") == 0)
         {
            dpeFuncP->blobSetup[dpeFuncP->numOfBlobFunc].blob_buffer_height = (UINT32)dpeInt;
         }
         else if (strcmp(readName, "p0_p1_mode") == 0)
         {
            dpeFuncP->blobSetup[dpeFuncP->numOfBlobFunc].p0_p1_mode = (UINT32)dpeInt;
         }
         free((void*)readName);
     }
      else if (strcmp((char*)node->father->tag, "EDGE_DETECT_SETUP") == 0)
      {
         XMLModes_getAttributeValue(node, "NAME", &readName);
         if (strcmp(readName, "enable") == 0)
         {
            dpeFuncP->edgeDetectSetup[dpeFuncP->numOfEdgeDetectFunc].enable = (UINT32)dpeInt;
         }
         else if (strcmp(readName, "gauss_filt_sigma") == 0)
         {
            dpeFuncP->edgeDetectSetup[dpeFuncP->numOfEdgeDetectFunc].gauss_filt_sigma = (float)dpeInt;
         }
         else if (strcmp(readName, "filt_before_census_en") == 0)
         {
            dpeFuncP->edgeDetectSetup[dpeFuncP->numOfEdgeDetectFunc].filt_before_census_en = (UINT32)dpeInt;
         }
         else if (strcmp(readName, "canny_thr_high") == 0)
         {
            dpeFuncP->edgeDetectSetup[dpeFuncP->numOfEdgeDetectFunc].canny_thr_high = (UINT32)dpeInt;
         }
         else if (strcmp(readName, "canny_thr_low") == 0)
         {
            dpeFuncP->edgeDetectSetup[dpeFuncP->numOfEdgeDetectFunc].canny_thr_low = (UINT32)dpeInt;
         }
         else if (strcmp(readName, "p0_p1_mode") == 0)
         {
            dpeFuncP->edgeDetectSetup[dpeFuncP->numOfEdgeDetectFunc].p0_p1_mode = (UINT32)dpeInt;
         }
         free((void*)readName);
      }
      else if (strcmp((char*)node->father->tag, "INFO_MAP_SETUP") == 0)
      {
         XMLModes_getAttributeValue(node, "NAME", &readName);
         if (strcmp(readName, "enable") == 0)
         {
            dpeFuncP->infoMapSetup[dpeFuncP->numOfInfoMapFunc].enable = (UINT32)dpeInt;
         }
         else if (strcmp(readName, "infomap_inv_en") == 0)
         {
            dpeFuncP->infoMapSetup[dpeFuncP->numOfInfoMapFunc].infomap_inv_en = (UINT32)dpeInt;
         }
         else if (strcmp(readName, "im_t") == 0)
         {
            dpeFuncP->infoMapSetup[dpeFuncP->numOfInfoMapFunc].im_t = (float)dpeInt;
         }
         else if (strcmp(readName, "box_size") == 0)
         {
            dpeFuncP->infoMapSetup[dpeFuncP->numOfInfoMapFunc].box_size = (UINT32)dpeInt;
         }
         else if (strcmp(readName, "length") == 0)
         {
            dpeFuncP->infoMapSetup[dpeFuncP->numOfInfoMapFunc].length = (UINT32)dpeInt;
         }
         else if (strcmp(readName, "width") == 0)
         {
            dpeFuncP->infoMapSetup[dpeFuncP->numOfInfoMapFunc].width = (UINT32)dpeInt;
         }
         else if (strcmp(readName, "low_high_var_sel") == 0)
         {
            dpeFuncP->infoMapSetup[dpeFuncP->numOfInfoMapFunc].low_high_var_sel = (UINT32)dpeInt;
         }
         else if (strcmp(readName, "var_min_offset") == 0)
         {
            dpeFuncP->infoMapSetup[dpeFuncP->numOfInfoMapFunc].var_min_offset = (float)dpeInt;
         }
         else if (strcmp(readName, "var_min_ratio") == 0)
         {
            dpeFuncP->infoMapSetup[dpeFuncP->numOfInfoMapFunc].var_min_ratio = (float)dpeInt;
         }
         else if (strcmp(readName, "var_max_offset") == 0)
         {
            dpeFuncP->infoMapSetup[dpeFuncP->numOfInfoMapFunc].var_max_offset = (float)dpeInt;
         }
         else if (strcmp(readName, "var_max_ratio") == 0)
         {
            dpeFuncP->infoMapSetup[dpeFuncP->numOfInfoMapFunc].var_max_ratio = (float)dpeInt;
         }
         else if (strcmp(readName, "eps_v") == 0)
         {
            dpeFuncP->infoMapSetup[dpeFuncP->numOfInfoMapFunc].eps_v = (float)dpeInt;
         }
         else if (strcmp(readName, "eps_o") == 0)
         {
            dpeFuncP->infoMapSetup[dpeFuncP->numOfInfoMapFunc].eps_o = (float)dpeInt;
         }
         else if (strcmp(readName, "eps_a") == 0)
         {
            dpeFuncP->infoMapSetup[dpeFuncP->numOfInfoMapFunc].eps_a = (float)dpeInt;
         }
         else if (strcmp(readName, "close_en") == 0)
         {
            dpeFuncP->infoMapSetup[dpeFuncP->numOfInfoMapFunc].close_en = (UINT32)dpeInt;
         }
         else if (strcmp(readName, "blob_enable") == 0)
         {
            dpeFuncP->infoMapSetup[dpeFuncP->numOfInfoMapFunc].blob_enable = (UINT32)dpeInt;
         }
         else if (strcmp(readName, "max_list_size") == 0)
         {
            dpeFuncP->infoMapSetup[dpeFuncP->numOfInfoMapFunc].max_list_size = (UINT32)dpeInt;
         }
         else if (strcmp(readName, "lin_dist") == 0)
         {
            dpeFuncP->infoMapSetup[dpeFuncP->numOfInfoMapFunc].lin_dist = (UINT32)dpeInt;
         }
         else if (strcmp(readName, "max_size") == 0)
         {
            dpeFuncP->infoMapSetup[dpeFuncP->numOfInfoMapFunc].max_size = (UINT32)dpeInt;
         }
         else if (strcmp(readName, "p0_p1_mode") == 0)
         {
            dpeFuncP->infoMapSetup[dpeFuncP->numOfInfoMapFunc].p0_p1_mode = (UINT32)dpeInt;
         }
         free((void*)readName);
      }
      else if (strcmp((char*)node->father->tag, "SUBPIXEL_SETUP") == 0)
      {
         XMLModes_getAttributeValue(node, "NAME", &readName);
         if (strcmp(readName, "enable") == 0)
         {
            dpeFuncP->subpixelSetup[dpeFuncP->numOfSubpixelFunc].enable = (UINT32)dpeInt;
         }
         else if (strcmp(readName, "pxl_cor") == 0)
         {
            dpeFuncP->subpixelSetup[dpeFuncP->numOfSubpixelFunc].pxl_cor = (UINT32)dpeInt;
         }
         else if (strcmp(readName, "census_msk_disp_mode") == 0)
         {
            dpeFuncP->subpixelSetup[dpeFuncP->numOfSubpixelFunc].census_msk_disp_mode = (UINT32)dpeInt;
         }
         else if (strcmp(readName, "census_cfg_msk_disp_thr") == 0)
         {
            dpeFuncP->subpixelSetup[dpeFuncP->numOfSubpixelFunc].census_cfg_msk_disp_thr = (UINT32)dpeInt;
         }
         else if (strcmp(readName, "hmsk") == 0)
         {
            dpeFuncP->subpixelSetup[dpeFuncP->numOfSubpixelFunc].hmsk = (UINT32)dpeInt;
         }
         else if (strcmp(readName, "vmsk") == 0)
         {
            dpeFuncP->subpixelSetup[dpeFuncP->numOfSubpixelFunc].vmsk = (UINT32)dpeInt;
         }
         else if (strcmp(readName, "aggr_msk_disp_mode") == 0)
         {
            dpeFuncP->subpixelSetup[dpeFuncP->numOfSubpixelFunc].aggr_msk_disp_mode = (UINT32)dpeInt;
         }
         else if (strcmp(readName, "aggr_cfg_msk_disp_thr") == 0)
         {
            dpeFuncP->subpixelSetup[dpeFuncP->numOfSubpixelFunc].aggr_cfg_msk_disp_thr = (UINT32)dpeInt;
         }
         else if (strcmp(readName, "census_flt_en") == 0)
         {
            dpeFuncP->subpixelSetup[dpeFuncP->numOfSubpixelFunc].census_flt_en = (UINT32)dpeInt;
         }
         else if (strcmp(readName, "weight1") == 0)
         {
            dpeFuncP->subpixelSetup[dpeFuncP->numOfSubpixelFunc].weight1 = (UINT32)dpeInt;
         }
         else if (strcmp(readName, "weight2") == 0)
         {
            dpeFuncP->subpixelSetup[dpeFuncP->numOfSubpixelFunc].weight2 = (UINT32)dpeInt;
         }
         else if (strcmp(readName, "weight3") == 0)
         {
            dpeFuncP->subpixelSetup[dpeFuncP->numOfSubpixelFunc].weight3 = (UINT32)dpeInt;
         }
         else if (strcmp(readName, "weight4") == 0)
         {
            dpeFuncP->subpixelSetup[dpeFuncP->numOfSubpixelFunc].weight4 = (UINT32)dpeInt;
         }
         else if (strcmp(readName, "p0_p1_mode") == 0)
         {
            dpeFuncP->subpixelSetup[dpeFuncP->numOfSubpixelFunc].p0_p1_mode = (UINT32)dpeInt;
         }
         free((void*)readName);
      }
      else if (strcmp((char*)node->father->tag, "PLANE_FIT_SETUP") == 0)
      {
         XMLModes_getAttributeValue(node, "NAME", &readName);
         if (strcmp(readName, "enable") == 0)
         {
            dpeFuncP->planeFitSetup[dpeFuncP->numOfPlaneFitFunc].enable = (UINT32)dpeInt;
         }
         else if (strcmp(readName, "plane_fit_en") == 0)
         {
            dpeFuncP->planeFitSetup[dpeFuncP->numOfPlaneFitFunc].plane_fit_en = (UINT32)dpeInt;
         }
         else if (strcmp(readName, "pixel_smooth_mode") == 0)
         {
            dpeFuncP->planeFitSetup[dpeFuncP->numOfPlaneFitFunc].pixel_smooth_mode = (UINT32)dpeInt;
         }
         else if (strcmp(readName, "grad_disp_thr") == 0)
         {
            dpeFuncP->planeFitSetup[dpeFuncP->numOfPlaneFitFunc].grad_disp_thr = (UINT32)dpeInt;
         }
         else if (strcmp(readName, "grad_min_pixels") == 0)
         {
            dpeFuncP->planeFitSetup[dpeFuncP->numOfPlaneFitFunc].grad_min_pixels = (UINT32)dpeInt;
         }
         else if (strcmp(readName, "smooth_disp_thr") == 0)
         {
            dpeFuncP->planeFitSetup[dpeFuncP->numOfPlaneFitFunc].smooth_disp_thr = (UINT32)dpeInt;
         }
         else if (strcmp(readName, "smooth_min_pixels") == 0)
         {
            dpeFuncP->planeFitSetup[dpeFuncP->numOfPlaneFitFunc].smooth_min_pixels = (UINT32)dpeInt;
         }
         else if (strcmp(readName, "p0_p1_mode") == 0)
         {
            dpeFuncP->planeFitSetup[dpeFuncP->numOfPlaneFitFunc].p0_p1_mode = (UINT32)dpeInt;
         }
         free((void*)readName);
     }
      else if (strcmp((char*)node->father->tag, "CONFIDENCE_SETUP") == 0)
      {
         XMLModes_getAttributeValue(node, "NAME", &readName);
         if (strcmp(readName, "enable") == 0)
         {
            dpeFuncP->confidenceSetup[dpeFuncP->numOfConfidenceFunc].enable = (UINT32)dpeInt;
         }
         else if (strcmp(readName, "ck") == 0)
         {
            dpeFuncP->confidenceSetup[dpeFuncP->numOfConfidenceFunc].ck = (UINT32)dpeInt;
         }
         else if (strcmp(readName, "ca") == 0)
         {
            dpeFuncP->confidenceSetup[dpeFuncP->numOfConfidenceFunc].ca = (UINT32)dpeInt;
         }
         else if (strcmp(readName, "cb") == 0)
         {
            dpeFuncP->confidenceSetup[dpeFuncP->numOfConfidenceFunc].cb = (UINT32)dpeInt;
         }
         else if (strcmp(readName, "rk") == 0)
         {
            dpeFuncP->confidenceSetup[dpeFuncP->numOfConfidenceFunc].rk = (UINT32)dpeInt;
         }
         else if (strcmp(readName, "ra") == 0)
         {
            dpeFuncP->confidenceSetup[dpeFuncP->numOfConfidenceFunc].ra = (UINT32)dpeInt;
         }
         else if (strcmp(readName, "rb") == 0)
         {
            dpeFuncP->confidenceSetup[dpeFuncP->numOfConfidenceFunc].rb = (UINT32)dpeInt;
         }
         else if (strcmp(readName, "wc0") == 0)
         {
            dpeFuncP->confidenceSetup[dpeFuncP->numOfConfidenceFunc].wc0 = (UINT32)dpeInt;
         }
         else if (strcmp(readName, "wr0") == 0)
         {
            dpeFuncP->confidenceSetup[dpeFuncP->numOfConfidenceFunc].wr0 = (UINT32)dpeInt;
         }
         else if (strcmp(readName, "wn") == 0)
         {
            dpeFuncP->confidenceSetup[dpeFuncP->numOfConfidenceFunc].wn = (UINT32)dpeInt;
         }
         else if (strcmp(readName, "w_c02") == 0)
         {
            dpeFuncP->confidenceSetup[dpeFuncP->numOfConfidenceFunc].w_c02 = (INT32)dpeInt;
         }
         else if (strcmp(readName, "w_c1") == 0)
         {
            dpeFuncP->confidenceSetup[dpeFuncP->numOfConfidenceFunc].w_c1 = (INT32)dpeInt;
         }
         else if (strcmp(readName, "w_unite") == 0)
         {
            dpeFuncP->confidenceSetup[dpeFuncP->numOfConfidenceFunc].w_unite = (INT32)dpeInt;
         }
         else if (strcmp(readName, "p0_p1_mode") == 0)
         {
            dpeFuncP->confidenceSetup[dpeFuncP->numOfConfidenceFunc].p0_p1_mode = (UINT32)dpeInt;
         }
         free((void*)readName);
      }
      else if (strcmp((char*)node->father->tag, "CONFIDENCE_BINS_SETUP") == 0)
      {
         XMLModes_getAttributeValue(node, "NAME", &readName);
         if (strcmp(readName, "enable") == 0)
         {
            dpeFuncP->confidenceBinsSetup[dpeFuncP->numOfConfidenceBinsFunc].enable = (UINT32)dpeInt;
         }
         else if (strcmp(readName, "bin_1") == 0)
         {
            dpeFuncP->confidenceBinsSetup[dpeFuncP->numOfConfidenceBinsFunc].bin_1 = (INT32)dpeInt;
         }
         else if (strcmp(readName, "bin_2") == 0)
         {
            dpeFuncP->confidenceBinsSetup[dpeFuncP->numOfConfidenceBinsFunc].bin_2 = (INT32)dpeInt;
         }
         else if (strcmp(readName, "bin_3") == 0)
         {
            dpeFuncP->confidenceBinsSetup[dpeFuncP->numOfConfidenceBinsFunc].bin_3 = (INT32)dpeInt;
         }
         else if (strcmp(readName, "bin_4") == 0)
         {
            dpeFuncP->confidenceBinsSetup[dpeFuncP->numOfConfidenceBinsFunc].bin_4 = (INT32)dpeInt;
         }
         else if (strcmp(readName, "bin_5") == 0)
         {
            dpeFuncP->confidenceBinsSetup[dpeFuncP->numOfConfidenceBinsFunc].bin_5 = (INT32)dpeInt;
         }
         else if (strcmp(readName, "bin_6") == 0)
         {
            dpeFuncP->confidenceBinsSetup[dpeFuncP->numOfConfidenceBinsFunc].bin_6 = (INT32)dpeInt;
         }
         else if (strcmp(readName, "bin_7") == 0)
         {
            dpeFuncP->confidenceBinsSetup[dpeFuncP->numOfConfidenceBinsFunc].bin_7 = (INT32)dpeInt;
         }
         else if (strcmp(readName, "bin_8") == 0)
         {
            dpeFuncP->confidenceBinsSetup[dpeFuncP->numOfConfidenceBinsFunc].bin_8 = (INT32)dpeInt;
         }
         else if (strcmp(readName, "bin_9") == 0)
         {
            dpeFuncP->confidenceBinsSetup[dpeFuncP->numOfConfidenceBinsFunc].bin_9 = (INT32)dpeInt;
         }
         else if (strcmp(readName, "bin_10") == 0)
         {
            dpeFuncP->confidenceBinsSetup[dpeFuncP->numOfConfidenceBinsFunc].bin_10 = (INT32)dpeInt;
         }
         else if (strcmp(readName, "bin_11") == 0)
         {
            dpeFuncP->confidenceBinsSetup[dpeFuncP->numOfConfidenceBinsFunc].bin_11 = (INT32)dpeInt;
         }
         else if (strcmp(readName, "bin_12") == 0)
         {
            dpeFuncP->confidenceBinsSetup[dpeFuncP->numOfConfidenceBinsFunc].bin_12 = (INT32)dpeInt;
         }
         else if (strcmp(readName, "bin_13") == 0)
         {
            dpeFuncP->confidenceBinsSetup[dpeFuncP->numOfConfidenceBinsFunc].bin_13 = (INT32)dpeInt;
         }
         else if (strcmp(readName, "bin_14") == 0)
         {
            dpeFuncP->confidenceBinsSetup[dpeFuncP->numOfConfidenceBinsFunc].bin_14 = (INT32)dpeInt;
         }
         else if (strcmp(readName, "bin_15") == 0)
         {
            dpeFuncP->confidenceBinsSetup[dpeFuncP->numOfConfidenceBinsFunc].bin_15 = (INT32)dpeInt;
         }
         else if (strcmp(readName, "spx_conf_thr_en") == 0)
         {
            dpeFuncP->confidenceBinsSetup[dpeFuncP->numOfConfidenceBinsFunc].spx_conf_thr_en = (UINT32)dpeInt;
         }
         else if (strcmp(readName, "spx_conf_thr") == 0)
         {
            dpeFuncP->confidenceBinsSetup[dpeFuncP->numOfConfidenceBinsFunc].spx_conf_thr = (UINT32)dpeInt;
         }
         else if (strcmp(readName, "p0_p1_mode") == 0)
         {
            dpeFuncP->confidenceBinsSetup[dpeFuncP->numOfConfidenceBinsFunc].p0_p1_mode = (UINT32)dpeInt;
         }
         free((void*)readName);
      }
      else if (strcmp((char*)node->father->tag, "MULTI_RES_FULL_IMAGE_SETUP") == 0)
      {
         XMLModes_getAttributeValue(node, "NAME", &readName);
         if (strcmp(readName, "enable") == 0)
         {
            dpeFuncP->multiResFullImageSetup[dpeFuncP->numOfMultiResFullImageFunc].enable = (UINT32)dpeInt;
         }
         else if (strcmp(readName, "p0_down_sample_factor") == 0)
         {
            dpeFuncP->multiResFullImageSetup[dpeFuncP->numOfMultiResFullImageFunc].p0_down_sample_factor = (float)dpeInt;
         }
         else if (strcmp(readName, "p1_down_sample_factor") == 0)
         {
            dpeFuncP->multiResFullImageSetup[dpeFuncP->numOfMultiResFullImageFunc].p1_down_sample_factor = (float)dpeInt;
         }
         else if (strcmp(readName, "hybrid_full_mode") == 0)
         {
            dpeFuncP->multiResFullImageSetup[dpeFuncP->numOfMultiResFullImageFunc].hybrid_full_mode = (UINT32)dpeInt;
         }
         else if (strcmp(readName, "min_disp") == 0)
         {
            dpeFuncP->multiResFullImageSetup[dpeFuncP->numOfMultiResFullImageFunc].min_disp = (UINT32)dpeInt;
         }
         else if (strcmp(readName, "ext_rng_thr_far_nv") == 0)
         {
             dpeFuncP->multiResFullImageSetup[dpeFuncP->numOfMultiResFullImageFunc].ext_rng_thr_far_nv = (UINT32)dpeInt;
         }
         else if (strcmp(readName, "ext_rng_thr_far_close") == 0)
         {
             dpeFuncP->multiResFullImageSetup[dpeFuncP->numOfMultiResFullImageFunc].ext_rng_thr_far_close = (UINT32)dpeInt;
         }
         free((void*)readName);
     }
      else if (strcmp((char*)node->father->tag, "MIN_DISPARITY_SETUP") == 0)
      {
         XMLModes_getAttributeValue(node, "NAME", &readName);
         if (strcmp(readName, "enable") == 0)
         {
            dpeFuncP->minDisparitySetup[dpeFuncP->numOfminDisparityFunc].enable = (UINT32)dpeInt;
         }
         else if (strcmp(readName, "min_disp") == 0)
         {
            dpeFuncP->minDisparitySetup[dpeFuncP->numOfminDisparityFunc].min_disp = (UINT32)dpeInt;
         }
         free((void*)readName);
      }
   }
}

/*
   Load values from xml buffer into db.
*/
ERRG_codeE XMLModesG_getFuncData(XMLModesG_dpeFuncT *dpeFunc, const char *buf)
{
   XMLDoc doc;
   ERRG_codeE retval;

   XMLDoc_init(&doc);
   retval = XMLModes_walkXml(&doc, (char *)buf, readDpeFuncValFromDoc, (void*)dpeFunc);
   XMLDoc_free(&doc);

   return retval;
}

ERRG_codeE XMLModesG_loadFromBuf(XMLDBModes_dbH db, const char *buf)
{
   XMLDoc doc;
   ERRG_codeE retval;

   XMLDoc_init(&doc);
   retval = XMLModes_walkXml(&doc, (char *)buf, readXmlValFromDoc, db);
   XMLDoc_free(&doc);

   return retval;
}

void * XMLModesG_getParsedXML(const char *socxml_path)
{
   // create xml'
   char *xmlbuf = NULL;
   unsigned int xmlsize;
   if (XMLDB_loadNuSocxml(&xmlbuf, &xmlsize, socxml_path) < 0)
   {
      //LOGG_PRINT(LOG_INFO_E, NULL, "Failed to load xml from path: %s:\n", xmlPath);
      printf("Failed to load xml from path: %s:\n", socxml_path);
      return NULL;
   }
   return xmlbuf;
}

#ifdef __cplusplus
}
#endif
