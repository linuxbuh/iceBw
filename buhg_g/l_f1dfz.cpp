/*$Id: l_f1dfz.c,v 1.31 2013/12/31 11:49:12 sasa Exp $*/
/*11.07.2015	22.12.2006	Белых А.И.	l_f1dfz.c
Работа с записями в документе формы 1ДФ
Возвращаем 0-вышли из документа
           1-удалили документ
*/
#include  "buhg_g.h"
#include "l_f1dfz.h"

enum
{
  COL_INN,
  COL_FIO,
  COL_NAH_DOH,
  COL_NAH_NAL,
  COL_VIP_DOH,
  COL_VIP_NAL,
  COL_OZ_DOH,
  COL_DATA_PNR,
  COL_DATA_USR,
  COL_LGOTA,
  COL_VID_DOK,
  COL_DATA_VREM,
  COL_KTO,  
  NUM_COLUMNS
};

enum
{
  SFK1,
  FK2,
  SFK2,
  FK3,
  SFK3,
  FK4,
  FK5,
  FK6,
  FK7,
  FK8,
  FK10,
  KOL_F_KL
};

class  l_f1dfz_data
 {
  public:

  class iceb_u_str nomd; //номер документа с которым работаем
  int metka_oth; //0-отчётный 1-новый отчётный 2-уточняющий
  int kvrt; //номер квартала
    
  //реквизиты выбранной строки
  class iceb_u_str inn_v;
  class iceb_u_str priz_v;
  class iceb_u_str lgota_v;
  class iceb_u_str pr_v;
  //Реквизиты только что введённой строки
  class iceb_u_str inn_tv;
  class iceb_u_str priz_tv;
  class iceb_u_str lgota_tv;
  class iceb_u_str pr_tv;

  
  GtkWidget *label_kolstr;
  GtkWidget *label_poisk;
  GtkWidget *sw;
  GtkWidget *treeview;
  GtkWidget *window;
  GtkWidget *knopka[KOL_F_KL];
  short     kl_shift; //0-отжата 1-нажата  
  int       snanomer;   //номер записи на которую надостать
  int       kolzap;     //Количество записей
  int voz;

  class l_f1dfz_rek rk;

  //Конструктор
  l_f1dfz_data()
   {
    voz=0;
    snanomer=0;
    kl_shift=0;
    window=treeview=NULL;
    clear_tv();
   }      
  void clear_tv()
   {
    inn_tv.new_plus("");
    priz_tv.new_plus("");
    lgota_tv.new_plus("");
    pr_tv.new_plus("");
   }
 };

gboolean   l_f1dfz_key_press(GtkWidget *widget,GdkEventKey *event,class l_f1dfz_data *data);
void l_f1dfz_vibor(GtkTreeSelection *selection,class l_f1dfz_data *data);
void l_f1dfz_v_row(GtkTreeView *treeview,GtkTreePath *arg1,GtkTreeViewColumn *arg2,
class l_f1dfz_data *data);
void  l_f1dfz_knopka(GtkWidget *widget,class l_f1dfz_data *data);
void l_f1dfz_add_columns (GtkTreeView *treeview);
void l_f1dfz_create_list (class l_f1dfz_data *data);

int l_f1dfz_v(const char *nomdok,class iceb_u_str *inn,class iceb_u_str *priz,class iceb_u_str *lgota,class iceb_u_str *pr,GtkWidget *wpredok);
int l_f1dfz1_v(const char *nomdok,class iceb_u_str *inn,class iceb_u_str *priz,class iceb_u_str *lgota,class iceb_u_str *pr,GtkWidget *wpredok);
int l_f1dfz_p(class l_f1dfz_rek *rek,GtkWidget *wpredok);
int l_f1dfz_prov(SQL_str row,class l_f1dfz_rek *rk,GtkWidget *wpredok);
void l_f1dfz_read_sh(class l_f1dfz_data *data);
void l_f1dfz_fk6(class l_f1dfz_data *data);
int   l_f1dfvs(const char *nomd,GtkWidget *wpredok);

