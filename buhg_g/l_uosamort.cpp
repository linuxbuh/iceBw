/*$Id: l_uosamort.c,v 1.25 2013/12/31 11:49:15 sasa Exp $*/
/*19.06.2019	17.07.2005	Белых А.И.	l_uosamort.c
Работа с записями амортизационных отчислений налогового учёта
*/
#include <errno.h>
#include "buhg_g.h"
enum
{
 FK2,
 SFK2,
 FK3,
 FK4,
 FK5,
 FK6,
 FK7,
 FK8,
 FK10,
 KOL_F_KL
};

enum
{
 COL_DATA,
 COL_SUMA,
 COL_PODR,
 COL_KODMO,
 COL_HZT,
 COL_HNA,
 COL_DATA_VREM,
 COL_KTO,  
 NUM_COLUMNS
};

class  l_uosamort_data
 {
  public:
  GtkWidget *label_kolstr;
  GtkWidget *label_bsiz;
  GtkWidget *sw;
  GtkWidget *treeview;
  GtkWidget *window;
  GtkWidget *knopka[KOL_F_KL];
  short     kl_shift; //0-отжата 1-нажата  
  int       snanomer;   //номер записи на которую надостать 
  int       kolzap;     //Количество записей

  class iceb_u_str zapros;

  int innom;
  class iceb_u_str naim;
  int podr;
  int kodmo;
  class iceb_u_str hzt;
  class iceb_u_str hna;
  short dr,mr,gr; /*Дата расчёта остатка*/
    
  class iceb_u_str data_v; //Выбранная дата 
  class iceb_u_str data_tv; /*Дата только что введённая*/
  class iceb_u_str imafprot; /*Имя файла с протоколом расчёта балансовой стоимости*/
  class iceb_u_str imafprot_ao; /*Имя файла с протоколом расчёта амортизации*/
  //Конструктор
  l_uosamort_data()
   {
    snanomer=0;
    kl_shift=0;
    window=treeview=NULL;
    
   }      
 };

gboolean   l_uosamort_key_press(GtkWidget *widget,GdkEventKey *event,class l_uosamort_data *data);
void l_uosamort_vibor(GtkTreeSelection *selection,class l_uosamort_data *data);
void l_uosamort_v_row(GtkTreeView *treeview,GtkTreePath *arg1,GtkTreeViewColumn *arg2,class l_uosamort_data *data);
void l_uosamort_knopka(GtkWidget *widget,class l_uosamort_data *data);
void l_uosamort_add_columns (GtkTreeView *treeview);
void l_uosamort_create_list(class l_uosamort_data *data);

//int l_uosamort_v(int innom,const int,const char*,const char *hzt,const char *hna,GtkWidget *wpredok);
int l_uosamort_v(int innom,int podr,int kodmo,const char *mes_god,const char *hzt,const char *hna,GtkWidget *wpredok);
void l_uosamort_udzap(class l_uosamort_data *data);
void l_uosamort_rasp(class l_uosamort_data *data);
void l_uosamort_bsiz(class l_uosamort_data *data);
void l_uosamort_ra(class l_uosamort_data *data);
void l_uosamort_sd(class l_uosamort_data *data);
void l_uosamort_ppr(class l_uosamort_data *data);

extern SQL_baza	bd;
extern short	startgoduos; /*Стартовый год*/
extern short	metkabo; //Если 1 то организация бюджетная
 
void l_uosamort(int innom,int podr,int kodmo,const char *hzt,const char *hna,GtkWidget *wpredok)
{
class l_uosamort_data data;
char bros[512];
char strsql[512];
SQL_str row;
class SQLCURSOR cur;

data.innom=innom;
data.podr=podr;
data.kodmo=kodmo;
data.hzt.new_plus(hzt);
data.hna.new_plus(hna);
data.naim.plus("");
iceb_u_poltekdat(&data.dr,&data.mr,&data.gr);
data.dr=1;
sprintf(strsql,"bsiz%d.lst",getpid());
data.imafprot.new_plus(strsql);
sprintf(strsql,"bsizam%d.lst",getpid());
data.imafprot_ao.new_plus(strsql);

/*узнаём наименование*/
sprintf(strsql,"select naim from Uosin where innom=%d",innom);
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
 data.naim.new_plus(row[0]);
  
data.window = gtk_window_new (GTK_WINDOW_TOPLEVEL);

gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);


