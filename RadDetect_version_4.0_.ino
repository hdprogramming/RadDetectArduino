#include <EEPROM.h>

//hdScience by hakki dumlupinar
//Compatible with the Arduino IDE 1.0

#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <Adafruit_ADS1015.h>


boolean RunProg,calibrated,Tone=true,menuon=false;
unsigned int i,cnt,t=0,t1=0,t2=0,freq=1000;
int eaddr;
String progad[5];
float Dly0,Dly1;
float vMax,vMin,mVolt,mVolt2,Vpp;
float thres=0,thmax,thmin;
int prog,sprog=-1,val,Max,MaxProg;
boolean updlcd; 
Adafruit_ADS1115 ads(0x48);

 int16_t adc0;  // we read from the ADC, we have a sixteen bit integer as a result
LiquidCrystal_I2C lcd(0x3f,16,2);  

void setup()
{
  
  Serial.begin(115200);
  eaddr=3;
  
pinMode(LED_BUILTIN, OUTPUT);

  lcd.init(); 
      Dly0=2;
      Dly1=150;             
   // Print a message to the LCD.
  lcd.backlight();
 thmax=100;
 thmin=0.5;

  ads.begin();
  progad[0]="Destination";
  progad[1]="PhotoRadDetect";
  progad[2]="IONDetect";
  progad[3]="Thresval()";
  progad[4]="Calibrate";
  progad[5]="Tone Options";

  MaxProg=sizeof(progad)/sizeof(String);
  if(prog==0)
  {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("RadDetect v4.0");
    lcd.setCursor(0,1);
    lcd.print("Y/A Butona basin");
     EEPROM.get(eaddr, thres);
     EEPROM.get(eaddr+sizeof(thres),freq);
     EEPROM.get(eaddr+sizeof(freq)+sizeof(thres),Tone);
    }
 menuon=true;
} 

   void loop()
{
  mctrl();
  prun();
  }
byte buttonsens()
{
  val=analogRead(A0);
  if(val>800)
  {
    return 0;
    }
if(val>700 && val<800)
{
    return 1;
    }
  if(val<700 && val>600)
{
   return 2;
   }
    if(val<600 && val>500)
{
 
  return 3;
    }
if(val<500)
{
  return 4;
  }
  
  }
void mctrl()
{
  if(buttonsens()==4&&prog!=0) //Ext Button
  {
    
    RunProg=false;
    menuon=true;
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Program:"+ String(prog));
    lcd.setCursor(0,1);
    lcd.print("> "+ String(progad[prog]));
    sprog=0;
    if(prog==3)
    {
       EEPROM.put(eaddr, thres);
            }
                         if(prog==5)
            {
               EEPROM.put(eaddr+sizeof(thres),freq);
              EEPROM.put(eaddr+sizeof(freq)+sizeof(thres),Tone);
              }
    delay(Dly1); 
    };
    if(buttonsens()==3&&prog!=0) //Ok Button
  {
    ResetVal();
    RunProg=true;
    menuon=false;
    updlcd=true;
   
    delay(Dly1); 
    };
  if(menuon)
  {
       if(buttonsens()==1) //Up Button
    {
      RunProg=false;
    
      bip();
    
   
    //Taşma engelleme kodu
      prog+=1;
      if(prog>MaxProg)
    {
      prog=1;
      }
      
    //Program Adı yazdırma
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Program:"+ String(prog));
    lcd.setCursor(0,1);
    lcd.print("> "+ String(progad[prog]));
     delay(Dly1); 
      }
      if(buttonsens()==2) //DownButton
      {
        RunProg=false;
    
   
     bip();
     
    //Reset end
    //Taşma engelleme kodu
      prog-=1;
      if(prog<1)
    {
      prog=MaxProg;
      }
      
    //Program Adı yazdırma
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Program:"+ String(prog));
    lcd.setCursor(0,1);
    lcd.print("> "+ String(progad[prog]));
    delay(Dly1);
        }
        
  }
}
 void ResetVal()
   {
     i=0;
     t=0;
     t1=0;
     t2=0;
     cnt=0;
     calibrated=false;
    }     
                                 
