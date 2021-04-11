/*$Id: iceb_snanomer.c,v 1.5 2013/12/31 11:49:19 sasa Exp $*/
/*18.12.2013	08.06.2006	Белых А.И.	iceb_snanomer.c
Стать в меню на нужный номер строки
*/
#include "iceb_libbuh.h"

void iceb_snanomer(int kolzap,int *snanomer,GtkWidget *treeview)
{

if(kolzap <= 0)
 return;

if(*snanomer  > kolzap-1)
   *snanomer= kolzap-1;

if(*snanomer  < 0)
   *snanomer=0;
     
 //  printf("poz=%d\n",data->snanomer);
char strsql[100];

sprintf(strsql,"%d",*snanomer);
GtkTreePath *path=gtk_tree_path_new_from_string(strsql);

 //  gtk_tree_view_scroll_to_cell(GTK_TREE_VIEW (data->treeview),path,NULL,TRUE,0,0);
gtk_tree_view_scroll_to_cell(GTK_TREE_VIEW (treeview),path,NULL,FALSE,0,0);

gtk_tree_view_set_cursor(GTK_TREE_VIEW (treeview),path,NULL,FALSE);
gtk_tree_path_free(path);

gtk_widget_grab_focus(treeview); /*установить фокус ввода на записи в списке*/


}
