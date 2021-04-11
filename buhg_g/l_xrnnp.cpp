/*$Id: l_xrnnp.c,v 1.28 2014/08/31 06:19:21 sasa Exp $*/
/*11.02.2016	14.04.2008	Белых А.И.	l_xrnnp.c
Реестр полученных налоговых накладных
*/
#include        <errno.h>
#include  "buhg_g.h"
#include "xrnn_poiw.h"
enum
{
 COL_M,
 COL_DATAD,
 COL_NOMDOK,
 COL_VIDDOK,
 COL_SUMADOK,
 COL_NDS,
 COL_PODSYS,
 COL_KOD_OP,
 COL_DATAVNN,
 COL_INN,
 COL_NAIM_KONTR,
 COL_KOMENT,
 COL_DATA_VREM,
 COL_KTO,  
 COL_NOMZAP,
 NUM_COLUMNS
};

enum
{
 FK2,
 SFK2,
 FK3,
 SFK3,
 FK4,
 FK5,
 FK6,
 Insert,
 FK10,
 KOL_F_KL
};

class  l_xrnnp_data
 {
  public:

  
  GtkWidget *label_kolstr;
  GtkWidget *label_poisk;
  GtkWidget *sw;
  GtkWidget *treeview;
  GtkWidget *window;
  GtkWidget *knopka[KOL_F_KL];
  GtkWidget *radiobutton1[2];
  short     kl_shift; //0-отжата 1-нажата  
  int       snanomer;   //номер записи на которую надостать
  int       kolzap;     //Количество записей
  
  class xrnn_poi rkp;
  class iceb_u_str datap;
  
  class iceb_u_str datan;
  class iceb_u_str datak;
  
  int nom_v_zap; /*Номер выбранной записи*/
  class iceb_u_str data_v_zap; /*Дата выбранной записи*/
  class iceb_u_str nnn_tv; /*номер налоговой накладной только что введённый*/
  
  class iceb_u_str zapros;    
  short metka_sort_dat; /*0-в порядке возростания дат 1-в порядке убывания дат*/
  //Конструктор
  l_xrnnp_data()
   {
    metka_sort_dat=0;
    snanomer=0;
    kl_shift=0;
    window=treeview=NULL;
    datan.new_plus("");
    datak.new_plus("");
    nom_v_zap=0;
    data_v_zap.plus("");
    nnn_tv.plus("");
    datap.plus("");
   }      

 };

gboolean   l_xrnnp_key_press(GtkWidget *widget,GdkEventKey *event,class l_xrnnp_data *data);
void l_xrnnp_vibor(GtkTreeSelection *selection,class l_xrnnp_data *data);
void l_xrnnp_v_row(GtkTreeView *treeview,GtkTreePath *arg1,GtkTreeViewColumn *arg2,
class l_xrnnp_data *data);
void  l_xrnnp_knopka(GtkWidget *widget,class l_xrnnp_data *data);
void l_xrnnp_add_columns (GtkTreeView *treeview);
int l_xrnnp_udzap(class l_xrnnp_data *data);
int  l_xrnnp_prov_row(SQL_str row,class l_xrnnp_data *data);
void l_xrnnp_rasp(class l_xrnnp_data *data);
void l_xrnnp_create_list (class l_xrnnp_data *data);

int l_xrnnp_v(class iceb_u_str *nom_nal_nak,int nom_zap,GtkWidget *wpredok);
int l_xrnnp_p(class xrnn_poi *rek_poi,GtkWidget *wpredok);
int l_xrnn_udgrz(class l_xrnnp_data *data);
void l_xrnnp_vr(class l_xrnnp_data *data);
void  l_xrnnp_radio1(GtkWidget *widget,class l_xrnnp_data *data);

extern class iceb_rnfxml_data rek_zag_nn;

extern SQL_baza  bd;

void l_xrnnp(short dnp,short mnp,short gnp,GtkWidget *wpredok)
{
l_xrnnp_data data;
char bros[512];

data.rkp.metka_poi=0;
data.rkp.nomzap_v.free_class();

if(dnp != 0)
 {
  sprintf(bros,"%d.%d.%d",dnp,mnp,gnp);
  data.datan.new_plus(bros);
  data.datap.new_plus(bros);
  data.rkp.datan.new_plus(bros);
  
 }
 
data.window = gtk_window_new (GTK_WINDOW_TOPLEVEL);

gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

//gtk_window_set_default_size (GTK_WINDOW  (data.window),400,400);


sprintf(bros,"%s %s",iceb_get_namesystem(),gettext("Реестр полученных налоговых накладных"));

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

g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(l_xrnnp_key_press),&data);
g_signal_connect_after(data.window,"key_release_event",G_CALLBACK(iceb_key_release),&data.kl_shift);

