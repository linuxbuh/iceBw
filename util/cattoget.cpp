/*$Id:$*/
/*01.08.2013	28.12.2010	Белых А.И.	cattoget.c
Программа замены iceb_u_catgets и catgets на gettext во всех файлах с расширением .c
*/
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <iceb_libbuh.h>

void cattoget_fil(const char *imaf_from);
void cattoget_ud(class iceb_u_str *stroka,const char *obr);
void cattoget_uds(class iceb_u_str *stroka,int nomer,char simv);
void cattoget_vs(class iceb_u_str *stroka,int nomer,char simv);
void cattoget_udstr(class iceb_u_str *stroka,const char *obr);
void cattoget_uddo1z(class iceb_u_str *stroka);
void cattoget_udvsk(class iceb_u_str *stroka);

const char		*name_system={"iceb_print"};
const char            *version={"0.0"};
nl_catd fils;

int main(int argc,char **argv)
{



GDir *dirp;
if((dirp=g_dir_open("./",0,NULL)) == NULL)
 {

  printf("Немогу открыть каталог!\n%s\n",strerror(errno));
  return(1);
 }

const char *imafil_read;
class iceb_u_str rashir("");

while((imafil_read=g_dir_read_name(dirp)) != NULL)
 {
  if(iceb_u_polen(imafil_read,&rashir,2,'.') != 0)
   continue;

  if(iceb_u_SRAV("c",rashir.ravno(),0) != 0)
   continue;

  if(iceb_u_SRAV("cattoget",imafil_read,1) == 0)
   continue;
  if(iceb_u_SRAV("iceb_print",imafil_read,1) == 0)
   continue;

  cattoget_fil(imafil_read);

  printf("Файл-%s\n",imafil_read);


 }
g_dir_close(dirp);


return(0);
}

