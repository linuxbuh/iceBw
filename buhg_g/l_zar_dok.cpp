/*$Id: l_zar_dok.c,v 1.25 2013/09/26 09:46:51 sasa Exp $*/
/*24.11.2016	24.10.2006	Белых А.И.	l_zar_dok.c
Работа со списком документов
*/
#include        <errno.h>
#include  "buhg_g.h"
#include  "l_zar_dok.h"

enum
{
  COL_METKA_ZAP,
  COL_DATAD_SD,
  COL_DATAD,
  COL_NOMD,
  COL_SUMA,
  COL_PODR,
  COL_KOMENT,
  COL_DATA_VREM,
  COL_KTO,  
  NUM_COLUMNS
};

enum
{
  FK2,
//  FK3,
  FK4,
  FK5,
  FK10,
  SFK2,
  KOL_F_KL
};

class  l_zar_dok_data
 {
  public:

  class l_zar_dok_rek poisk;

  class iceb_u_str datad;
  class iceb_u_str datad_sd;
  class iceb_u_str nomdok;
  short prn;
    
  class iceb_u_str datad_tv;
  class iceb_u_str nomdok_tv;

  short dn,mn,gn;
  short dk,mk,gk;
  short dz,mz,gz;
  class iceb_u_str zapros;
    
  GtkWidget *label_kolstr;
  GtkWidget *label_poisk;
  GtkWidget *sw;
  GtkWidget *treeview;
  GtkWidget *window;
  GtkWidget *knopka[KOL_F_KL];
  short     kl_shift; //0-отжата 1-нажата  
  int       snanomer;   //номер записи на которую надостать
  int       kolzap;     //Количество записей
  int       metka_voz;  //0-выбрали 1-нет  
  //Конструктор
  l_zar_dok_data()
   {
    snanomer=0;
    metka_voz=kl_shift=0;
    window=treeview=NULL;
    datad_tv.new_plus("");
    nomdok_tv.new_plus("");
    dk=mk=gk=dn=mn=gn=0;
   }      
 };

gboolean   l_zar_dok_key_press(GtkWidget *widget,GdkEventKey *event,class l_zar_dok_data *data);
void l_zar_dok_vibor(GtkTreeSelection *selection,class l_zar_dok_data *data);
void l_zar_dok_v_row(GtkTreeView *treeview,GtkTreePath *arg1,GtkTreeViewColumn *arg2,
class l_zar_dok_data *data);
void  l_zar_dok_knopka(GtkWidget *widget,class l_zar_dok_data *data);
void l_zar_dok_add_columns (GtkTreeView *treeview);
int  l_zar_dok_prov_row(SQL_str row,class l_zar_dok_data *data);
void l_zar_dok_rasp(class l_zar_dok_data *data);
void l_zar_dok_create_list (class l_zar_dok_data *data);

int l_zar_dok_p(class l_zar_dok_rek *rek_poi,GtkWidget *wpredok);


extern SQL_baza  bd;
extern short	startgodz; /*Стаpтовый год для расчёта сальдо*/

int l_zar_dok(GtkWidget *wpredok)
{
l_zar_dok_data data;
char bros[512];

data.poisk.clear_data();

data.dz=data.dn=1;
data.mz=data.mn=1;
data.gn=startgodz;
if(data.gn == 0)
 {
  short d,m;
  iceb_u_poltekdat(&d,&m,&data.gn);
 }
data.gz=data.gn;

data.window = gtk_window_new (GTK_WINDOW_TOPLEVEL);

gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

//gtk_window_set_default_size (GTK_WINDOW  (data.window),400,400);


sprintf(bros,"%s %s",iceb_get_namesystem(),gettext("Список документов"));

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

g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(l_zar_dok_key_press),&data);
g_signal_connect_after(data.window,"key_release_event",G_CALLBACK(iceb_key_release),&data.kl_shift);

GtkWidget *hbox = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 1);
gtk_box_set_homogeneous (GTK_BOX(hbox),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
gtk_container_add (GTK_CONTAINER (data.window), hbox);

GtkWidget *vbox1 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 1);
gtk_box_set_homogeneous (GTK_BOX(vbox1),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
GtkWidget *vbox2 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 1);
gtk_box_set_homogeneous (GTK_BOX(vbox2),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет

gtk_box_pack_start (GTK_BOX (hbox), vbox1, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox), vbox2, TRUE, TRUE, 0);
gtk_widget_show(hbox);