GtkWidget *hbox = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 1);
gtk_box_set_homogeneous (GTK_BOX(hbox),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
gtk_container_add (GTK_CONTAINER (data.window), hbox);

GtkWidget *vbox1 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 1);
gtk_box_set_homogeneous (GTK_BOX(vbox1),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
GtkWidget *vbox2 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 1);
gtk_box_set_homogeneous (GTK_BOX(vbox2),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет

gtk_box_pack_start (GTK_BOX (hbox), vbox1, FALSE, FALSE,1);
gtk_box_pack_start (GTK_BOX (hbox), vbox2, TRUE, TRUE,1);
gtk_widget_show(hbox);

data.label_kolstr=gtk_label_new (gettext("Реестр полученных налоговых накладных"));


gtk_box_pack_start (GTK_BOX (vbox2),data.label_kolstr,FALSE, FALSE,1);

gtk_widget_show(vbox1);
gtk_widget_show(vbox2);

data.label_poisk=gtk_label_new ("");

gtk_box_pack_start (GTK_BOX (vbox2),data.label_poisk,FALSE, FALSE,1);

data.sw = gtk_scrolled_window_new (NULL, NULL);

gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (data.sw),GTK_SHADOW_ETCHED_IN);
gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (data.sw),GTK_POLICY_AUTOMATIC,GTK_POLICY_AUTOMATIC);
gtk_box_pack_start(GTK_BOX (vbox2), data.sw, TRUE, TRUE,1);

GtkWidget *hboxradio = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
gtk_box_set_homogeneous(GTK_BOX(hboxradio),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет

/*вставляем группу радиокнопок*/
data.radiobutton1[0]=gtk_radio_button_new_with_label_from_widget(NULL,gettext("По возростанию дат"));
gtk_box_pack_start (GTK_BOX (hboxradio),data.radiobutton1[0], TRUE, TRUE, 0);

g_signal_connect(data.radiobutton1[0], "clicked",G_CALLBACK(l_xrnnp_radio1),&data);

data.radiobutton1[1]=gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(data.radiobutton1[0]),gettext("По убыванию дат"));
gtk_box_pack_start (GTK_BOX (hboxradio),data.radiobutton1[1], TRUE, TRUE, 0);
g_signal_connect(data.radiobutton1[1], "clicked",G_CALLBACK(l_xrnnp_radio1),&data);


gtk_box_pack_start(GTK_BOX (vbox2), hboxradio, FALSE,FALSE, 0);

gtk_widget_show_all(hboxradio);

//Кнопки

sprintf(bros,"F2 %s",gettext("Запись"));
data.knopka[FK2]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK2], TRUE, TRUE,1);
g_signal_connect(data.knopka[FK2], "clicked",G_CALLBACK(l_xrnnp_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[FK2],gettext("Ввод новой записи"));
gtk_widget_set_name(data.knopka[FK2],iceb_u_inttochar(FK2));
gtk_widget_show(data.knopka[FK2]);

sprintf(bros,"%sF2 %s",RFK,gettext("Корректировать"));
data.knopka[SFK2]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[SFK2],TRUE,TRUE,1);
g_signal_connect(data.knopka[SFK2], "clicked",G_CALLBACK(l_xrnnp_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[SFK2],gettext("Корректировка выбранной записи"));
gtk_widget_set_name(data.knopka[SFK2],iceb_u_inttochar(SFK2));
gtk_widget_show(data.knopka[SFK2]);


sprintf(bros,"F3 %s",gettext("Удалить"));
data.knopka[FK3]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK3],TRUE,TRUE,1);
g_signal_connect(data.knopka[FK3], "clicked",G_CALLBACK(l_xrnnp_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[FK3],gettext("Удаление выбранной записи"));
gtk_widget_set_name(data.knopka[FK3],iceb_u_inttochar(FK3));
gtk_widget_show(data.knopka[FK3]);

sprintf(bros,"%sF3 %s",RFK,gettext("Удалить"));
data.knopka[SFK3]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[SFK3],TRUE,TRUE,1);
g_signal_connect(data.knopka[SFK3], "clicked",G_CALLBACK(l_xrnnp_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[SFK3],gettext("Удалить все записи"));
gtk_widget_set_name(data.knopka[SFK3],iceb_u_inttochar(SFK3));
gtk_widget_show(data.knopka[SFK3]);

sprintf(bros,"F4 %s",gettext("Поиск"));
data.knopka[FK4]=gtk_button_new_with_label(bros);
g_signal_connect(data.knopka[FK4], "clicked",G_CALLBACK(l_xrnnp_knopka),&data);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK4],TRUE,TRUE,1);
gtk_widget_set_tooltip_text(data.knopka[FK4],gettext("Поиск нужных записей"));
gtk_widget_set_name(data.knopka[FK4],iceb_u_inttochar(FK4));
gtk_widget_show(data.knopka[FK4]);

sprintf(bros,"F5 %s",gettext("Печать"));
data.knopka[FK5]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK5],TRUE,TRUE,1);
g_signal_connect(data.knopka[FK5], "clicked",G_CALLBACK(l_xrnnp_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[FK5],gettext("Распечатка записей"));
gtk_widget_set_name(data.knopka[FK5],iceb_u_inttochar(FK5));
gtk_widget_show(data.knopka[FK5]);


sprintf(bros,"F6 %s",gettext("Импорт"));
data.knopka[FK6]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK6],TRUE,TRUE,1);
g_signal_connect(data.knopka[FK6], "clicked",G_CALLBACK(l_xrnnp_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[FK6],gettext("Импорт документов"));
gtk_widget_set_name(data.knopka[FK6],iceb_u_inttochar(FK6));
gtk_widget_show(data.knopka[FK6]);

sprintf(bros,"Insert %s",gettext("Отметить"));
data.knopka[Insert]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[Insert],TRUE,TRUE,1);
g_signal_connect(data.knopka[Insert], "clicked",G_CALLBACK(l_xrnnp_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[Insert],gettext("Отметить запись"));
gtk_widget_set_name(data.knopka[Insert],iceb_u_inttochar(Insert));
gtk_widget_show(data.knopka[Insert]);

sprintf(bros,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1),data.knopka[FK10],TRUE,TRUE,1);
gtk_widget_set_tooltip_text(data.knopka[FK10],gettext("Завершение работы в этом окне"));
g_signal_connect(data.knopka[FK10], "clicked",G_CALLBACK(l_xrnnp_knopka),&data);
gtk_widget_set_name(data.knopka[FK10],iceb_u_inttochar(FK10));
gtk_widget_show(data.knopka[FK10]);


