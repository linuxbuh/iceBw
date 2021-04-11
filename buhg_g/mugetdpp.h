/*$Id:$*/

class mugetdpp_str
 {
  public:
   /*реквизиты самого последнего прихода*/
   class iceb_u_str datap;
   double cena;
   double kolih;
   class iceb_u_str kodkontr;
   class iceb_u_str ei;
   class iceb_u_str shetu;
   int sklad;
   int nomkart;

   /*Последние приходы на всех складах*/
   class iceb_u_spisok dataps;
   class iceb_u_double cenas;
   class iceb_u_double kolihs;
   class iceb_u_spisok kodkontrs;
   class iceb_u_spisok eis;
   class iceb_u_spisok shetus;
   class iceb_u_int sklads;
   class iceb_u_int nomkarts;


   mugetdpp_str()
    {
     clear();
    }

   void clear()
    {
     datap.new_plus("");
     cena=0.;
     kolih=0.;
     kodkontr.new_plus("");
     ei.new_plus("");
     shetu.new_plus("");
     sklad=0;
     nomkart=0;
     
     dataps.free_class();
     cenas.free_class();  
     kolihs.free_class();
     kodkontrs.free_class();
     eis.free_class();
     shetus.free_class();
     sklads.free_class();
     nomkarts.free_class();
     
    }
 };
 