data.label_kolstr=gtk_label_new (gettext("Список документов"));


gtk_box_pack_start (GTK_BOX (vbox2),data.label_kolstr,FALSE, FALSE, 0);

gtk_widget_show(vbox1);
gtk_widget_show(vbox2);

data.label_poisk=gtk_label_new ("");

gtk_box_pack_start (GTK_BOX (vbox2),data.label_poisk,FALSE, FALSE, 0);

data.sw = gtk_scrolled_window_new (NULL, NULL);

gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (data.sw),GTK_SHADOW_ETCHED_IN);
gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (data.sw),GTK_POLICY_AUTOMATIC,GTK_POLICY_AUTOMATIC);
//gtk_box_pack_start (GTK_BOX (vbox2), data.sw, TRUE, TRUE, 0);
gtk_box_pack_end (GTK_BOX (vbox2), data.sw, TRUE, TRUE, 0);

//Кнопки

sprintf(bros,"F2 %s",gettext("Запись"));
data.knopka[FK2]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK2], TRUE, TRUE, 0);
g_signal_connect(data.knopka[FK2], "clicked",G_CALLBACK(l_zar_dok_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[FK2],gettext("Ввод новой записи"));
gtk_widget_set_name(data.knopka[FK2],iceb_u_inttochar(FK2));
gtk_widget_show(data.knopka[FK2]);

sprintf(bros,"%sF2 %s",RFK,gettext("Просмотр"));
data.knopka[SFK2]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[SFK2],TRUE,TRUE, 0);
g_signal_connect(data.knopka[SFK2], "clicked",G_CALLBACK(l_zar_dok_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[SFK2],gettext("Просмотр выбранного документа"));
gtk_widget_set_name(data.knopka[SFK2],iceb_u_inttochar(SFK2));
gtk_widget_show(data.knopka[SFK2]);


sprintf(bros,"F4 %s",gettext("Поиск"));
data.knopka[FK4]=gtk_button_new_with_label(bros);
g_signal_connect(data.knopka[FK4], "clicked",G_CALLBACK(l_zar_dok_knopka),&data);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK4],TRUE,TRUE, 0);
gtk_widget_set_tooltip_text(data.knopka[FK4],gettext("Поиск нужных записей"));
gtk_widget_set_name(data.knopka[FK4],iceb_u_inttochar(FK4));
gtk_widget_show(data.knopka[FK4]);

sprintf(bros,"F5 %s",gettext("Печать"));
data.knopka[FK5]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK5],TRUE,TRUE, 0);
g_signal_connect(data.knopka[FK5], "clicked",G_CALLBACK(l_zar_dok_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[FK5],gettext("Распечатка записей"));
gtk_widget_set_name(data.knopka[FK5],iceb_u_inttochar(FK5));
gtk_widget_show(data.knopka[FK5]);

sprintf(bros,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1),data.knopka[FK10],TRUE,TRUE, 0);
gtk_widget_set_tooltip_text(data.knopka[FK10],gettext("Завершение работы в этом окне"));
g_signal_connect(data.knopka[FK10], "clicked",G_CALLBACK(l_zar_dok_knopka),&data);
gtk_widget_set_name(data.knopka[FK10],iceb_u_inttochar(FK10));
gtk_widget_show(data.knopka[FK10]);


gtk_widget_realize(data.window);
gdk_window_set_cursor(gtk_widget_get_window(data.window),gdk_cursor_new_for_display(gtk_widget_get_display(data.window),ICEB_CURSOR));

gtk_widget_grab_focus(data.knopka[FK10]);

l_zar_dok_create_list(&data);

gtk_widget_show(data.window);

gtk_window_maximize(GTK_WINDOW(data.window));
//gtk_window_fullscreen(GTK_WINDOW(data.window));


gtk_main();


if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));

return(data.metka_voz);

}


