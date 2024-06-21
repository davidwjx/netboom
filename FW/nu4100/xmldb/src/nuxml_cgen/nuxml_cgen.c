/*
* nuxml_cgen - code generator tool xml parsing
*/

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include "sxmlc.h"

#define HEADER_FILE_NAME "xml_path.h"
#define SRC_FILE_NAME "xml_path.c"

#define XMLDB_PATH_SEPARATOR '.'
#define XMLDB_ENUM_PATH_SEPARTOR '_'

//#define XMLDB_ONLY_LEAVES

#define XMLDB_MAX_PATH_DEPTH (20)
#define XMLDB_MAX_TAG_LEN  (128)
#define XMLDB_MAX_TEXT_LEN  (128) 
#define XMLDB_MAX_PATH_LEN  (XMLDB_MAX_TAG_LEN*XMLDB_MAX_PATH_DEPTH)

#define ERRG_SUCCEEDED(err)  (err == 0)
#define ERRG_FAILED(err)     (err < 0)

typedef enum
{
   XMLDB__RET_SUCCESS             = 0,
   XMLDB__ERR_UNEXPECTED          = -1,
   XMLDB__ERR_INVALID_ARGS        = -2,
   XMLDB__ERR_OUT_OF_MEM          = -3,
   XMLDB__ERR_OUT_OF_RSRCS        = -4,
   XMLDB__ERR_NULL_PTR            = -5,
   XMLDB__ERR_TIMEOUT             = -6,
   XMLDB__ERR_IO_ERROR            = -7,
   XMLDB__ERR_NOT_SUPPORTED       = -8,
   XMLDB__ERR_ILLEGAL_STATE       = -9,
   XMLDB__EMPTY_ENTRY             = -10,
} ERRG_codeE;


typedef void(*nodeCbT)(XMLNode *node, void *arg);


const char *hdrfile_pre = "\
#ifndef __XML_PATH_H__\n\
#define __XML_PATH_H__\n\
\n\
#ifdef __cplusplus\n\
extern \"C\" {\n\
#endif\n\
\n\
";

const char *hdrfile_post = "\
\n\
#ifdef __cplusplus\n\
}\n\
#endif\n\
#endif//__XML_PATH_H__\
";

const char *srcfile_pre = "\
#include \"inu_common.h\"\n\
#include \"xml_path.h\"\n\
\n\
#ifdef __cplusplus\n\
extern \"C\" {\n\
#endif\n\n\
";

const char *srcfile_post = "\
#ifdef __cplusplus\n\
}\n\
#endif\n\
";


/*
   Build an array of nodes correpsonding to the path up to the root of the tree from start node.
   First node in path_nodes is start node. Depth is 0 for root, 1 for root child etc.
   Return depth of path (>= 0) or -1 if max_depth exceeded
*/
static int build_node_path(const XMLNode *start_node, const XMLNode *path_nodes[], unsigned int max_depth)
{
   XMLNode *current_father;
   unsigned int i = 0;

   path_nodes[i] = start_node;
   current_father = start_node->father;
   while (current_father)
   {
      i++;
      if (i > max_depth)
         break;
      path_nodes[i] = current_father;
      current_father = current_father->father;
   }
   if (i > max_depth)
      return -1;

   return i;
}

/*
   Build a path string from root to node: "root.node1.node2."
   if reverse is true you get: "node2.node1.root."
   Size is the sizeof(path) on input, and on output is the strlen the path (not counting eos).
   Return the depth of the path (0 for root, 1 for child, etc) or -1 on error: exceeded
   maximum depth or size limit on path.
*/
static int build_path_str(const XMLNode *node, int reverse, char *path, unsigned int *size)
{
   const XMLNode *path_nodes[XMLDB_MAX_PATH_DEPTH];
   size_t path_len = 0;
   unsigned int maxlen = *size-1;//1 space for eos
   int i,j;
   int depth = 0;
   int ret = 0;

   depth = build_node_path(node, path_nodes, XMLDB_MAX_PATH_DEPTH);
   if (depth >= 0)
   {
      ret = depth;
      for (i = 0; i <= depth; i++)
      {
         j = (reverse) ? i : (depth-i); //reverse the order 
         size_t tag_len = strlen(path_nodes[j]->tag);
         if (path_len + tag_len + 1 <= maxlen)
         {
            memcpy(&path[path_len], path_nodes[j]->tag, tag_len);
            path_len += strlen(path_nodes[j]->tag);
            path[path_len++] = XMLDB_PATH_SEPARATOR;
         }
         else
         {
            ret = -1; //exceeded path size
            break;
         }
      }
   }
   else
   {
      ret = -1; //exceeded depth
   }

   if (ret >= 0)
   {
       path[path_len] = '\0';//eos
       *size = (unsigned int)path_len;
   }
   //printf("path: %s\n", path);
   return ret;
}