sprintf(bros,"%s %s",iceb_get_namesystem(),gettext("Амортизационные отчисления налогового учёта"));

gtk_window_set_title (GTK_WINDOW (data.window),bros);
gtk_container_set_border_width (GTK_CONTAINER (data.window), 5);

g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);


if(wpredok != NULL)
 {
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR_GDITE));
  //Удерживать окно над породившем его окном всегда
  gtk_window_set_transient_for(GTK_WINDOW(data.window),GTK_WINDOW(wpredok));
  //Закрыть окно если окно предок удалено
  gtk_window_set_destroy_with_parent(GTK_WINDOW(data.window),TRUE);
 }

g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(l_uosamort_key_press),&data);
g_signal_connect_after(data.window,"key_release_event",G_CALLBACK(iceb_key_release),&data.kl_shift);

GtkWidget *hbox = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 1);
gtk_box_set_homogeneous (GTK_BOX(hbox),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
gtk_container_add (GTK_CONTAINER (data.window), hbox);

GtkWidget *vbox1 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 1);
gtk_box_set_homogeneous (GTK_BOX(vbox1),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
GtkWidget *vbox2 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 1);
gtk_box_set_homogeneous (GTK_BOX(vbox2),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет

gtk_box_pack_start (GTK_BOX (hbox), vbox1, FALSE, FALSE, 1);
gtk_box_pack_start (GTK_BOX (hbox), vbox2, TRUE, TRUE, 1);
gtk_widget_show(hbox);

class iceb_u_str zagol;
zagol.plus(gettext("Амортизационные отчисления налогового учёта"));
zagol.ps_plus(gettext("Инвентарный номер"));
zagol.plus(":");
zagol.plus(data.innom);
zagol.plus("/");
zagol.plus(data.naim.ravno());

data.label_kolstr=gtk_label_new(zagol.ravno());


gtk_box_pack_start (GTK_BOX (vbox2),data.label_kolstr,FALSE, FALSE, 1);

gtk_widget_show(vbox1);
gtk_widget_show(vbox2);




data.sw = gtk_scrolled_window_new (NULL, NULL);

gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (data.sw),GTK_SHADOW_ETCHED_IN);
gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (data.sw),GTK_POLICY_AUTOMATIC,GTK_POLICY_AUTOMATIC);
gtk_box_pack_start(GTK_BOX (vbox2), data.sw, TRUE, TRUE, 1);

data.label_bsiz=gtk_label_new("");
gtk_box_pack_start(GTK_BOX (vbox2), data.label_bsiz, FALSE, FALSE, 1);

gtk_widget_show(data.label_bsiz);

//Кнопки


