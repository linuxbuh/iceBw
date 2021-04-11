/*$Id: iceb_pm_pr.c,v 1.6 2013/08/13 06:10:12 sasa Exp $*/
/*29.07.2013	18.06.2010	Белых А.И.	iceb_pm_pr.c
Пунк меню для выбора всех, только приходов, только расходов
*/
#include "iceb_libbuh.h"


/***********************************/
void iceb_pm_pr(GtkWidget **opt,int metka_pr)
{

*opt=gtk_combo_box_text_new();
int nom=0;
gtk_combo_box_text_insert_text (GTK_COMBO_BOX_TEXT(*opt),nom++,gettext("Все записи"));
gtk_combo_box_text_insert_text (GTK_COMBO_BOX_TEXT(*opt),nom++,gettext("Только приходы"));
gtk_combo_box_text_insert_text (GTK_COMBO_BOX_TEXT(*opt),nom++,gettext("Только расходи"));

gtk_combo_box_set_active(GTK_COMBO_BOX(*opt),metka_pr);
}
