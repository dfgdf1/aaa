#include "ht1621b.h"
#include "math.h"

//-----------------------------------------------------------------------------------------
//函数名称：void Ht1621_Init(void)
//功能描述: HT1621初始化
//说 明：初始化后，液晶屏所有字段均显示
//-----------------------------------------------------------------------------------------
//volatile __SYS_STATE     	sys_state;

void ht1621b_init(void)
{
    ht_gpio_init();
    SET_HT_CS();//HT1621_CS=1;
    SET_HT_WR();//HT1621_WR=1;
    SET_HT_DATA();//HT1621_DAT=1;
    DelayMS(20); //延时使LCD工作电压稳定
    Ht1621WrCmd(SYSEN); //打开系统震荡器
    Ht1621WrCmd(RC256); //使用内部振荡器
    Ht1621WrCmd(BIAS); //1/2偏置 4com
    Ht1621WrCmd(LCDON);//偏置开
    //Ht1621WrCmd(SYSDIS);
    //Ht1621WrCmd(WDTDIS); //禁止看门狗
    //Ht1621WrCmd(TONEOFF); //关闭声音
    DelayMS(20); //延时一段时间

    //Ht1621WrOneNum(0x02,0xff);
//	DelayMS(20); //延时一段时间
    //Ht1621WrAllData(0xFF); //全屏灭
}

void ht_gpio_init(void)
{
    /* enable  clock */
    rcu_periph_clock_enable(RCU_GPIOB);
    rcu_periph_clock_enable(RCU_GPIOE);

    /* configure GPIO port */

    // CS
    gpio_init(GPIOB, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_12);

    // WR
    gpio_init(GPIOB, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_15);

    // DATA
    gpio_init(GPIOB, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_14);

    // 背光
    gpio_init(GPIOE, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_7);

    SET_HT_LIGHT();
}
//-----------------------------------------------------------------------------------------
//函数名称：Delay()
//功 能：延时子程序
//-----------------------------------------------------------------------------------------
void Delay(uint16_t us) //5,7,9
{
    while(--us);
}

//-----------------------------------------------------------------------------------------
//函数名称：DelayMS()
//功 能：延时子程序
//-----------------------------------------------------------------------------------------
void DelayMS(uint16_t iMs)
{
    uint16_t i,j;
    for(i=0; i<iMs; i++)
    {
        for(j=0; j<250; j++)
        {
            Delay(1);
        }
    }
}


//-----------------------------------------------------------------------------------------
//函数名称：Ht1621Wr_Data()
//功 能：写数据函数,cnt为传送数据位数,数据传送为低位在前
//-----------------------------------------------------------------------------------------
void Ht1621Wr_Data(uint8_t Data,uint8_t cnt)
{
    uint8_t i;
    for (i=0; i<cnt; i++)
    {
        CLS_HT_WR();

        //DelayMS(3);
        Delay(1);

        if((Data & 0x80)==0x80)
        {
            SET_HT_DATA();
        }
        else
        {
            CLS_HT_DATA();
        }

        SET_HT_WR();
        //DelayMS(3);
        Delay(1);
        Data = Data << 1;
    }
}
//-----------------------------------------------------------------------------------------
//函数名称：void Ht1621WrOneData(uchar Addr,uchar Data)
//功能描述: HT1621在指定地址写入数据函数
//参数说明：Addr为写入初始地址，Data为写入数据
//说 明：因为HT1621的数据位4位，所以实际写入数据为参数的后4位
//-----------------------------------------------------------------------------------------
void Ht1621WrOneData(uint8_t Addr,uint8_t Data)
{
    CLS_HT_CS();
    //DelayMS(1);
    Delay(1);
    Ht1621Wr_Data(0xa0,3); //写入数据标志101
    Ht1621Wr_Data(Addr<<2,6); //写入地址数据
    Ht1621Wr_Data(Data<<4,4); //写入数据
    SET_HT_CS();//
}
//-----------------------------------------------------------------------------------------
//函数名称：void Ht1621WrOneNum(uchar Addr,uchar Num)
//功能描述: HT1621在指定地址写入一个数字（0-9）函数
//参数说明：Addr为写入初始地址，Num为写入数据
//说 明：因为HT1621的数据位4位，所以实际写入数据为参数的后4位
//-----------------------------------------------------------------------------------------

