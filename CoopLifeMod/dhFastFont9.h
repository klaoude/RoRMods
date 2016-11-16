#ifndef DHFASTFONT9_H_
#define DHFASTFONT9_H_


//*******************************************************************
// Change Log
//*******************************************************************
// - Version 0.1 - July 9, 2005
//    - Initial Release
//*******************************************************************

#include <string.h>
#include <stdio.h>
#include <d3dx9.h>


class dhFastFont9{

protected:

   enum constants{
      cMaxChars = 120,
      cMaxVerts = cMaxChars * 6,
      cVertexFVF = (D3DFVF_XYZ|D3DFVF_TEX1|D3DFVF_DIFFUSE)
   };

   struct FFSVertex{
      float x, y, z;
      DWORD colour;
      float tu,tv;

      void Init(float p_x,float p_y,DWORD p_colour,float p_tu,float p_tv){
         x=p_x;
         y=p_y;
         colour=p_colour;
         tu=p_tu;
         tv=p_tv;
         z=0.0f;
      }
   };

   IDirect3DTexture9 *m_font_texture;

   IDirect3DDevice9 *m_device;

   IDirect3DVertexBuffer9 *m_vb;

   D3DXMATRIX m_old_projection;
   D3DXMATRIX m_new_projection;

   D3DXMATRIX m_old_view;
   D3DXMATRIX m_new_view;

   int m_tex_width;
   int m_tex_height;
   float m_char_width;
   float m_char_height;

   WORD m_metrics[256];

   bool m_is_okay;
   bool m_has_metrics;

   DWORD m_colour;

   void init_quad(char p_char,float p_x,float p_y,FFSVertex *p_verts);
   void init_quad_pretty(char p_char,float p_x,float p_y,FFSVertex *p_verts);

   inline HRESULT init(const char *p_metrics_name);
   inline bool draw(const char *p_text,int p_length);
   inline bool draw_pretty(const char *p_text,int p_length);

   inline int length_pretty(const char *p_text,int p_length);
   inline int length(int p_length);

public:
   inline dhFastFont(void);
   inline dhFastFont(IDirect3DDevice9 *p_device,const char *p_file_name,const char *p_metrics_name,D3DFORMAT p_format = D3DFMT_A8R8G8B8);

   inline void Init(IDirect3DDevice9 *p_device,const char *p_file_name,const char *p_metrics_name,D3DFORMAT p_format = D3DFMT_A8R8G8B8);
   inline void Init(IDirect3DDevice9 *p_device,IDirect3DTexture9 *p_texture,const char *p_metrics_name);

   ~dhFastFont9(void);

   void Kill(void);

   inline bool IsOkay(void);

   inline bool Draw(const char *p_text,float p_x,float p_y,DWORD p_colour);
   inline bool DrawFast(const char *p_text,float p_x,float p_y);
   inline bool DrawFastTransformed(const char *p_text,D3DXMATRIX *p_matrix);

   inline void SetColour(DWORD p_colour);
   inline int Length(const char *p_text);

   inline void SetProjection(D3DXMATRIX *p_projection);
   inline void SetStates(void);
};