sprintf(bros,"F2 %s",gettext("Ввести"));
data.knopka[FK2]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK2],TRUE,TRUE, 0);
g_signal_connect(data.knopka[FK2], "clicked",G_CALLBACK(l_uosamort_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[FK2],gettext("Ввод новой записи"));
gtk_widget_set_name(data.knopka[FK2],iceb_u_inttochar(FK2));
gtk_widget_show(data.knopka[FK2]);

sprintf(bros,"%sF2 %s",RFK,gettext("Корректировать"));
data.knopka[SFK2]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[SFK2],TRUE,TRUE, 0);
g_signal_connect(data.knopka[SFK2], "clicked",G_CALLBACK(l_uosamort_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[SFK2],gettext("Корректировка выбранной записи"));
gtk_widget_set_name(data.knopka[SFK2],iceb_u_inttochar(SFK2));
gtk_widget_show(data.knopka[SFK2]);

sprintf(bros,"F3 %s",gettext("Удалить"));
data.knopka[FK3]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK3],TRUE,TRUE, 0);
g_signal_connect(data.knopka[FK3], "clicked",G_CALLBACK(l_uosamort_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[FK3],gettext("Удалить выбранную запись"));
gtk_widget_set_name(data.knopka[FK3],iceb_u_inttochar(FK3));
gtk_widget_show(data.knopka[FK3]);

sprintf(bros,"F4 %s",gettext("Расчёт"));
data.knopka[FK4]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK4],TRUE,TRUE, 0);
g_signal_connect(data.knopka[FK4], "clicked",G_CALLBACK(l_uosamort_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[FK4],gettext("Расчёт амортизации"));
gtk_widget_set_name(data.knopka[FK4],iceb_u_inttochar(FK4));
gtk_widget_show(data.knopka[FK4]);

sprintf(bros,"F5 %s",gettext("Печать"));
data.knopka[FK5]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK5],TRUE,TRUE, 0);
g_signal_connect(data.knopka[FK5], "clicked",G_CALLBACK(l_uosamort_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[FK5],gettext("Распечатка записей"));
gtk_widget_set_name(data.knopka[FK5],iceb_u_inttochar(FK5));
gtk_widget_show(data.knopka[FK5]);

sprintf(bros,"F6 %s",gettext("Документы"));
data.knopka[FK6]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK6],TRUE,TRUE, 0);
g_signal_connect(data.knopka[FK6], "clicked",G_CALLBACK(l_uosamort_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[FK6],gettext("Получить список документов"));
gtk_widget_set_name(data.knopka[FK6],iceb_u_inttochar(FK6));
gtk_widget_show(data.knopka[FK6]);

sprintf(bros,"F7 %s",gettext("Протокол"));
data.knopka[FK7]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK7],TRUE,TRUE, 0);
g_signal_connect(data.knopka[FK7], "clicked",G_CALLBACK(l_uosamort_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[FK7],gettext("Просмотр протокола определения балансовой стоимости"));
gtk_widget_set_name(data.knopka[FK7],iceb_u_inttochar(FK7));
gtk_widget_show(data.knopka[FK7]);

sprintf(bros,"F8 %s",gettext("Смена даты"));
data.knopka[FK8]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK8],TRUE,TRUE, 0);
g_signal_connect(data.knopka[FK8], "clicked",G_CALLBACK(l_uosamort_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[FK8],gettext("Сменить дату на которую рассчитать остаточную стоимость"));
gtk_widget_set_name(data.knopka[FK8],iceb_u_inttochar(FK8));
gtk_widget_show(data.knopka[FK8]);


sprintf(bros,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1),data.knopka[FK10],TRUE,TRUE, 0);
gtk_widget_set_tooltip_text(data.knopka[FK10],gettext("Завершение работы в этом окне"));
g_signal_connect(data.knopka[FK10], "clicked",G_CALLBACK(l_uosamort_knopka),&data);
gtk_widget_set_name(data.knopka[FK10],iceb_u_inttochar(FK10));
gtk_widget_show(data.knopka[FK10]);


gtk_widget_realize(data.window);
gdk_window_set_cursor(gtk_widget_get_window(data.window),gdk_cursor_new_for_display(gtk_widget_get_display(data.window),ICEB_CURSOR));

//gtk_widget_grab_focus(data.knopka[FK10]);

l_uosamort_create_list(&data);
gtk_widget_show(data.window);

/*if(metka == 0)*/
  gtk_window_maximize(GTK_WINDOW(data.window));


gtk_main();

