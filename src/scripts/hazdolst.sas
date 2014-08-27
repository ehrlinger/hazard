%macro hazdolst;
/* %put &prefix; */
    
data _null_;
   infile "&prefix..lst" length=ll;
   length bfr $200;
   input @1 code $1. @;
   if code='$' then
      file log;
   else
      file print linesleft=lleft header=prthdr;
   bfrlen = ll-1;
   input bfr $varying200. bfrlen;
   select(code);
      when('+') do;
         link getcount;
         put +count bfr $varying200. bfrlen @;
      end;
      when('@') do;
         link getcount;
         put @count bfr $varying200. bfrlen @;
      end;
      when('<') 
         if lleft<input(bfr,4.) then
            put _page_;
         else;
      when('/') do;
         do i = 1 to input(bfr,4.);
            put;
         end;
      end;
      when(';') put bfr $varying200. bfrlen @;
      when('$') put bfr $varying200. bfrlen;
      otherwise error 'ERROR: Unrecognized line from print file.';
   end;
   return;
  getcount:
   i = index(bfr,',');
   count = input(substr(bfr,1,i-1),4.);
   bfr = substr(bfr,i+1);
   bfrlen = bfrlen-i;
   return;
  prthdr:
   put;
   return;
   run;
%mend;
