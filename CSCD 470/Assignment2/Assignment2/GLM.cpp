/*    
Modified GLM.cpp
Original Source:
Nate Robins, 1997, 2000
nate@pobox.com, http://www.pobox.com/~nate
Modified by: Sensable Technologies

Modified version of the Wavefront OBJ model file format reader/writer/manipulator.

Includes routines for generating smooth normals with
preservation of edges, welding redundant vertices & texture
coordinate generation (spheremap and planar projections) + more.
  
*/

#include "GLM.h"


/* glmFirstPass: first pass at a Wavefront OBJ file that gets all the
 * statistics of the model (such as #vertices, #normals, etc)
 *
 * model - properly initialized GLMmodel structure
 * file  - (fopen'd) file descriptor 
 */
static GLvoid glmFirstPass(GLMmodel* model, FILE* file) 
{
    GLuint numvertices;        /* number of vertices in model */
    GLuint numnormals;         /* number of normals in model */
    GLuint numtriangles;       /* number of triangles in model */
    
    int v, n, t;
    char buf[128];
    
    numvertices = numnormals = numtriangles = 0;
	
    while(fscanf(file,"%s", buf) != EOF) {
       
        switch(buf[0]) {
        case '#':               /* comment */
            /* eat up rest of line */
            fgets(buf, sizeof(buf), file);
            break;
        case 'v':               /* v, vn, vt */
            switch(buf[1]) {
            case '\0':          /* vertex */
                /* eat up rest of line */
                fgets(buf, sizeof(buf), file);
                numvertices++;
                break;
            case 'n':           /* normal */
                /* eat up rest of line */
                fgets(buf, sizeof(buf), file);
                numnormals++;
                break;
            
            default:
                //printf("glmFirstPass(): Unknown token \"%s\".\n", buf);
                //exit(1);
                fgets(buf, sizeof(buf), file);
                break;
            }
            break;
            
            case 'f':               /* face */
                v = n = t = 0;
                fscanf(file, "%s", buf);
                /* can be one of %d, %d//%d, %d/%d, %d/%d/%d %d//%d */
                if (strstr(buf, "//")) {
                    /* v//n */
                    sscanf(buf, "%d//%d", &v, &n);
                    fscanf(file, "%d//%d", &v, &n);
                    fscanf(file, "%d//%d", &v, &n);
                    numtriangles++;
                    
                    while(fscanf(file, "%d//%d", &v, &n) > 0) {
                        numtriangles++;
                        
                    }
                } else if (sscanf(buf, "%d/%d/%d", &v, &t, &n) == 3) {
                    /* v/t/n */
                    fscanf(file, "%d/%d/%d", &v, &t, &n);
                    fscanf(file, "%d/%d/%d", &v, &t, &n);
                    numtriangles++;
                   
                    while(fscanf(file, "%d/%d/%d", &v, &t, &n) > 0) {
                        numtriangles++;
                        
                    }
                } else if (sscanf(buf, "%d/%d", &v, &t) == 2) {
                    /* v/t */
                    fscanf(file, "%d/%d", &v, &t);
                    fscanf(file, "%d/%d", &v, &t);
                    numtriangles++;
                    
                    while(fscanf(file, "%d/%d", &v, &t) > 0) {
                        numtriangles++;
                       
                    }
                } else {
                    /* v */
                    fscanf(file, "%d", &v);
                    fscanf(file, "%d", &v);
                    numtriangles++;
                    while(fscanf(file, "%d", &v) > 0) {
                        numtriangles++;
                    }


                   
                }
                break;
                
            default:
                /* eat up rest of line */
                fgets(buf, sizeof(buf), file);
                break;
        }

	
  }
  
  /* set the stats in the model structure */
  model->numvertices  = numvertices;
  model->numnormals   = numnormals;
  
  model->numtriangles = numtriangles;
  model->numindices = numtriangles*3;

  
  /* allocate memory for the triangles in each group */
  
  
}

/* glmSecondPass: second pass at a Wavefront OBJ file that gets all
 * the data.
 *
 * model - properly initialized GLMmodel structure
 * file  - (fopen'd) file descriptor 
 */