/***********************************/
/*Создаем список для просмотра */
/***********************************/
void l_zar_dok_create_list (class l_zar_dok_data *data)
{
iceb_clock sss(data->window);
GtkListStore *model=NULL;
GtkTreeIter iter;
SQLCURSOR cur,cur1;
char strsql[1024];
int  kolstr=0;
SQL_str row,row1;

data->kl_shift=0; //0-отжата 1-нажата  


if(data->treeview != NULL)
  gtk_widget_destroy(data->treeview);

data->treeview = gtk_tree_view_new();


gtk_container_add (GTK_CONTAINER (data->sw), data->treeview);

g_signal_connect(data->treeview,"row_activated",G_CALLBACK(l_zar_dok_v_row),data);

GtkTreeSelection *selection=gtk_tree_view_get_selection(GTK_TREE_VIEW(data->treeview));
gtk_tree_selection_set_mode(selection,GTK_SELECTION_SINGLE);
g_signal_connect(selection,"changed",G_CALLBACK(l_zar_dok_vibor),data);

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
G_TYPE_STRING, 
G_TYPE_INT);

memset(strsql,'\0',sizeof(strsql));
if(data->dk != 0)
  sprintf(strsql,"select * from Zardok where (datsd >='%04d-%02d-%02d' and datsd <= '%04d-%02d-%02d')",data->gn,data->mn,data->dn,data->gk,data->mk,data->dk);
else
  sprintf(strsql,"select * from Zardok where datsd >='%04d-%02d-%02d'",data->gn,data->mn,data->dn);

data->zapros.new_plus(strsql);
memset(strsql,'\0',sizeof(strsql));


if(data->poisk.metka_poi == 1)
 {
  if(data->poisk.datak.getdlinna() > 1)
   {
    sprintf(strsql," and (datd >='%s' and datd <= '%s')",data->poisk.datan.ravno_sqldata(),data->poisk.datak.ravno_sqldata());
    data->zapros.plus(strsql);
   }
  else
   {
    if(data->poisk.datan.getdlinna() > 1)
     sprintf(strsql," and datd >='%s'",data->poisk.datan.ravno_sqldata());
    data->zapros.plus(strsql);
   }
 }
 
data->zapros.plus(" order by datsd,nomd desc");

if((kolstr=cur.make_cursor(&bd,data->zapros.ravno())) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  return;
 }
//gtk_list_store_clear(model);

iceb_u_str ss[NUM_COLUMNS];

data->kolzap=0;
double suma_dok=0.;
while(cur.read_cursor(&row) != 0)
 {
  /*printf("%s %s %s %s %s\n",row[0],row[1],row[2],row[3],row[7]);*/
  
  if(l_zar_dok_prov_row(row,data) != 0)
    continue;


  if(iceb_u_SRAV(data->nomdok_tv.ravno(),row[2],0) == 0 && iceb_u_SRAV(data->datad.ravno(),iceb_u_datzap(row[1]),0) == 0)
    data->snanomer=data->kolzap;

  suma_dok=0.;
  //Узнаём сумму по документа
  sprintf(strsql,"select SUM(suma) from Zarp where datz='%s' and nd='%s'",row[1],row[2]);
  
  if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) == 1)
   {
    if(row1[0] != NULL)
     suma_dok=atof(row1[0]);
   }
  ss[COL_SUMA].new_plus(suma_dok);

  //Номер документа
  ss[COL_NOMD].new_plus(row[2]);

  //Подразделение
  ss[COL_PODR].new_plus(row[3]);
  sprintf(strsql,"select naik from Podr where kod=%s",row[3]);
  if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) == 1)
   {
    ss[COL_PODR].plus(" ");
    ss[COL_PODR].plus(row1[0]);
   }

  /*Метка прихода/расхода*/
  if(row[7][0] == '2')
    ss[COL_METKA_ZAP].new_plus("-");
  else
    ss[COL_METKA_ZAP].new_plus("+");
  /*дата создания документа*/
  ss[COL_DATAD_SD].new_plus(iceb_u_datzap(row[8]));
  
  gtk_list_store_append (model, &iter);

  gtk_list_store_set (model, &iter,
  COL_DATAD_SD,ss[COL_DATAD_SD].ravno(),
  COL_DATAD,iceb_u_datzap(row[1]),
  COL_NOMD,row[2],
  COL_PODR,ss[COL_PODR].ravno(),
  COL_KOMENT,row[4],
  COL_DATA_VREM,iceb_u_vremzap(row[6]),
  COL_KTO,iceb_kszap(row[5],data->window),
  COL_METKA_ZAP,ss[COL_METKA_ZAP].ravno(),
  COL_SUMA,ss[COL_SUMA].ravno(),
  NUM_COLUMNS,data->kolzap,
  -1);

  data->kolzap++;
 }
