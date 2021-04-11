/*$Id: rpvnn_viw.c,v 1.11 2014/07/31 07:08:26 sasa Exp $*/
/*19.07.2014	30.08.2005	Белых А.И.	rpvnn_viw.c
Строка выдачи итога для реестра выданных налоговых накладных
*/
#include "buhg_g.h"

void rpvnn_viw(short dn,short mn,short gn,char *vsego,double *itogo,double *itogo_u,int *kol_strok,FILE *ff)
{
class iceb_u_str str_pod;

if(iceb_u_sravmydat(dn,mn,gn,1,1,2010) < 0)
  str_pod.plus("---------------------------------------------------------------------------------------------------------------------------");

if(iceb_u_sravmydat(dn,mn,gn,1,1,2010) >= 0 && iceb_u_sravmydat(dn,mn,gn,1,3,2013) < 0 )
 str_pod.plus("---------------------------------------------------------------------------------------------------------------------------------");

if(iceb_u_sravmydat(dn,mn,gn,1,3,2013) >= 0 )
 str_pod.plus("-----------------------------------------------------------------------------------------------------------------------------------------------");

fprintf(ff,"%s\n",str_pod.ravno());


if(iceb_u_sravmydat(dn,mn,gn,1,1,2010) >= 0)
 fprintf(ff,"|%*s|%10.2f|",iceb_u_kolbait(70,vsego),vsego,itogo[0]);
else
 fprintf(ff,"|%*s|%10.2f|",iceb_u_kolbait(61,vsego),vsego,itogo[0]);

if(itogo[1] != 0.)
 fprintf(ff,"%9.2f|",itogo[1]);
else
 fprintf(ff,"%9s|","");

if(itogo[2] != 0.)
 fprintf(ff,"%8.2f|",itogo[2]);
else
 fprintf(ff,"%8s|","");


if(itogo[3] != 0.)
 fprintf(ff,"%9.2f|",itogo[3]);
else
 fprintf(ff,"%9s|","");

if(itogo[4] != 0.)
 fprintf(ff,"%9.2f|",itogo[4]);
else
 fprintf(ff,"%9s|","");


if(itogo[5] != 0.)
 fprintf(ff,"%9.2f|",itogo[5]);
else
 fprintf(ff,"%9s|","");

if(iceb_u_sravmydat(dn,mn,gn,1,3,2013) >= 0 )
 {
  if(itogo[6] != 0.)
   fprintf(ff,"%10.2f|",itogo[6]);
  else
   fprintf(ff,"%10s|","");
 }

fprintf(ff,"\n");


fprintf(ff,"%s\n",str_pod.ravno());

if(iceb_u_sravmydat(dn,mn,gn,1,1,2010) >= 0)
 {
  const char *repl={"З них включено до уточнюючих розрахунків за звітний період"};
  fprintf(ff,"|%*s|%10.2f|",iceb_u_kolbait(70,repl),repl,itogo_u[0]);

  if(itogo_u[1] != 0.)
   fprintf(ff,"%9.2f|",itogo_u[1]);
  else
   fprintf(ff,"%9s|","");

  if(itogo_u[2] != 0.)
   fprintf(ff,"%8.2f|",itogo_u[2]);
  else
   fprintf(ff,"%8s|","");


  if(itogo_u[3] != 0.)
   fprintf(ff,"%9.2f|",itogo_u[3]);
  else
   fprintf(ff,"%9s|","");

  if(itogo_u[4] != 0.)
   fprintf(ff,"%9.2f|",itogo_u[4]);
  else
   fprintf(ff,"%9s|","");


  if(itogo_u[5] != 0.)
   fprintf(ff,"%9.2f|",itogo_u[5]);
  else
   fprintf(ff,"%9s|","");

  if(iceb_u_sravmydat(dn,mn,gn,1,3,2013) >= 0 )
    {
    if(itogo_u[5] != 0.)
     fprintf(ff,"%10.2f|",itogo_u[6]);
    else
     fprintf(ff,"%10s|","");
   }

  fprintf(ff,"\n");


  fprintf(ff,"%s\n",str_pod.ravno());

 }
}
