#include<iostream>
#include<ctime>
#include<string>
#include<sstream>
#include"stdio.h"

using namespace std;
#include"moonstuff.h"

void dispDate(string tag,calendar c);//displays the date calculated


// The number of hours between NEW/FULL phases is OFFSET + DELTA
// We will just store DELTA so we can use bytes
#define OFFSET 300

#define DECL "static const int" 



void nearestJH(calendar &c) {

  double JD= double(floor(c.jh+0.5))/24.0+ (1.0/24.0/60.0/60.0); 
  
  c=JDtoCalendar(JD);


}

int main()
{	
	
 
  calendar c;

 
  c.dd=22;
  c.mm=7;
  c.yy=2016;
 
  FILE *fil=fopen("progmem.h","w");


  // now calculate the phase index for a New Moon  before this date 
  MYDOUBLE k=floor(cal2Kish(c))-1;

  // Convert back to a calendar format. This should be the date of a new moon.

  c=calcNewMoonPhaseK(k);    

  dispDate(" raw :",c);

  nearestJH(c);

  dispDate("nint :",c);

  // Write out to Arduino file 
  fprintf(fil,"%s yy_ref=%d;\n",DECL,c.yy);
  fprintf(fil,"%s mm_ref=%d;\n",DECL,c.mm);
  fprintf(fil,"%s dd_ref=%d;\n",DECL,c.dd);
  fprintf(fil,"%s hr_ref=%d;\n",DECL,c.hr);
  fprintf(fil,"%s _dfh_off=%d;\n",DECL,OFFSET);        


  fprintf(fil,"const byte _djh[] PROGMEM = {");      

  long J_hrs;
  long J_hrs_last=0;
  int delta=0;

  // Julian hours of last phase
  J_hrs_last=c.jh;


  // 
  for(int i=0;i<1200;i++) {
    if (i>0) fprintf(fil,",");
    if (i%20 ==0) fprintf(fil,"\n");
    c=calcFullMoonPhaseK(k+i+0.5);    
    nearestJH(c);
    dispDate("Full :",c);
    J_hrs=c.jh;
 
    int delta=J_hrs-J_hrs_last-OFFSET;
    fprintf(fil,"%d,",delta);
    J_hrs_last=J_hrs;
    c=calcNewMoonPhaseK(k+i+1.0);
    nearestJH(c);
    dispDate(" New :",c);    
    J_hrs=c.jh;
    delta=J_hrs-J_hrs_last-OFFSET;
    J_hrs_last=J_hrs;
    fprintf(fil,"%d",delta);
  }
  
  fprintf(fil,"};\n");
  fclose(fil);

  return 0;
}


void dispDate(string tag,calendar c)
{
  cout<<tag << " "  <<c.dd<<"/"<<c.mm<<"/"<<c.yy<<"  "<<c.hr<<":"<<c.min<<":"<<c.sec<<   endl;
}



