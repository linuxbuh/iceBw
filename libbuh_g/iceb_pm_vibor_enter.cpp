/*$Id: iceb_pm_vibor_enter.c,v 1.1 2013/08/13 06:25:27 sasa Exp $*/
/*30.07.2013	30.07.2013	Белых А.И.	iceb_pm_vibor_enter.c
Пунк меню для выбора из заданных значений
*/
#include "iceb_libbuh.h"


void iceb_pm_vibor_enter(class iceb_u_spisok *strmenu,GtkWidget **opt,const char *text_in_enter)
{
*opt=gtk_combo_box_text_new_with_entry();
int nom_as=-1; /*номер активной строки*/
for(int nom=0; nom < strmenu->kolih(); nom++)
 {
  if(iceb_u_SRAV(strmenu->ravno(nom),text_in_enter,0) == 0)
   nom_as=nom;
  gtk_combo_box_text_insert_text (GTK_COMBO_BOX_TEXT(*opt),nom,strmenu->ravno(nom));
 }

if(nom_as >= 0)
  gtk_combo_box_set_active(GTK_COMBO_BOX(*opt),nom_as);
}