/********************************/
/*Открываем файл и делаем замены и выгружаем во временный файл 
который в конце переименовываем в исходный*/
/*********************************/
void cattoget_fil(const char *imaf_from)
{
char imaf_out[56];
char stroka[2048];
class iceb_u_str new_stroka("");
FILE *ff_out,*ff_from;

sprintf(imaf_out,"cattoget%d.tmp",getpid());


if((ff_from=fopen(imaf_from,"r")) == NULL)
 {
  printf("Ошибка открытия файла %s\n%d %s\n",imaf_from,errno,strerror(errno));
  return;
 }

if((ff_out=fopen(imaf_out,"w")) == NULL)
 {
  printf("Ошибка открытия файла %s\n%d %s\n",imaf_out,errno,strerror(errno));
  return;
 }
//int nomer_str=0;
int metka_window_default_size=0;
class iceb_u_str pred_str(""); /*запоминается предыдущая строка*/
while(fgets(stroka,sizeof(stroka),ff_from) != NULL)
 {
  if(iceb_u_SRAV("//GtkWidget",stroka,1) == 0)
   continue;  
//  printf("%s-%d %s\n",imaf_from,++nomer_str,stroka);  
  new_stroka.new_plus(stroka);

//data.bar=gtk_progress_bar_new();
//gtk_progress_bar_set_show_text(GTK_PROGRESS_BAR(data.bar),TRUE);

  if(iceb_u_strstrm(stroka,"data.bar=gtk_progress_bar_new()") == 1) /*Найден образец*/
   {
    fprintf(ff_out,"%s",new_stroka.ravno());
    fprintf(ff_out,"gtk_progress_bar_set_show_text(GTK_PROGRESS_BAR(data.bar),TRUE); /*Показывать текст в строке*/\n");
    continue;
   }

//gtk_signal_connect(GTK_OBJECT(data.risunok),"event",GTK_SIGNAL_FUNC(xdkdok_draw),&data); /*для gtk2.0*/ 
//gtk_signal_connect(GTK_OBJECT(data.risunok),"draw",GTK_SIGNAL_FUNC(xdkdok_draw),&data);   /*для gtk3.0*/
  if(iceb_u_strstrm(stroka,"gtk_signal_connect(GTK_OBJECT(data.risunok),\"event\",GTK_SIGNAL_FUNC(xdkdok_draw),&data);") == 1) /*Найден образец*/
   {
    iceb_u_zvstr(&new_stroka,"gtk_signal_connect(GTK_OBJECT(data.risunok),\"event\",GTK_SIGNAL_FUNC(xdkdok_draw),&data);","g_signal_connect(data.risunok,\"draw\",G_CALLBACK(xdkdok_draw),&data);");
    /*выводим здесь так как дальше пропадает одна скобка почемуто*/
    fprintf(ff_out,"%s",new_stroka.ravno());
    continue;
   }
//gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
//gtk_signal_connect (GTK_OBJECT (item), "activate",GtkSignalFunc(mater_get_pm0),&data.poisk->mcena);
//g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);


  if(iceb_u_strstrm(stroka,"gtk_signal_connect(GTK_OBJECT(") == 1 \
  || iceb_u_strstrm(stroka,"gtk_signal_connect (GTK_OBJECT (") == 1 \
  || iceb_u_strstrm(stroka,"gtk_signal_connect( GTK_OBJECT(") == 1 \
  || iceb_u_strstrm(stroka,"gtk_signal_connect(GTK_OBJECT (") == 1) /*Найден образец*/
   {
    iceb_u_zvstr(&new_stroka,"gtk_signal_connect(GTK_OBJECT(","g_signal_connect(");
    iceb_u_zvstr(&new_stroka,"gtk_signal_connect (GTK_OBJECT (","g_signal_connect(");
    iceb_u_zvstr(&new_stroka,"gtk_signal_connect( GTK_OBJECT(","g_signal_connect(");
    iceb_u_zvstr(&new_stroka,"gtk_signal_connect(GTK_OBJECT (","g_signal_connect(");

    cattoget_uds(&new_stroka,1,')');

   }

  if(iceb_u_strstrm(stroka,"gtk_signal_connect_after(GTK_OBJECT(") == 1)
   {
    iceb_u_zvstr(&new_stroka,"gtk_signal_connect_after(GTK_OBJECT(","g_signal_connect_after(");
    cattoget_uds(&new_stroka,1,')');
   }  


//gtk_signal_connect_object( GTK_OBJECT( ok_button ), "clicked",GTK_SIGNAL_FUNC( gtk_widget_destroy ),dialog);
//g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
  if(iceb_u_strstrm(stroka,"gtk_signal_connect_object") == 1 )
   {
    iceb_u_zvstr(&new_stroka,"gtk_signal_connect_object","g_signal_connect");
    cattoget_ud(&new_stroka,"GTK_OBJECT(");

   }
  if(iceb_u_strstrm(stroka,"GTK_SIGNAL_FUNC") == 1) /*Найден образец*/
     iceb_u_zvstr(&new_stroka,"GTK_SIGNAL_FUNC","G_CALLBACK");
  if(iceb_u_strstrm(stroka,"GtkSignalFunc(") == 1) /*Найден образец*/
     iceb_u_zvstr(&new_stroka,"GtkSignalFunc(","G_CALLBACK(");

//gtk_object_set_user_data(GTK_OBJECT(data.entry[E_KOD]),(gpointer)E_KOD);
//gtk_widget_set_name(data.entry[E_KOD],iceb_u_inttochar(E_KOD));/******************/

  if(iceb_u_strstrm(stroka,"gtk_object_set_user_data(GTK_OBJECT(") == 1) /*Найден образец*/
   {
    iceb_u_zvstr(&new_stroka,"gtk_object_set_user_data(GTK_OBJECT(","gtk_widget_set_name(");

    if(iceb_u_strstrm(stroka,"(gpointer)") == 1) /*Найден образец*/
     iceb_u_zvstr(&new_stroka,"(gpointer)","iceb_u_inttochar(");
   
    cattoget_uds(&new_stroka,1,')');
    cattoget_vs(&new_stroka,1,')');

   }   

  if(iceb_u_strstrm(stroka,"gtk_window_set_default_size") == 1) /*Найден образец*/
   metka_window_default_size=1;

//gtk_window_set_default_size (GTK_WINDOW  (data.window),400,-1);
  if(iceb_u_strstrm(stroka,"gtk_window_set_default_size (GTK_WINDOW  (data.window),400,-1);") == 1) /*Найден образец*/
    iceb_u_zvstr(&new_stroka,"gtk_window_set_default_size (GTK_WINDOW  (data.window),400,-1);","gtk_window_set_default_size (GTK_WINDOW  (data.window),400,400);");
   
  //gtk_widget_set_usize(GTK_WIDGET(data.sw),400,300);
//gtk_window_set_default_size(GTK_WINDOW(data.window),400,300);
  if(iceb_u_strstrm(stroka,"gtk_widget_set_usize") == 1) /*Найден образец*/
   {
    pred_str.new_plus(stroka);
    if(metka_window_default_size > 0)
     continue;
    iceb_u_zvstr(&new_stroka,"gtk_widget_set_usize","gtk_window_set_default_size");
    iceb_u_zvstr(&new_stroka,"GTK_WIDGET","GTK_WINDOW");
    iceb_u_zvstr(&new_stroka,"data.sw","data.window");
 
    continue;
   }
//gtk_widget_set_usize(GTK_WIDGET(data.view),400,300);
  if(iceb_u_strstrm(stroka,"gtk_widget_set_usize(GTK_WIDGET(data.view)") == 1) /*Найден образец*/
   continue;

  if(iceb_u_strstrm(stroka,"GDK_F") == 1) /*Найден образец*/
   iceb_u_zvstr(&new_stroka,"GDK_F","GDK_KEY_F");

  if(iceb_u_strstrm(stroka,"GDK_plus") == 1) /*Найден образец*/
   iceb_u_zvstr(&new_stroka,"GDK_plus","GDK_KEY_plus");

  if(iceb_u_strstrm(stroka,"GDK_KP_Add") == 1) /*Найден образец*/
   iceb_u_zvstr(&new_stroka,"GDK_KP_Add","GDK_KEY_KP_Add");

  if(iceb_u_strstrm(stroka,"GDK_Escape") == 1) /*Найден образец*/
   iceb_u_zvstr(&new_stroka,"GDK_Escape","GDK_KEY_Escape");

  if(iceb_u_strstrm(stroka,"GDK_KP_0") == 1) /*Найден образец*/
   iceb_u_zvstr(&new_stroka,"GDK_KP_0","GDK_KEY_KP_0");

  if(iceb_u_strstrm(stroka,"GDK_Page_Down") == 1) /*Найден образец*/
   iceb_u_zvstr(&new_stroka,"GDK_Page_Down","GDK_KEY_Page_Down");

  if(iceb_u_strstrm(stroka,"GDK_Page_Up") == 1) /*Найден образец*/
   iceb_u_zvstr(&new_stroka,"GDK_Page_Up","GDK_KEY_Page_Up");

  if(iceb_u_strstrm(stroka,"GDK_minus") == 1) /*Найден образец*/
   iceb_u_zvstr(&new_stroka,"GDK_minus","GDK_KEY_minus");

  if(iceb_u_strstrm(stroka,"GDK_KP_Subtract") == 1) /*Найден образец*/
   iceb_u_zvstr(&new_stroka,"GDK_KP_Subtract","GDK_KEY_KP_Subtract");

  if(iceb_u_strstrm(stroka,"GDK_KP_Insert") == 1) /*Найден образец*/
   iceb_u_zvstr(&new_stroka,"GDK_KP_Insert","GDK_KEY_KP_Insert");

  if(iceb_u_strstrm(stroka,"GDK_Insert") == 1) /*Найден образец*/
   iceb_u_zvstr(&new_stroka,"GDK_Insert","GDK_KEY_Insert");

/************************************ Нет этого в gtk2.0 ***********************************/
//GtkWidget *hbox = gtk_hbox_new (FALSE, 1);
//GtkWidget *hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL,1);

  if(iceb_u_strstrm(stroka,"gtk_hbox_new") == 1) /*Найден образец*/
   {
    iceb_u_zvstr(&new_stroka,"gtk_hbox_new","gtk_box_new");
    iceb_u_zvstr(&new_stroka,"FALSE","GTK_ORIENTATION_HORIZONTAL");
    iceb_u_zvstr(&new_stroka,"TRUE","GTK_ORIENTATION_HORIZONTAL");
   }
  if(iceb_u_strstrm(stroka,"gtk_vbox_new") == 1) /*Найден образец*/
   {
    iceb_u_zvstr(&new_stroka,"gtk_vbox_new","gtk_box_new");
    iceb_u_zvstr(&new_stroka,"FALSE","GTK_ORIENTATION_VERTICAL");
    iceb_u_zvstr(&new_stroka,"TRUE","GTK_ORIENTATION_VERTICAL");
   }

  if(iceb_u_strstrm(stroka,"data->window->window") == 1) /*Найден образец*/
   iceb_u_zvstr(&new_stroka,"data->window->window","gtk_widget_get_window(data->window)");
  if(iceb_u_strstrm(stroka,"data.window->window") == 1) /*Найден образец*/
   iceb_u_zvstr(&new_stroka,"data.window->window","gtk_widget_get_window(data.window)");

//gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK2]),"clicked");
//g_signal_emit_by_name(data->knopka[FK2],"clicked");
  if(iceb_u_strstrm(stroka,"gtk_signal_emit_by_name(") == 1) /*Найден образец*/
   {
    iceb_u_zvstr(&new_stroka,"gtk_signal_emit_by_name(","g_signal_emit_by_name(");
    cattoget_ud(&new_stroka,"GTK_OBJECT(");
   }

//int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//int knop=atoi(gtk_widget_get_name(widget));

  if(iceb_u_strstrm(stroka,"GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)))") == 1) /*Найден образец*/
   iceb_u_zvstr(&new_stroka,"GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)))","atoi(gtk_widget_get_name(widget))");

//GtkTooltips *tooltips[KOL_F_KL];
  if(iceb_u_strstrm(stroka,"GtkTooltip") == 1) /*Найден образец*/
   {   
    pred_str.new_plus(stroka);
    continue;
   }
//gtk_tooltips_set_tip(tooltips[FK2],data.knopka[FK2],gettext("Ввод новой записи"),NULL);
//gtk_widget_set_tooltip_text(data.knopka[FK2],gettext("Ввод новой записи"));
  if(iceb_u_strstrm(stroka,"gtk_tooltips_set_tip(") == 1) /*Найден образец*/
   {
    iceb_u_zvstr(&new_stroka,"gtk_tooltips_set_tip(","gtk_widget_set_tooltip_text(");
   
    /*Удалить до запятой*/
    cattoget_uddo1z(&new_stroka);

    cattoget_udstr(&new_stroka,",NULL");
   }

//tooltips[FK2]=gtk_tooltips_new();
  if(iceb_u_strstrm(stroka,"gtk_tooltips_new()") == 1) /*Найден образец*/
   {
    pred_str.new_plus(stroka);
    continue;
   }
//data.entry[E_KOD] = gtk_entry_new_with_max_length (4);
//gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_NAIM]),80);
  if(iceb_u_strstrm(stroka,"gtk_entry_new_with_max_length") == 1) /*Найден образец*/
   {
    iceb_u_zvstr(&new_stroka,"gtk_entry_new_with_max_length","gtk_entry_new");

    class iceb_u_str p1;
    class iceb_u_str p2;
    class iceb_u_str p3;
    iceb_u_polen(stroka,&p1,1,'=');
    iceb_u_polen(stroka,&p2,2,'(');
    iceb_u_polen(p2.ravno(),&p3,1,')');
    /*удаляем цифру в скобках*/
    cattoget_udvsk(&new_stroka);
  
    fprintf(ff_out,"%s",new_stroka.ravno()); /*выводим строку потому что после неё должна быть новая строка*/
    
    new_stroka.new_plus("gtk_entry_set_max_length(GTK_ENTRY(");
    new_stroka.plus(p1.ravno());    
    new_stroka.plus("),",p3.ravno());
    new_stroka.plus(");\n");

    fprintf(ff_out,"%s",new_stroka.ravno());

    pred_str.new_plus(stroka);
    continue;    
   }


//gtk_entry_select_region(GTK_ENTRY(widget),0,text->getdlinna());
//gtk_editable_select_region(GTK_EDITABLE(widget),0,text->getdlinna());
  if(iceb_u_strstrm(stroka,"gtk_entry_select_region(GTK_ENTRY(") == 1) /*Найден образец*/
   iceb_u_zvstr(&new_stroka,"gtk_entry_select_region(GTK_ENTRY(","gtk_editable_select_region(GTK_EDITABLE(");

//gtk_idle_add((GtkFunction)glkniw_r1,&data);
//g_idle_add((GSourceFunc)glkniw_r1,&data);
  if(iceb_u_strstrm(stroka,"gtk_idle_add") == 1) /*Найден образец*/
   {
    iceb_u_zvstr(&new_stroka,"gtk_idle_add","g_idle_add");
    iceb_u_zvstr(&new_stroka,"GtkFunction","GSourceFunc");
   }

//gtk_progress_bar_set_bar_style(GTK_PROGRESS_BAR(data.bar),GTK_PROGRESS_CONTINUOUS);
  if(iceb_u_strstrm(stroka,"gtk_progress_bar_set_bar_style") == 1) /*Найден образец*/
   {
    pred_str.new_plus(stroka);
    continue;
   }
//gtk_progress_bar_set_orientation(GTK_PROGRESS_BAR(data.bar),GTK_PROGRESS_LEFT_TO_RIGHT);
  if(iceb_u_strstrm(stroka,"gtk_progress_bar_set_orientation") == 1) /*Найден образец*/
   {
    pred_str.new_plus(stroka);
    continue;
   }
//gtk_entry_set_position(GTK_ENTRY(data->entry),pozic);
//gtk_editable_set_position(GTK_EDITABLE(data->entry),pozic);
  if(iceb_u_strstrm(stroka,"gtk_entry_set_position(GTK_ENTRY(") == 1) /*Найден образец*/
   iceb_u_zvstr(&new_stroka,"gtk_entry_set_position(GTK_ENTRY(","gtk_editable_set_position(GTK_EDITABLE(");

//group=gtk_radio_button_group(GTK_RADIO_BUTTON(data.radiobutton[0]));
//group=gtk_radio_button_get_group(GTK_RADIO_BUTTON(data.radiobutton[0]));
  if(iceb_u_strstrm(stroka,"gtk_radio_button_group") == 1) /*Найден образец*/
   iceb_u_zvstr(&new_stroka,"gtk_radio_button_group","gtk_radio_button_get_group");








/*******************************************************************************************/
/************************************ Нет этого в gtk2.0 ***********************************/
/********************************************************************************************/
//GtkWidget *hbox = gtk_hbox_new (FALSE, 1);
//GtkWidget *hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL,1);

  if(iceb_u_strstrm(stroka,"gtk_hbox_new") == 1) /*Найден образец*/
   {
    class iceb_u_str p1;
    class iceb_u_str p2;
    char str_tmp[1024];        
    memset(str_tmp,'\0',sizeof(str_tmp));

    iceb_u_polen(stroka,&p1,1,'=');
    if(iceb_u_polen(p1.ravno(),&p2,2,'*') != 0)
     p2.new_plus(p1.ravno());

    int metka_sk=0;    

    if(iceb_u_strstrm(pred_str.ravno(),"for(") == 1) /*Найден образец*/
     {
      metka_sk=1;
      fprintf(ff_out," {\n");
     }
    iceb_u_zvstr(&new_stroka,"gtk_hbox_new","gtk_box_new");

    if(iceb_u_strstrm(stroka,"FALSE") == 1) /*Найден образец*/
     {
      iceb_u_zvstr(&new_stroka,"FALSE","GTK_ORIENTATION_HORIZONTAL");
      sprintf(str_tmp,"gtk_box_set_homogeneous (GTK_BOX(%s),FALSE); //Устанваливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет",p2.ravno());
     }
    if(iceb_u_strstrm(stroka,"TRUE") == 1) /*Найден образец*/
     {
      iceb_u_zvstr(&new_stroka,"TRUE","GTK_ORIENTATION_HORIZONTAL");
      sprintf(str_tmp,"gtk_box_set_homogeneous(GTK_BOX(%s),TRUE); //Устанваливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет",p2.ravno());
     }
    fprintf(ff_out,"%s",new_stroka.ravno());

    if(str_tmp[0] != '\0')
      fprintf(ff_out,"%s\n",str_tmp);

    if(metka_sk == 1)
      fprintf(ff_out," }\n");
          
    pred_str.new_plus(stroka);
    continue;    
 
   }
 
  if(iceb_u_strstrm(stroka,"gtk_vbox_new") == 1) /*Найден образец*/
   {
    /*В цикле не создаётся*/
    class iceb_u_str p1;
    class iceb_u_str p2;
    char str_tmp[1024];        
    memset(str_tmp,'\0',sizeof(str_tmp));

    iceb_u_polen(stroka,&p1,1,'=');
    if(iceb_u_polen(p1.ravno(),&p2,2,'*') != 0)
     p2.new_plus(p1.ravno());

    iceb_u_zvstr(&new_stroka,"gtk_vbox_new","gtk_box_new");
    if(iceb_u_strstrm(stroka,"FALSE") == 1) /*Найден образец*/
     {
      iceb_u_zvstr(&new_stroka,"FALSE","GTK_ORIENTATION_VERTICAL");
      sprintf(str_tmp,"gtk_box_set_homogeneous (GTK_BOX(%s),FALSE); //Устанваливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет",p2.ravno());
     }
    if(iceb_u_strstrm(stroka,"TRUE") == 1) /*Найден образец*/
     {
      iceb_u_zvstr(&new_stroka,"TRUE","GTK_ORIENTATION_VERTICAL");
      sprintf(str_tmp,"gtk_box_set_homogeneous (GTK_BOX(%s),TRUE); //Устанваливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет",p2.ravno());
     }

    fprintf(ff_out,"%s",new_stroka.ravno());

    if(str_tmp[0] != '\0')
      fprintf(ff_out,"%s\n",str_tmp);

    pred_str.new_plus(stroka);
    continue;    
   }

//GtkWidget *separator1=gtk_hseparator_new();
  if(iceb_u_strstrm(stroka,"gtk_hseparator_new();") == 1) /*Найден образец*/
    iceb_u_zvstr(&new_stroka,"gtk_hseparator_new();","gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);");

  if(iceb_u_strstrm(stroka,"gtk_vseparator_new();") == 1) /*Найден образец*/
    iceb_u_zvstr(&new_stroka,"gtk_vseparator_new();","gtk_separator_new(GTK_ORIENTATION_VERTICAL);");

//data.vert_panel=gtk_vpaned_new();
//data.vert_panel=gtk_paned_new(GTK_ORIENTATION_VERTICAL);
  if(iceb_u_strstrm(stroka,"gtk_vpaned_new();") == 1) /*Найден образец*/
    iceb_u_zvstr(&new_stroka,"gtk_vpaned_new();","gtk_paned_new(GTK_ORIENTATION_VERTICAL);");

  if(iceb_u_strstrm(stroka,"gtk_hpaned_new();") == 1) /*Найден образец*/
    iceb_u_zvstr(&new_stroka,"gtk_hpaned_new();","gtk_paned_new(GTK_ORIENTATION_HORIZONTAL);");

/*********************************************************************************************/
/************************************************************************************************/

  fprintf(ff_out,"%s",new_stroka.ravno());

  pred_str.new_plus(stroka);

  memset(stroka,'\0',sizeof(stroka));
 }
 
fclose(ff_out);
fclose(ff_from);
 
rename(imaf_out,imaf_from);

}
/********************************/
/*удаляем цифру в скобках*/
/****************************/
void cattoget_udvsk(class iceb_u_str *stroka)
{
int dlina=stroka->getdlinna()+1;
char stroka1[dlina];
memset(stroka1,'\0',sizeof(stroka1));
int nomer_str=0;
int metkaz=0;
for(int ii=0; ii < stroka->getdlinna(); ii++)
 {
  if(stroka->ravno()[ii] == '(')
   {
    stroka1[nomer_str++]=stroka->ravno()[ii];
    metkaz=1;
   }
  if(stroka->ravno()[ii] == ')')
   {
    for(; ii < stroka->getdlinna(); ii++)
      stroka1[nomer_str++]=stroka->ravno()[ii];
    break;
   }

  if(metkaz == 1)
   continue;

  stroka1[nomer_str++]=stroka->ravno()[ii];
 }
stroka->new_plus(stroka1);
}

