/*$Id:$*/
/*01.03.2016	10.04.2015	Белых А.И.	iceb_sizw.c
Запись размера окна в файл
*/

#include <errno.h>
#include "iceb_libbuh.h"

int iceb_sizw(const char *name_ok,gint gor,gint vert,GtkWidget *wpredok)
{
class iceb_u_str stroka("");
int metka_vs=0;
class iceb_u_str filname(g_get_home_dir());
filname.plus(G_DIR_SEPARATOR_S,ICEB_KAT_ALX);
filname.plus(G_DIR_SEPARATOR_S,ICEB_IMAF_SIZW);

//printf("%s-%s\n",__FUNCTION__,filname.ravno());

class iceb_u_str imaf_tmp(g_get_home_dir());
imaf_tmp.plus(G_DIR_SEPARATOR_S,ICEB_KAT_ALX);
imaf_tmp.plus(G_DIR_SEPARATOR_S,name_ok);
imaf_tmp.plus(".tmp");

FILE *ff;
FILE *ff_tmp;

if((ff=fopen(filname.ravno(),"r")) == NULL)
 {
  if(errno == ENOENT) /*нет такого файла*/
   {
    if((ff=fopen(filname.ravno(),"w")) == NULL)
     {
      if(errno == ENOENT) /*нет такого файла или каталога*/
       return(1);
      iceb_er_op_fil(filname.ravno(),__FUNCTION__,errno,wpredok);
      return(1);
     }
    
    fprintf(ff,"#%s\n",gettext("Размеры окон"));
    
    fprintf(ff,"%s|%d %d\n",name_ok,gor,vert);
    fclose(ff);
    return(0);
   }
  else
   {
    iceb_er_op_fil(filname.ravno(),__FUNCTION__,errno,wpredok);
    return(1);
   }
 }


if((ff_tmp=fopen(imaf_tmp.ravno(),"w")) == NULL)
 {
  iceb_er_op_fil(imaf_tmp.ravno(),__FUNCTION__,errno,wpredok);
  return(1);
 }

class iceb_u_str pole1("");

while(iceb_u_fgets(&stroka,ff) == 0)
 {

  if(iceb_u_polen(stroka.ravno(),&pole1,1,'|') == 0)
  if(iceb_u_SRAV(name_ok,pole1.ravno(),0) == 0)
   {
    fprintf(ff_tmp,"%s|%d %d\n",name_ok,gor,vert);
    metka_vs=1;
    continue;    
   }
  fprintf(ff_tmp,"%s",stroka.ravno());
 }

if(metka_vs == 0)
    fprintf(ff_tmp,"%s|%d %d\n",name_ok,gor,vert);


fclose(ff_tmp);

fclose(ff);

unlink(filname.ravno());

rename(imaf_tmp.ravno(),filname.ravno());


return(0);
}