int rasf1df(const char *nomd,int metka_oth,class iceb_u_spisok*,class iceb_u_spisok*,GtkWidget *wpredok);
void zagrf1df(const char *nomd,int kolst,int kvrt,GtkWidget *wpredok);
int zagf1df_csvw(const char *nomd,int kolst,GtkWidget *wpredok);

extern SQL_baza  bd;

int  l_f1dfz(const char *nomd,GtkWidget *wpredok)
{
class l_f1dfz_data data;
char strsql[512];
data.nomd.new_plus(nomd);



data.window = gtk_window_new (GTK_WINDOW_TOPLEVEL);

gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

//gtk_window_set_default_size (GTK_WINDOW  (data.window),400,400);


sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Форма 1ДФ"));

gtk_window_set_title (GTK_WINDOW (data.window),strsql);
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

g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(l_f1dfz_key_press),&data);
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

data.label_kolstr=gtk_label_new (gettext("Форма 1ДФ"));


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


sprintf(strsql,"%sF1 %s",RFK,gettext("Шапка"));
data.knopka[SFK1]=gtk_button_new_with_label(strsql);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[SFK1], TRUE, TRUE, 0);
g_signal_connect(data.knopka[SFK1], "clicked",G_CALLBACK(l_f1dfz_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[SFK1],gettext("Просмотр шапки документа"));
gtk_widget_set_name(data.knopka[SFK1],iceb_u_inttochar(SFK1));
gtk_widget_show(data.knopka[SFK1]);

sprintf(strsql,"F2 %s",gettext("Запись"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK2], TRUE, TRUE, 0);
g_signal_connect(data.knopka[FK2], "clicked",G_CALLBACK(l_f1dfz_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[FK2],gettext("Ввод новой записи для работника вашой организации"));
gtk_widget_set_name(data.knopka[FK2],iceb_u_inttochar(FK2));
gtk_widget_show(data.knopka[FK2]);

sprintf(strsql,"%sF2 %s",RFK,gettext("Корректировать"));
data.knopka[SFK2]=gtk_button_new_with_label(strsql);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[SFK2], TRUE, TRUE, 0);
g_signal_connect(data.knopka[SFK2], "clicked",G_CALLBACK(l_f1dfz_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[SFK2],gettext("Корректировка выбранной записи"));
gtk_widget_set_name(data.knopka[SFK2],iceb_u_inttochar(SFK2));
gtk_widget_show(data.knopka[SFK2]);


sprintf(strsql,"F3 %s",gettext("Удалить"));
data.knopka[FK3]=gtk_button_new_with_label(strsql);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK3],TRUE,TRUE, 0);
g_signal_connect(data.knopka[FK3], "clicked",G_CALLBACK(l_f1dfz_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[FK3],gettext("Удаление выбранной записи"));
gtk_widget_set_name(data.knopka[FK3],iceb_u_inttochar(FK3));
gtk_widget_show(data.knopka[FK3]);

sprintf(strsql,"%sF3 %s",RFK,gettext("Удаление"));
data.knopka[SFK3]=gtk_button_new_with_label(strsql);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[SFK3], TRUE, TRUE, 0);
g_signal_connect(data.knopka[SFK3], "clicked",G_CALLBACK(l_f1dfz_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[SFK3],gettext("Удаление документа"));
gtk_widget_set_name(data.knopka[SFK3],iceb_u_inttochar(SFK3));
gtk_widget_show(data.knopka[SFK3]);

sprintf(strsql,"F4 %s",gettext("Поиск"));
data.knopka[FK4]=gtk_button_new_with_label(strsql);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK4],TRUE,TRUE, 0);
g_signal_connect(data.knopka[FK4], "clicked",G_CALLBACK(l_f1dfz_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[FK4],gettext("Поиск нужных записей"));
gtk_widget_set_name(data.knopka[FK4],iceb_u_inttochar(FK4));
gtk_widget_show(data.knopka[FK4]);

sprintf(strsql,"F5 %s",gettext("Печать"));
data.knopka[FK5]=gtk_button_new_with_label(strsql);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK5],TRUE,TRUE, 0);
g_signal_connect(data.knopka[FK5], "clicked",G_CALLBACK(l_f1dfz_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[FK5],gettext("Распечатка формы 1ДФ и получение файлов электронной отчётности"));
gtk_widget_set_name(data.knopka[FK5],iceb_u_inttochar(FK5));
gtk_widget_show(data.knopka[FK5]);

sprintf(strsql,"F6 %s",gettext("Загрузка"));
data.knopka[FK6]=gtk_button_new_with_label(strsql);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK6],TRUE,TRUE, 0);
g_signal_connect(data.knopka[FK6], "clicked",G_CALLBACK(l_f1dfz_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[FK6],gettext("Загрузка формы 1ДФ из файла"));
gtk_widget_set_name(data.knopka[FK6],iceb_u_inttochar(FK6));
gtk_widget_show(data.knopka[FK6]);


sprintf(strsql,"F7 %s",gettext("Военный сбор"));
data.knopka[FK7]=gtk_button_new_with_label(strsql);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK7],TRUE,TRUE, 0);
g_signal_connect(data.knopka[FK7], "clicked",G_CALLBACK(l_f1dfz_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[FK7],gettext("Переход в режим работы с записями военного сбора"));
gtk_widget_set_name(data.knopka[FK7],iceb_u_inttochar(FK7));
gtk_widget_show(data.knopka[FK7]);

sprintf(strsql,"F8 %s",gettext("Запись"));
data.knopka[FK8]=gtk_button_new_with_label(strsql);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK8],TRUE,TRUE, 0);
g_signal_connect(data.knopka[FK8], "clicked",G_CALLBACK(l_f1dfz_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[FK8],gettext("Ввод новой записи для постороннего человека"));
gtk_widget_set_name(data.knopka[FK8],iceb_u_inttochar(FK8));
gtk_widget_show(data.knopka[FK8]);

sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
gtk_box_pack_start(GTK_BOX(vbox1),data.knopka[FK10],TRUE,TRUE, 0);
gtk_widget_set_tooltip_text(data.knopka[FK10],gettext("Завершение работы в этом окне"));
g_signal_connect(data.knopka[FK10], "clicked",G_CALLBACK(l_f1dfz_knopka),&data);
gtk_widget_set_name(data.knopka[FK10],iceb_u_inttochar(FK10));
gtk_widget_show(data.knopka[FK10]);


gtk_widget_realize(data.window);
gdk_window_set_cursor(gtk_widget_get_window(data.window),gdk_cursor_new_for_display(gtk_widget_get_display(data.window),ICEB_CURSOR));

//gtk_widget_grab_focus(data.knopka[FK10]);

l_f1dfz_read_sh(&data); //перед чтением
l_f1dfz_create_list(&data);

gtk_widget_show(data.window);
//if(metka_rr == 0)
  gtk_window_maximize(GTK_WINDOW(data.window));
//gtk_window_fullscreen(GTK_WINDOW(data.window));


gtk_main();


if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));