unlink(data.imafprot.ravno());
unlink(data.imafprot_ao.ravno());

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));


}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  l_uosamort_knopka(GtkWidget *widget,class l_uosamort_data *data)
{

int knop=atoi(gtk_widget_get_name(widget));
//g_print("uosamort_knopka knop=%d\n",knop);

data->kl_shift=0; //Сбрасываем нажатый сшифт так как при после запуска нового меню он не сбрасывается

switch (knop)
 {
  case FK2:
    if(l_uosamort_v(data->innom,data->podr,data->kodmo,"",data->hzt.ravno(),data->hna.ravno(),data->window) == 0)
     l_uosamort_create_list(data);
    return;  

  case SFK2:
    if(l_uosamort_v(data->innom,data->podr,data->kodmo,data->data_v.ravno(),data->hzt.ravno(),data->hna.ravno(),data->window) == 0)
      l_uosamort_create_list(data);
    return;  

  case FK3:
    if(data->kolzap == 0)
      return;
    l_uosamort_udzap(data);
    l_uosamort_create_list(data);
    return;  


  case FK4:
    l_uosamort_ra(data);
    l_uosamort_create_list(data);
    return;  

  case FK5:
    l_uosamort_rasp(data);
    return;  

  case FK6:
    uosrsdw(data->innom,data->window);
    return;  

  case FK7:
    l_uosamort_ppr(data);
    return;  

  case FK8:
    l_uosamort_sd(data);
    return;  

    
  case FK10:
    gtk_widget_destroy(data->window);
    return;
 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   l_uosamort_key_press(GtkWidget *widget,GdkEventKey *event,class l_uosamort_data *data)
{
iceb_u_str repl;
//printf("uosamort_key_press keyval=%d state=%d\n",event->keyval,event->state);

switch(event->keyval)
 {
  case GDK_KEY_F2:
    if(data->kl_shift == 0)
      g_signal_emit_by_name(data->knopka[FK2],"clicked");
    if(data->kl_shift == 1)
      g_signal_emit_by_name(data->knopka[SFK2],"clicked");
    return(TRUE);

  case GDK_KEY_F3:
    g_signal_emit_by_name(data->knopka[FK3],"clicked");
    return(TRUE);

  case GDK_KEY_F4:
    g_signal_emit_by_name(data->knopka[FK4],"clicked");
    return(TRUE);

  case GDK_KEY_F5:
    g_signal_emit_by_name(data->knopka[FK5],"clicked");
    return(TRUE);

  case GDK_KEY_F6:
    g_signal_emit_by_name(data->knopka[FK6],"clicked");
    return(TRUE);


  case GDK_KEY_F7:
    g_signal_emit_by_name(data->knopka[FK7],"clicked");
    return(TRUE);

  case GDK_KEY_F8:
    g_signal_emit_by_name(data->knopka[FK8],"clicked");
    return(TRUE);



  case GDK_KEY_Escape:
  case GDK_KEY_F10:
    g_signal_emit_by_name(data->knopka[FK10],"clicked");
    return(FALSE);

  case ICEB_REG_L:
  case ICEB_REG_R:
    printf("uosamort_key_press-Нажата клавиша Shift\n");

    data->kl_shift=1;

    return(TRUE);

  default:
    printf("Не выбрана клавиша !\n");
    break;
 }

return(TRUE);
}
/***********************************/
/*Создаем список для просмотра */
/***********************************/
void l_uosamort_create_list (class l_uosamort_data *data)
{
GtkListStore *model=NULL;
GtkTreeIter iter;
SQLCURSOR cur;
SQLCURSOR cur1;
char strsql[512];
int  kolstr=0;
SQL_str row;
iceb_u_str zagolov;

//printf("uosamort_create_list %d\n",data->snanomer);
data->kl_shift=0; //0-отжата 1-нажата  

gdk_window_set_cursor(gtk_widget_get_window(data->window),gdk_cursor_new_for_display(gtk_widget_get_display(data->window),ICEB_CURSOR_GDITE));
iceb_refresh();

if(data->treeview != NULL)
  gtk_widget_destroy(data->treeview);

data->treeview = gtk_tree_view_new();


gtk_container_add (GTK_CONTAINER (data->sw), data->treeview);

g_signal_connect(data->treeview,"row_activated",G_CALLBACK(l_uosamort_v_row),data);

GtkTreeSelection *selection=gtk_tree_view_get_selection(GTK_TREE_VIEW(data->treeview));
gtk_tree_selection_set_mode(selection,GTK_SELECTION_SINGLE);
g_signal_connect(selection,"changed",G_CALLBACK(l_uosamort_vibor),data);

gtk_tree_selection_set_mode (gtk_tree_view_get_selection (GTK_TREE_VIEW (data->treeview)),GTK_SELECTION_SINGLE);




model = gtk_list_store_new (NUM_COLUMNS+1, 
G_TYPE_STRING, 
G_TYPE_STRING, 
G_TYPE_STRING, 
G_TYPE_STRING, 
G_TYPE_STRING, 
G_TYPE_STRING, 
G_TYPE_STRING, 
G_TYPE_STRING, 
G_TYPE_INT);

//sprintf(strsql,"select * from Uosamor where innom=%d and god >= %d order by god desc,mes desc",data->innom,startgoduos);
sprintf(strsql,"select * from Uosamor where innom=%d and da >= '%04d-01-01' order by da desc",data->innom,startgoduos);

data->zapros.new_plus(strsql);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  return;
 }


data->kolzap=0;

short den=0,mes=0,god=0;
while(cur.read_cursor(&row) != 0)
 {
//  printf("%s %s %s %s\n",row[0],row[1],row[2],row[3]);
  
  iceb_u_rsdat(&den,&mes,&god,row[1],2);
  
  //mes=atoi(row[1]);
//  god=atoi(row[2]);
  sprintf(strsql,"%02d.%d",mes,god);
  if(iceb_u_SRAV(strsql,data->data_tv.ravno(),0) == 0)
   data->snanomer=data->kolzap;
  
  gtk_list_store_append (model, &iter);

  gtk_list_store_set (model, &iter,
  COL_DATA,iceb_u_datzap(row[1]),
  COL_SUMA,row[5],
  COL_PODR,row[2],
  COL_KODMO,row[8],
  COL_HZT,row[3],
  COL_HNA,row[4],
  COL_DATA_VREM,iceb_u_vremzap(row[7]),
  COL_KTO,iceb_kszap(row[6],data->window),
  NUM_COLUMNS,data->kolzap,
  -1);

  data->kolzap++;
 }
data->data_tv.new_plus("");
gtk_tree_view_set_model (GTK_TREE_VIEW(data-> treeview),GTK_TREE_MODEL (model));

g_object_unref (GTK_TREE_MODEL (model));

l_uosamort_add_columns (GTK_TREE_VIEW (data->treeview));


if(data->kolzap == 0)
 {
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[SFK2]),FALSE);//Недоступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK3]),FALSE);//Недоступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK5]),FALSE);//Недоступна
 }
