#include "ht1621b.h"
#include "math.h"

//-----------------------------------------------------------------------------------------
//�������ƣ�void Ht1621_Init(void)
//��������: HT1621��ʼ��
//˵ ������ʼ����Һ���������ֶξ���ʾ
//-----------------------------------------------------------------------------------------
//volatile __SYS_STATE     	sys_state;

void ht1621b_init(void)
{
    ht_gpio_init();
    SET_HT_CS();//HT1621_CS=1;
    SET_HT_WR();//HT1621_WR=1;
    SET_HT_DATA();//HT1621_DAT=1;
    DelayMS(20); //��ʱʹLCD������ѹ�ȶ�
    Ht1621WrCmd(SYSEN); //��ϵͳ����
    Ht1621WrCmd(RC256); //ʹ���ڲ�����
    Ht1621WrCmd(BIAS); //1/2ƫ�� 4com
    Ht1621WrCmd(LCDON);//ƫ�ÿ�
    //Ht1621WrCmd(SYSDIS);
    //Ht1621WrCmd(WDTDIS); //��ֹ���Ź�
    //Ht1621WrCmd(TONEOFF); //�ر�����
    DelayMS(20); //��ʱһ��ʱ��

    //Ht1621WrOneNum(0x02,0xff);
//	DelayMS(20); //��ʱһ��ʱ��
    //Ht1621WrAllData(0xFF); //ȫ����
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

    // ����
    gpio_init(GPIOE, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_7);

    SET_HT_LIGHT();
}
//-----------------------------------------------------------------------------------------
//�������ƣ�Delay()
//�� �ܣ���ʱ�ӳ���
//-----------------------------------------------------------------------------------------
void Delay(uint16_t us) //5,7,9
{
    while(--us);
}

//-----------------------------------------------------------------------------------------
//�������ƣ�DelayMS()
//�� �ܣ���ʱ�ӳ���
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
//�������ƣ�Ht1621Wr_Data()
//�� �ܣ�д���ݺ���,cntΪ��������λ��,���ݴ���Ϊ��λ��ǰ
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
//�������ƣ�void Ht1621WrOneData(uchar Addr,uchar Data)
//��������: HT1621��ָ����ַд�����ݺ���
//����˵����AddrΪд���ʼ��ַ��DataΪд������
//˵ ������ΪHT1621������λ4λ������ʵ��д������Ϊ�����ĺ�4λ
//-----------------------------------------------------------------------------------------
void Ht1621WrOneData(uint8_t Addr,uint8_t Data)
{
    CLS_HT_CS();
    //DelayMS(1);
    Delay(1);
    Ht1621Wr_Data(0xa0,3); //д�����ݱ�־101
    Ht1621Wr_Data(Addr<<2,6); //д���ַ����
    Ht1621Wr_Data(Data<<4,4); //д������
    SET_HT_CS();//
}
//-----------------------------------------------------------------------------------------
//�������ƣ�void Ht1621WrOneNum(uchar Addr,uchar Num)
//��������: HT1621��ָ����ַд��һ�����֣�0-9������
//����˵����AddrΪд���ʼ��ַ��NumΪд������
//˵ ������ΪHT1621������λ4λ������ʵ��д������Ϊ�����ĺ�4λ
//-----------------------------------------------------------------------------------------

void Ht1621WrOneNum(uint8_t Addr,uint8_t num)
{
    CLS_HT_CS();//HT1621_CS=0;
    //DelayMS(1);
    Delay(1);
    Ht1621Wr_Data(0xa0,3); //д�����ݱ�־101
    Ht1621Wr_Data(Addr<<2,6); //д���ַ����
    //Ht1621Wr_Data(num <<4,4); //дһ������������
	Ht1621Wr_Data(num,8);
    SET_HT_CS();//

}
//-----------------------------------------------------------------------------------------
//�������ƣ�void Ht1621WrAllData()
//��������: HT1621����д�뷽ʽ����
//˵ ����HT1621������λ4λ���˴�ÿ������Ϊ8λ��д������������8λ����
//-----------------------------------------------------------------------------------------
void Ht1621WrAllData(uint8_t data)
{
    uint8_t i;
    CLS_HT_CS();//

    Ht1621Wr_Data(0xa0,3); //д�����ݱ�־101
    Ht1621Wr_Data(0x00<<2,6); //д���ַ����
    for (i=0; i<16; i++)
    {
        Ht1621Wr_Data(data,8);// //д������
    }
    SET_HT_CS();//

}
//-----------------------------------------------------------------------------------------
//�������ƣ�void Ht1621WrCmd(uchar Cmd)
//��������: HT1621����д�뺯��
//����˵����CmdΪд����������
//˵ ����д�������ʶλ100
//-----------------------------------------------------------------------------------------
void Ht1621WrCmd(uint8_t Cmd)
{
    CLS_HT_CS();//HT1621_CS=0;
    //DelayMS(1);
    Delay(1);
    Ht1621Wr_Data(0x80,4); //д�������־100
    Ht1621Wr_Data(Cmd,8); //д����������
    SET_HT_CS();//HT1621_CS=1;
}

//-----------------------------------------------------------------------------------------
//InputPowerShow(uint16_t power)
//��������: ���빦����ʾ
//����˵����power--ʵ�ʴ��ݹ���
//˵ ������ʾ���빦��
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
//�������ƣ�OutputPowerShow(uint16_t power)
//��������: ���������ʾ
//����˵����power--ʵ�ʴ��ݹ���
//˵ ������ʾ�������
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
//�������ƣ�SocShow(uint8_t soc)
//��������: Soc��ʾ
//����˵����soc--ʣ������ٷֱ�
//˵ ������ʾʣ������ٷֱ�
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