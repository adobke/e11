#define leftneg 9
#define leftpos 8
#define leftenable 6

#define rightneg 7 
#define rightpos 12
#define rightenable 11

void halt()
{
  digitalWrite(leftneg,LOW);
  digitalWrite(leftpos,LOW);
  digitalWrite(rightneg,LOW);
  digitalWrite(rightpos,LOW);
}

void go(int lpower,int rpower)
{
  if (lpower > 0)
    setpower(leftneg,leftpos,leftenable,lpower,HIGH);
  if (lpower < 0)
    setpower(leftneg,leftpos,leftenable,abs(lpower),LOW);
  if (rpower > 0)
    setpower(rightneg,rightpos,rightenable,rpower,HIGH);
  if (rpower < 0)
    setpower(rightneg,rightpos,rightenable,abs(rpower),LOW);
}

void setpower(int negpin,int pospin,int enablepin,int power,boolean forward)
{
  if(forward)
  {
    digitalWrite(negpin,LOW);
    digitalWrite(pospin,HIGH);
  }
  else
  {
    digitalWrite(negpin,HIGH);
    digitalWrite(pospin,LOW); 
  }
  analogWrite(enablepin,power);
}