else
 {
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[SFK2]),TRUE);//Доступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK3]),TRUE);//Доступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK5]),TRUE);//Доступна
 }

gtk_widget_show (data->treeview);
gtk_widget_show (data->sw);

iceb_snanomer(data->kolzap,&data->snanomer,data->treeview);



zagolov.new_plus(gettext("Амортизационные отчисления налогового учёта"));
zagolov.ps_plus(gettext("Инвентарный номер"));
zagolov.plus(":");
zagolov.plus(data->innom);
zagolov.plus("/");
zagolov.plus(data->naim.ravno());

sprintf(strsql," %s:%d",gettext("Количество записей"),data->kolzap);
zagolov.ps_plus(strsql);

gtk_label_set_text(GTK_LABEL(data->label_kolstr),zagolov.ravno());

gtk_widget_show(data->label_kolstr);

l_uosamort_bsiz(data);

gdk_window_set_cursor(gtk_widget_get_window(data->window),gdk_cursor_new_for_display(gtk_widget_get_display(data->window),ICEB_CURSOR));

}
/*****************/
/*Создаем колонки*/
/*****************/

void l_uosamort_add_columns(GtkTreeView *treeview)
{
GtkCellRenderer *renderer;


renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Дата"), renderer,"text", COL_DATA,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Сумма"), renderer,"text", COL_SUMA,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Подразделение"), renderer,"text", COL_PODR,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Мат.отв."), renderer,"text", COL_KODMO,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Шифр затрат аморт-отчислений"), renderer,"text", COL_HZT,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Группа"), renderer,"text", COL_HNA,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Дата и время записи"), renderer,
"text", COL_DATA_VREM,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Кто записал"), renderer,
"text", COL_KTO,NULL);

}
/****************************/
/*Чтение реквизитов стороки на которую установлена подсветка строки*/
/**********************/

