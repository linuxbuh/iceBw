/*$Id: iceb_l_plansh.c,v 1.69 2014/07/31 07:08:29 sasa Exp $*/
/*29.11.2017	24.11.2003	Белых А.И.	iceb_l_plansh.c
Работа с планом счетов
Если вернули 0- выбрали контрагента
             1- нет
*/
#include        <errno.h>
#include        "iceb_libbuh.h"
#include        "iceb_l_plansh.h"

enum
{
  FK1,
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

enum
{
 COL_SHET,
 COL_NAIM,
 COL_BLOKIR,
 COL_VID,
 COL_SALDO,
 COL_STATUS,
 COL_TIP,
 COL_KSB,
 COL_DATA_VREM,
 COL_KTO,  
 NUM_COLUMNS
};



class plansh_data
 {
  public:
  

  plansh_rek poisk;
  short metka_poi;  //0-не проверять 1-проверять
    
  //Выбранный счет
  class iceb_u_str shetv;  //выбранный счёт
  class iceb_u_str naimv;
  class iceb_u_str shet_tv; //Только что введённый счёт
           
  GtkWidget *label_kolstr;
  GtkWidget *label_poisk;
  GtkWidget *sw;
  GtkWidget *treeview;
  GtkWidget *window;
  GtkWidget *label_link;
  GtkWidget *knopka[KOL_F_KL];
  class iceb_u_str name_window;
  
  short     kl_shift; //0-отжата 1-нажата  
  int       snanomer;   //номер записи на которую надостать
  int       kolzap;     //Количество записей
  short     metka_rr;   //0-ввод и корректировка 1-чистый выбор
  int       metka_voz;  //0-выбрано 1-не выбрано
    
  //Конструктор
  plansh_data()
   {
    metka_voz=1;
    metka_rr=0;
    metka_poi=0; //0-не проверять 1-проверять
    snanomer=0;
    kolzap=kl_shift=0;
    window=treeview=NULL;
   }  

 };

void plansh_create_list (class plansh_data *data);

int iceb_l_plansh_v(class iceb_u_str *shetk,GtkWidget *wpredok);
int iceb_l_plansh_p(class plansh_rek *rek_poi,GtkWidget *wpredok);
void       plansh_add_columns (GtkTreeView *treeview);
gboolean   plansh_key_press(GtkWidget *,GdkEventKey *,class plansh_data *);
void       plansh_knopka(GtkWidget *,class plansh_data *);
void       plansh_v_row(GtkTreeView *,GtkTreePath *,GtkTreeViewColumn *,class plansh_data *);
void       plansh_vibor(GtkTreeSelection *,class plansh_data *);
void       plansh_udnz();
gint       plansh_udnz11(class plansh_udnz_data *data);
void       plansh_snkod(class plansh_data *data);
int        plansh_prov_row(SQL_str row,class plansh_data *data);
void       plansh_sm_kod(char *kodkon);
int 	udplsh(class plansh_data *);
int provudsh(const char *shet,int met,GtkWidget*);
void        rasplsh(class plansh_data *data);
int  plansh_vksb(class plansh_data *data);
int iceb_l_plansh_vksb(class iceb_u_str *kod_subbal,char *nadpis,GtkWidget *wpredok);
void iceb_l_plansh_vri(class plansh_data *data);
void iceb_l_plansh_uns(GtkWidget *wpredok);
void iceb_l_plansh_bs(class plansh_data *data);


int iceb_l_plansh(int metka_rr,//0-ввод и корректировка 1-чистый выбор
iceb_u_str *kod,
iceb_u_str *naim,
GtkWidget  *wpredok)
{
int gor=0;
int ver=0;
class plansh_data data;
char bros[1024];

data.metka_rr=metka_rr;
data.name_window.plus(__FUNCTION__);

if(naim->getdlinna() > 1)
 {
  data.metka_poi=1;
  data.poisk.naim.plus(naim->ravno());
 }

//printf("l_plansh-data.metka_rr=%d data.metka_poi=%d\n",data.metka_rr,data.metka_poi);
 
data.window = gtk_window_new (GTK_WINDOW_TOPLEVEL);

gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

if(data.metka_rr == 1)
 {
  if(iceb_sizwr(data.name_window.ravno(),&gor,&ver) == 0)
   {
    gtk_window_set_default_size (GTK_WINDOW  (data.window),gor,ver);
   }
 }


sprintf(bros,"%s %s",iceb_get_namesystem(),gettext("План счетов"));

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

g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(plansh_key_press),&data);
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
gtk_widget_show(vbox1);
gtk_widget_show(vbox2);
gtk_widget_show(hbox);

data.label_kolstr=gtk_label_new (gettext("План счетов"));


gtk_box_pack_start (GTK_BOX (vbox2),data.label_kolstr,FALSE, FALSE, 0);

data.label_poisk=gtk_label_new ("");

gtk_box_pack_start (GTK_BOX (vbox2),data.label_poisk,FALSE, FALSE, 0);

data.sw = gtk_scrolled_window_new (NULL, NULL);

gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (data.sw),GTK_SHADOW_ETCHED_IN);
gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (data.sw),GTK_POLICY_AUTOMATIC,GTK_POLICY_AUTOMATIC);
gtk_box_pack_end (GTK_BOX (vbox2), data.sw, TRUE, TRUE, 0);


//Кнопки
sprintf(bros,"<a href=\"%s/%s.html\" title=\"%s\">F1 %s</a>",ICEB_PUT_NA_HTML,__FUNCTION__,gettext("Просмотр документации"),gettext("Помощь"));
data.label_link=gtk_label_new(bros);
gtk_label_set_use_markup (GTK_LABEL (data.label_link), TRUE);
g_signal_connect(data.label_link, "activate-current-link", G_CALLBACK (iceb_activate_link), NULL);

data.knopka[FK1]=gtk_button_new();
gtk_container_add (GTK_CONTAINER (data.knopka[FK1]), data.label_link);
g_signal_connect(data.knopka[FK1],"clicked",G_CALLBACK(plansh_knopka),&data);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK1], TRUE, TRUE, 0);