data->datad_tv.new_plus("");
data->nomdok_tv.new_plus("");

gtk_tree_view_set_model (GTK_TREE_VIEW(data-> treeview),GTK_TREE_MODEL (model));

g_object_unref (GTK_TREE_MODEL (model));

l_zar_dok_add_columns (GTK_TREE_VIEW (data->treeview));


if(data->kolzap == 0)
 {
//  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK3]),FALSE);//Недоступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK5]),FALSE);//Недоступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[SFK2]),FALSE);//Недоступна
 }
else
 {
//  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK3]),TRUE);//Доступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK5]),TRUE);//Доступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[SFK2]),TRUE);//Доступна
 }

gtk_widget_show (data->treeview);
gtk_widget_show (data->sw);

//Стать подсветкой стороки на нужный номер строки
iceb_snanomer(data->kolzap,&data->snanomer,data->treeview);


iceb_u_str stroka;
iceb_u_str zagolov;
zagolov.plus(gettext("Список документов"));
zagolov.plus(" ",gettext("Стартовый год"));
zagolov.plus(" ");
zagolov.plus(startgodz);

sprintf(strsql," %s:%d",gettext("Количество записей"),data->kolzap);
zagolov.plus(strsql);

gtk_label_set_text(GTK_LABEL(data->label_kolstr),zagolov.ravno());

if(data->poisk.metka_poi == 1)
 {
  
  zagolov.new_plus(gettext("Поиск"));
  zagolov.plus(" !!!");

  iceb_str_poisk(&zagolov,data->poisk.datan_sd.ravno(),gettext("Дата начала (документа)"));
  iceb_str_poisk(&zagolov,data->poisk.datak_sd.ravno(),gettext("Дата конца (документа)"));
  iceb_str_poisk(&zagolov,data->poisk.datan.ravno(),gettext("Дата начала (начисления)"));
  iceb_str_poisk(&zagolov,data->poisk.datak.ravno(),gettext("Дата конца (начисления)"));
  iceb_str_poisk(&zagolov,data->poisk.podr.ravno(),gettext("Подразделение"));
  iceb_str_poisk(&zagolov,data->poisk.koment.ravno(),gettext("Коментарий"));
  iceb_str_poisk(&zagolov,data->poisk.nomdok.ravno(),gettext("Номер документа"));
  

  iceb_label_set_text_color(data->label_poisk,zagolov.ravno(),"red");
  gtk_widget_show(data->label_poisk);
 }
else
 gtk_widget_hide(data->label_poisk); 

gtk_widget_show(data->label_kolstr);

}

/*****************/
/*Создаем колонки*/
/*****************/

void l_zar_dok_add_columns(GtkTreeView *treeview)
{
GtkCellRenderer *renderer;


renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,"M", renderer,"text", COL_METKA_ZAP,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Дата док-та"), renderer,"text", COL_DATAD_SD,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Дата начисления"), renderer,"text", COL_DATAD,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Ном.док."), renderer,"text", COL_NOMD,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Сумма"), renderer,"text", COL_SUMA,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Подразделение"), renderer,"text", COL_PODR,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Коментарий"), renderer,"text", COL_KOMENT,NULL);

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
/*Выбор строки*/
/**********************/

void l_zar_dok_vibor(GtkTreeSelection *selection,class l_zar_dok_data *data)
{
GtkTreeModel *model;
GtkTreeIter  iter;


if(gtk_tree_selection_get_selected(selection,&model,&iter) != TRUE)
 return;

gchar *datd;
gchar *datd_sd;
gchar *nomd;
gint  nomer;
gchar *metka_zap;

gtk_tree_model_get(model,&iter,
COL_DATAD,&datd,
COL_DATAD_SD,&datd_sd,
COL_NOMD,&nomd,
COL_METKA_ZAP,&metka_zap,
NUM_COLUMNS,&nomer,-1);

data->datad.new_plus(datd);
data->datad_sd.new_plus(datd_sd);
data->nomdok.new_plus(nomd);
data->snanomer=nomer;

if(metka_zap[0] == '+')
 data->prn=1;
else
 data->prn=2;
 
g_free(datd);
g_free(nomd);
g_free(metka_zap);


}