void l_uosamort_vibor(GtkTreeSelection *selection,class l_uosamort_data *data)
{
//printf("uosamort_vibor\n");
GtkTreeModel *model;
GtkTreeIter  iter;


if(gtk_tree_selection_get_selected(selection,&model,&iter) != TRUE)
 return;

gchar *data_v;
gint  nomer;

gtk_tree_model_get(model,&iter,COL_DATA,&data_v,NUM_COLUMNS,&nomer,-1);

data->data_v.new_plus(data_v);
data->snanomer=nomer;

g_free(data_v);

//printf("uosamort_vibor-%s %d\n",data->kodv.ravno(),data->snanomer);

}
/****************************/
/*Выбор строки по двойному клику мышкой*/
/**********************/
void l_uosamort_v_row(GtkTreeView *treeview,GtkTreePath *arg1,GtkTreeViewColumn *arg2,
class l_uosamort_data *data)
{

g_signal_emit_by_name(data->knopka[SFK2],"clicked");

}
/*****************************/
/*Удаление записи            */
/*****************************/

void l_uosamort_udzap(class l_uosamort_data *data)
{


char strsql[512];

if(iceb_pbpds(data->data_v.ravno(),data->window) != 0)
  return;

if(iceb_menu_danet(gettext("Удалить запись ? Вы уверены ?"),2,data->window) == 2)
 return;

short d1=0,m1=0,g1=0;


iceb_u_rsdat(&d1,&m1,&g1,data->data_v.ravno(),1);

//sprintf(strsql,"delete from Uosamor where innom=%d and god=%d and mes=%d",data->innom,g1,m1);
sprintf(strsql,"delete from Uosamor where innom=%d and da >= '%04d-%02d-01' and da <= '%04d-%02d-31'",data->innom,g1,m1,g1,m1);

iceb_sql_zapis(strsql,0,0,data->window);






}
/**********************************/
/*Распечатать список*/
/****************************/