void Ht1621WrOneNum(uint8_t Addr,uint8_t num)
{
    CLS_HT_CS();//HT1621_CS=0;
    //DelayMS(1);
    Delay(1);
    Ht1621Wr_Data(0xa0,3); //写入数据标志101
    Ht1621Wr_Data(Addr<<2,6); //写入地址数据
    //Ht1621Wr_Data(num <<4,4); //写一个完整的数。
	Ht1621Wr_Data(num,8);
    SET_HT_CS();//

}
//-----------------------------------------------------------------------------------------
//函数名称：void Ht1621WrAllData()
//功能描述: HT1621连续写入方式函数
//说 明：HT1621的数据位4位，此处每次数据为8位，写入数据总数按8位计算
//-----------------------------------------------------------------------------------------
void Ht1621WrAllData(uint8_t data)
{
    uint8_t i;
    CLS_HT_CS();//

    Ht1621Wr_Data(0xa0,3); //写入数据标志101
    Ht1621Wr_Data(0x00<<2,6); //写入地址数据
    for (i=0; i<16; i++)
    {
        Ht1621Wr_Data(data,8);// //写入数据
    }
    SET_HT_CS();//

}
//-----------------------------------------------------------------------------------------
//函数名称：void Ht1621WrCmd(uchar Cmd)
//功能描述: HT1621命令写入函数
//参数说明：Cmd为写入命令数据
//说 明：写入命令标识位100
//-----------------------------------------------------------------------------------------
void Ht1621WrCmd(uint8_t Cmd)
{
    CLS_HT_CS();//HT1621_CS=0;
    //DelayMS(1);
    Delay(1);
    Ht1621Wr_Data(0x80,4); //写入命令标志100
    Ht1621Wr_Data(Cmd,8); //写入命令数据
    SET_HT_CS();//HT1621_CS=1;
}