gtk_widget_realize(data.window);
gdk_window_set_cursor(gtk_widget_get_window(data.window),gdk_cursor_new_for_display(gtk_widget_get_display(data.window),ICEB_CURSOR));

gtk_widget_grab_focus(data.knopka[FK10]);

l_xrnnp_create_list(&data);

gtk_widget_show(data.window);

gtk_window_maximize(GTK_WINDOW(data.window));
//gtk_window_fullscreen(GTK_WINDOW(data.window));


gtk_main();


if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));


}
/******************************/
/*Обработчик нажатия радиокнопок*/
/**********************************/

void  l_xrnnp_radio1(GtkWidget *widget,class l_xrnnp_data *data)
{
if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(data->radiobutton1[0])) == TRUE)
 {
  if(data->metka_sort_dat == 0)
   return;
  data->metka_sort_dat=0;
 }
if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(data->radiobutton1[1])) == TRUE)
 {
  if(data->metka_sort_dat == 1)
   return;
  data->metka_sort_dat=1;
 }

l_xrnnp_create_list(data);

}


/***********************************/
/*Создаем список для просмотра */
/***********************************/
void l_xrnnp_create_list (class l_xrnnp_data *data)
{
class iceb_gdite_data gdite;
iceb_gdite(&gdite,0,data->window);

iceb_clock sss(data->window);
GtkListStore *model=NULL;
GtkTreeIter iter;
SQLCURSOR cur;
char strsql[512];
int  kolstr=0;
SQL_str row;

data->kl_shift=0; //0-отжата 1-нажата  


if(data->treeview != NULL)
  gtk_widget_destroy(data->treeview);

data->treeview = gtk_tree_view_new();


gtk_container_add (GTK_CONTAINER (data->sw), data->treeview);

g_signal_connect(data->treeview,"row_activated",G_CALLBACK(l_xrnnp_v_row),data);

GtkTreeSelection *selection=gtk_tree_view_get_selection(GTK_TREE_VIEW(data->treeview));
gtk_tree_selection_set_mode(selection,GTK_SELECTION_SINGLE);
g_signal_connect(selection,"changed",G_CALLBACK(l_xrnnp_vibor),data);

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
G_TYPE_STRING, 
G_TYPE_INT,
G_TYPE_INT);


sprintf(strsql,"select * from Reenn1 where datd >= '%s'",data->datan.ravno_sqldata());


if(data->datan.getdlinna() > 1 && data->datak.getdlinna() > 1)
  sprintf(strsql,"select * from Reenn1 where datd >= '%s' and datd <= '%s'",data->datan.ravno_sqldata(),data->datak.ravno_sqldata());

if(data->metka_sort_dat == 0)
 strcat(strsql," order by datd asc,nnn asc");
else
 strcat(strsql," order by datd desc,nnn asc");

data->zapros.new_plus(strsql);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  return;
 }
//gtk_list_store_clear(model);

iceb_u_str ss[NUM_COLUMNS];