sprintf(bros,"F2 %s",gettext("Запись"));
data.knopka[FK2]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK2], TRUE, TRUE, 0);
g_signal_connect(data.knopka[FK2], "clicked",G_CALLBACK(plansh_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[FK2],gettext("Ввод новой записи"));
gtk_widget_set_name(data.knopka[FK2],iceb_u_inttochar(FK2));

sprintf(bros,"%sF2 %s",RFK,gettext("Корректировать"));
data.knopka[SFK2]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[SFK2],TRUE,TRUE, 0);
g_signal_connect(data.knopka[SFK2], "clicked",G_CALLBACK(plansh_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[SFK2],gettext("Корректировка выбранной запси"));
gtk_widget_set_name(data.knopka[SFK2],iceb_u_inttochar(SFK2));


sprintf(bros,"F3 %s",gettext("Удалить"));
data.knopka[FK3]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK3],TRUE,TRUE, 0);
g_signal_connect(data.knopka[FK3], "clicked",G_CALLBACK(plansh_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[FK3],gettext("Удаление выбранной запси"));
gtk_widget_set_name(data.knopka[FK3],iceb_u_inttochar(FK3));

sprintf(bros,"%sF3 %s",RFK,gettext("Удалить"));
data.knopka[SFK3]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[SFK3],TRUE,TRUE, 0);
g_signal_connect(data.knopka[SFK3], "clicked",G_CALLBACK(plansh_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[SFK3],gettext("Удаление выбранной запси"));
gtk_widget_set_name(data.knopka[SFK3],iceb_u_inttochar(SFK3));

sprintf(bros,"F4 %s",gettext("Поиск"));
data.knopka[FK4]=gtk_button_new_with_label(bros);
g_signal_connect(data.knopka[FK4], "clicked",G_CALLBACK(plansh_knopka),&data);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK4],TRUE,TRUE, 0);
gtk_widget_set_tooltip_text(data.knopka[FK4],gettext("Поиск нужных записей"));
gtk_widget_set_name(data.knopka[FK4],iceb_u_inttochar(FK4));

sprintf(bros,"F5 %s",gettext("Печать"));
data.knopka[FK5]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK5],TRUE,TRUE, 0);
g_signal_connect(data.knopka[FK5], "clicked",G_CALLBACK(plansh_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[FK5],gettext("Распечатка записей"));
gtk_widget_set_name(data.knopka[FK5],iceb_u_inttochar(FK5));

sprintf(bros,"F6 %s",gettext("Код субб."));
data.knopka[FK6]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK6],TRUE,TRUE, 0);
g_signal_connect(data.knopka[FK6], "clicked",G_CALLBACK(plansh_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[FK6],gettext("Ввести для счёта (субсчёта) код суббаланса"));
gtk_widget_set_name(data.knopka[FK6],iceb_u_inttochar(FK6));

sprintf(bros,"F7 %s",gettext("Импорт"));
data.knopka[FK7]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK7],TRUE,TRUE, 0);
g_signal_connect(data.knopka[FK7], "clicked",G_CALLBACK(plansh_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[FK7],gettext("Импорт плана счетов из файла"));
gtk_widget_set_name(data.knopka[FK7],iceb_u_inttochar(FK7));

sprintf(bros,"F8 %s",gettext("Блокировка"));
data.knopka[FK8]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK8],TRUE,TRUE, 0);
g_signal_connect(data.knopka[FK8], "clicked",G_CALLBACK(plansh_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[FK8],gettext("Блокировка/разблокировка счёта. По заблокированному счёту невозможно делать, корректировать, удалять проводки."));
gtk_widget_set_name(data.knopka[FK8],iceb_u_inttochar(FK8));

sprintf(bros,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1),data.knopka[FK10],TRUE,TRUE, 0);
gtk_widget_set_tooltip_text(data.knopka[FK10],gettext("Завершение работы в этом окне"));
g_signal_connect(data.knopka[FK10], "clicked",G_CALLBACK(plansh_knopka),&data);
gtk_widget_set_name(data.knopka[FK10],iceb_u_inttochar(FK10));


gtk_widget_realize(data.window);
gdk_window_set_cursor(gtk_widget_get_window(data.window),gdk_cursor_new_for_display(gtk_widget_get_display(data.window),ICEB_CURSOR));


plansh_create_list (&data);

gtk_widget_show_all(data.window);
if(metka_rr == 0)
 gtk_window_maximize(GTK_WINDOW(data.window));

if(data.kolzap == 0)
 iceb_menu_soob(gettext("Не найдено ни одной записи\nВы можете ввести любой план счетов нажимая клавишу F2\nВы можете импортировать готовый план счетов нажав клавишу F7"),data.window);

gtk_main();

if(data.metka_voz == 0)
 {
  kod->new_plus(data.shetv.ravno());
  naim->new_plus(data.naimv.ravno());
 }

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));

return(data.metka_voz);

}

/***********************************/
/*Создаем список для просмотра */
/***********************************/
void plansh_create_list (class plansh_data *data)
{
class iceb_gdite_data gdite;
iceb_gdite(&gdite,0,data->window);

GtkListStore *model=NULL;
GtkTreeIter iter;
SQLCURSOR cur;
char strsql[512];
int  kolstr=0;
SQL_str row;

gdk_window_set_cursor(gtk_widget_get_window(data->window),gdk_cursor_new_for_display(gtk_widget_get_display(data->window),ICEB_CURSOR_GDITE));

if(data->treeview != NULL)
  gtk_widget_destroy(data->treeview);

data->kl_shift=0;

data->treeview = gtk_tree_view_new();


gtk_container_add (GTK_CONTAINER (data->sw), data->treeview);

g_signal_connect(data->treeview,"row_activated",G_CALLBACK(plansh_v_row),data);

GtkTreeSelection *selection=gtk_tree_view_get_selection(GTK_TREE_VIEW(data->treeview));
gtk_tree_selection_set_mode(selection,GTK_SELECTION_SINGLE);
g_signal_connect(selection,"changed",G_CALLBACK(plansh_vibor),data);

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
G_TYPE_INT);


sprintf(strsql,"select * from Plansh order by ns asc");

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  return;
 }
//gtk_list_store_clear(model);

iceb_u_str s3;
iceb_u_str s4;
iceb_u_str s5;
iceb_u_str s6;
iceb_u_str s9;
class iceb_u_str blokir("");