//-----------------------------------------------------------------------------------------
//InputPowerShow(uint16_t power)
//功能描述: 输入功率显示
//参数说明；power--实际传递功率
//说 明：显示输入功率
//-----------------------------------------------------------------------------------------
void InputPowerShow(uint16_t power)
{
    uint16_t powerBuff = power;
    uint8_t showNum = 0;
    uint16_t powerNum = 0;
    uint8_t num = 0;
    do
    {
        showNum++;
        powerBuff /= 10;
    } while (powerBuff > 0);
    powerBuff = power;
    if(num == 4)
    {
        powerNum = powerBuff / 1000;
        showNum = NumChoose(powerNum);
        Ht1621WrOneNum(0x00, showNum);
        powerNum = (powerBuff % 1000) / 100;
        showNum = NumChoose(powerNum);
        Ht1621WrOneNum(0x02, showNum);
        powerNum = (powerBuff % 100) / 10;
        showNum = NumChoose(powerNum);
        Ht1621WrOneNum(0x04, showNum);
        powerNum = (powerBuff % 10) / 1;
        showNum = NumChoose(powerNum);
        Ht1621WrOneNum(0x06, showNum);
    }
    else if(num == 3)
    {
        powerNum = powerBuff / 100;
        showNum = NumChoose(powerNum);
        Ht1621WrOneNum(0x02, showNum);
        powerNum = (powerBuff % 100) / 10;
        showNum = NumChoose(powerNum);
        Ht1621WrOneNum(0x04, showNum);
        powerNum = (powerBuff % 10) / 1;
        showNum = NumChoose(powerNum);
        Ht1621WrOneNum(0x06, showNum);
    }
    else if(num == 2)
    {
        powerNum = powerBuff / 10;
        showNum = NumChoose(powerNum);
        Ht1621WrOneNum(0x04, showNum);
        powerNum = (powerBuff % 10) / 1;
        showNum = NumChoose(powerNum);
        Ht1621WrOneNum(0x06, showNum);
    }
    else if(num == 1)
    {
        powerNum = powerBuff / 1;
        showNum = NumChoose(powerNum);
        Ht1621WrOneNum(0x06, showNum);
    }
}
//-----------------------------------------------------------------------------------------
//函数名称：OutputPowerShow(uint16_t power)
//功能描述: 输出功率显示
//参数说明；power--实际传递功率
//说 明：显示输出功率
//-----------------------------------------------------------------------------------------
void OutputPowerShow(uint16_t power)
{
    uint16_t powerBuff = power;
    uint8_t showNum = 0;
    uint16_t powerNum = 0;
    uint8_t num = 0;
    do
    {
        num++;
        powerBuff /= 10;
    } while (powerBuff > 0);
    powerBuff = power;
    if(num == 4)
    {
        powerNum = powerBuff / 1000;
        showNum = NumChoose(powerNum);
        Ht1621WrOneNum(0x10, showNum);
        powerNum = (powerBuff % 1000) / 100;
        showNum = NumChoose(powerNum);
        Ht1621WrOneNum(0x12, showNum);
        powerNum = (powerBuff % 100) / 10;
        showNum = NumChoose(powerNum);
        Ht1621WrOneNum(0x14, showNum);
        powerNum = (powerBuff % 10) / 1;
        showNum = NumChoose(showNum);
        Ht1621WrOneNum(0x16, showNum);
    }
    else if(num == 3)
    {
        powerNum = powerBuff / 100;
        showNum = NumChoose(powerNum);
        Ht1621WrOneNum(0x12, showNum);
        powerNum = (powerBuff % 100) / 10;
        showNum = NumChoose(powerNum);
        Ht1621WrOneNum(0x14, num);
        powerNum = (powerBuff % 10) / 1;
        showNum = NumChoose(powerNum);
        Ht1621WrOneNum(0x16, showNum);
    }
    else if(num == 2)
    {
        powerNum = powerBuff / 10;
        showNum = NumChoose(powerNum);
        Ht1621WrOneNum(0x14, showNum);
        powerNum = (powerBuff % 10) / 1;
        showNum = NumChoose(powerNum);
        Ht1621WrOneNum(0x16, showNum);
    }
    else if(num == 1)
    {
        powerNum = powerBuff / 1;
        showNum = NumChoose(powerNum);
        Ht1621WrOneNum(0x16, showNum);
    }
}
//-----------------------------------------------------------------------------------------
//函数名称：SocShow(uint8_t soc)
//功能描述: Soc显示
//参数说明；soc--剩余电量百分比
//说 明：显示剩余电量百分比
//-----------------------------------------------------------------------------------------
void SocShow(uint8_t soc)
{
    uint8_t showNum = 0;
    uint8_t socBuff = soc;
    uint8_t socNum = 0;
    uint8_t num = 0;
    do
    {
        num++;
        socBuff /= 10;
    } while (socBuff > 0);
    socBuff = soc;
    if(num == 3)
    {
        socNum = socBuff / 100;
        showNum = NumChoose(socNum);
        Ht1621WrOneNum(0x12, showNum);
        socNum = (socBuff % 100) / 10;
        showNum = NumChoose(socNum);
        Ht1621WrOneNum(0x14, num);
        socNum = (socBuff % 10) / 1;
        showNum = NumChoose(socNum);
        Ht1621WrOneNum(0x16, showNum);
    }
    else if(num == 2)
    {
        socNum = socBuff / 10;
        showNum = NumChoose(socNum);
        Ht1621WrOneNum(0x14, showNum);
        socNum = (socBuff % 10) / 1;
        showNum = NumChoose(socNum);
        Ht1621WrOneNum(0x16, showNum);
    }
    else if(num == 1)
    {
        socNum = socBuff / 1;
        showNum = NumChoose(socNum);
        Ht1621WrOneNum(0x16, showNum);
    }
    if(soc >= 80)
    {
        Ht1621WrOneNum(0x0c,0x08);
        DelayMS(10);
        Ht1621WrOneNum(0x0E,0x0F);
    }
    else if((soc < 80) && (soc >= 60))
    {
        Ht1621WrOneNum(0x0E,0x0F);
    }
    else if((soc < 60) && (soc >= 40))
    {
        Ht1621WrOneNum(0x0E,0x07);
    }
    else if((soc < 40) && (soc >= 20))
    {
        Ht1621WrOneNum(0x0E,0x03);
    }
    else if((soc < 20) && (soc >= 0))
    {
        Ht1621WrOneNum(0x0E,0x01);
    }
}
void InputStateShow(void)
{

}
void OutputStateShow(void)
{

}
void ErrorStateShow(void)
{

}