#ifdef XMLDB_ONLY_LEAVES
static void countLeaves(XMLNode *node, void *arg)
{
   (void)node;
   unsigned int *countp = (unsigned int *)arg;
   if(node->n_children == 0)
      (*countp)++;
}
static void fillLeavesTbl(XMLNode *node, void *arg)
{
   static char pathbuf[XMLDB_MAX_PATH_LEN]; //static large buff
   static unsigned int nodecnt = 0;
   unsigned int pathLen = sizeof(pathbuf);
   char **pathTbl = (char **)arg;
   int ret;
   if(node->n_children == 0)
   {
      ret = build_path_str(node, 0, pathbuf, &pathLen);
      assert(ret >= 0);
      pathTbl[nodecnt] = (char *)malloc(strlen(pathbuf) + 1);
      strcpy(pathTbl[nodecnt], pathbuf);
      nodecnt++;//next time
   }
}
#else
static void countNodes(XMLNode *node, void *arg)
{
   (void)node;
   unsigned int *countp = (unsigned int *)arg;
   (*countp)++;
}

static void fillPathTbl(XMLNode *node, void *arg)
{
   static char pathbuf[XMLDB_MAX_PATH_LEN]; //static large buff
   static unsigned int nodecnt = 0;
   unsigned int pathLen = sizeof(pathbuf);
   char **pathTbl = (char **)arg;
   int ret;

   //build path
   ret = build_path_str(node, 0, pathbuf, &pathLen);
   assert(ret >= 0);
   //add to pathtable 
   pathTbl[nodecnt] = (char *)malloc(strlen(pathbuf) + 1);
   strcpy(pathTbl[nodecnt], pathbuf);
   nodecnt++;//next time
}
#endif

static void freePathTbl(char **pathTbl, unsigned int numPaths)
{
   unsigned int i;
   if (!pathTbl)
      return;

   for (i = 0; i < numPaths; i++)
   {
      if (pathTbl[i])
         free(pathTbl[i]);
   }
   free(pathTbl);
}

static int writeToFile(FILE *f, const char *str)
{
   size_t ret = fwrite(str, strlen(str), 1, f);
   return (ret < 1) ? -1 : 0;
}
static int writeHdrPre(FILE *hfile)
{
   fprintf(hfile, "/*\nxmldb_path.h - auto generated file - %s:%s\n*/\n", __DATE__,__TIME__);
   return writeToFile(hfile, hdrfile_pre);
}

static int writeSrcPre(FILE *hfile)
{
   fprintf(hfile, "/*\nxmldb_path.c - auto generated file - %s:%s\n*/\n", __DATE__,__TIME__);
   return writeToFile(hfile, srcfile_pre);
}

static int writePathInitFunction(FILE *f, char **pathTbl, unsigned int numPaths)
{
   unsigned int i;

   writeToFile(f, "void XMLDB_initPathTable(const char **pathTbl)\n{\n");
   for (i = 0; i < numPaths; i++)
   {
      //Change to upper case and replace dots with underscores
      fprintf(f, "\tpathTbl[%d]=\"%s\";\n", i, pathTbl[i]);
   }
   writeToFile(f, "};\n\n");
   return 0;
}

static int writePathInitProto(FILE *f, char **pathTbl, unsigned int numPaths)
{
	return writeToFile(f, "extern void XMLDB_initPathTable(const char **pathTbl);\n");
}

static int writePathEnum(FILE *f, char **pathTbl, unsigned int numPaths)
{
   unsigned int i,j;
   writeToFile(f, "typedef enum\n{\n");
   for (i = 0; i < numPaths; i++)
   {
      //Change to upper case and replace dots with underscores
      for (j = 0; j < strlen(pathTbl[i]); j++)
      {
         if (pathTbl[i][j] == XMLDB_PATH_SEPARATOR)
            pathTbl[i][j] = XMLDB_ENUM_PATH_SEPARTOR;
         else
            pathTbl[i][j] = (char)toupper(pathTbl[i][j]);
      }
      fprintf(f, "\t%sE= %d,\n", pathTbl[i], i);
   }
   fprintf(f, "\tXMLDB_NUM_PATHS_E= %d,\n", i);
   writeToFile(f, "}XMLDB_pathE;\n");
   return 0;

}

static int writeHeaderFile(char **pathTbl, unsigned int numPaths)
{
   FILE *hfile;
   int ret = 0;

   hfile = fopen(HEADER_FILE_NAME, "w");
   if (!hfile)
      ret = -1;
   
   if(ret >=0)
      ret = writeHdrPre(hfile);

   if (ret >= 0)
      ret = writePathEnum(hfile, pathTbl, numPaths);

   if (ret >= 0)
      ret = writePathInitProto(hfile, pathTbl, numPaths);

   if (ret >= 0)
      ret = writeToFile(hfile, hdrfile_post);

   if (ret >= 0)
   {
      fclose(hfile);
   }
   return ret;
}

