/*$Id: iceb_pm_nds.c,v 1.13 2014/07/31 07:08:29 sasa Exp $*/
/*17.07.2014	24.10.2007	Белых А.И.	iceb_pm_nds.c
Пунк меню для ввода НДС
*/
#include "iceb_libbuh.h"



/*******************************************/
void iceb_pm_nds(short d,short m,short g,GtkWidget **opt,int nds)
{
char bros[512];

if(d == 0)
 iceb_u_poltekdat(&d,&m,&g);
 
double pnds=iceb_pnds(d,m,g,NULL);

sprintf(bros,"%.f %s",pnds,gettext("НДС"));

*opt=gtk_combo_box_text_new();
int nom=0;
gtk_combo_box_text_insert_text (GTK_COMBO_BOX_TEXT(*opt),nom++,bros);
gtk_combo_box_text_insert_text (GTK_COMBO_BOX_TEXT(*opt),nom++,gettext("0% НДС реализация на територии Украины"));
gtk_combo_box_text_insert_text (GTK_COMBO_BOX_TEXT(*opt),nom++,gettext("0% НДС экспорт"));
gtk_combo_box_text_insert_text (GTK_COMBO_BOX_TEXT(*opt),nom++,gettext("Освобождение от НДС статья 5"));
gtk_combo_box_text_insert_text (GTK_COMBO_BOX_TEXT(*opt),nom++,gettext("7% НДС на мед.препараты"));

gtk_combo_box_set_active(GTK_COMBO_BOX(*opt),nds);
}

/********************************/
void iceb_pm_nds(const char *data,GtkWidget **opt,int nds)
{
short d,m,g;
iceb_u_rsdat(&d,&m,&g,data,0);
iceb_pm_nds(d,m,g,opt,nds);

}