/********************************/
/*от ( до первой запятой удаляем*/
/****************************/
void cattoget_uddo1z(class iceb_u_str *stroka)
{
int dlina=stroka->getdlinna()+1;
char stroka1[dlina];
memset(stroka1,'\0',sizeof(stroka1));
int nomer_str=0;
int metkaz=0;
for(int ii=0; ii < stroka->getdlinna(); ii++)
 {
  
  if(stroka->ravno()[ii] == '(')
   {
    metkaz=1;
    stroka1[nomer_str++]=stroka->ravno()[ii];
    continue;
   }
  if(stroka->ravno()[ii] == ',')
   {
    ii++;
    for(; ii < stroka->getdlinna(); ii++)
      stroka1[nomer_str++]=stroka->ravno()[ii];
    break;
   }
  if(metkaz == 1)
   continue;

  stroka1[nomer_str++]=stroka->ravno()[ii];

 }
stroka->new_plus(stroka1);

}

/*********************************/
/*Удалить цепочку символов*/
/*********************************/
void cattoget_udstr(class iceb_u_str *stroka,const char *obr)
{
int dlina=stroka->getdlinna()+1;
char stroka1[dlina];
memset(stroka1,'\0',sizeof(stroka1));
int nomer_str=0;
for(int ii=0; ii < stroka->getdlinna(); ii++)
 {
  if(iceb_u_SRAV(&stroka->ravno()[ii],obr,1) == 0)
   {
    ii+=strlen(obr);
   }

  stroka1[nomer_str++]=stroka->ravno()[ii];

 }
stroka->new_plus(stroka1);
}

