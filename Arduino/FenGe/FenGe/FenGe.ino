
#include <String.h>
String test_ssid="1,3,5,7,9";

void setup()
{
    Serial.begin(9600);
    delay(10);
    Serial.println();
    Serial.println();
    Serial.println("test String: ");
  
    String part01 = fenge(test_ssid,",",0);
    String part02 = fenge(test_ssid,",",1);
    String part03 = fenge(test_ssid,",",2);
    String part04 = fenge(test_ssid,",",3);
    String part05 = fenge(test_ssid,",",4);
    Serial.print(part01);
    Serial.print(part02);
    Serial.print(part03);
    Serial.print(part04);
    Serial.println(part05);   //没有，输出-1
    Serial.println("---");
}
  
void loop()
{
  
}

String fenge(String str,String fen,int index)
{
 int weizhi;
 String temps[str.length()];
 int i=0;
 do
 {
    weizhi = str.indexOf(fen);
    if(weizhi != -1)
    {
      temps[i] =  str.substring(0,weizhi);
      str = str.substring(weizhi+fen.length(),str.length());
      i++;
      }
      else {
        if(str.length()>0)
        temps[i] = str;
      }
 }
  while(weizhi>=0);

  if(index>i)return "-1";
  return temps[index];
}
