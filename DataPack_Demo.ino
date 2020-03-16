// 2020.3.15 用数据包传（不）定长数据— — — — 【未在硬件上测试】
// 通过（MATLAB）与arduino的串口通信控制12、2~7、8、10号引脚的输出
#define Switch 12 //MOS管连接的引脚

char part = '*';
byte Pnumber = 0; //int Pnumber = 0;//
byte data[5] = {63,63,63,63,63}; // 即：?????
byte term = 0;
unsigned char rec = 0; // 接收状态标志
unsigned char count = 0; // 数据包地址
unsigned char finishFlag = 0; // 接收完成标志

void setup()
{
  Serial.begin(115200, SERIAL_8N1); //数据位8，无奇偶校验，停止位1
  pinMode(Switch, OUTPUT); //控制继电器引脚
  digitalWrite(Switch, Pnumber);

  DDRD = 0xFF; PORTD = 0x00;  //DDRD = B11111111;
  DDRB = 0xFF; //PORTB = 0x00;
  
  /*	【这一大段是用来设置定时器使9号引脚输出方波的，可看可不看】
  // 下面的等值于 OC1A = 输出用于定时器1的CTC模式
  pinMode(9, OUTPUT);  // 设置默认关联了D9（查表得知）为输出
  TCCR1B=0; // 只是个复位的习惯可以不要
  TCCR1A=_BV(COM1A0); // 要是困扰的话你也可以写成 TCCR1A = 0b01000000; 
  //这里和上一个表示了 = CTC 模式, 开启比较器（默认输出）, prescaler = 1
  TCCR1B = _BV(WGM12)|_BV(CS10) ; //  这里等值于 TCCR1B = 0b00001001;

  // 16位的寄存器如何确定一个频率
  OCR1A = 0; // 计算为：F_CPU/2/Need_HZ-1 来得到你想要的，注意要四舍五入来截取频率
  */
}

void loop()
{
  //————接收数据包————
  if (Serial.available() > 0)  
  {
    term = Serial.read();
    if ( term == 0x3C ) // 帧头：<
	{	rec = 1;	count = 0;	}
	if ( term == 0x3E ) // 帧尾：>
	{	rec = 0;	finishFlag = 1;
		part = data[0];    Pnumber = data[1];
	}
	if (rec)
	{	data[count++] = term;	} // 数据存进data数组
  }
  //————对12号引脚、PortD进行操作————
  if (finishFlag)
  {
    switch(part)
    {
      case 'L': digitalWrite(Switch, Pnumber); break; //MOS管控制激光
      //Pnumber = 1- (comdata[1] - '0');//这句是开串口监视器时用的
      case 'A': { PORTD = Pnumber; break; //控制DA模块（其中0、1号引脚被串口占用）
                  digitalWrite(8, Pnumber&1); digitalWrite(10, (Pnumber&2)>>1);
                  // 8号引脚取数据的最后一位，10取数据倒数第二位【9号引脚是时钟线】
				}
      default: ;
    }
    finishFlag = 0;
  }
}
