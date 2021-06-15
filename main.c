
#include <math.h>
/**
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
 * A/D 转化初始化
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
 */
#define ADCCON				0xC0053000
#define ADCDAT 				0xC0053004
#define ADCINTENB 			0xC0053008
#define ADCINTCLR 			0xC005300C
#define PRESCALERCON 		0xC0053010

#define rADCCON				(*(volatile unsigned int *)ADCCON)
#define rADCDAT				(*(volatile unsigned int *)ADCDAT)
#define rADCINTENB			(*(volatile unsigned int *)ADCINTENB)
#define rADCINTCLR			(*(volatile unsigned int *)ADCINTCLR)
#define rPRESCALERCON		(*(volatile unsigned int *)PRESCALERCON)

#define RESET_ID_ADC                60

#define APEN_BITP       (15)
#define PRES_BITP       (0)

/* ADCCON */
#define DATA_SEL_VAL    (0)     /* 0:5clk, 1:4clk, 2:3clk, 3:2clk, 4:1clk, 5:0clk */
#define CLK_CNT_VAL     (6)     /* 28nm ADC */

#define DATA_SEL_BITP   (10)    /* 13:10 */
#define CLK_CNT_BITP    (6)     /* 9:6 */
#define ASEL_BITP       (3)
#define ADCON_STBY      (2)
#define ADEN_BITP       (0)

/* ADCINTENB */
#define AIEN_BITP       (0)

/* ADCINTCLR */
#define AICL_BITP       (0)
struct NX_RSTCON_RegisterSet
{
	 volatile unsigned int    REGRST[(69+31)>>5];
};
static struct NX_RSTCON_RegisterSet *__g_pRegister;
typedef enum
{
    RSTCON_ASSERT   = 0UL,
    RSTCON_NEGATE   = 1UL
}RSTCON;
void NX_RSTCON_SetRST(unsigned int RSTIndex, RSTCON STATUS)
{
	unsigned int regvalue;

    regvalue    = *(volatile unsigned int  *)(&__g_pRegister->REGRST[RSTIndex >> 5]);

    regvalue    &= ~(1UL << (RSTIndex & 0x1f));
    regvalue    |= (STATUS & 0x01) << (RSTIndex & 0x1f);

    *(volatile unsigned int  *)(&__g_pRegister->REGRST[RSTIndex >> 5]) = (unsigned int  )(regvalue);
}
void adc_init(char ch)
{
	unsigned int adccon = 0;
	unsigned int reg = 0;

	s5p6818_serial_write_string(0, "\n********** adc init!! ***********\n\r");

	unsigned int RSTIndex = RESET_ID_ADC;
	__g_pRegister = (struct NX_RSTCON_RegisterSet *)((void *)0xC0012000);
	NX_RSTCON_SetRST(RSTIndex, RSTCON_ASSERT);
	NX_RSTCON_SetRST(RSTIndex, RSTCON_NEGATE);

	adccon = ((DATA_SEL_VAL & 0xf) << DATA_SEL_BITP) |
	         ((CLK_CNT_VAL & 0xf) << CLK_CNT_BITP) |
	         (0 << ADCON_STBY);
	rADCCON = adccon;

	reg = rPRESCALERCON;
	reg &= ~(0x3FF << PRES_BITP);
	reg |= 0xFF << PRES_BITP;
	reg |= 0x1 << APEN_BITP;
	rPRESCALERCON = reg;

	rADCINTCLR = (1 << AICL_BITP);
	rADCINTENB = (1 << AIEN_BITP);

	reg = rADCCON;
	reg &= ~(0x7 << ASEL_BITP);
	reg |= (ch << ASEL_BITP);
	rADCCON = reg;
}





/**
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
 * GPIO 初始化
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
 * LED      GROUP       NUMBER
 * 
 * D7       A           28
 * D8       E           13
 * D9       B           12
 * D10      C           2
 */