return(data.voz);
}


/***********************************/
/*Создаем список для просмотра */
/***********************************/
void l_f1dfz_create_list (class l_f1dfz_data *data)
{
class iceb_gdite_data gdite;
iceb_gdite(&gdite,0,data->window);

iceb_clock sss(data->window);
GtkListStore *model=NULL;
GtkTreeIter iter;
class SQLCURSOR cur,cur1;
char strsql[512];
int  kolstr=0;
SQL_str row,row1;

data->kl_shift=0; //0-отжата 1-нажата  


if(data->treeview != NULL)
  gtk_widget_destroy(data->treeview);

data->treeview = gtk_tree_view_new();


gtk_container_add (GTK_CONTAINER (data->sw), data->treeview);

g_signal_connect(data->treeview,"row_activated",G_CALLBACK(l_f1dfz_v_row),data);

GtkTreeSelection *selection=gtk_tree_view_get_selection(GTK_TREE_VIEW(data->treeview));
gtk_tree_selection_set_mode(selection,GTK_SELECTION_SINGLE);
g_signal_connect(selection,"changed",G_CALLBACK(l_f1dfz_vibor),data);

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
G_TYPE_STRING, 
G_TYPE_STRING, 
G_TYPE_STRING, 
G_TYPE_STRING, 
G_TYPE_INT);


sprintf(strsql,"select * from F8dr1 where nomd='%s' order by inn asc,pr desc",data->nomd.ravno());

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  return;
 }