static GLvoid
glmSecondPass(GLMmodel* model, FILE* file) 
{
    GLuint numvertices;        /* number of vertices in model */
    GLuint numnormals;         /* number of normals in model */
    GLuint numtriangles;       /* number of triangles in model */
    GLfloat* vertices;         /* array of vertices  */
    GLfloat* normals;          /* array of normals */
   
    GLuint numindices = 0;
    
    int v, n, t;
    char buf[128];
    
    /* set the pointer shortcuts */

  
    /* on the second pass through the file, read all the data into the
    allocated arrays */
    numvertices = numnormals  = 0;
    numtriangles = 0;

#ifdef WIN32
   
    while (fscanf_s(file, "%s", buf, sizeof(buf)) != EOF) {
#else
    while (fscanf(file, "%s", buf) != EOF) {
#endif
 
        switch(buf[0]) {
        case '#':               /* comment */
            /* eat up rest of line */
            fgets(buf, sizeof(buf), file);
            break;
        case 'v':               /* v, vn, vt */
            switch(buf[1]) {
            case '\0':          /* vertex */
           
                fscanf(file, "%f %f %f", &model->vertices[numvertices].x, &model->vertices[numvertices].y, &model->vertices[numvertices].z);
                   
                numvertices++;
                break;
            case 'n':           /* normal */
             

                fscanf(file, "%f %f %f", &model->normals[numvertices].x, &model->normals[numvertices].y, &model->normals[numvertices].z);
                numnormals++;
                break;
           
            }
            break;
           
            case 'f':               /* face */
                v = n = t = 0;

#ifdef WIN32
                fscanf_s(file, "%s", buf, sizeof(buf));
#else
                 fscanf(file, "%s", buf);
#endif
                
                /* can be one of %d, %d//%d, %d/%d, %d/%d/%d %d//%d */
                if (strstr(buf, "//")) {
                    /* v//n */
                    sscanf(buf, "%d//%d", &v, &n);
                    model->indices[numindices++] = v - 1;
                   
                    fscanf(file, "%d//%d", &v, &n);
                    model->indices[numindices++] = v - 1;
                   
                    fscanf(file, "%d//%d", &v, &n);
                    model->indices[numindices++] = v - 1;
                   
                   
                } else if (sscanf(buf, "%d/%d/%d", &v, &t, &n) == 3) {
                    /* v/t/n */
                     model->indices[numindices++] = v - 1;
                    
                    fscanf(file, "%d/%d/%d", &v, &t, &n);
                    model->indices[numindices++] = v - 1;
                   
                    fscanf(file, "%d/%d/%d", &v, &t, &n);
                    model->indices[numindices++] = v - 1;
                   
                  
                   
                } else if (sscanf(buf, "%d/%d", &v, &t) == 2) {
                    /* v/t */
                  model->indices[numindices++] = v - 1;
                    
                  fscanf(file, "%d/%d", &v, &t);
                  model->indices[numindices++] = v - 1;
                    
                   fscanf(file, "%d/%d", &v, &t);
                   model->indices[numindices++] = v - 1;
              
                    
                } else {
                    /* v */
                    sscanf(buf, "%d", &v);
                    model->indices[numindices] = v - 1;  
                  
                    numindices++;
                    fscanf(file, "%d", &v);
                    model->indices[numindices] = v - 1; 
                   
                    numindices++;
                    fscanf(file, "%d", &v);
                    model->indices[numindices] = v - 1; 
                   
                    numindices++;
                  
                   
                }
                break;
                
            default:
                /* eat up rest of line */
                fgets(buf, sizeof(buf), file);
                break;
    }
  }
 
}


/* public functions */



GLMmodel* 
glmReadOBJ(const char* filename)
{
    GLMmodel* model;
    FILE* file;
    
    /* open the file */
#ifdef WIN32
    fopen_s(&file, filename, "rb");
#else
    file = fopen(filename, "r");
#endif
	
    if (!file) {
        fprintf(stderr, "Can't open data file \"%s\".\n", filename);
        
        exit(1);
    }
	
    /* allocate a new model */
    model = (GLMmodel*)malloc(sizeof(GLMmodel));
    
    model->numvertices   = 0;
    model->vertices    = NULL;
    model->numnormals    = 0;
    model->normals     = NULL;
    model->numtriangles  = 0;
  
	
    /* make a first pass through the file to get a count of the number
    of vertices, normals, texcoords & triangles */
    glmFirstPass(model, file);
	
    /* allocate memory */
    model->vertices = (vec3*)malloc(sizeof(vec3) * (model->numvertices ));

	model->normals = (vec3*)malloc(sizeof(vec3) *(model->numvertices));

    model->indices = (GLuint*)malloc(sizeof(GLuint) *(model->numindices));

    
    /* rewind to beginning of file and read in the data this pass */
    rewind(file);
	
    glmSecondPass(model, file);

	model->numnormals = model->numvertices;


    /* close the file */
    fclose(file);
	
    return model;
}