data->kolzap=0;
double sumad=0.;
double sumands=0.;
double isumad=0.;
double isumands=0.;
float kolstr1=0;
while(cur.read_cursor(&row) != 0)
 {
//  printf("%s %s %s %s\n",row[0],row[1],row[2],row[3]);
  iceb_pbar(gdite.bar,kolstr,++kolstr1);

  if(l_xrnnp_prov_row(row,data) != 0)
    continue;


  if(iceb_u_SRAV(data->nnn_tv.ravno(),row[1],0) == 0)
    data->snanomer=data->kolzap;

  ss[COL_M].new_plus("");
  if(data->rkp.nomzap_v.find(atoi(row[0])) >= 0)
   ss[COL_M].new_plus("*");

  //Дата документа
  ss[COL_DATAD].new_plus(iceb_u_datzap(row[1]));
  
  //Наименование
//  ss[COL_NOMDOK].new_plus(row[2]);

  sumad=atof(row[3])+atof(row[5])+atof(row[7])+atof(row[9]);
  sumands=atof(row[4])+atof(row[6])+atof(row[8])+atof(row[10]);
  isumad+=sumad;
  isumands+=sumands;
  
  //Сумма по документа
  ss[COL_SUMADOK].new_plus(sumad);
  //Сумма НДС
  ss[COL_NDS].new_plus(sumands);

  //ПОДСИСТЕМА
  ss[COL_PODSYS].new_plus("");
  if(atoi(row[13]) == 1)
    ss[COL_PODSYS].new_plus(ICEB_MP_MATU);
  if(atoi(row[13]) == 2)
    ss[COL_PODSYS].new_plus(ICEB_MP_USLUGI);
  if(atoi(row[13]) == 3)
    ss[COL_PODSYS].new_plus(ICEB_MP_UOS);
  if(atoi(row[13]) == 4)
    ss[COL_PODSYS].new_plus(ICEB_MP_UKR);
  if(atoi(row[13]) == 5)
    ss[COL_PODSYS].new_plus(gettext("ГК"));

  //Дата выписки налоговой накладной
  ss[COL_DATAVNN].new_plus(iceb_u_datzap(row[21]));
    
  gtk_list_store_append (model, &iter);

  gtk_list_store_set (model, &iter,
  COL_M,ss[COL_M].ravno(),
  COL_DATAD,ss[COL_DATAD].ravno(),
  COL_NOMDOK,row[2],
  COL_VIDDOK,row[22],
  COL_SUMADOK,ss[COL_SUMADOK].ravno(),
  COL_NDS,ss[COL_NDS].ravno(),
  COL_PODSYS,ss[COL_PODSYS].ravno(),
  COL_KOD_OP,row[20],
  COL_DATAVNN,ss[COL_DATAVNN].ravno(),
  COL_INN,row[11],
  COL_KOMENT,row[16],
  COL_NAIM_KONTR,row[12],
  COL_DATA_VREM,iceb_u_vremzap(row[19]),
  COL_KTO,iceb_kszap(row[18],data->window),
  COL_NOMZAP,atoi(row[0]),
  NUM_COLUMNS,data->kolzap,
  -1);

  data->kolzap++;
 }

data->nnn_tv.new_plus("");

gtk_tree_view_set_model (GTK_TREE_VIEW(data-> treeview),GTK_TREE_MODEL (model));

g_object_unref (GTK_TREE_MODEL (model));

l_xrnnp_add_columns (GTK_TREE_VIEW (data->treeview));


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
zagolov.plus(gettext("Реестр полученных налоговых накладных"));

sprintf(strsql," %s:%d",gettext("Количество записей"),data->kolzap);
zagolov.plus(strsql);

sprintf(strsql,"\n%s:%s %s:%.2f+%.2f=%.2f",
gettext("Дата начала"),
data->datan.ravno(),
gettext("Сумма по документам"),
isumad-isumands,isumands,isumad);


zagolov.plus(strsql);
gtk_label_set_text(GTK_LABEL(data->label_kolstr),zagolov.ravno());

if(data->rkp.metka_poi == 1)
 {
  
  zagolov.new_plus(gettext("Поиск"));
  zagolov.plus(" !!!");

  iceb_str_poisk(&zagolov,data->rkp.datan.ravno(),gettext("Дата начала"));
  iceb_str_poisk(&zagolov,data->rkp.datak.ravno(),gettext("Дата конца"));
  iceb_str_poisk(&zagolov,data->rkp.nnn.ravno(),gettext("Номер налоговой накладной"));
  iceb_str_poisk(&zagolov,data->rkp.inn.ravno(),gettext("Инд.нал.номер"));
  iceb_str_poisk(&zagolov,data->rkp.naim_kontr.ravno(),gettext("Наименование контрагента"));
  iceb_str_poisk(&zagolov,data->rkp.nomdok.ravno(),gettext("Номер документа"));
  iceb_str_poisk(&zagolov,data->rkp.koment.ravno(),gettext("Коментарий"));
  iceb_str_poisk(&zagolov,data->rkp.kodgr.ravno(),gettext("Код группы"));
  iceb_str_poisk(&zagolov,data->rkp.podsistema.ravno(),gettext("Подсистема"));
  iceb_str_poisk(&zagolov,data->rkp.kodop.ravno(),gettext("Код операции"));
  iceb_str_poisk(&zagolov,data->rkp.datavnn.ravno(),gettext("Дата выписки нал-вой нак-ной начало периода"));
  iceb_str_poisk(&zagolov,data->rkp.datavnk.ravno(),gettext("Дата выписки нал-вой нак-ной конец периода"));
  iceb_str_poisk(&zagolov,data->rkp.viddok.ravno(),gettext("Вид документа"));


  if(data->rkp.metka_ins == 1) //*Только отмеченные
    zagolov.ps_plus(gettext("Только отмеченные записи"));
  if(data->rkp.metka_ins == 2) //*Только не отмеченные\C5
    zagolov.ps_plus(gettext("Только не отмеченные записи"));
  

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

void l_xrnnp_add_columns(GtkTreeView *treeview)
{
GtkCellRenderer *renderer;


renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,"*", renderer,"text", COL_M,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Дата"), renderer,"text", COL_DATAD,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Ном.док."), renderer,"text", COL_NOMDOK,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Вид д-а"), renderer,"text", COL_VIDDOK,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Сумма с НДС"), renderer,"text", COL_SUMADOK,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Сумма НДС"), renderer,"text", COL_NDS,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Подсистема"), renderer,"text", COL_PODSYS,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Код операции"), renderer,"text", COL_KOD_OP,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Дата в.н.н."), renderer,"text", COL_DATAVNN,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Инд.н.н."), renderer,"text", COL_INN,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Наименование контрагента"), renderer,"text", COL_NAIM_KONTR,NULL);

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