//gtk_list_store_clear(model);

iceb_u_str ss[NUM_COLUMNS];

data->kolzap=0;
float kolstr1=0.;
double suma_vip_doh=0.;
double suma_vip_nal=0.;
double suma_nah_doh=0.;
double suma_nah_nal=0.;
class iceb_u_spisok fiz_lico;

while(cur.read_cursor(&row) != 0)
 {
//  printf("%s %s %s %s\n",row[0],row[1],row[2],row[3]);
  iceb_pbar(gdite.bar,kolstr,++kolstr1);
  if(l_f1dfz_prov(row,&data->rk,data->window) != 0)
   continue;
  

  if(iceb_u_SRAV(data->inn_tv.ravno(),row[1],0) == 0 && \
     iceb_u_SRAV(data->priz_tv.ravno(),row[4],0) == 0 && \
     iceb_u_SRAV(data->lgota_tv.ravno(),row[7],0) == 0 && \
     iceb_u_SRAV(data->pr_tv.ravno(),row[13],0) == 0 )
        data->snanomer=data->kolzap;

  //Идентификационный номер
  ss[COL_INN].new_plus(row[1]);
  if(fiz_lico.find(row[1]) < 0)
   fiz_lico.plus(row[1]);
   
  //Фамилия
  ss[COL_FIO].new_plus(row[10]);
  if(ss[COL_FIO].getdlinna() <= 1)
   {
    sprintf(strsql,"select fio from Kartb where inn='%s'",row[1]);
    if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) == 1)
      ss[COL_FIO].new_plus(row1[0]);
   }
   
  //Сумма выплаченного дохода
//  ss[COL_VIP_DOH].new_plus(row[2]);
  suma_vip_doh+=atof(row[2]);
  
  //Сумма выплаченного НАЛОГА
//  ss[COL_VIP_NAL].new_plus(row[3]);
  suma_vip_nal+=atof(row[3]);
  
  //Сумма начисленного дохода
//  ss[COL_NAH_DOH].new_plus(row[11]);
  suma_nah_doh+=atof(row[11]);
  
  //Сумма начисленного НАЛОГА
  suma_nah_nal+=atof(row[12]);
  

  //Дата приёма на работу
   if(row[5][0] != '0')
     ss[COL_DATA_PNR].new_plus(iceb_u_datzap(row[5]));
   else
     ss[COL_DATA_PNR].new_plus("");
  //Дата увольнения с работы
   if(row[6][0] != '0')
     ss[COL_DATA_USR].new_plus(iceb_u_datzap(row[6]));
   else
     ss[COL_DATA_USR].new_plus("");
   

  

  
  gtk_list_store_append (model, &iter);

  gtk_list_store_set (model, &iter,
  COL_INN,row[1],
  COL_FIO,row[10],
  COL_VIP_DOH,row[2],
  COL_VIP_NAL,row[3],
  COL_NAH_DOH,row[11],
  COL_NAH_NAL,row[12],
  COL_OZ_DOH,row[4],
  COL_DATA_PNR,ss[COL_DATA_PNR].ravno(),
  COL_DATA_USR,ss[COL_DATA_USR].ravno(),
  COL_LGOTA,row[7],
  COL_VID_DOK,row[13],
  COL_DATA_VREM,iceb_u_vremzap(row[9]),
  COL_KTO,iceb_kszap(row[8],data->window),
  NUM_COLUMNS,data->kolzap,
  -1);

  data->kolzap++;
 }
data->clear_tv();
gtk_tree_view_set_model (GTK_TREE_VIEW(data-> treeview),GTK_TREE_MODEL (model));

g_object_unref (GTK_TREE_MODEL (model));

l_f1dfz_add_columns (GTK_TREE_VIEW (data->treeview));


if(data->kolzap == 0)
 {
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK3]),FALSE);//Недоступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK5]),FALSE);//Недоступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[SFK2]),FALSE);//Недоступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[SFK3]),FALSE);//Недоступна
 }
else
 {
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK3]),TRUE);//Доступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK5]),TRUE);//Доступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[SFK2]),TRUE);//Доступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[SFK3]),TRUE);//Доступна
 }