/**********************************************/
/*Удаляет символ встетившийся н-й раз*/
/****************************************/
void cattoget_uds(class iceb_u_str *stroka,int nomer,char simv)
{
int dlina=stroka->getdlinna()+1;
char stroka1[dlina];
memset(stroka1,'\0',sizeof(stroka1));
int nomer_str=0;
int nomer_simv=0;
for(int ii=0; ii < stroka->getdlinna(); ii++)
 {
  if(stroka->ravno()[ii] == simv)
   {
    nomer_simv++;
    if(nomer_simv == nomer)
     {
      continue;
     }
   }
  stroka1[nomer_str++]=stroka->ravno()[ii];
 }

stroka->new_plus(stroka1);

}

/**********************************************/
/*вставить такойже символ перед встетивмся н-й раз*/
/****************************************/
void cattoget_vs(class iceb_u_str *stroka,int nomer,char simv)
{
int dlina=stroka->getdlinna()+2;
char stroka1[dlina];
memset(stroka1,'\0',sizeof(stroka1));
int nomer_str=0;
int nomer_simv=0;
for(int ii=0; ii < stroka->getdlinna(); ii++)
 {
  if(stroka->ravno()[ii] == simv)
   {
    nomer_simv++;
    if(nomer_simv == nomer)
      stroka1[nomer_str++]=simv;
   }
  stroka1[nomer_str++]=stroka->ravno()[ii];
 }
stroka->new_plus(stroka1);
}




/***********************************/
/*Удаляет заданный образец и перувую правую скобку после образца*/
/****************************************************************/
void cattoget_ud(class iceb_u_str *stroka,const char *obr)
{
int dlina=(stroka->getdlinna()+strlen(obr))*2+1;
int dlina_obr=strlen(obr);
char stroka1[dlina];
memset(stroka1,'\0',sizeof(stroka1));

int nomer_str=0;
int metka_ud=0;
int metka_sk=0;
for(int ii=0; ii < stroka->getdlinna(); ii++)
 {
  if(iceb_u_SRAV(&stroka->ravno()[ii],obr,1) == 0)
   {
    ii+=dlina_obr-1;
    metka_ud=1;    
    continue;
   }

  if(metka_ud == 1 && metka_sk == 0 && stroka->ravno()[ii] ==')')
   {
    metka_sk=1;
    continue; 
   }    
  stroka1[nomer_str++]=stroka->ravno()[ii];
 }
stroka->new_plus(stroka1);
}