void l_uosamort_rasp(class l_uosamort_data *data)
{
char strsql[512];
SQL_str row;
FILE *ff;
SQLCURSOR cur;
iceb_u_spisok imaf;
iceb_u_spisok naimot;
int kolstr=0;




if((kolstr=cur.make_cursor(&bd,data->zapros.ravno())) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  return;
 }

sprintf(strsql,"uosamort%d.lst",getpid());

imaf.plus(strsql);
naimot.plus(gettext("Амортизационные отчисления налогового учёта"));

if((ff = fopen(strsql,"w")) == NULL)
 {
  iceb_er_op_fil(strsql,"",errno,data->window);
  return;
 }

iceb_zagolov(gettext("Амортизационные отчисления налогового учёта"),ff,data->window);
fprintf(ff,"%s:%d %s\n",gettext("Инвентарный номер"),data->innom,data->naim.ravno());

fprintf(ff,"\
-----------------------------------------------------------------------------------------\n");

fprintf(ff,"\
   Дата   |  Сумма   |Подр.|Мат.от.|Шифр затрат|Группа|Дата и время запис.| Кто записал\n");
  
fprintf(ff,"\
-----------------------------------------------------------------------------------------\n");

double itogo=0.;
while(cur.read_cursor(&row) != 0)
 {
  fprintf(ff,"%s %10.2f %-5s %-7s %-*s %-*s %s %s\n",
  iceb_u_datzap(row[1]),
  atof(row[5]),
  row[2],
  row[8],
  iceb_u_kolbait(11,row[3]),
  row[3],
  iceb_u_kolbait(6,row[4]),
  row[4],
  iceb_u_vremzap(row[7]),iceb_kszap(row[6],data->window));
  
  itogo+=atof(row[5]);
 }
fprintf(ff,"\
-----------------------------------------------------------------------------------------\n");
fprintf(ff,"%*s %10.2f\n",iceb_u_kolbait(10,gettext("Итого")),gettext("Итого"),itogo);

fprintf(ff,"%s: %d\n",gettext("Количество записей"),kolstr);
iceb_podpis(ff,data->window);

fclose(ff);
iceb_ustpeh(imaf.ravno(0),3,data->window);
iceb_rabfil(&imaf,&naimot,data->window);

}
/**********************************/
/*Расчёт балансовой стоимоси и износа*/
/***************************************/
void l_uosamort_bsiz(class l_uosamort_data *data)
{
short dt=data->dr,mt=data->mr,gt=data->gr;
double bs=0.,iz=0.;
double bs1=0.,iz1=0.,iz11=0.;

if(dt == 0)
 {
  iceb_u_poltekdat(&dt,&mt,&gt);
  dt=1;
 }
FILE *ff=NULL;

if((ff = fopen(data->imafprot.ravno(),"w")) == NULL)
 {
  iceb_er_op_fil(data->imafprot.ravno(),"",errno,data->window);
  return;
 }

char strsql[1024];
if(data->innom < 0)
 { 
  bs=iz=0.;
  char bros1[512];  
  char bros[512];
  sprintf(bros1,"Стартовая балансовая стоимость для группы %d",data->innom*-1);
  memset(bros,'\0',sizeof(bros));
  
  if(iceb_poldan(bros1,bros,"uosnast.alx",data->window) != 0)
   {
    sprintf(strsql,"Не найдена %s !!",bros1);
    iceb_menu_soob(strsql,data->window);    
   }
  else
    bs=atof(bros);

  memset(bros,'\0',sizeof(bros));
  sprintf(bros1,"Стартовый износ для группы %d",data->innom*-1);
  if(iceb_poldan(bros1,bros,"uosnast.alx",data->window) != 0)
   {
    sprintf(strsql,"Не найден %s !!",bros1);
    iceb_menu_soob(strsql,data->window);    
   }
  else
    iz=atof(bros);

   
  bsiz23w(data->hna.ravno(),dt,mt,gt,&bs1,&iz1,&iz11,ff,data->window);
 }
else
 {
  class bsizw_data bal_st;
//  bsiz(in,podd,dt,mt,gt,&bs,&iz,&bs1,&iz1,&iz11,&bsby,&izby,&bs1by,&iz1by,&iz11by,ff);

  bsizw(data->innom,data->podr,dt,mt,gt,&bal_st,ff,data->window);
  bs=bal_st.sbs;
  iz=bal_st.siz;
  bs1=bal_st.bs;
  iz1=bal_st.iz;
  iz11=bal_st.iz1;

 }
fclose(ff);
iceb_ustpeh(data->imafprot.ravno(),3,data->window);

class iceb_u_str strok;

sprintf(strsql,"%s   %s",gettext("Бал.стоимость"),gettext("Износ"));
strok.plus(strsql);

sprintf(strsql,"%10.2f %10.2f",bs,iz);
strok.ps_plus(strsql);

sprintf(strsql,"%10.2f %10.2f",bs1,iz1);
strok.ps_plus(strsql);

sprintf(strsql,"%s:%9.2f",gettext("Амортизация"),iz11);
strok.ps_plus(strsql);

sprintf(strsql,"%10.2f %10.2f",bs+bs1,iz+iz1+iz11);
strok.ps_plus(strsql);

sprintf(strsql,"%*s:%9.2f",iceb_u_kolbait(11,gettext("Остаток")),gettext("Остаток"),(bs+bs1)-(iz+iz1+iz11));
strok.ps_plus(strsql);

sprintf(strsql,"%*s:%02d.%02d.%d",iceb_u_kolbait(11,gettext("Дата остатка")),gettext("Дата остатка"),dt,mt,gt);
strok.ps_plus(strsql);

gtk_label_set_text(GTK_LABEL(data->label_bsiz),strok.ravno());

}

/***************************/
/*Расчёт амортизации*/
/***************************/