#define uint_32 unsigned int
// 枚举 GPIO 引脚
typedef struct
{
    uint_32 OUT;
    uint_32 OUTENB;
    uint_32 DETMODE0;
    uint_32 DETMODE1;
    uint_32 INTENB;
    uint_32 DET;
    uint_32 PAD;
    uint_32 RSVD;
    uint_32 ALTFN0;
    uint_32 ALTFN1;
}gpio;

// 枚举灯的开关
typedef enum
{
    LED_OFF = 0,
    LED_ON
} led_state;

// 枚举灯
typedef enum
{
    D7 = 0,
    D8,
    D9,
    D10
} led;

#define GPIOA (*(volatile gpio *)0xC001A000)
#define GPIOB (*(volatile gpio *)0xC001B000)
#define GPIOC (*(volatile gpio *)0xC001C000)
#define GPIOE (*(volatile gpio *)0xC001E000)

// GPIO 初始化
void init_GPIO(void)
{
    // 设置 GPIO 引脚为 GPIO 功能
    GPIOA.ALTFN1 &= ~(0x3 << 24); // d7    24:25---00

    GPIOE.ALTFN0 &= ~(0x3 << 26); // d8    26:27---00

    GPIOB.ALTFN0 &= ~(0x3 << 24); // d9    24:25---00
    GPIOB.ALTFN0 |= (0x2 << 24);  // d9    24:25---10

    GPIOC.ALTFN0 &= ~(0x3 << 4);  // d10    4: 5---00
    GPIOC.ALTFN0 |= (0x1 << 4);   // d10    4: 5---01
    /******************************************************************/
    /******************************************************************/

    // 设置 GPIO 引脚为输出功能
    GPIOA.OUTENB |= (0x1 << 28); //d7    28---1
    GPIOE.OUTENB |= (0x1 << 13); //d8    13---1
    GPIOB.OUTENB |= (0x1 << 12); //d9    12---1
    GPIOC.OUTENB |= (0x1 << 2);  //d10    2---1
}




/**
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
 * PWM 初始化
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
 */

// 枚举有关 PWM 的寄存器
typedef struct
{
    uint_32 TCFG0;
    uint_32 TCFG1;
    uint_32 TCON;
    uint_32 TCNTB0;
    uint_32 TCMPB0;
    uint_32 TCNTO0;
    uint_32 TCNTB1;
    uint_32 TCMPB1;
    uint_32 TCNTO1;
    uint_32 TCNTB2;
    uint_32 TCMPB2;
    uint_32 TCNTO2;
    uint_32 TCNTB3;
    uint_32 TCMPB3;
    uint_32 TCNTO3;
    uint_32 TCNTB4;
    uint_32 TCNTO4;
    uint_32 TINT_CSTAT;
} pwm;
#define PWM (*(volatile pwm *)0xC0018000) // PWM寄存器的基址


void pwm_init()
{
    //采用定时器 2
    //1.设置GPIOC14引脚为PWM功能  GPIOCALTFN0[29:28] = 0b10
    GPIOC.ALTFN0 &= ~(3 << 28);
    GPIOC.ALTFN0 |= (2 << 28);
    //2.设置对PCLK时钟的一级分频值,进行50分频  TCFG0[15:8] = 49
    PWM.TCFG0 &= ~(0xFF << 8);
    PWM.TCFG0 |= (49 << 8);
    //3.设置对PCLK时钟的二级分频值，进行2分频  TCFG1[11:8] = 0b0010
    PWM.TCFG1 &= ~(0xF << 8);
    PWM.TCFG1 |= (2 << 8);
    //4.设置 TCNTB2 计数缓冲寄存器为100和 TCMPB2 比较缓冲寄存器为0
    //初始占空比为0；
    PWM.TCNTB2 = 100;
    PWM.TCMPB2 = 0;
    //5.pwm2 打开电平翻转功能
    PWM.TCON |= (0x1 << 14);
    //6.第一个周期时开启手动加载TCNTB2和TCMPB2到递减计数器中
    PWM.TCON |= (1 << 13);
    //7.开启自动加载，关闭手动加载并且使能pwm2
    PWM.TCON |= (1 << 15);
    PWM.TCON &= ~(1 << 13);
    PWM.TCON |= (1 << 12);
}
//设定pwm2的比较缓冲器的值，即设置pwm的占空比
void set_cmp(int cmp)
{
    PWM.TCMPB2 = cmp;
}