static int writeSrcfile(char **pathTbl, unsigned int numPaths)
{
   FILE *hfile;
   int ret = 0;

   hfile = fopen(SRC_FILE_NAME, "wb");
   if (!hfile)
      ret = -1;

   if (ret >= 0)
      ret = writeSrcPre(hfile);

   if (ret >= 0)
      ret = writePathInitFunction(hfile, pathTbl, numPaths);

   if (ret >= 0)
      ret = writeToFile(hfile, srcfile_post);

   if (ret >= 0)
   {
      fclose(hfile);
   }
   return ret;

}

static int loadNuSocxml(const char *xmlname, char **xmlbuf, unsigned int *xmlsize)
{
   int ret = 0;
   char *buf = NULL;
   unsigned int size;
   
   //open and load xml file
   FILE *xmlfile = fopen(xmlname, "r");
   if (!xmlfile)
      return -1;

   fseek(xmlfile, 0L, SEEK_END);
   size = ftell(xmlfile);
   fseek(xmlfile, 0L, SEEK_SET);

   buf = (char *)malloc(size+1);
   if (buf)
   {
      size_t r = fread(buf, size, 1, xmlfile);
      if((r != 1) && (!feof(xmlfile)))
      {
         printf("read file error %s\n",strerror(errno));
         ret = -1;
         free(buf);
      }
      buf[size] = '\0';//otherwise sxmlc parser chokes
   }
   else
   {
      ret = -1;
   }
   
   if (ret >= 0)
   {
      *xmlbuf = buf;
      *xmlsize = size;
   }

   fclose(xmlfile);
   return ret;
}


/*
   walk sub-tree recursively invoking the cb 
   post-order walk (cb is called as the recursion returns from all children)
*/
static void walkNodes(XMLNode* node, nodeCbT cb, void *arg)
{
   int i;

   cb(node, arg);
   for (i = 0; i < node->n_children; i++)
   {
      walkNodes(node->children[i], cb, arg);
   }
   //cb(node, arg);
   //printf("%s\n", node->tag);
}


/*
   walk the xml buffer invoking the cb
*/
static ERRG_codeE walkXml(XMLDoc *docp, char *buf, nodeCbT cb, void *arg)
{
   ERRG_codeE retval = XMLDB__RET_SUCCESS;
   int ret = 0;

   //parse xml buffer and set values in hash table

   ret = XMLDoc_parse_buffer_DOM(buf, NULL, docp);
   if (ret)
   {
      if (docp->i_root >= 0)
      {
         walkNodes(docp->nodes[docp->i_root], cb, arg);
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

int nuxml_cgen(const char *xmlname)
{
   XMLDoc doc;
   char **pathTbl = NULL;
   unsigned int nodecnt = 0;
   ERRG_codeE ret = 0;
   char *xmlbuf = NULL;
   unsigned int size;

   doc.init_value = 0;
   XMLDoc_init(&doc);

   ret = loadNuSocxml(xmlname, &xmlbuf, &size);
   if(ERRG_FAILED(ret))
      goto cleanup;

#ifdef XMLDB_ONLY_LEAVES
   ret = walkXml(&doc, (char *)xmlbuf, countLeaves, &nodecnt); 
   if(ERRG_FAILED(ret))
      goto cleanup;
   pathTbl =(char **)malloc(sizeof(char *) * nodecnt);
   if (!pathTbl)
      goto cleanup;
   ret = walkXml(&doc, (char *)xmlbuf, fillLeavesTbl, pathTbl);
   if (ERRG_FAILED(ret))
      goto cleanup;
#else
   ret = walkXml(&doc, (char *)xmlbuf, countNodes, &nodecnt); 
   if(ERRG_FAILED(ret))
      goto cleanup;

   //allocate char*of nodes
   pathTbl =(char **)malloc(sizeof(char *) * nodecnt);
   if (!pathTbl)
      goto cleanup;
   
   //walk tree again and set each char* to string(allocated) of full path to root
   ret = walkXml(&doc, (char *)xmlbuf, fillPathTbl, pathTbl);
   if (ERRG_FAILED(ret))
      goto cleanup;
#endif
   //output:
   ret = writeSrcfile(pathTbl, nodecnt);
   if(ERRG_FAILED(ret))
      goto cleanup;

   ret = writeHeaderFile(pathTbl, nodecnt);
   if(ERRG_FAILED(ret))
      goto cleanup;

cleanup:
   if (xmlbuf)
      free(xmlbuf);
   if(doc.init_value && doc.nodes)
      XMLDoc_free(&doc);
   freePathTbl(pathTbl, nodecnt);
   return ret;
}

void usage(void)
{
   printf("nuxml_cgen <xml_file>");
}

int main(int argc, char *argv[])
{
   int ret;
	printf("nuxml_cgen tool version 1.0\n");
#ifdef XMLDB_ONLY_LEAVES
	printf("only-leaves mode\n");
#endif
	printf("\n");
   
	if(argc < 2)
	{
	   printf("missing argument\n");
		usage();
		exit(EXIT_FAILURE);
	}
	
	ret = nuxml_cgen(argv[1]);	
	if(ret < 0)
	{
		printf("exit error\n");
		exit(EXIT_FAILURE);
	} 
	printf("success\n");
	exit(EXIT_SUCCESS);	
}