gtk_widget_show (data->treeview);
gtk_widget_show (data->sw);

//Стать подсветкой стороки на нужный номер строки
iceb_snanomer(data->kolzap,&data->snanomer,data->treeview);


iceb_u_str stroka;
iceb_u_str zagolov;
zagolov.plus(gettext("Форма 1ДФ"));
zagolov.plus(" ");
zagolov.plus(gettext("Номер документа"));
zagolov.plus(":");
zagolov.plus(data->nomd.ravno());
zagolov.plus(" ");
if(data->metka_oth == 0)
  zagolov.plus(gettext("Отчётный"));
if(data->metka_oth == 1)
  zagolov.plus(gettext("Новый отчётный"));
if(data->metka_oth == 2)
  zagolov.plus(gettext("Уточняющий"));

sprintf(strsql,"\n%s:%d %s:%d",
gettext("Количество записей"),data->kolzap,
gettext("Количество физических лиц"),fiz_lico.kolih());

zagolov.plus(strsql);

zagolov.ps_plus(gettext("Сумма выплаченного дохода"));
zagolov.plus(":");
zagolov.plus(suma_vip_doh);

zagolov.plus(" ");
zagolov.plus(gettext("Сумма выплаченного налога"));
zagolov.plus(":");
zagolov.plus(suma_vip_nal);

zagolov.ps_plus(gettext("Сумма начисленного дохода"));
zagolov.plus(":");
zagolov.plus(suma_nah_doh);

zagolov.plus(" ");
zagolov.plus(gettext("Сумма начисленного налога"));
zagolov.plus(":");
zagolov.plus(suma_nah_nal);

gtk_label_set_text(GTK_LABEL(data->label_kolstr),zagolov.ravno());

if(data->rk.metka_poi == 0)
 {
  printf("Формирование заголовка с реквизитами поиска.\n");
  
  zagolov.new_plus(gettext("Поиск"));
  zagolov.plus(" !!!");

  iceb_str_poisk(&zagolov,data->rk.fio.ravno(),gettext("Фамилия"));
  iceb_str_poisk(&zagolov,data->rk.inn.ravno(),gettext("Инд.нал.номер"));
  iceb_str_poisk(&zagolov,data->rk.kod_doh.ravno(),gettext("Код дохода"));

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

void l_f1dfz_add_columns(GtkTreeView *treeview)
{
GtkCellRenderer *renderer;
//GtkTreeModel *model = gtk_tree_view_get_model (treeview);
//GdkColor color;


renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Иден.номер"), renderer,"text", COL_INN,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Фамилия Имя Отчество"), renderer,"text", COL_FIO,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Вып-ный доход"), renderer,"text", COL_VIP_DOH,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Вып-ный налог"), renderer,"text", COL_VIP_NAL,NULL);


renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Нач-ный доход"), renderer,"text", COL_NAH_DOH,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Нач-ный налог"), renderer,"text", COL_NAH_NAL,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("П/д"), renderer,"text", COL_OZ_DOH,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Дата приёма"), renderer,"text", COL_DATA_PNR,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Дата увольнения"), renderer,"text", COL_DATA_USR,NULL);


renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Льгота"), renderer,"text", COL_LGOTA,NULL);


renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("В/д"), renderer,"text", COL_VID_DOK,NULL);



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

void l_f1dfz_vibor(GtkTreeSelection *selection,class l_f1dfz_data *data)
{
GtkTreeModel *model;
GtkTreeIter  iter;


if(gtk_tree_selection_get_selected(selection,&model,&iter) != TRUE)
 return;


gchar *inn;
gchar *priz;
gchar *lgota;
gchar *pr;
gint  nomer;


gtk_tree_model_get(model,&iter,
COL_INN,&inn,
COL_OZ_DOH,&priz,
COL_LGOTA,&lgota,
COL_VID_DOK,&pr,
NUM_COLUMNS,&nomer,-1);

data->inn_v.new_plus(inn);
data->priz_v.new_plus(priz);
data->lgota_v.new_plus(lgota);
data->pr_v.new_plus(pr);

data->snanomer=nomer;

g_free(inn);
g_free(priz);
g_free(lgota);
g_free(pr);


}

