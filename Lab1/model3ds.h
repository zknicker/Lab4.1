/***************************************************************************/
/*                                                                         */
/*                            www.xbdev.net                                */
/*                         bkenwright@xbdev.net                            */
/*                                3ds.h                                    */
/*                                                                         */
/***************************************************************************/
/***************************************************************************/
/*
   What we have here is a totally self contained 3DS file loader, that is
   easy to use and extremely powerful.
 
   Using it?
   <1> #include "3ds.h"
   <2> C3DS myObj;
   <3> myObj.Create("cube.3ds");
 
   <4> Access the 3D data through the m_Object member..vertices etc
       myObj.m_Object.m_iNumMaterials;
         myObj.m_Object.m_iNumMeshs;
         .. etc
 
   <5> myObj.Release(); // Tidy up when you've finished.
 
*/
/***************************************************************************/
 
#pragma once
 
#include <stdio.h>
#include <vector>
using namespace std ;
 
 
 
//>----- Entry point (Primary Chunk at the start of the file ----------------
#define           PRIMARY                 0x4D4D
 
//>----- Main Chunks --------------------------------------------------------
#define           EDIT3DS                 0x3D3D  // Start of our actual objects
#define           KEYF3DS                 0xB000  // Start of the keyframe information
 
//>----- General Chunks -----------------------------------------------------
#define           VERSION                 0x0002
#define           MESH_VERSION      0x3D3E
#define           KFVERSION         0x0005
#define           COLOR_F                 0x0010
#define           COLOR_24          0x0011
#define           LIN_COLOR_24      0x0012
#define           LIN_COLOR_F       0x0013
#define           INT_PERCENTAGE    0x0030
#define           FLOAT_PERC        0x0031
#define           MASTER_SCALE      0x0100
#define           IMAGE_FILE        0x1100
#define           AMBIENT_LIGHT     0X2100
 
//>----- Object Chunks -----------------------------------------------------
#define           NAMED_OBJECT      0x4000
#define           OBJ_MESH          0x4100
#define           MESH_VERTICES     0x4110
#define           VERTEX_FLAGS      0x4111
#define           MESH_FACES        0x4120
#define           MESH_MATER        0x4130
#define           MESH_TEX_VERT     0x4140
#define           MESH_XFMATRIX     0x4160
#define           MESH_COLOR_IND    0x4165
#define           MESH_TEX_INFO     0x4170
#define           HEIRARCHY         0x4F00
 
 
//>----- Material Chunks ---------------------------------------------------
#define           MATERIAL          0xAFFF
#define           MAT_NAME          0xA000
#define           MAT_AMBIENT       0xA010
#define           MAT_DIFFUSE       0xA020
#define           MAT_SPECULAR      0xA030
#define           MAT_SHININESS     0xA040
#define           MAT_FALLOFF       0xA052
#define           MAT_EMISSIVE      0xA080
#define           MAT_SHADER        0xA100
#define           MAT_TEXMAP        0xA200
#define           MAT_TEXFLNM       0xA300
 
#define           OBJ_LIGHT         0x4600
#define           OBJ_CAMERA        0x4700
 
//>----- KeyFrames Chunks --------------------------------------------------
#define           ANIM_HEADER       0xB00A
#define           ANIM_OBJ          0xB002
 
#define           ANIM_NAME         0xB010
#define           ANIM_POS          0xB020
#define           ANIM_ROT          0xB021
#define           ANIM_SCALE        0xB022
 
 
 
 
class C3DS
{
public:
 
      struct stMaterial
      {
            char szName[256];
            struct{ unsigned char r,g,b ; }Colour;
            char szTextureFile[256];
      };
 
      struct stVert
      {
            float x, y, z;
      };
      struct stFace
      {   // 3 Sides of a triangle make a face.
            unsigned int A, B, C;
            int MaterialID;
      };
      struct stTex
      {
            float tu, tv;
      };
 
      struct stMesh
      {
            char         szMeshName[256];
            int          iNumVerts;
            stVert*      pVerts;
            int          iNumFaces;
            stFace*      pFaces;
            stTex*       pTexs;
 
            stMesh()
            {
                  iNumVerts  = 0;
                  pVerts     = NULL;
                  iNumFaces  = 0;
                  pFaces     = NULL;
                  pTexs      = NULL;
            }
           
      };
 
 
protected:
      struct stChunk
      {
            unsigned short ID;
            unsigned int length;
            unsigned int bytesRead;
      };
 
public:
      C3DS(void);
      ~C3DS(void);
 
      bool Create(char* szFileName);
      void Release();
	  vector<Vertex> GetVertexInfo();

public:
 
      int                m_iNumMeshs;
      vector<stMesh>     m_pMeshs;
 
      int                m_iNumMaterials;
      vector<stMaterial> m_pMaterials;
 
protected:
 
      FILE* m_fp;
 
      void ParseChunk        (stChunk* Chunk);
      void GetMaterialName   (stChunk* Chunk);
      void GetDiffuseColour  (stChunk* Chunk);
      void GetTexFileName    (stChunk* Chunk);
      void GetMeshObjectName (stChunk* Chunk);
      void ReadMeshTexCoords (stChunk* Chunk);
      void ReadMeshVertices  (stChunk* Chunk);
      void ReadMeshFaces     (stChunk* Chunk);
      void ReadMeshMaterials (stChunk* Chunk);
      int GetString          (char* pBuffer);
      void SkipChunk         (stChunk *pChunk);
      void ReadChunk         (stChunk *pChunk);
 
};
 
 
 
 
// DEBUG DEBUG DEBUG DEBUG DEBUG DEBUG DEBUG DEBUG DEBUG DEBUG DEBUG
// DEBUG DEBUG DEBUG DEBUG DEBUG DEBUG DEBUG DEBUG DEBUG DEBUG DEBUG
// DEBUG DEBUG DEBUG DEBUG DEBUG DEBUG DEBUG DEBUG DEBUG DEBUG DEBUG
// Debugging Function
void debug_op(char *s);
void DisplayRawData(C3DS* pObj);