data->kolzap=0;
int metka=0;
float kolstr1=0.;
while(cur.read_cursor(&row) != 0)
 {
  iceb_pbar(gdite.bar,kolstr,++kolstr1);
  
  if(plansh_prov_row(row,data) != 0)
    continue;


  if(iceb_u_SRAV(data->shet_tv.ravno(),row[0],0) == 0)
    data->snanomer=data->kolzap;

  //Вид счета счет/субсчет
  metka=atoi(row[3]);

  if(metka == 0)
   s3.new_plus(gettext("Счет"));
  if(metka == 1)
   s3.new_plus(gettext("Субсчёт"));

  //Сальдо свернутоЁ/развернутоЁ
  metka=atoi(row[6]);
  if(metka == 0)
   s4.new_plus(" ");

  if(metka == 3)
   s4.new_plus(gettext("Развернутое"));

  //Статус  балансовый/внебалансовый
  metka=atoi(row[7]);
  if(metka == 0)
    s5.new_plus(gettext("Балансовый"));
  if(metka == 1)
    s5.new_plus(gettext("Внебалансовый"));

  //Тип счета активный/пассивный/активно-пассивный
  metka=atoi(row[2]);
  if(metka == 0)
    s6.new_plus(gettext("Активный"));
  if(metka == 1)
    s6.new_plus(gettext("Пассивный"));
  if(metka == 2)
    s6.new_plus(gettext("Активно-пассивный"));
  if(metka == 3)
    s6.new_plus(gettext("Контрактивный"));
  if(metka == 4)
    s6.new_plus(gettext("Контрпасивный"));

  s9.new_plus("");
  if(atoi(row[9]) != 0)
   s9.new_plus(row[9]);

  if(atoi(row[10]) == 1)
   blokir.new_plus(gettext("Заблокирован"));
  else
   blokir.new_plus("");   
  
  gtk_list_store_append (model, &iter);

  gtk_list_store_set (model, &iter,
  COL_SHET,row[0],
  COL_NAIM,row[1],
  COL_BLOKIR,blokir.ravno(),
  COL_VID,s3.ravno(),
  COL_SALDO,s4.ravno(),
  COL_STATUS,s5.ravno(),
  COL_TIP,s6.ravno(),
  COL_KSB,s9.ravno(),
  COL_DATA_VREM,iceb_u_vremzap(row[5]),
  COL_KTO,iceb_kszap(row[4],data->window),
  NUM_COLUMNS,data->kolzap,
  -1);

  data->kolzap++;
 }
data->shet_tv.new_plus("");

gtk_tree_view_set_model (GTK_TREE_VIEW(data-> treeview),GTK_TREE_MODEL (model));

g_object_unref (GTK_TREE_MODEL (model));

plansh_add_columns (GTK_TREE_VIEW (data->treeview));


if(data->kolzap == 0)
 {
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK3]),FALSE);//Недоступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[SFK3]),FALSE);//Недоступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK5]),FALSE);//Недоступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK6]),FALSE);//Недоступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[SFK2]),FALSE);//Недоступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK8]),FALSE);//Недоступна
 }
else
 {
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK3]),TRUE);//Доступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[SFK3]),TRUE);//Доступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK5]),TRUE);//Доступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK6]),TRUE);//Доступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[SFK2]),TRUE);//Доступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK8]),TRUE);//Доступна
 }


gtk_widget_show (data->treeview);
gtk_widget_show (data->sw);

//Стать подсветкой стороки на нужный номер строки
iceb_snanomer(data->kolzap,&data->snanomer,data->treeview);


iceb_u_str stroka;
iceb_u_str zagolov;
zagolov.plus(gettext("План счетов"));

sprintf(strsql,"%s %s",iceb_get_namebase(),iceb_get_pnk("00",0,data->window));
zagolov.ps_plus(strsql);

sprintf(strsql," %s:%d",gettext("Количество записей"),data->kolzap);
zagolov.plus(strsql);

gtk_label_set_text(GTK_LABEL(data->label_kolstr),zagolov.ravno());

if(data->metka_poi == 1)
 {
  
  zagolov.new_plus(gettext("Поиск"));
  zagolov.plus(" !!!");
  iceb_str_poisk(&zagolov,data->poisk.shet.ravno(),gettext("Счёт"));
  iceb_str_poisk(&zagolov,data->poisk.naim.ravno(),gettext("Наименование счёта"));
  iceb_str_poisk(&zagolov,data->poisk.kod_subbal.ravno(),gettext("Код суббаланса"));
  
  iceb_label_set_text_color(data->label_poisk,zagolov.ravno(),"#F90101");
//  gtk_label_set_text(GTK_LABEL(data->label_poisk),zagolov.ravno());
  gtk_widget_show(data->label_poisk);
 }
else
 gtk_widget_hide(data->label_poisk); 

gtk_widget_show(data->label_kolstr);



gdk_window_set_cursor(gtk_widget_get_window(data->window),gdk_cursor_new_for_display(gtk_widget_get_display(data->window),ICEB_CURSOR));


}

/*****************/
/*Создаем колонки*/
/*****************/

void plansh_add_columns (GtkTreeView *treeview)
{
GtkCellRenderer *renderer;

renderer = gtk_cell_renderer_text_new ();

gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Счет"), renderer,"text", COL_SHET,NULL);

renderer = gtk_cell_renderer_text_new ();

gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Наименование"), renderer,"text", COL_NAIM,NULL);
renderer = gtk_cell_renderer_text_new ();

renderer = gtk_cell_renderer_text_new ();

gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Блокировка"), renderer,"text", COL_BLOKIR,NULL);
renderer = gtk_cell_renderer_text_new ();

gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Вид счета"), renderer,
"text", COL_VID,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Сальдо"), renderer,
"text", COL_SALDO,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Статус"), renderer,
"text", COL_STATUS,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Тип счета"), renderer,
"text", COL_TIP,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("К/с"), renderer,"text", COL_KSB,NULL);

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

void plansh_vibor(GtkTreeSelection *selection,class plansh_data *data)
{
GtkTreeModel *model;
GtkTreeIter  iter;


if(gtk_tree_selection_get_selected(selection,&model,&iter) != TRUE)
 return;

gchar *kod;
gchar *naim;
gint  nomer;
gtk_tree_model_get(model,&iter,COL_SHET,&kod,COL_NAIM,&naim,NUM_COLUMNS,&nomer,-1);

data->shetv.new_plus(kod);
data->naimv.new_plus(naim);
data->snanomer=nomer;

g_free(kod);
g_free(naim);


}