// 延迟函数
void delay()
{
    for (uint_32 i = 0; i < 5000; i++)
        for (uint_32 j = 0; j < 1500; j++)
            ;
}

// 控制 GPIO 引脚电平
void control_level(led l, led_state ls)
{
    if (ls == LED_ON)
    {
        switch (l)
        {
        case D7:
            GPIOA.OUT |= (0X1 << 28);
            break;
        case D8:
            GPIOE.OUT |= (0X1 << 13);
            break;
        case D9:
            GPIOB.OUT |= (0X1 << 12);
            break;
        case D10:
            GPIOC.OUT |= (0X1 << 2);
            break;
        }
    }
    else
    {
        switch (l)
        {
        case D7:
            GPIOA.OUT &= (~(0X1 << 28));
            break;
        case D8:
            GPIOE.OUT &= (~(0X1 << 13));
            break;
        case D9:
            GPIOB.OUT &= (~(0X1 << 12));
            break;
		case D10:
            GPIOC.OUT &= (~(0X1 << 2));
            break;
        }
    }
}



// A/D
char string[1024] = "";

char* itoa(unsigned int value)
{

	int i = 0, j = 0;
	int len = 0;
	do
	{
		if(value == 0)
		{
			string[i++] = 0;
		}
		else
		{
			char c = value % 10 + '0';
			string[i++] = c;
			value /= 10;
		}
	}
		while(value != 0);
	string[i] = '\0';
	len = strlen(string);
	for(j=0; j<len/2; j++)
	{
		char temp = string[j];
		string[j] = string[len-j-1];
		string[len-j-1] = temp;
	}
	return string;

}


thread_event(){
    reg = rADCCON;
    reg |= (1 << ADEN_BITP);
    rADCCON = reg;

    x = (rADCDAT & 0xfff);      
    data = 1.8 * x / 4095;      
    if(data < 1.2){
        control(D10, OFF);      // 超过液位开关，关闭
    }
    control(D10,ON);
    delay(1);   //间隔一秒检测一次
}

int main(){
/*******************************************************/
    unsigned int reg = 0;
	unsigned int data = 0;
	char str[] = "0";
	s5p6818_serial_write_string(0, "\n********** test!! ***********\n\r");

    init() // 总的初始化 包括GPIO /  A/D 转换
    pthread_t thread;
    pthread_create(&thread, NULL, thread_event, NULL);

	while (1) {
		reg = rADCCON;
		reg |= (1 << ADEN_BITP);
		rADCCON = reg;

		x = (rADCDAT & 0xfff);      //  CPU的ADC0通道的A/D转换结果
		data = 1.8 * x / 4095;      //  ADC0对应的电压值 = (x/4095)*1.8V

		s5p6818_serial_write_string(0, "data= ");

		unsigned char out[50];
		sprintf(out,"%.3f V",data);
		s5p6818_serial_write_string(0, out);
		s5p6818_serial_write_string(0, "\n");


        // 条件判断，通过电压值控制LED灯亮灭
		if(data < 1.2){
            /* 状态2 */
			control(D9, OFF);   //阀门3
            control(D8, ON);    //阀门2
            set_cmp(95);        // PWM 95%
            delay(10*60)    //延迟10分钟
            /* 状态3 */
            control(D7, ON);    //阀门1
            set_cmp(75);        //PWM 75%

            delay(30)       //延迟30秒
            /* 状态4 */
            control(D8, OFF); 
            set_cmp(50);       //PWM 50%
            delay(30)       //延迟30秒  
            /* 状态5 */
            control(D7, OFF);
            set_cmp(0);        //PWM 0%
		}
        /* 状态1 */
        set_cmp(0);     //重新设置一遍总没错
        control(D9, ON);    
	}
}

