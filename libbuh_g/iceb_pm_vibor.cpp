/*$Id: iceb_pm_vibor.c,v 1.7 2013/08/13 06:10:12 sasa Exp $*/
/*29.07.2013	20.11.2007	Белых А.И.	iceb_pm_vibor.c
Пунк меню для выбора из заданных значений
*/
#include "iceb_libbuh.h"


void iceb_pm_vibor(class iceb_u_spisok *strmenu,GtkWidget **opt,int nomer_str)
{
*opt=gtk_combo_box_text_new();
for(int nom=0; nom < strmenu->kolih(); nom++)
  gtk_combo_box_text_insert_text (GTK_COMBO_BOX_TEXT(*opt),nom,strmenu->ravno(nom));
gtk_combo_box_set_active(GTK_COMBO_BOX(*opt),nomer_str);
}