/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  plansh_knopka(GtkWidget *widget,class plansh_data *data)
{

int knop=atoi(gtk_widget_get_name(widget));

switch (knop)
 {
  case FK1:
    g_signal_emit_by_name(data->label_link,"activate-current-link");
    return;
  case FK2:
    if(iceb_l_plansh_v(&data->shet_tv,data->window) == 0)
      plansh_create_list (data);
    return;  

  case SFK2:
    if(data->kolzap == 0)
     return;
    if(iceb_l_plansh_v(&data->shetv,data->window) == 0)
      plansh_create_list (data);
    return;  

  case FK3:
    if(data->kolzap == 0)
      return;
    if(udplsh(data) == 0)
      plansh_create_list(data);
    return;  

  case SFK3:
    if(data->kolzap == 0)
      return;
    iceb_l_plansh_uns(data->window);
    plansh_create_list(data);
    return;  


  case FK4:
    if(iceb_l_plansh_p(&data->poisk,data->window) == 0)
     data->metka_poi=1;
    else
     data->metka_poi=0;
     
    plansh_create_list (data);
    return;  

  case FK5:
//    gtk_widget_hide(data->window);
    if(data->kolzap == 0)
      return;
    rasplsh(data);
//    gtk_widget_show(data->window);
    return;  

  case FK6:
    if(plansh_vksb(data) == 0)
     plansh_create_list(data);
    return;  

  case FK7:
    iceb_l_plansh_vri(data);
    plansh_create_list(data);
    return;  

  case FK8:

    iceb_l_plansh_bs(data);
    plansh_create_list(data);
    return;  
    
  case FK10: 
    if(data->metka_rr == 1)
      iceb_sizww(data->name_window.ravno(),data->window);
    gtk_widget_destroy(data->window);
    data->metka_voz=1;
    return;
 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   plansh_key_press(GtkWidget *widget,GdkEventKey *event,class plansh_data *data)
{

switch(event->keyval)
 {

  case GDK_KEY_F1:
    g_signal_emit_by_name(data->knopka[FK1],"clicked");
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
void plansh_v_row(GtkTreeView *treeview,GtkTreePath *arg1,GtkTreeViewColumn *arg2,
class plansh_data *data)
{
if(data->metka_rr == 0)
 {
  g_signal_emit_by_name(data->knopka[SFK2],"clicked");
  return;
 }

iceb_sizww(data->name_window.ravno(),data->window);

gtk_widget_destroy(data->window);

data->metka_voz=0;

}


/*************************************/
/*Проверка записей на реквизиты поиска*/
/**************************************/

int  plansh_prov_row(SQL_str row,class plansh_data *data)
{
if(data->metka_poi == 0)
 return(0);

//Полное сравнение
if(iceb_u_proverka(data->poisk.shet.ravno(),row[0],0,0) != 0)
 return(1);

//Поиск образца в строке
if(data->poisk.naim.ravno()[0] != '\0' && iceb_u_strstrm(row[1],data->poisk.naim.ravno()) == 0)
 return(1);

if(iceb_u_proverka(data->poisk.kod_subbal.ravno(),row[9],0,0) != 0)
 return(1);
 
return(0);
}
/*******************************/
/*Удаление записи              */
/*******************************/

int udplsh(class plansh_data *data)
{


if(iceb_menu_danet(gettext("Удалить запись ? Вы уверены ?"),2,data->window) == 2)
 return(1);

class iceb_clock kk(data->window);


if(provudsh(data->shetv.ravno(),1,data->window) != 0)
  return(1);


char strsql[312];
sprintf(strsql,"delete from Plansh where ns='%s'",data->shetv.ravno());
iceb_sql_zapis(strsql,1,0,data->window);

return(0);

}
/*************************************/
/*Проверка возможности удаления счета*/
/*************************************/
int provudsh(const char *shet,int met,GtkWidget *wpredok)
{
char   strsql[512];


memset(strsql,'\0',sizeof(strsql));
sprintf(strsql,"select ns from Saldo where ns='%s' limit 1",shet);
if(iceb_sql_readkey(strsql,wpredok) > 0)
 {
  if(met != 0)
   {
    sprintf(strsql,"%s Saldo",gettext("Удалить невозможно ! Есть записи в таблице"));
    iceb_menu_soob(strsql,wpredok);
   }
  return(1);
 }

sprintf(strsql,"select sh from Zarsdo where sh='%s' limit 1",shet);
if(iceb_sql_readkey(strsql,wpredok) > 0)
 {
  if(met != 0)
   {
    sprintf(strsql,"%s Zarsdo",gettext("Удалить невозможно ! Есть записи в таблице"));
    iceb_menu_soob(strsql,wpredok);
   }
  return(1);
 }

sprintf(strsql,"select shet from Upldok2a where shet='%s' limit 1",shet);
if(iceb_sql_readkey(strsql,wpredok) > 0)
 {
  if(met != 0)
   {
    sprintf(strsql,"%s Upldok2a",gettext("Удалить невозможно ! Есть записи в таблице"));
    iceb_menu_soob(strsql,wpredok);
   }
  return(1);
 }

if(met != 0)
 sprintf(strsql,"select datp,sh,shk,kodkon,deb,kre from Prov where sh='%s' or shk='%s' limit 1",shet,shet);
else 
 sprintf(strsql,"select datp,sh,shk,kodkon,deb,kre from Prov where sh like '%s%%' or shk like '%s%%' limit 1",shet,shet); 

if(iceb_sql_readkey(strsql,wpredok) > 0)
 {
  if(met != 0)
   {
    iceb_u_str soob;
    sprintf(strsql,"%s Prov",gettext("Удалить невозможно ! Есть записи в таблице"));
    soob.plus(strsql);
    iceb_menu_soob(&soob,wpredok);
   }
  return(1);
 }    

sprintf(strsql,"select shetu from Kart where shetu='%s' limit 1",shet);

if(iceb_sql_readkey(strsql,wpredok) > 0)
 {
  if(met != 0)
   {
    sprintf(strsql,"%s Kart",gettext("Удалить невозможно ! Есть записи в таблице"));
    iceb_menu_soob(strsql,wpredok);
   }
  return(1);
 }

   
sprintf(strsql,"select shet from Nash where shet='%s' limit 1",shet);
//   printw("%s\n",strsql);
if(iceb_sql_readkey(strsql,wpredok) > 0)
   {
    if(met != 0)
     {
      sprintf(strsql,"%s Nash",gettext("Удалить невозможно ! Есть записи в таблице"));
      iceb_menu_soob(strsql,wpredok);
     }
    return(1);
   }


sprintf(strsql,"select shet from Uder where shet='%s' limit 1",shet);

if(iceb_sql_readkey(strsql,wpredok) > 0)
   {
    if(met != 0)
     {
      sprintf(strsql,"%s Uder",gettext("Удалить невозможно ! Есть записи в таблице"));
      iceb_menu_soob(strsql,wpredok);
     }
    return(1);
   }

sprintf(strsql,"select shetu from Uosinp where shetu='%s' limit 1",shet);

if(iceb_sql_readkey(strsql,wpredok) > 0)
   {
    if(met != 0)
     {
      sprintf(strsql,"%s Uosinp",gettext("Удалить невозможно ! Есть записи в таблице"));
      iceb_menu_soob(strsql,wpredok);
     }
    return(1);
   }

sprintf(strsql,"select shetu from Uslugi where shetu='%s' limit 1",shet);

if(iceb_sql_readkey(strsql,wpredok) > 0)
   {
    if(met != 0)
     {
      sprintf(strsql,"%s Uslugi",gettext("Удалить невозможно ! Есть записи в таблице"));
      iceb_menu_soob(strsql,wpredok);
     }
    return(1);
   }

sprintf(strsql,"select shetu from Usldokum1 where shetu='%s' limit 1",shet);

if(iceb_sql_readkey(strsql,wpredok) > 0)
   {
    if(met != 0)
     {
      sprintf(strsql,"%s Usldokum1",gettext("Удалить невозможно ! Есть записи в таблице"));
      iceb_menu_soob(strsql,wpredok);
     }
    return(1);
   }
 
sprintf(strsql,"select shetu from Usldokum2 where shetu='%s' limit 1",shet);

if(iceb_sql_readkey(strsql,wpredok) > 0)
   {
    if(met != 0)
     {
      sprintf(strsql,"%s Usldokum2",gettext("Удалить невозможно ! Есть записи в таблице"));
      iceb_menu_soob(strsql,wpredok);
     }
    return(1);
   }

sprintf(strsql,"select shet from Zarsoc where shet='%s' limit 1",shet);

if(iceb_sql_readkey(strsql,wpredok) > 0)
   {
    if(met != 0)
     {
      sprintf(strsql,"%s Zarsoc",gettext("Удалить невозможно ! Есть записи в таблице"));
      iceb_menu_soob(strsql,wpredok);
     }
    return(1);
   }

sprintf(strsql,"select shet from Zarsocz where shet='%s' limit 1",shet);

if(iceb_sql_readkey(strsql,wpredok) > 0)
   {
    if(met != 0)
     {
      sprintf(strsql,"%s Zarsocz",gettext("Удалить невозможно ! Есть записи в таблице"));
      iceb_menu_soob(strsql,wpredok);
     }
    return(1);
   }


sprintf(strsql,"select shet from Kas where shet='%s' limit 1",shet);

if(iceb_sql_readkey(strsql,wpredok) > 0)
   {
    if(met != 0)
     {
      sprintf(strsql,"%s Kas",gettext("Удалить невозможно ! Есть записи в таблице"));
      iceb_menu_soob(strsql,wpredok);
     }
    return(1);
   }

sprintf(strsql,"select shetkas from Kasop1 where shetkas='%s' limit 1",shet);

if(iceb_sql_readkey(strsql,wpredok) > 0)
   {
    if(met != 0)
     {
      sprintf(strsql,"%s Kasop1",gettext("Удалить невозможно ! Есть записи в таблице"));
      iceb_menu_soob(strsql,wpredok);
     }
    return(1);
   }

sprintf(strsql,"select shetkor from Kasop1 where shetkor='%s' limit 1",shet);
if(iceb_sql_readkey(strsql,wpredok) > 0)
   {
    if(met != 0)
     {
      sprintf(strsql,"%s Kasop1",gettext("Удалить невозможно ! Есть записи в таблице"));
      iceb_menu_soob(strsql,wpredok);
     }
    return(1);
   }

sprintf(strsql,"select shetkas from Kasop2 where shetkas='%s' limit 1",shet);

if(iceb_sql_readkey(strsql,wpredok) > 0)
   {
    if(met != 0)
     {
      sprintf(strsql,"%s Kasop2",gettext("Удалить невозможно ! Есть записи в таблице"));
      iceb_menu_soob(strsql,wpredok);
     }
    return(1);
   }

sprintf(strsql,"select shetkor from Kasop2 where shetkor='%s' limit 1",shet);

if(iceb_sql_readkey(strsql,wpredok) > 0)
   {
    if(met != 0)
     {
      sprintf(strsql,"%s Kasop2",gettext("Удалить невозможно ! Есть записи в таблице"));
      iceb_menu_soob(strsql,wpredok);
     }
    return(1);
   }


sprintf(strsql,"select shetk from Kasord where shetk='%s' limit 1",shet);

if(iceb_sql_readkey(strsql,wpredok) > 0)
   {
    if(met != 0)
     {
      sprintf(strsql,"%s Kasord",gettext("Удалить невозможно ! Есть записи в таблице"));
      iceb_menu_soob(strsql,wpredok);
     }
    return(1);
   }


sprintf(strsql,"select shet from Ukrkras where shet='%s' limit 1",shet);

if(iceb_sql_readkey(strsql,wpredok) > 0)
   {
    if(met != 0)
     {
      sprintf(strsql,"%s Ukrkras",gettext("Удалить невозможно ! Есть записи в таблице"));
      iceb_menu_soob(strsql,wpredok);
     }
    return(1);
   }


sprintf(strsql,"select shet from Ukrdok1 where shet='%s' limit 1",shet);

if(iceb_sql_readkey(strsql,wpredok) > 0)
   {
    if(met != 0)
     {
      sprintf(strsql,"%s Ukrdok1",gettext("Удалить невозможно ! Есть записи в таблице"));
      iceb_menu_soob(strsql,wpredok);
     }
    return(1);
   }

sprintf(strsql,"select shet from Upldok2a where shet='%s' limit 1",shet);

if(iceb_sql_readkey(strsql,wpredok) > 0)
   {
    if(met != 0)
     {
      sprintf(strsql,"%s Upldok2a",gettext("Удалить невозможно ! Есть записи в таблице"));
      iceb_menu_soob(strsql,wpredok);
     }
    return(1);
   }
sprintf(strsql,"select shet from Opldok where shet='%s' limit 1",shet);

if(iceb_sql_readkey(strsql,wpredok) > 0)
   {
    if(met != 0)
     {
      sprintf(strsql,"%s Opldok",gettext("Удалить невозможно ! Есть записи в таблице"));
      iceb_menu_soob(strsql,wpredok);
     }
    return(1);
   }

return(0);
 
}
/****************************************/
void iceb_l_plansh_hap(int nom_str,int *kolstr,FILE *ff,GtkWidget *wpredok)
{
if(kolstr != NULL)
 *kolstr+=5;

fprintf(ff,"%80s %s N%d\n","",gettext("Страница"),nom_str);

fprintf(ff,"\
-------------------------------------------------------------------------------------------------------------------------------------------------------\n");  
fprintf(ff,gettext("\
Номер |   Тип    | Сальдо   |     Вид        |    Статус     |К/с|Блокировка|                 Наименование                     |Дата записи, кто записал\n\
счета |  счета   |          |    счета       |     счета     |   |          |                    счёта                         |\n"));

fprintf(ff,"\
-------------------------------------------------------------------------------------------------------------------------------------------------------\n");  
}


/***********************************/
/*Распечатка плана счетов          */
/***********************************/

void rasplsh(class plansh_data *data)
{
char  strsql[512];
int   kolstr=0;
SQL_str  row;
SQLCURSOR cur;
FILE   *ff;
char    imaf[64];
class iceb_u_str bros("");


//printf("rasplsh\n");
sprintf(strsql,"select * from Plansh order by ns asc");

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  return;
 }
sprintf(imaf,"ps%d.lst",getpid());

if((ff = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,data->window);

  return;
 }
char imaf_eksport[64];

sprintf(imaf_eksport,"ps.csv");

class iceb_fopen fil;
if(fil.start(imaf_eksport,"w",data->window) != 0)
 return;

fprintf(fil.ff,"#%s\n#%s %s %s\n",
gettext("Экспорт плана счетов"),
gettext("Выгружено из базы"),
iceb_get_namebase(),
iceb_get_pnk("00",1,data->window));

time_t vrem;
time(&vrem);
struct tm *bf;
bf=localtime(&vrem);

fprintf(fil.ff,"\
#%s %d.%d.%d%s  %s:%02d:%02d\n",
gettext("По состоянию на"),
bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,
gettext("г."),
gettext("Время"),
bf->tm_hour,bf->tm_min);

fprintf(fil.ff,"#Расшифровка полей записи:\n\
#1  Номер счета\n\
#2  Наименование счета\n\
#3  0-активный 1-пассивный 2-активно-пассивный\n\
#4  0-счет 1-субсчет\n\
#5  сальдо 0-свернутое 3-развернутое\n\
#6  0-балансовый счет 1-вне балансовый счет\n\
#7  0-национальная валюта или номер валюты из справочника валют (пока не задействовано)\n\
#8  код суббаланса\n");

iceb_zagolov(gettext("План счетов"),ff,data->window);

short	vplsh=0; /*0-двух уровневый план счетов 1-многоуровневый*/

if(iceb_poldan("Многопорядковый план счетов",strsql,"nastrb.alx",data->window) == 0)
 if(iceb_u_SRAV("Вкл",strsql,1) == 0)
  vplsh=1;

if(vplsh == 0)
 fprintf(ff,"%s.\n",gettext("Двухпорядковый"));
if(vplsh == 1)
 fprintf(ff,"%s.\n",gettext("Многопорядковый"));

if(data->poisk.shet.getdlinna() > 1)
 fprintf(ff,"%s:%s\n",gettext("Счёт"),data->poisk.shet.ravno());
if(data->poisk.naim.getdlinna() > 1)
 fprintf(ff,"%s:%s\n",gettext("Наименование счёта"),data->poisk.naim.ravno());
if(data->poisk.kod_subbal.getdlinna() > 1)
 fprintf(ff,"%s:%s\n",gettext("Код суббаланса"),data->poisk.kod_subbal.ravno());
 

iceb_l_plansh_hap(1,NULL,ff,data->window);

short tipsh,vidsh;
class iceb_u_str tip("");
class iceb_u_str msald("");
class iceb_u_str vid("");
class iceb_u_str ksb("");
class iceb_u_str blokir("");

while(cur.read_cursor(&row) != 0)
 {
  if(plansh_prov_row(row,data) != 0)
    continue;

  tipsh=atoi(row[2]);
  vidsh=atoi(row[3]);

  tip.new_plus("");
  if(tipsh == 0)
    tip.new_plus(gettext("Активный"));
  if(tipsh == 1)
    tip.new_plus(gettext("Пасивный"));
  if(tipsh == 2)
    tip.new_plus(gettext("Активно-пассивный"));
  if(tipsh == 3)
    tip.new_plus(gettext("Контрактивный"));
  if(tipsh == 4)
    tip.new_plus(gettext("Контрпасивный"));

  if(row[6][0] == '3')
    msald.new_plus(gettext("Развернуто"));
  else
    msald.new_plus("");
    
  vid.new_plus("");
  if(vidsh == 0)
    vid.new_plus(gettext("Счет"));
  if(vidsh == 1)
    vid.new_plus(gettext("Субсчёт"));

  bros.new_plus("");
  if(row[7][0] == '0')
    bros.new_plus(gettext("Балансовый"));
  if(row[7][0] == '1')
    bros.new_plus(gettext("Внебалансовый"));

  if(atoi(row[9]) != 0)
   ksb.new_plus(row[9]);
  else
   ksb.new_plus("");

  if(atoi(row[10]) == 1)
   blokir.new_plus(gettext("Заблокирован"));
  else
   blokir.new_plus("");


  fprintf(ff,"%-*s|%-*s|%-*.*s|%-*.*s|%-*s|%-*s|%-*.*s|%-*.*s|%s %s\n",
  iceb_u_kolbait(6,row[0]),row[0],
  iceb_u_kolbait(10,vid.ravno()),vid.ravno(),
  iceb_u_kolbait(10,msald.ravno()),iceb_u_kolbait(10,msald.ravno()),msald.ravno(),
  iceb_u_kolbait(16,tip.ravno()),iceb_u_kolbait(16,tip.ravno()),tip.ravno(),
  iceb_u_kolbait(15,bros.ravno()),bros.ravno(),
  iceb_u_kolbait(3,ksb.ravno()),ksb.ravno(),
  iceb_u_kolbait(10,blokir.ravno()),
  iceb_u_kolbait(10,blokir.ravno()),
  blokir.ravno(),
  iceb_u_kolbait(50,row[1]),
  iceb_u_kolbait(50,row[1]),
  row[1],
  iceb_u_vremzap(row[5]),iceb_kszap(row[4],data->window));

  for(int nomer=50; nomer < iceb_u_strlen(row[1]);nomer+=50)
   fprintf(ff,"%6s|%10s|%10s|%16s|%15s|%3s|%10s|%-*.*s|\n",
   "",
   "",
   "",
   "",
   "",
   "",
   "",
   iceb_u_kolbait(50,iceb_u_adrsimv(nomer,row[1])),
   iceb_u_kolbait(50,iceb_u_adrsimv(nomer,row[1])),
   iceb_u_adrsimv(nomer,row[1]));

  fprintf(fil.ff,"%s|%s|%s|%s|%s|%s|%s|%s|\n",
  row[0],row[1],row[2],row[3],row[6],row[7],row[8],row[9]);

 }
fprintf(ff,"ICEB_LST_END\n\
-------------------------------------------------------------------------------------------------------------------------------------------------------\n");  
fprintf(ff,"%s: %d\n",gettext("Количество счетов"),kolstr);
iceb_podpis(ff,data->window);

fclose(ff);
fil.end();

class spis_oth oth;

oth.spis_imaf.plus(imaf);
oth.spis_naim.plus(gettext("План счетов"));

int orient=iceb_ustpeh(imaf,3,data->window);
iceb_rnl(imaf,orient,NULL,&iceb_l_plansh_hap,data->window);

oth.spis_imaf.plus(imaf_eksport);
oth.spis_naim.plus(gettext("Файл для экспорта плана счетов в другую базу"));

iceb_rabfil(&oth,data->window);

}
/***************************/
/*Ввод и корректировка кода суббаланса*/
/**************************************/
int  plansh_vksb(class plansh_data *data)
{
char strsql[512];
SQL_str row;
class SQLCURSOR cur;

sprintf(strsql,"select nais,ksb from Plansh where ns='%s'",data->shetv.ravno());
if(iceb_sql_readkey(strsql,&row,&cur,data->window) != 1)
 {
  iceb_menu_soob(gettext("Не найдено счёт в плане счетов !"),data->window);
  return(1);
 }
sprintf(strsql,"%s\n%s %s",gettext("Введите код суббаланса"),data->shetv.ravno(),row[0]);
class iceb_u_str kodsubbal("");
if(atoi(row[1]) != 0)
  kodsubbal.new_plus(row[1]);


if(iceb_l_plansh_vksb(&kodsubbal,strsql,data->window) != 0)
 return(1);  
//if(iceb_menu_vvod1(strsql,&kodsubbal,10,data->window) != 0)
// return(1);

if(kodsubbal.getdlinna() > 1)
 {
  sprintf(strsql,"select kod from Glksubbal where kod=%d",kodsubbal.ravno_atoi());
  if(iceb_sql_readkey(strsql,data->window) != 1)
   {
    sprintf(strsql,"%s %d !",gettext("Не найден код суббаланса"),kodsubbal.ravno_atoi());
    iceb_menu_soob(strsql,data->window);
    return(1);
   }
 }

sprintf(strsql,"update Plansh set ksb=%d,vrem=%ld,ktoi=%d where ns='%s'",
kodsubbal.ravno_atoi(),
time(NULL),
iceb_getuid(data->window),
data->shetv.ravno());

iceb_sql_zapis(strsql,0,0,data->window);

return(0);
}

/*******************************************/
/*Импорт плана счетов*/
/******************************************/
void iceb_l_plansh_import(class plansh_data *data)
{

class iceb_u_str imaf_imp("");

if(iceb_menu_import(&imaf_imp,gettext("Импорт плана счетов из файла"),"",__FUNCTION__,data->window) != 0)
 return;
class iceb_u_str stroka("");
char strsql[1024];

FILE *ff;

if((ff = fopen(imaf_imp.ravno(),"r")) == NULL)
 {
  iceb_er_op_fil(imaf_imp.ravno(),"",errno,data->window);
  return;
 }

class iceb_u_str nomer_sheta("");
class iceb_u_str naim_sheta("");
int tips=0;
int vids=0;
int saldo=0;
int stat=0;
int val=0;
int kodsubbal=0;
int kolih_zag_zap=0;
while(iceb_u_fgets(&stroka,ff) == 0)
 {
  
  if(iceb_u_pole2(stroka.ravno(),'|') < 8) 
   continue;

  iceb_u_polen(stroka.ravno(),&nomer_sheta,1,'|');

  if(nomer_sheta.getdlinna() <= 1)
   continue;

  /*проверяем может уже такой номер есть в плане счетов*/
  sprintf(strsql,"select ns from Plansh where ns='%s'",nomer_sheta.ravno());
  if(iceb_sql_readkey(strsql,data->window) > 0)
   {
    sprintf(strsql,"%s %s",gettext("В плане счетов уже есть счёт"),nomer_sheta.ravno());
    iceb_menu_soob(strsql,data->window);
    continue;
   }
  iceb_u_polen(stroka.ravno(),&naim_sheta,2,'|');
  iceb_u_polen(stroka.ravno(),&tips,3,'|');
  iceb_u_polen(stroka.ravno(),&vids,4,'|');
  iceb_u_polen(stroka.ravno(),&saldo,5,'|');
  iceb_u_polen(stroka.ravno(),&stat,6,'|');
  iceb_u_polen(stroka.ravno(),&val,7,'|');
  iceb_u_polen(stroka.ravno(),&kodsubbal,7,'|');
  /*проверяем код суббаланса*/
  if(kodsubbal != 0)
   {
    sprintf(strsql,"select kod from Glksubbal where kod=%d",kodsubbal);
    if(iceb_sql_readkey(strsql,data->window) != 1)
     kodsubbal=0;
   }



  sprintf(strsql,"insert into Plansh (ns,nais,tips,vids,ktoi,vrem,saldo,stat,val,ksb) \
values('%s','%s',%d,%d,%d,%ld,%d,%d,%d,%d)",
  nomer_sheta.ravno(),
  naim_sheta.ravno_filtr(),
  tips,
  vids,
  iceb_getuid(data->window),
  time(NULL),
  saldo,
  stat,
  val,
  kodsubbal);
  
  if(iceb_sql_zapis(strsql,1,0,data->window) == 0)
   kolih_zag_zap++;
  
 }

fclose(ff);

sprintf(strsql,"%s %d",gettext("Количество загруженых записей"),kolih_zag_zap);
iceb_menu_soob(strsql,data->window);

}
/*********************************************/
/*Загрузка плана счетов из базы*/
/*****************************************/
void iceb_l_plansh_imortb(const char *imaf,GtkWidget *wpredok)
{
class iceb_gdite_data gdite;
iceb_gdite(&gdite,0,wpredok);
SQL_str row;
class SQLCURSOR cur;
int kolstr=0;
char strsql[1024];
sprintf(strsql,"select str from Alx where fil='%s' order by ns asc",imaf);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,"",strsql,wpredok);
  return;
 }  
if(kolstr == 0)
 {
  iceb_menu_soob(gettext("Не найден план счетов в базе!"),wpredok);
  return;
 }
class iceb_u_str nomer_sheta("");
class iceb_u_str naim_sheta("");
int tips=0;
int vids=0;
int saldo=0;
int stat=0;
int val=0;
int kodsubbal=0;
int kolih_zag_zap=0;
float kolstr1=0;
while(cur.read_cursor(&row) != 0)
 {
  iceb_pbar(gdite.bar,kolstr,++kolstr1);
  
  if(iceb_u_pole2(row[0],'|') < 8) 
   continue;

  iceb_u_polen(row[0],&nomer_sheta,1,'|');

  if(nomer_sheta.getdlinna() <= 1)
   continue;

  /*проверяем может уже такой номер есть в плане счетов*/
  sprintf(strsql,"select ns from Plansh where ns='%s'",nomer_sheta.ravno());
  if(iceb_sql_readkey(strsql,wpredok) > 0)
   {
//    sprintf(strsql,"%s %s\n",gettext("В плане счетов уже есть счёт"),nomer_sheta.ravno());
    continue;
   }
  iceb_u_polen(row[0],&naim_sheta,2,'|');
  iceb_u_polen(row[0],&tips,3,'|');
  iceb_u_polen(row[0],&vids,4,'|');
  iceb_u_polen(row[0],&saldo,5,'|');
  iceb_u_polen(row[0],&stat,6,'|');
  iceb_u_polen(row[0],&val,7,'|');
  iceb_u_polen(row[0],&kodsubbal,7,'|');

  sprintf(strsql,"insert into Plansh (ns,nais,tips,vids,ktoi,vrem,saldo,stat,val,ksb) \
values('%s','%s',%d,%d,%d,%ld,%d,%d,%d,%d)",
  nomer_sheta.ravno(),
  naim_sheta.ravno_filtr(),
  tips,
  vids,
  iceb_getuid(wpredok),
  time(NULL),
  saldo,
  stat,
  val,
  kodsubbal);
  
  if(iceb_sql_zapis(strsql,1,0,wpredok) == 0)
   kolih_zag_zap++;
  
 }
gdite.close();


sprintf(strsql,"%s %d",gettext("Количество загруженых записей"),kolih_zag_zap);
iceb_menu_soob(strsql,wpredok);
}
/*****************************************/
/*Меню выбора режима импорта*/
/**************************************/
void iceb_l_plansh_vri(class plansh_data *data)
{

class iceb_u_str titl;
class iceb_u_str zagolovok;
class iceb_u_spisok punkt_m;

titl.plus(gettext("Выбор"));


zagolovok.plus(gettext("Выберите нужное"));

punkt_m.plus(gettext("Импорт плана счетов для хозрасчётных организаций Украины"));//0
punkt_m.plus(gettext("Импорт плана счетов для бюджетных организаций Украины"));//1
punkt_m.plus(gettext("Импорт плана счетов для банков Украины"));//2
punkt_m.plus(gettext("Импорт плана счетов для хозрасчётных организаций России"));//3
punkt_m.plus(gettext("Импорт плана счетов из файла"));//4


int nomer=0;
nomer=iceb_menu_mv(&titl,&zagolovok,&punkt_m,nomer,data->window);

switch(nomer)
 {
  case 0:
   iceb_l_plansh_imortb("plansh_hz_ua.alx",data->window);
   break;

  case 1:
   iceb_l_plansh_imortb("plansh_bd_ua.alx",data->window);
   break;

  case 2:
   iceb_l_plansh_imortb("plansh_bn_ua.alx",data->window);
   break;

  case 3:
   iceb_l_plansh_imortb("plansh_hz_ru.alx",data->window);
   break;

  case 4:
   iceb_l_plansh_import(data);
   break;
 }

}
/*********************************************/
/*Удаление неиспользуемых счетов*/
/**************************************/
void iceb_l_plansh_uns(GtkWidget *wpredok)
{

if(iceb_menu_danet(gettext("Удалить не используемые счета? Вы уверены?"),2,wpredok) == 2)
 return;



class iceb_gdite_data gdite;
iceb_gdite(&gdite,0,wpredok);

SQL_str row;
class SQLCURSOR cur;
int kolstr=0;
char strsql[1024];

sprintf(strsql,"select ns,nais from Plansh order by ns asc");

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,"",strsql,wpredok);
  return;
 }  