void l_xrnnp_vibor(GtkTreeSelection *selection,class l_xrnnp_data *data)
{
GtkTreeModel *model;
GtkTreeIter  iter;


if(gtk_tree_selection_get_selected(selection,&model,&iter) != TRUE)
 return;
gchar *datazap;
gint nom_v_zap;
gint nomer;


gtk_tree_model_get(model,&iter,COL_DATAD,&datazap,COL_NOMZAP,&nom_v_zap,NUM_COLUMNS,&nomer,-1);

data->nom_v_zap=nom_v_zap;
data->data_v_zap.new_plus(datazap);
data->snanomer=nomer;

g_free(datazap);

}

/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  l_xrnnp_knopka(GtkWidget *widget,class l_xrnnp_data *data)
{
int nomer_v_sp=0;

int knop=atoi(gtk_widget_get_name(widget));
//g_print("l_xrnnp_knopka knop=%d\n",knop);
data->kl_shift=0;
switch (knop)
 {
  case FK2:
    if(l_xrnnp_v(&data->nnn_tv,0,data->window) == 0)
      l_xrnnp_create_list(data);
    return;  

  case SFK2:
    if(data->kolzap == 0)
      return;
    if(l_xrnnp_v(&data->nnn_tv,data->nom_v_zap,data->window) == 0)
      l_xrnnp_create_list(data);
    return;  

  case FK3:
    if(data->kolzap == 0)
      return;
    if(l_xrnnp_udzap(data) == 0)
      l_xrnnp_create_list(data);
    return;  
  
  case SFK3:
    if(l_xrnn_udgrz(data) == 0)
      l_xrnnp_create_list(data);
    return;     

  case FK4:
    if(l_xrnnp_p(&data->rkp,data->window) == 0)
     {
      data->datan.new_plus(data->rkp.datan.ravno());
      data->datak.new_plus(data->rkp.datak.ravno());   
     }
    else
     {
      data->datan.new_plus(data->datap.ravno());
      data->datak.new_plus("");   
    
     }
    l_xrnnp_create_list(data);
    return;  

  case FK5:
     l_xrnnp_vr(data);
    return;  

  case FK6:
    if(rnn_imp(1,data->window) == 0)
      l_xrnnp_create_list(data);
    return;  

  case Insert:

    if((nomer_v_sp=data->rkp.nomzap_v.find(data->nom_v_zap)) >= 0)
     data->rkp.nomzap_v.new_plus(0,nomer_v_sp);
    else
     data->rkp.nomzap_v.plus(data->nom_v_zap,-1);

    l_xrnnp_create_list(data);
     
    return;
        
  case FK10:
    gtk_widget_destroy(data->window);
    return;
 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   l_xrnnp_key_press(GtkWidget *widget,GdkEventKey *event,class l_xrnnp_data *data)
{

switch(event->keyval)
 {

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

  case GDK_KEY_Insert:
    g_signal_emit_by_name(data->knopka[Insert],"clicked");
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
void l_xrnnp_v_row(GtkTreeView *treeview,GtkTreePath *arg1,GtkTreeViewColumn *arg2,
class l_xrnnp_data *data)
{

g_signal_emit_by_name(data->knopka[SFK2],"clicked");



}

/*****************************/
/*Удаление записи            */
/*****************************/

int l_xrnnp_udzap(class l_xrnnp_data *data)
{
if(iceb_pbpds(data->data_v_zap.ravno(),data->window) != 0)
 return(1);

if(iceb_menu_danet(gettext("Удалить запись ? Вы уверены ?"),2,data->window) != 1)
 return(1);

char strsql[512];

sprintf(strsql,"delete from Reenn1 where nz=%d",data->nom_v_zap);

if(iceb_sql_zapis(strsql,0,0,data->window) != 0)
 return(1);


return(0);
}
/****************************/
/*Проверка записей          */
/*****************************/

int  l_xrnnp_prov_row(SQL_str row,class l_xrnnp_data *data)
{
if(data->rkp.metka_poi == 0)
 return(0);
//Полное сравнение
if(iceb_u_proverka(data->rkp.nnn.ravno(),row[2],0,0) != 0)
 return(1);

if(iceb_u_proverka(data->rkp.inn.ravno(),row[11],0,0) != 0)
 return(1);

if(iceb_u_proverka(data->rkp.nomdok.ravno(),row[15],0,0) != 0)
 return(1);

if(iceb_u_proverka(data->rkp.viddok.ravno(),row[22],0,0) != 0)
 return(1);

if(iceb_u_proverka(data->rkp.kodgr.ravno(),row[17],0,0) != 0)
 return(1);

if(data->rkp.podsistema.getdlinna() > 1)
 {
  class iceb_u_str metka_pod("");

  if(row[13][0] == '1')
   metka_pod.new_plus(ICEB_MP_MATU);
  if(row[13][0] == '2')
   metka_pod.new_plus(ICEB_MP_USLUGI);
  if(row[13][0] == '3')
   metka_pod.new_plus(ICEB_MP_UOS);
  if(row[13][0] == '4')
   metka_pod.new_plus(ICEB_MP_UKR);
  if(row[13][0] == '5')
   metka_pod.new_plus(gettext("ГК"));
  if(iceb_u_proverka(data->rkp.podsistema.ravno(),metka_pod.ravno(),0,0) != 0)
   return(1); 
 }





if(iceb_u_proverka(data->rkp.kodop.ravno(),row[20],0,0) != 0)
 return(1);

//Поиск образца в строке

if(iceb_u_proverka(data->rkp.naim_kontr.ravno(),row[12],4,0) != 0)
 return(1);

if(iceb_u_proverka(data->rkp.koment.ravno(),row[16],4,0) != 0)
 return(1);
 
if(data->rkp.datavnn.getdlinna() > 1)
 {
  short d,m,g;
  iceb_u_rsdat(&d,&m,&g,row[21],2);
  short d1,m1,g1;
  iceb_u_rsdat(&d1,&m1,&g1,data->rkp.datavnn.ravno(),1);
  if(iceb_u_sravmydat(d,m,g,d1,m1,g1) > 0)
   return(1);
 }   

if(data->rkp.datavnk.getdlinna() > 1)
 {
  short d,m,g;
  iceb_u_rsdat(&d,&m,&g,row[21],2);
  short d1,m1,g1;
  iceb_u_rsdat(&d1,&m1,&g1,data->rkp.datavnk.ravno(),1);
  if(iceb_u_sravmydat(d,m,g,d1,m1,g1) < 0)
   return(1);
 }   

if(data->rkp.metka_ins == 1) //*Только отмеченные
 if(data->rkp.nomzap_v.find(atoi(row[0])) < 0)
 return(1);

if(data->rkp.metka_ins == 2) //*Только не отмеченные\C5
 if(data->rkp.nomzap_v.find(atoi(row[0])) >= 0)
 return(1);

return(0);
}
/*************************************/
/*Распечатка записей                 */
/*************************************/
void l_xrnnp_rasp(class l_xrnnp_data *data)
{
/*Для налоговых накладных нужно определить тип периода для этого нужны две даты*/
/*при проверки дата конца если не заполнена будет устанолена на конец месяца первой даты*/
if(iceb_rsdatp(data->rkp.datan.ravno(),&data->rkp.datak,data->window) != 0)
 return;

data->rkp.pr_ras=1;
if(xrnn_rasp_r(&data->rkp,data->window) != 0)
 return;

iceb_rabfil(&data->rkp.imaf,&data->rkp.naim,data->window);

}
/*********************/
/*удаление нескольких записей*/
/*****************************/
int l_xrnn_udgrz(class l_xrnnp_data *data)
{



if(iceb_menu_danet(gettext("Удалить все записи? Вы уверены?"),2,data->window) != 1)
 return(1);



short dt,mt,gt;

iceb_u_poltekdat(&dt,&mt,&gt);


short dk=0;
short mk=0;
short gk=0;
iceb_u_rsdat(&dk,&mk,&gk,data->rkp.datak.ravno(),1);

if(iceb_parol(0,data->window) != 0)
 return(1);

class iceb_gdite_data gdite;
iceb_gdite(&gdite,0,data->window);

SQL_str row;
class SQLCURSOR cur;
float kolstr1=0;
char strsql[200];
int kolstr=0;
if((kolstr=cur.make_cursor(&bd,data->zapros.ravno())) < 0)
 {
  iceb_msql_error(&bd,__FUNCTION__,data->zapros.ravno(),data->window);
  return(1);
 }

short d=0;
short m=0;
short g=0;
short mz=0,gz=0;
while(cur.read_cursor(&row) != 0)
 {
//  strzag(LINES-1,0,kolstr,++kolstr1);
  iceb_pbar(gdite.bar,kolstr,++kolstr1);
  if(l_xrnnp_prov_row(row,data) != 0)
   continue;
  iceb_u_rsdat(&d,&m,&g,row[1],2);
  if(mz != m || gz != g)
   {
    if(iceb_pbpds(m,g,data->window) != 0)
     return(0);
    mz=m;
    gz=g;
   }
  sprintf(strsql,"delete from Reenn1 where nz=%s",row[0]);
  iceb_sql_zapis(strsql,1,0,data->window);

 } 

return(0);
}
/*************************************/
/*формирование Запроса о получении сведений с Единого реестра налоговых накладных*/
/**********************************************************************************/

void l_xrnnp_raspzp(class l_xrnnp_data *data)
{
char strsql[1024];
data->rkp.pr_ras=1;



class SQLCURSOR cur,cur1;
SQL_str row;
int kolstr=0;
if((kolstr=cur.make_cursor(&bd,data->zapros.ravno())) < 0)
 {
  iceb_msql_error(&bd,__FUNCTION__,data->zapros.ravno(),data->window);
  return;
 }




if(kolstr == 0)
 return;
class iceb_u_str nom_zapr("");

if(iceb_menu_vvod1(gettext("Введите номер запроса"),&nom_zapr,32,"",data->window) != 0)
 return;

class iceb_gdite_data gdite;
char imaf[64];

FILE *ff;
sprintf(imaf,"zps%d.lst",getpid());

if((ff=fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,data->window);
  return;
 }

int period_type=0;
char imaf_xml[64];
FILE *ff_xml;
iceb_rnfxml(&rek_zag_nn,data->window); /*чтение данных для xml файлов*/
if(iceb_openxml(data->rkp.datan.ravno(),data->rkp.datak.ravno(),imaf_xml,"J13","001","01",1,&period_type,&rek_zag_nn,&ff_xml,data->window) != 0)
 return;

//Заголовок файла
iceb_rnn_sap_xml(data->rkp.datan.ravno(),data->rkp.datak.ravno(),"J13","001",1,1,"J1300101.xsd",period_type,&rek_zag_nn,ff_xml);

class iceb_u_str edrpou("");
class iceb_u_str inn("");
if(iceb_sql_readkey("select kod,innn from Kontragent where kodkon='00'",&row,&cur1,data->window) == 1)
 {
  edrpou.new_plus(row[0]);
  inn.new_plus(row[1]);
 }

fprintf(ff_xml," <DECLARBODY>\n");
if(nom_zapr.getdlinna() > 1)
  fprintf(ff_xml,"   <HNUM>%s</HNUM>\n",nom_zapr.ravno());
fprintf(ff_xml,"   <HNAME>%s</HNAME>\n",iceb_u_filtr_xml(iceb_get_pnk("00",1,data->window)));
fprintf(ff_xml,"   <HTIN>%s</HTIN>\n",edrpou.ravno());
fprintf(ff_xml,"   <HNPDV>%s</HNPDV>\n",inn.ravno());
fprintf(ff_xml,"   <HKSTI>%s</HKSTI>\n",rek_zag_nn.kod_dpi.ravno());
/*название органа гос. нал. службы Украины*/
//fprintf(ff_xml,"   <HSTI>%s</HSTI>\n", );
/*електронный адрес*/
//fprintf(ff_xml,"   <HEMAIL>%s</HEMAIL>\n", );
 
float kolstr1=0;
class iceb_u_str nom_dok("");
class iceb_u_str nom_dok1("");
class iceb_u_str nom_dok2("");
char bros[512];
short dv,mv,gv;
iceb_zagolov(gettext("Запрос на получение сведений из Единого реестра налоговых накладных"),data->rkp.datan.ravno(),data->rkp.datak.ravno(),ff,data->window);
fprintf(ff,"%s:%s\n",gettext("Номер запроса"),nom_zapr.ravno());
fprintf(ff,"\
---------------------------------------------------------------------\n");
fprintf(ff,"\
  N   |Номер документа|Дата выписки|Вид док.|Индивидуальный нал.ном.|\n");
/*******
123456 123456789012345 123456789012 12345678   
***********/
fprintf(ff,"\
---------------------------------------------------------------------\n");


int nomer_str=0;
while(cur.read_cursor(&row) != 0)
 {
  iceb_pbar(gdite.bar,kolstr,++kolstr1);
  if(l_xrnnp_prov_row(row,data) != 0)
    continue;

  nom_dok.new_plus("");
  nom_dok1.new_plus("");
  nom_dok2.new_plus("");
    
  if(iceb_u_polen(row[2],&nom_dok,1,'/') != 0)
   nom_dok.new_plus(row[2]);
  else
   {
    memset(strsql,'\0',sizeof(strsql)); //обязательно
    iceb_u_polen(row[2],strsql,sizeof(strsql),2,'/');
    /*Если первый символ после косой не ноль то это код вида деятельности*/
    sprintf(bros,"%*.*s",iceb_u_kolbait(1,strsql),iceb_u_kolbait(1,strsql),strsql);
    if(atoi(bros) > 0)
     {
      nom_dok1.new_plus(bros);
      nom_dok2.new_plus(iceb_u_adrsimv(1,strsql));
     }    
    else
     nom_dok2.new_plus(strsql);
   }
  iceb_u_rsdat(&dv,&mv,&gv,row[21],2);

  nomer_str++;
      
  fprintf(ff_xml,"   <T1RXXXXG21 ROWNUM=\"%d\">%s</T1RXXXXG21>\n",nomer_str,nom_dok.ravno()); 
  if(nom_dok1.getdlinna() > 1)
   fprintf(ff_xml,"   <T1RXXXXG22 ROWNUM=\"%d\">%s</T1RXXXXG22>\n",nomer_str,nom_dok1.ravno()); 
  if(nom_dok2.getdlinna() > 1)
   fprintf(ff_xml,"   <T1RXXXXG23 ROWNUM=\"%d\">%s</T1RXXXXG23>\n",nomer_str,nom_dok2.ravno()); 
 
  fprintf(ff_xml,"   <T1RXXXXG3D ROWNUM=\"%d\">%02d%02d%04d</T1RXXXXG3D>\n",nomer_str,dv,mv,gv); 
 
  fprintf(ff_xml,"   <T1RXXXXG4S ROWNUM=\"%d\">%s</T1RXXXXG4S>\n",nomer_str,row[22]); 
 
  fprintf(ff_xml,"   <T1RXXXXG5 ROWNUM=\"%d\">%s</T1RXXXXG5>\n",nomer_str,row[11]); 
   
  fprintf(ff,"%6d %-*s %02d.%02d.%04d   %-*s %-*s %s\n",
  nomer_str,
  iceb_u_kolbait(15,row[2]),row[2],
  dv,mv,gv,
  iceb_u_kolbait(8,row[22]),row[22],
  iceb_u_kolbait(12,row[11]),row[11],
  row[12]);
  
 } 

fprintf(ff,"\
---------------------------------------------------------------------\n");


class iceb_fioruk_rk rukov;
iceb_fioruk(1,&rukov,data->window);
short dt,mt,gt;
iceb_u_poltekdat(&dt,&mt,&gt);

fprintf(ff_xml,"   <HFILL>%02d%02d%d</HFILL>\n",dt,mt,gt);
fprintf(ff_xml,"   <HBOS>%s</HBOS>\n",rukov.fio.ravno_filtr_xml());
fprintf(ff_xml,"   <HKBOS>%s</HKBOS>\n",rukov.inn.ravno());
fprintf(ff_xml," </DECLARBODY>\n");
fprintf(ff_xml,"</DECLAR>\n");

fclose(ff_xml);
iceb_podpis(ff,data->window);
fclose(ff);

class iceb_u_spisok imafil;
class iceb_u_spisok namefil;

imafil.plus(imaf);
namefil.plus(gettext("Запрос на получение сведений из Единого реестра налоговых накладных"));

iceb_ustpeh(imaf,3,data->window);

imafil.plus(imaf_xml);
namefil.plus(gettext("Запрос на получение сведений из Единого реестра налоговых накладных в формате xml"));

gdite.close();
iceb_rabfil(&imafil,&namefil,data->window);

}
/************************/
/*реестр для проверки*/
/**************************/
void l_xrnnp_reeprov(class l_xrnnp_data *data)
{
char strsql[1024];



class SQLCURSOR cur,cur1;
SQL_str row;
int kolstr=0;
if((kolstr=cur.make_cursor(&bd,data->zapros.ravno())) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  return;
 }


if(kolstr == 0)
 return;

class iceb_gdite_data gdite;
char imaf[64];
float kolstr1=0.;

sprintf(imaf,"rnnsvod%d",getpid());

class iceb_fopen fil;
if(fil.start(imaf,"w",data->window) != 0)
 return;

iceb_zagolov(gettext("Реестр полученных налоговых накладных для проверки"),data->rkp.datan.ravno(),data->rkp.datak.ravno(),fil.ff,data->window);

int nomer_str=0;
double itogo[3];
memset(itogo,'\0',sizeof(itogo));

fprintf(fil.ff,"\
----------------------------------------------------------------------------------------------------------------------------\n");

while(cur.read_cursor(&row) != 0)
 {
  iceb_pbar(gdite.bar,kolstr,++kolstr1);
  if(l_xrnnp_prov_row(row,data) != 0)
    continue;
  class iceb_u_str data_pol(iceb_u_datzap(row[1]));
  class iceb_u_str data_vip(iceb_u_datzap(row[21]));
  
  double suma_podok=atof(row[3])+atof(row[5])+atof(row[7])+atof(row[9]);
  double suma_beznds=suma_podok-atof(row[4])-atof(row[6])-atof(row[8])-atof(row[10]);
  double suma_nds=atof(row[4])+atof(row[6])+atof(row[8])+atof(row[10]);
  itogo[0]+=suma_podok;
  itogo[1]+=suma_beznds;
  itogo[2]+=suma_nds;
  
  fprintf(fil.ff,"|%-5d|%10s|%10s|%-*s|%-*s|%-*.*s|%-*s|%10.2f|%10.2f|%10.2f|\n",
  ++nomer_str,
  data_pol.ravno(),
  data_vip.ravno(),
  iceb_u_kolbait(10,row[2]),
  row[2],
  iceb_u_kolbait(6,row[22]),
  row[22],
  iceb_u_kolbait(30,row[12]),
  iceb_u_kolbait(30,row[12]),
  row[12],
  iceb_u_kolbait(12,row[11]),
  row[11],
  suma_podok,
  suma_beznds,
  suma_nds);
 }

fprintf(fil.ff,"\
----------------------------------------------------------------------------------------------------------------------------\n");

fprintf(fil.ff,"%*s|%10.2f|%10.2f|%10.2f|\n",
iceb_u_kolbait(90,gettext("Итого")),
gettext("Итого"),
itogo[0],
itogo[1],
itogo[2]);


iceb_podpis(fil.ff,data->window);

fil.end();  

class spis_oth oth;

oth.spis_imaf.plus(imaf);
oth.spis_naim.plus(gettext("Реестр полученных налоговых накладных для проверки"));

iceb_ustpeh(imaf,3,data->window);


gdite.close();
iceb_rabfil(&oth,data->window);

}

/***************************/
/*меню выбора распечтаки*/
/****************************/
void l_xrnnp_vr(class l_xrnnp_data *data)
{

class iceb_u_str titl;
class iceb_u_str zagolovok;
class iceb_u_spisok punkt_m;

titl.plus(gettext("Выбор"));


zagolovok.plus(gettext("Выберите нужное"));

punkt_m.plus(gettext("Распечатать реестр налоговых накладных"));//0
punkt_m.plus(gettext("Распечатать запрос на получение сведений из Единого реестра налоговых накладных"));//1
punkt_m.plus(gettext("Распечатать реестр налоговых накладных для проверки"));//2



int nomer=0;
nomer=iceb_menu_mv(&titl,&zagolovok,&punkt_m,nomer,data->window);
switch(nomer)
 {
 
  case 0:
  l_xrnnp_rasp(data);
   break;
 
  case 1:
   l_xrnnp_raspzp(data);
   break;

  case 2:
   l_xrnnp_reeprov(data);
   break;
 }
}