/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  l_zar_dok_knopka(GtkWidget *widget,class l_zar_dok_data *data)
{
iceb_u_str repl;

int knop=atoi(gtk_widget_get_name(widget));
//g_print("l_zar_dok_knopka knop=%d\n",knop);
data->kl_shift=0;
switch (knop)
 {
  case FK2:
    if((data->prn=l_zar_dok_prn(data->window)) < 1)
      return;
    
    data->datad_tv.new_plus("");
    data->nomdok_tv.new_plus("");
    if(l_zar_dok_hap(&data->datad_tv,&data->nomdok_tv,data->prn,data->window) == 0)
      l_zar_dok_create_list(data);
    return;  

  case SFK2:
    if(data->kolzap == 0)
      return;
    l_zar_dok_zap(data->prn,data->datad_sd.ravno_god(),data->datad.ravno(),data->nomdok.ravno(),data->window);

    l_zar_dok_create_list(data);
    return;  
/****************
  case FK3:
    if(data->kolzap == 0)
      return;
    if(iceb_menu_danet(gettext("Удалить запись ? Вы уверены ?"),2,data->window) != 1)
     return;
    l_zar_dok_udzap(data);
    l_zar_dok_create_list(data);
    return;  
  
****************/

  case FK4:
    if(l_zar_dok_p(&data->poisk,data->window)  == 0)
     {
      if(data->poisk.datan_sd.getdlinna() > 1)
       iceb_u_rsdat(&data->dn,&data->mn,&data->gn,data->poisk.datan_sd.ravno(),1);
      if(data->poisk.datak_sd.getdlinna() > 1)
       iceb_u_rsdat(&data->dk,&data->mk,&data->gk,data->poisk.datak_sd.ravno(),1);
     }
    else
     {
      data->dn=data->dz;
      data->mn=data->mz;
      data->gn=data->gz;
      data->dk=data->mk=data->gk=0;
     }
    l_zar_dok_create_list(data);

    return;  

  case FK5:
    l_zar_dok_rasp(data);
    return;  

    
  case FK10:
    data->metka_voz=1;
    gtk_widget_destroy(data->window);
    return;
 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   l_zar_dok_key_press(GtkWidget *widget,GdkEventKey *event,class l_zar_dok_data *data)
{
iceb_u_str repl;

switch(event->keyval)
 {

  case GDK_KEY_F2:

    if(data->kl_shift == 0)
      g_signal_emit_by_name(data->knopka[FK2],"clicked");
    else
      g_signal_emit_by_name(data->knopka[SFK2],"clicked");

    return(TRUE);
/*****   
  case GDK_KEY_F3:
      g_signal_emit_by_name(data->knopka[FK3],"clicked");
    return(TRUE);
************/
  case GDK_KEY_F4:
    g_signal_emit_by_name(data->knopka[FK4],"clicked");
    return(TRUE);

  case GDK_KEY_F5:
    g_signal_emit_by_name(data->knopka[FK5],"clicked");
    return(TRUE);

  
  case GDK_KEY_Escape:
  case GDK_KEY_F10:
    g_signal_emit_by_name(data->knopka[FK10],"clicked");
    return(FALSE);

  case ICEB_REG_L:
  case ICEB_REG_R:

    data->kl_shift=1;

    return(TRUE);

  default:
    printf("Не выбрана клавиша !\n");
    break;
 }

return(TRUE);
}
/****************************/
/*Выбор строки*/
/**********************/
void l_zar_dok_v_row(GtkTreeView *treeview,GtkTreePath *arg1,GtkTreeViewColumn *arg2,
class l_zar_dok_data *data)
{
g_signal_emit_by_name(data->knopka[SFK2],"clicked");
}

/****************************/
/*Проверка записей          */
/*****************************/

int  l_zar_dok_prov_row(SQL_str row,class l_zar_dok_data *data)
{
if(data->poisk.metka_poi == 0)
 return(0);

if(iceb_u_proverka(data->poisk.podr.ravno(),row[3],0,0) != 0)
 return(1);

if(iceb_u_proverka(data->poisk.nomdok.ravno(),row[2],4,0) != 0)
 return(1);

if(iceb_u_proverka(data->poisk.koment.ravno(),row[4],4,0) != 0)
 return(1);

   
return(0);
}
/*************************************/
/*Распечатка записей                 */
/*************************************/
void l_zar_dok_rasp(class l_zar_dok_data *data)
{
char strsql[512];
SQL_str row,row1;
FILE *ff;
SQLCURSOR cur,cur1;
iceb_u_spisok imaf;
iceb_u_spisok naimot;
int kolstr=0;


if((kolstr=cur.make_cursor(&bd,data->zapros.ravno())) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),data->zapros.ravno(),data->window);
  return;
 }

sprintf(strsql,"grupp%d.lst",getpid());

imaf.plus(strsql);
naimot.plus(gettext("Список документов"));

if((ff = fopen(strsql,"w")) == NULL)
 {
  iceb_er_op_fil(strsql,"",errno,data->window);
  return;
 }

iceb_zagolov(gettext("Список документов"),data->dn,data->mn,data->gn,data->dk,data->mk,data->gk,ff,data->window);

if(data->poisk.metka_poi == 1)
 {

  if(data->poisk.podr.getdlinna() > 1)
   fprintf(ff,"%s:%s\n",gettext("Подразделение"),data->poisk.podr.ravno());
  if(data->poisk.koment.getdlinna() > 1)
   fprintf(ff,"%s:%s\n",gettext("Коментарий"),data->poisk.koment.ravno());

 }

fprintf(ff,"\
-----------------------------------------------------------------------------------------------------\n");

fprintf(ff,gettext("\
Дата докум|Дата начи.|Номер д.|   Сумма    |   Подразделение    |Дата и время запис.| Кто записал\n"));
/*
1234567890 12345678 12345678901234567890
*/
fprintf(ff,"\
-----------------------------------------------------------------------------------------------------\n");

class iceb_u_str naim("");
int kolzap=0;
double suma_dok=0.;
double itogo=0.;
while(cur.read_cursor(&row) != 0)
 {
  if(l_zar_dok_prov_row(row,data) != 0)
    continue;
  suma_dok=0.;
  //Узнаём сумму по документа
  sprintf(strsql,"select SUM(suma) from Zarp where datz='%s' and nd='%s'",row[1],row[2]);
  
  if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) == 1)
   {
    if(row1[0] != NULL)
     suma_dok=atof(row1[0]);
   }
  itogo+=suma_dok;  
  naim.new_plus(row[3]);

  sprintf(strsql,"select naik from Podr where kod=%s",row[3]);
  if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) == 1)
   {
    naim.plus(" ",row1[0]);
   }
  class iceb_u_str datasd(iceb_u_datzap(row[8]));
  fprintf(ff,"%s %10s %-*s %12.2f %-*.*s %s %s\n",
  datasd.ravno(),
  iceb_u_datzap(row[1]),
  iceb_u_kolbait(8,row[2]),row[2],
  suma_dok,
  iceb_u_kolbait(20,naim.ravno()),iceb_u_kolbait(20,naim.ravno()),naim.ravno(),
  iceb_u_vremzap(row[6]),iceb_kszap(row[5],data->window));
  kolzap++;
 }
fprintf(ff,"\
-----------------------------------------------------------------------------------------------------\n");
fprintf(ff,"%*s %12.2f\n",
iceb_u_kolbait(31,gettext("Итого")),
gettext("Итого"),
itogo);

fprintf(ff,"%s: %d\n",gettext("Количество записей"),kolzap);
iceb_podpis(ff,data->window);

fclose(ff);
iceb_ustpeh(imaf.ravno(0),3,data->window);
iceb_rabfil(&imaf,&naimot,data->window);

}
/****************************/
/*Выбор прихода/расхода*/
/*************************/
int l_zar_dok_prn(GtkWidget *wpredok)
{
class iceb_u_str titl;
class iceb_u_str zagolovok;
class iceb_u_spisok punkt_m;

titl.plus(gettext("Выбор"));


zagolovok.plus(gettext("Выберите нужное"));


punkt_m.plus(gettext("Ввод начислений"));//0
punkt_m.plus(gettext("Ввод удержаний"));//1


int nomer=0;
return(iceb_menu_mv(&titl,&zagolovok,&punkt_m,nomer,wpredok)+1);


}
