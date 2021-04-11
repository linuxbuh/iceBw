/*$Id: dirfajl.c,v 5.11 2013-04-07 10:22:54 sasa Exp $*/
/*23.05.2018    15.12.1992      Белых А.И.   iceb_dirfajl.c
Программа просмотра имен файлов заданных по
трафарету
Если вернули 0 значит файлов не нашли

*/
#include	"iceb_libbuh.h"
#include 	<dirent.h>


DIR             *dirp;
struct dirent *dppp;


int iceb_dirfajl(const char *name,
int mi,   //Метка поиска 1-до точки , 2-росле точки, 3 полное имя файла
class iceb_u_spisok *mfa, /*Массив с именами файлов*/
GtkWidget *wpredok)
{
//int             len;
class iceb_u_str bros("");
int             voz;

//len = strlen(name);

if((dirp = opendir(".")) == NULL) 
 {
  perror(".");
  iceb_menu_soob(gettext("Не могу открыть каталог !"),wpredok);

  return(0);
 }
if((dppp = readdir(dirp)) == NULL) 
 {
  perror(__FUNCTION__);
  iceb_menu_soob(gettext("Не могу прочесть каталог !"),wpredok);
  return(0);
 }
voz=0;

for (; dppp != NULL; )
 {
  if(iceb_u_SRAV(name,"*",0) == 0)
    goto vper;
      
  if(iceb_u_polen(dppp->d_name,&bros,3,'.') != 0)
   if(iceb_u_polen(dppp->d_name,&bros,mi,'.') == 0)
    if(iceb_u_proverka(name,bros.ravno(),2,1) == 0)
//    if(strcmp(bros.ravno(),name) == 0 &&  (int)strlen(bros.ravno()) == len) 
     {
vper:;

       voz++;
       mfa->plus(dppp->d_name);
     }
   dppp = readdir(dirp);
 }
closedir(dirp);

return(voz);

}