void prun()
{
  if(RunProg)
  {
     switch(prog){
  case 1:
          calib(1000);
          RadSens();
          break;
    
    case 2:
          RadSens2(); 
          break;
    case 3:
          thresval();
          break;
    case 4:
         calib(1000);
          break;
    case 5:
          toneopt();
          break;
   
    }   
  }
  }
  void calib(float t)
{
  if(calibrated==false)
  {
  vMin=5000;
  vMax=0;
   //Calibration time t is ms
  lcd.setCursor(0,0);
  lcd.clear();
  lcd.print("Calibrating..."); 
  if(Tone==true)
  {
  tone(3,freq,100);
  }
   else{
    noTone(3);
    }
   delay(300);
       for(i=0;i<t;i+=Dly0){
       adc0 = ads.readADC_SingleEnded(0);
   mVolt = (adc0 * 0.1875);
  if (mVolt>vMax){
    vMax=mVolt;
        }
        if(mVolt<vMin)
        {
          vMin=mVolt;
          }
    delay(Dly0);
    }
     Vpp=vMax-vMin;
     lcd.clear();
     lcd.setCursor(0,0);
     lcd.print("Vpp:"+String(Vpp)+"mV");
     lcd.setCursor(0,1);
     lcd.print("Thres:"+String(thres)+"mV");
     Serial.println("vMax:"+String(vMax)+" vMin:"+String(vMin)+" Vpp:"+String(Vpp));
            delay(1000);
      noTone(3);
     digitalWrite(LED_BUILTIN,LOW);
      calibrated=true;
   
  }
   
  }
   void RadSens2()
  {

    t2=millis();
     
    if((t2-t1)>1000)
    {
      lcd.clear();
      lcd.setCursor(0,0);
       lcd.print("Count:"+String(cnt));
      lcd.setCursor(0,1);
      lcd.print("Time:"+String(t)+"s");
      t+=1;
      t1=t2;
          }
         
       adc0 = ads.readADC_SingleEnded(0);
   mVolt = (adc0 * 0.1875);
              delay(100);
              adc0 = ads.readADC_SingleEnded(0);
   mVolt2 = (adc0 * 0.1875);
           delay(100);
           Serial.println(mVolt2);
           if (mVolt2>(mVolt+thres))
           {
           cnt++;
           bip();
           }
                
     }
  
  void RadSens()
  {
        t2=millis();
    if((t2-t1)>1000)
    {
      lcd.clear();
      lcd.setCursor(0,0);
       lcd.print("Count:"+String(cnt));
      lcd.setCursor(0,1);
      lcd.print("Time:"+String(t)+"s");
      t+=1;
      t1=t2;
     }
    
     adc0 = ads.readADC_SingleEnded(0);
   mVolt = (adc0 * 0.1875);
    Serial.println(String(mVolt));
   if(mVolt>(vMax+thres) || mVolt<(vMin-thres))
    {
     cnt+=1;
   digitalWrite(LED_BUILTIN,HIGH);
   bip();
    }
    else
     {
      digitalWrite(LED_BUILTIN,LOW);
          
     }
     delay(Dly0);       
                
              
    }
  
  void bip()
  {
    if(Tone==true)
            {
             tone(3,freq,10);
             }
              else{ noTone(3);}
    }
    void thresval()
   {
                                
    if(updlcd==true)
    {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Thres:"+String(thres)+"mV");
      
        }
         if(buttonsens()==1)
               {
                menuon=false;
                if(thres<thmax)
                {
                thres+=0.1f;
                updlcd=true;
                bip();
                }
                
              }
              if(buttonsens()==2)
              {
                menuon=false;
                if(thres>thmin)
                {
                thres-=0.1f;
                updlcd=true;
                bip();
                }
                
               }
               if(buttonsens()==0)
               {
                updlcd=false;
                }
        delay(Dly1);
           }  
    void toneopt()
    {
       
       if(buttonsens()==0)
        {
          updlcd=false;
          
          }
                        
                if(buttonsens()==1 )
               {
                switch(sprog){
                  case 0:
                  Tone=true;
                  break;
                  case 1:
                  freq+=100;
                  break;
                 }
                 menuon=false;
                 updlcd=true;
                bip();
                 delay(Dly1);
                      
              }
              if(buttonsens()==2 )
              {
                 menuon=false;
                switch(sprog){
                  case 0:
                  Tone=false;
                  break;
                  case 1:
                  freq-=100;
                  break;
                  
                }
                updlcd=true;
                bip();
                 delay(Dly1);
              }
              if(buttonsens()==3)
              {
                
                sprog++;
                if(sprog==2) sprog=0;
                    updlcd=true;  
                           
                }
              if(updlcd)
              {
        if(Tone==true)
        {
      if(sprog==0)
      {
        lcd.clear();
       lcd.setCursor(0,0);
       lcd.print(">Tone:On");
       lcd.setCursor(0,1);
       lcd.print(" ToneFreq:"+String(freq)+"Hz");
      }
         if(sprog==1)
      {
        lcd.clear();
       lcd.setCursor(0,0);
       lcd.print(" Tone:On");
       lcd.setCursor(0,1);
       lcd.print(">ToneFreq:"+String(freq)+"Hz");
      }
          
         }
          else
        {
      
       if(sprog==0)
      {
         lcd.clear();
       lcd.setCursor(0,0);
       lcd.print(">Tone:Off");
       lcd.setCursor(0,1);
       lcd.print(" ToneFreq:"+String(freq)+"Hz");
      }
         if(sprog==1)
          {
         lcd.clear();
       lcd.setCursor(0,0);
       lcd.print(" Tone:Off");
       lcd.setCursor(0,1);
       lcd.print(">ToneFreq:"+String(freq)+"Hz");
          }
        }
      }  
    }
     
         