inline dhFastFont9::dhFastFont(void){

   m_font_texture=NULL;

   m_vb=NULL;

   m_tex_width=-1;
   m_tex_height=-1;
   m_char_width=-1;
   m_char_height=-1;

   memset(m_metrics,0,256);

   m_is_okay=false;
   m_has_metrics=false;
}
inline dhFastFont9::dhFastFont(IDirect3DDevice9 *p_device,const char *p_file_name,const char *p_metrics_name,D3DFORMAT p_format){

   Init(p_device,p_file_name,p_metrics_name,p_format);

}
inline HRESULT dhFastFont9::init(const char *p_metrics_name){
D3DSURFACE_DESC desc;
HRESULT hr;
FILE *metric_file;
IDirect3DSurface9 *back_buffer;
D3DSURFACE_DESC surf_desc;

   m_font_texture->GetLevelDesc(0,&desc);

   m_tex_width=desc.Width;
   m_tex_height=desc.Height;

   m_char_width=m_tex_width/16.0f;
   m_char_height=m_tex_height/16.0f;

   m_has_metrics=false;

   
   if(p_metrics_name!=NULL){
 
      //ToDo:put this into its own read_metrics routine
      metric_file=fopen(p_metrics_name, "rb");
      if (!metric_file){
         #if defined(_DEBUG)
            OutputDebugString("dhFastFont9::init Error loading Font Metrics - doesn't exist");
         #endif
      }else if(fread(m_metrics,2,256,metric_file) != 256){
         #if defined(_DEBUG)
            OutputDebugString("dhFastFont9::init Error loading Font Metrics");
         #endif
      }else{
         m_has_metrics=true;
      }
      if(metric_file){
         fclose(metric_file);
      }

   }

   m_device->GetBackBuffer(0,0,D3DBACKBUFFER_TYPE_MONO,&back_buffer);
   back_buffer->GetDesc(&surf_desc);
   back_buffer->Release();

   D3DXMatrixOrthoOffCenterLH(&m_new_projection,
                              0.0f,    //Left
                              (float)surf_desc.Width,  //Right
                              (float)surf_desc.Height,  //Bottom
                              0.0f,    //Top
                              0.0f,    //Near
                              100.0f); //Far

   D3DXMatrixIdentity(&m_new_view);
   
   hr=m_device->CreateVertexBuffer((cMaxVerts+1)*sizeof(FFSVertex),
                                   D3DUSAGE_WRITEONLY|D3DUSAGE_DYNAMIC ,
                                   cVertexFVF,
                                   D3DPOOL_DEFAULT,
                                   &m_vb,
                                   NULL);

   return hr;
}

inline void dhFastFont9::Kill(void){

   if(m_font_texture)
   {
      m_font_texture->Release();
      m_font_texture=NULL;
   }

   if(m_vb)
   {
      m_vb->Release();
      m_vb=NULL;
   }

}
inline dhFastFont9::~dhFastFont9(void){

   Kill();

}


inline bool dhFastFont9::IsOkay(void){

   return m_is_okay;

}
inline void dhFastFont9::SetProjection(D3DXMATRIX *p_projection){

   m_new_projection=(*p_projection);
}