/**********************************/
/*выбор загрузки*/
/***********************************/
void l_f1dfz_fk6(class l_f1dfz_data *data)
{
class iceb_u_str titl;
class iceb_u_str zagolovok;
class iceb_u_spisok punkt_m;
int nomer=0;
titl.plus(gettext("Загрузка"));

zagolovok.plus(gettext("Загрузка"));

punkt_m.plus(gettext("Загрузка из файла в формате csv"));//0
punkt_m.plus(gettext("Загрузка из файла в формате dbf"));//1


while(nomer >= 0)
 {

  nomer=iceb_menu_mv(&titl,&zagolovok,&punkt_m,nomer,NULL);
  switch (nomer)
   {
    case -1:
      return;

    case 0:
      zagf1df_csvw(data->nomd.ravno(),data->kolzap,data->window);
      return;
      break;


    case 1:
      zagrf1df(data->nomd.ravno(),data->kolzap,data->kvrt,data->window);
      return;
      break;

   }
   
 }
}


/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  l_f1dfz_knopka(GtkWidget *widget,class l_f1dfz_data *data)
{
char    strsql[512];
SQL_str row;
class SQLCURSOR cur;
iceb_u_str repl;
class iceb_u_spisok imaf;
class iceb_u_spisok naimf;

int knop=atoi(gtk_widget_get_name(widget));
//g_print("l_f1dfz_knopka knop=%d\n",knop);
data->kl_shift=0;
switch (knop)
 {
  case SFK1:
    if(l_f1df_v(&data->nomd,data->window) == 0)
     {
      l_f1dfz_read_sh(data);
      l_f1dfz_create_list(data);
     }
  return;
    
  case FK2:
    if(l_f1dfz_v(data->nomd.ravno(),&data->inn_tv,&data->priz_tv,&data->lgota_tv,&data->pr_tv,data->window) == 0)
      l_f1dfz_create_list(data);
    return;  

  case SFK2:
    sprintf(strsql,"select fio from F8dr1 where nomd='%s' and inn='%s' and priz='%s' and lgota='%s' and pr=%d",
    data->nomd.ravno(),
    data->inn_v.ravno(),
    data->priz_v.ravno(),
    data->lgota_v.ravno(),
    data->pr_v.ravno_atoi());
    if(iceb_sql_readkey(strsql,&row,&cur,data->window) != 1)
     {
      iceb_menu_soob(gettext("Не найдена запись для корректировки !"),data->window);
      return;
     }
    if(row[0][0] == '\0')
     {
      //корректировка тех кто работает в организации
      if(l_f1dfz_v(data->nomd.ravno(),&data->inn_v,&data->priz_v,&data->lgota_v,&data->pr_v,data->window) == 0)
        l_f1dfz_create_list(data);
     }
    else
     {
      //корректировка записей посторонних людей
      if(l_f1dfz1_v(data->nomd.ravno(),&data->inn_v,&data->priz_v,&data->lgota_v,&data->pr_v,data->window) == 0)
        l_f1dfz_create_list(data);
     }
    return;  
  
  case FK3:
    if(iceb_menu_danet(gettext("Удалить запись ? Вы уверены ?"),2,data->window) == 2)
      return;

    sprintf(strsql,"delete from F8dr1 where nomd='%s' and inn='%s' \
and priz='%s' and lgota='%s' and pr=%d",
    data->nomd.ravno(),
    data->inn_v.ravno(),
    data->priz_v.ravno(),
    data->lgota_v.ravno(),
    data->pr_v.ravno_atoi());

    iceb_sql_zapis(strsql,0,0,data->window);

    l_f1dfz_create_list(data);

    return;
  
  case SFK3:
    if(iceb_menu_danet(gettext("Удалить документ ? Вы уверены ?"),2,data->window) == 2)
      return;

    sprintf(strsql,"delete from F8dr1 where nomd='%s'",data->nomd.ravno());
    if(iceb_sql_zapis(strsql,0,0,data->window) != 0)
     return;

    sprintf(strsql,"delete from F8dr where nomd='%s'",data->nomd.ravno());
    iceb_sql_zapis(strsql,0,0,data->window);

    sprintf(strsql,"delete from F1dfvs where nomd='%s'",data->nomd.ravno());
    iceb_sql_zapis(strsql,0,0,data->window);

    data->voz=1;
    gtk_widget_destroy(data->window);
    return;

  case FK4:
    l_f1dfz_p(&data->rk,data->window);
    l_f1dfz_create_list(data);
    return;

  case FK5:
    if(rasf1df(data->nomd.ravno(),data->metka_oth,&imaf,&naimf,data->window) == 0)
       iceb_rabfil(&imaf,&naimf,data->window);
    return;

  case FK6:
    l_f1dfz_fk6(data);

    l_f1dfz_create_list(data);
    return;

  case FK7: /*Переход в режим работы с военным сбором*/
    l_f1dfvs(data->nomd.ravno(),data->window);
    l_f1dfz_create_list(data);
    return;  

  case FK8:
    if(l_f1dfz1_v(data->nomd.ravno(),&data->inn_tv,&data->priz_tv,&data->lgota_tv,&data->pr_tv,data->window) == 0)
      l_f1dfz_create_list(data);
    return;  

    
  case FK10:
    data->voz=0;
    gtk_widget_destroy(data->window);
    return;
 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   l_f1dfz_key_press(GtkWidget *widget,GdkEventKey *event,class l_f1dfz_data *data)
{
iceb_u_str repl;

switch(event->keyval)
 {

  case GDK_KEY_F1:

//    if(data->kl_shift == 0)
//      g_signal_emit_by_name(data->knopka[FK1],"clicked");
//    else
      g_signal_emit_by_name(data->knopka[SFK1],"clicked");
    
    return(TRUE);

  case GDK_KEY_F2:

    if(data->kl_shift == 0)
      g_signal_emit_by_name(data->knopka[FK2],"clicked");
    else
      g_signal_emit_by_name(data->knopka[SFK2],"clicked");
    
    return(TRUE);

  case GDK_KEY_F3:
    if(data->kl_shift == 0)
      g_signal_emit_by_name(data->knopka[FK3],"clicked");
    else
      g_signal_emit_by_name(data->knopka[SFK3],"clicked");
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
void l_f1dfz_v_row(GtkTreeView *treeview,GtkTreePath *arg1,GtkTreeViewColumn *arg2,
class l_f1dfz_data *data)
{
g_signal_emit_by_name(data->knopka[SFK2],"clicked");

}

/************************/
/*Проверка записи*/
/*******************/

int l_f1dfz_prov(SQL_str row,class l_f1dfz_rek *rk,GtkWidget *wpredok)
{
if(rk->metka_poi == 1)
 return(0);

//Полное сравнение
if(iceb_u_proverka(rk->kod_doh.ravno(),row[4],0,0) != 0)
 return(1);

//Поиск образца в строке
if(rk->inn.ravno()[0] != '\0' && iceb_u_strstrm(row[1],rk->inn.ravno()) == 0)
 return(1);

if(rk->fio.getdlinna() > 1)
 {
  if(row[10][0] != '\0')
   {
    if(iceb_u_strstrm(row[10],rk->fio.ravno()) == 0)
     return(1);
   }
  else
   {
    char strsql[512];
    SQL_str row1;
    class SQLCURSOR cur;
    sprintf(strsql,"select fio from Kartb where inn='%s'",row[1]);
    if(iceb_sql_readkey(strsql,&row1,&cur,wpredok) == 1)
     if(iceb_u_strstrm(row1[0],rk->fio.ravno()) == 0)
      return(1);
     

   }  
 }
   
return(0);


}
/************************/
/*Чтение шапки документа*/
/************************/
void l_f1dfz_read_sh(class l_f1dfz_data *data)
{
char strsql[512];
SQL_str row;
class SQLCURSOR cur;

sprintf(strsql,"select kvrt,vidd from F8dr where nomd='%s'",data->nomd.ravno());
if(iceb_sql_readkey(strsql,&row,&cur,data->window) != 1)
 {
  iceb_menu_soob(gettext("Не найдена шапка документа !"),data->window);
  return;
 }
data->kvrt=atoi(row[0]);
data->metka_oth=atoi(row[1]);

}
