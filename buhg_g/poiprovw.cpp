/*$Id: poiprovw.c,v 1.5 2013/05/17 14:58:23 sasa Exp $*/
/*26.03.2008	26.03.2008	Белых А.И.	poiprovw.c
Поиск проводок
*/
#include "buhg_g.h"
#include  "prov_poi.h"

int  m_poiprov(class prov_poi_data *rkp);
void l_prov(class prov_poi_data *rekv_m_poiprov,GtkWidget *wpredok);


void poiprovw()
{
static class prov_poi_data data;

for(;;)
 {  

  if(m_poiprov(&data) != 0)
   return;

  forzappoi(&data);

  l_prov(&data,NULL);
 }
}