inline void dhFastFont9::SetColour(DWORD p_colour){

   m_colour=p_colour;

}
inline void dhFastFont9::init_quad(char p_char,float p_x,float p_y,FFSVertex *p_verts){
float tu,tv;
float tu2,tv2;
float x2,y2;
unsigned char uchar = (unsigned char)p_char;

   x2=p_x+m_char_width;
   y2=p_y+m_char_height;

   tu=(uchar%16)/16.0f;
   tu2=tu + 1/16.0f;

   tv=(uchar - (uchar%16))/256.0f;
   tv2=tv+ 1/16.0f;

   p_verts[0].Init(p_x,y2, m_colour,tu, tv2);
   p_verts[1].Init(p_x,p_y,m_colour,tu, tv);
   p_verts[2].Init(x2, p_y,m_colour,tu2,tv);
   p_verts[3]=p_verts[0];
   p_verts[4]=p_verts[2];
   p_verts[5].Init(x2, y2, m_colour,tu2,tv2);
}
inline void dhFastFont9::init_quad_pretty(char p_char,float p_x,float p_y,FFSVertex *p_verts){
float tu,tv;
float tu2,tv2;
float x2,y2;
float width_scale;
unsigned char uchar = (unsigned char)p_char;

   x2=p_x+m_metrics[uchar];
   y2=p_y+m_char_height;

   width_scale=m_metrics[uchar]/m_char_width;

   tu=(uchar%16)/16.0f;
   tu2=tu + ((1/16.0f)*width_scale);

   tv=(uchar - (uchar%16))/256.0f;
   tv2=tv+ 1/16.0f;

   p_verts[0].Init(p_x,y2, m_colour,tu, tv2);
   p_verts[1].Init(p_x,p_y,m_colour,tu, tv);
   p_verts[2].Init(x2, p_y,m_colour,tu2,tv);
   p_verts[3]=p_verts[0];
   p_verts[4]=p_verts[2];
   p_verts[5].Init(x2, y2, m_colour,tu2,tv2);
}
inline void dhFastFont9::SetStates(void){


   m_device->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
   m_device->SetTextureStageState( 0, D3DTSS_COLORARG1,  D3DTA_TEXTURE);
   m_device->SetTextureStageState( 0, D3DTSS_COLORARG2,  D3DTA_DIFFUSE);

   m_device->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
   m_device->SetRenderState( D3DRS_SRCBLEND,   D3DBLEND_SRCALPHA );
   m_device->SetRenderState( D3DRS_DESTBLEND,  D3DBLEND_INVSRCALPHA );

   m_device->SetRenderState( D3DRS_ALPHATESTENABLE, TRUE );
   m_device->SetRenderState( D3DRS_ALPHAREF, 0x8 );

   m_device->SetRenderState( D3DRS_LIGHTING, false );

   m_device->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
   m_device->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );
   m_device->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );

   m_device->SetTexture(0, m_font_texture);

   m_device->SetStreamSource(0,m_vb,0,sizeof(FFSVertex));
   m_device->SetFVF(cVertexFVF);

   m_device->SetTransform(D3DTS_PROJECTION,&m_new_projection);
   m_device->SetTransform(D3DTS_VIEW,&m_new_view);
   
}
inline bool dhFastFont9::draw(const char *p_text,int p_length){
FFSVertex *vert_ptr;
int counter;
float x;

   m_vb->Lock(0,0,(void **)&vert_ptr,D3DLOCK_DISCARD);

   for(counter=0;counter < p_length;counter++){
      x= counter*m_char_width;

      init_quad(p_text[counter],x,0,vert_ptr);

      vert_ptr+=6;
   }

   m_vb->Unlock();

   
   return true;

}
inline bool dhFastFont9::draw_pretty(const char *p_text,int p_length){
FFSVertex *vert_ptr;
int counter;
float x=0;
unsigned char last_char;

   m_vb->Lock(0,0,(void **)&vert_ptr,D3DLOCK_DISCARD );

   for(counter=0;counter < p_length;counter++){
      if(counter!=0){
         last_char=p_text[counter-1];
         x+=m_metrics[last_char];
      }

      init_quad_pretty(p_text[counter],x,0,vert_ptr);

      vert_ptr+=6;
   }

   m_vb->Unlock();

   return true;

}
inline int dhFastFont9::length_pretty(const char *p_text,int p_length){
int counter;
int pix_length=0;
unsigned char current_char;

   for(counter=0;counter < p_length;counter++){
      current_char=p_text[counter];
      pix_length+=m_metrics[current_char];
   }

   return pix_length;
}
inline int dhFastFont9::length(int p_length){

   return (int)(p_length * m_char_width);

}

inline int dhFastFont9::Length(const char *p_text){
int str_length;
int pix_length;

   if(strlen(p_text) > cMaxChars){
      str_length=cMaxChars;
   }else{
      str_length=(int)strlen(p_text);
   }

   if(m_has_metrics){
      pix_length=length_pretty(p_text,str_length);
   }else{
      pix_length=length(str_length);
   }

   return pix_length;
}