void l_uosamort_ra(class l_uosamort_data *data)
{
char strsql[1024];    
char bros[512];
memset(bros,'\0',sizeof(bros));
short kvrt=0,m1=0,g1=0;
short dt,mt,gt;
iceb_u_poltekdat(&dt,&mt,&gt);    

//if(iceb_u_proverka(spgnu,data->hna.ravno(),0,1) == 0)    
if(uosprovarw(data->hna.ravno(),0,data->window) == 1)    
 {
  if(metkabo == 0)
   {
    if(iceb_menu_vvod1(gettext("Введите квартал (1,2,3,4,)"),bros,2,"",data->window) != 0)
     return;      
    printf("bros=%s\n",bros);      
    kvrt=(short)atoi(bros);
    if(kvrt < 1 || kvrt > 4)
     {
       sprintf(strsql,"%s\n%d %s",gettext("Неправильно введён квартал!"),kvrt,bros);
       iceb_menu_soob(strsql,data->window);
       return;
     } 
   }
  sprintf(bros,"%d",gt);
  if(iceb_menu_vvod1(gettext("Введите год"),bros,5,"",data->window) != 0)
   return;
       

  g1=(short)atoi(bros);
  if(g1 < 1997 || g1 > gt+1000)
   {
    iceb_menu_soob(gettext("Не правильно ввели год!"),data->window);
    return;
   } 
 }
else
 {
  sprintf(bros,"%d.%d",mt,gt);
  sprintf(strsql,"%s (%s)",gettext("Введите дату"),gettext("м.г"));

  if(iceb_menu_vvod1(strsql,bros,7,"",data->window) != 0)
   return;
    
  if(iceb_u_rsdat1(&m1,&g1,bros) != 0)
   {
    iceb_menu_soob(gettext("Не правильно введена дата!"),data->window);
    return;
   }
 }

if(kvrt == 1)
  m1=1;
if(kvrt == 2)
  m1=4;
if(kvrt == 3)
  m1=7;
if(kvrt == 4)
 m1=10;
if(metkabo == 1)
  m1=1;
  

for(int i=m1 ; i < m1+3; i++)
if(iceb_pbpds(i,g1,data->window) != 0)
  return;


FILE *ff=NULL;

if(data->innom > 0)
 {
  if((ff = fopen(data->imafprot_ao.ravno(),"w")) == NULL)
   {
    iceb_er_op_fil(data->imafprot_ao.ravno(),"",errno,data->window);
    return;
   }

  amortw(data->innom,m1,g1,1,kvrt,data->hna.ravno(),ff,data->window);

  fclose(ff);
  iceb_ustpeh(data->imafprot_ao.ravno(),3,data->window);
 }
else
 {
  if((ff = fopen(data->imafprot_ao.ravno(),"w")) == NULL)
   {
    iceb_er_op_fil(data->imafprot_ao.ravno(),"",errno,data->window);
    return;
   }
  
  amort23w(data->innom,data->podr,data->kodmo,kvrt,g1,data->hzt.ravno(),ff,data->window);
  fclose(ff);
  iceb_ustpeh(data->imafprot_ao.ravno(),3,data->window);
 } 
}
/*************************/
/*Смена даты расчёта остаточной стоимости*/
/****************************************/
void l_uosamort_sd(class l_uosamort_data *data)
{

class iceb_u_str datar;

if(iceb_menu_vvod1(gettext("Введите дату на которую расчитать остаточную стоимость (д.м.г)"),&datar,10,"",data->window) != 0)
 return;

if(datar.prov_dat() != 0)
 {
  iceb_menu_soob(gettext("Не правильно введена дата!"),data->window);
  return;
 }

iceb_u_rsdat(&data->dr,&data->mr,&data->gr,datar.ravno(),1);
/*Узнаём где числится на эту дату*/
int podraz=0,kodol=0;
if(poiinw(data->innom,data->dr,data->mr,data->gr,&podraz,&kodol,data->window) == 0)
 data->podr=podraz;
l_uosamort_bsiz(data);

}
/********************************/
/*Просмотр протоколов расчёта*/
/********************************/

void l_uosamort_ppr(class l_uosamort_data *data)
{

class iceb_u_str titl;
class iceb_u_str zagolovok;
class iceb_u_spisok punkt_m;

titl.plus(gettext("Выбор"));


zagolovok.plus(gettext("Выберите нужное"));

punkt_m.plus(gettext("Просмотр протокола расчёта балансовой стоимости и износа"));//0
punkt_m.plus(gettext("Просмотр протокола расчёта амортизации"));//1


int nomer=0;
while(nomer >= 0)
 {


  nomer=iceb_menu_mv(&titl,&zagolovok,&punkt_m,nomer,data->window);

  switch (nomer)
   {
    case -1:
      return;

    case 0:
      iceb_prosf(data->imafprot.ravno(),data->window);
      return;

    case 1:
      iceb_prosf(data->imafprot_ao.ravno(),data->window);
      return;
   }
 }
}