if(kolstr == 0)
 {
  iceb_menu_soob(gettext("Не найден план счетов в базе!"),wpredok);
  return;
 }
int kolud=0;
float kolstr1=0;
while(cur.read_cursor(&row) != 0)
 {
  iceb_pbar(gdite.bar,kolstr,++kolstr1);
  if(provudsh(row[0],0,wpredok) != 0)
    continue;
//  printw("%*s %s\n",iceb_u_kolbait(6,row[0]),row[0],row[1]);

  sprintf(strsql,"delete from Plansh where ns='%s'",row[0]);
  if(sql_zap(&bd,strsql) != 0)
   {
    if(sql_nerror(&bd) == ER_DBACCESS_DENIED_ERROR) //Толко чтение
     {
      iceb_menu_soob(gettext("У вас нет полномочий для выполнения этой операции !"),wpredok);
      return;
     }
    else
     iceb_msql_error(&bd,gettext("Ошибка удаления записи !"),strsql,wpredok);
    continue;
   }
  kolud++;
 }
gdite.close();

sprintf(strsql,"%s %d",gettext("Количество удалённых счетов"),kolud);
iceb_menu_soob(strsql,wpredok);



}
/******************************************/
/*блокировка/разблокировка счёта*/
/******************************************/
void iceb_l_plansh_bs(class plansh_data *data)
{
SQL_str row;
class SQLCURSOR cur;
char strsql[1024];
int metka_bs=0;

sprintf(strsql,"select bs from Plansh where ns='%s'",data->shetv.ravno_filtr());
if(iceb_sql_readkey(strsql,&row,&cur,data->window) <= 0)
 return;

metka_bs=atoi(row[0]);

sprintf(strsql,"select gkb from icebuser where login='%s'",iceb_u_getlogin());

if(iceb_sql_readkey(strsql,&row,&cur,data->window) != 1)
 return;
if(atoi(row[0]) == 1)
 {
  iceb_menu_soob(gettext("Блокировать/разблокировать счёт может тот кому разрешена блокировка/разблокировка проводок!"),data->window);
  return;
 }
if(metka_bs == 0)
 sprintf(strsql,"update Plansh set bs=1 where ns='%s'",data->shetv.ravno_filtr());
else
 sprintf(strsql,"update Plansh set bs=0 where ns='%s'",data->shetv.ravno_filtr());

iceb_sql_zapis(strsql,0,0,data->window);
}