inline bool dhFastFont9::Draw(const char *p_text,float p_x,float p_y,DWORD p_colour){
bool ret;

   m_device->GetTransform(D3DTS_PROJECTION,&m_old_projection);
   m_device->GetTransform(D3DTS_VIEW,&m_old_view);
   
   SetStates();

   m_colour=p_colour;

   ret=DrawFast(p_text,p_x,p_y);

   m_device->SetTexture(0, NULL);

   m_device->SetTransform(D3DTS_PROJECTION,&m_old_projection);
   m_device->SetTransform(D3DTS_VIEW,&m_old_view);
   
   return ret;
}

inline bool dhFastFont9::DrawFast(const char *p_text,float p_x,float p_y){
D3DXMATRIX trans_matrix;
int length;
bool ret;

   if(strlen(p_text) > cMaxChars){
      length=cMaxChars;
   }else{
      length=(int)strlen(p_text);
   }

   if(m_has_metrics){
      ret=draw_pretty(p_text,length);
   }else{
      ret=draw(p_text,length);
   }

   D3DXMatrixIdentity(&trans_matrix);
   D3DXMatrixTranslation(&trans_matrix,p_x,p_y,0);
   m_device->SetTransform(D3DTS_WORLD,&trans_matrix);

   m_device->SetTexture(0,m_font_texture);
   m_device->SetStreamSource(0,m_vb,0,sizeof(FFSVertex));
   m_device->SetFVF(cVertexFVF);

   m_device->DrawPrimitive(D3DPT_TRIANGLELIST,0,length*2);

   return ret;
}
inline bool dhFastFont9::DrawFastTransformed(const char *p_text,D3DXMATRIX *p_matrix){
int length;
bool ret;

   if(strlen(p_text) > cMaxChars){
      length=cMaxChars;
   }else{
      length=(int)strlen(p_text);
   }

   if(m_has_metrics){
      ret=draw_pretty(p_text,length);
   }else{
      ret=draw(p_text,length);
   }

   m_device->SetTransform(D3DTS_WORLD,p_matrix);

   m_device->SetTexture(0,m_font_texture);
   m_device->SetStreamSource(0,m_vb,0,sizeof(FFSVertex));
   m_device->SetFVF(cVertexFVF);

   m_device->DrawPrimitive(D3DPT_TRIANGLELIST,0,length*2);

   return ret;
}



inline void dhFastFont9::Init(IDirect3DDevice9 *p_device,const char *p_file_name,const char *p_metrics_name,D3DFORMAT p_format){
HRESULT hr;

   m_device=p_device;

   hr=D3DXCreateTextureFromFileEx(m_device, //Our D3D Device
                                  p_file_name,    //Filename of our texture
                                  D3DX_DEFAULT, //Width:D3DX_DEFAULT = Take from file 
                                  D3DX_DEFAULT, //Height:D3DX_DEFAULT = Take from file
                                  1,            //MipLevels
                                  0,            //Usage, Is this to be used as a Render Target? 0 == No
                                  p_format,
                                  D3DPOOL_MANAGED,//Pool, let D3D Manage our memory
                                  D3DX_DEFAULT, //Filter:Default filtering
                                  D3DX_DEFAULT, //MipFilter, used for filtering mipmaps
                                  0,            //Disable ColourKey
                                  NULL,         //SourceInfo, returns extra info if we want it (we don't)
                                  NULL,         //Palette:We're not using one
                                  &m_font_texture);  // Our texture goes here.

   if(FAILED(hr)){
      return;
   }

   hr=init(p_metrics_name);

   if(FAILED(hr)){
      return;
   }

   m_is_okay=true;
}

inline void dhFastFont9::Init(IDirect3DDevice9 *p_device,IDirect3DTexture9 *p_texture,const char *p_metrics_name){
HRESULT hr;

   m_device=p_device;

   m_font_texture=p_texture;
   
   //This way we can safely Release it in our Destructor without freeing
   //something we don't own
   m_font_texture->AddRef();

   hr=init(p_metrics_name);

   if(FAILED(hr)){
      return;
   }

   m_is_okay=true;
}

#endif //#ifndef DHFASTFONT9_H_









