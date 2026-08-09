#include "stm32f10x.h"

volatile uint16_t AD_Value;

void AD_Init(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);      // 开启 DMA1 时钟AHB 总线
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1,ENABLE);//低速时钟APB1

    // ADC时钟最大不能超过14MHz。72MHz / 6 = 12MHz
    RCC_ADCCLKConfig(RCC_PCLK2_Div6);

    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AIN;
    GPIO_InitStructure.GPIO_Pin=GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
    GPIO_Init(GPIOA,&GPIO_InitStructure);


    DMA_InitTypeDef DMA_InitStructure;
    DMA_InitStructure.DMA_BufferSize=1;// 传输计数器，指定传输几次,这里只测 1 个通道，所以是 1
    DMA_InitStructure.DMA_DIR=DMA_DIR_PeripheralSRC;// 数据传输方向：外设 -> 存储器
    DMA_InitStructure.DMA_M2M=DMA_M2M_Disable;//ADC外设到内存
    DMA_InitStructure.DMA_MemoryBaseAddr=(uint32_t)&AD_Value;;//数据存储地址
    DMA_InitStructure.DMA_MemoryDataSize=DMA_MemoryDataSize_HalfWord;//ADC的分辨率12位 (HalfWord)
    DMA_InitStructure.DMA_MemoryInc=DMA_MemoryInc_Disable;// 存储器地址自增：否 (始终写入 AD_Value)
    DMA_InitStructure.DMA_Mode=DMA_Mode_Circular;//循环模式ADC 不停转，DMA 不停搬
    DMA_InitStructure.DMA_PeripheralBaseAddr=(uint32_t)&ADC1->DR;//外设数据起始地址
    DMA_InitStructure.DMA_PeripheralDataSize=DMA_PeripheralDataSize_HalfWord;
    DMA_InitStructure.DMA_PeripheralInc=DMA_PeripheralInc_Disable;//外设地址不自增 (始终读 ADC1->DR)
    DMA_InitStructure.DMA_Priority=DMA_Priority_Medium;
    DMA_Init(DMA1_Channel1,&DMA_InitStructure);
    

    //配置 ADC 规则组通道
    // 采样时间稍长一点可以让电压更稳定，55.5 周期
    ADC_RegularChannelConfig(ADC1,ADC_Channel_4,1,ADC_SampleTime_55Cycles5);

    ADC_InitTypeDef ADC_InitStructure;
    ADC_InitStructure.ADC_ContinuousConvMode=ENABLE;//连续转换
    ADC_InitStructure.ADC_DataAlign=ADC_DataAlign_Right;//右对齐精度高
    ADC_InitStructure.ADC_ExternalTrigConv=ADC_ExternalTrigConv_None;//配置 ADC 的外部触发源无，使用软件触发
    ADC_InitStructure.ADC_Mode=ADC_Mode_Independent;//单个ADC独立工作模式
    ADC_InitStructure.ADC_NbrOfChannel=1;//通道数量
    ADC_InitStructure.ADC_ScanConvMode=DISABLE;//非扫描模式
    ADC_Init(ADC1,&ADC_InitStructure);
    
    DMA_Cmd(DMA1_Channel1,ENABLE);//ADC1 -> DMA1 Channel 1
    //开启 ADC 的 DMA 请求
    ADC_DMACmd(ADC1, ENABLE);
    ADC_Cmd(ADC1,ENABLE);

    //ADC校准
    ADC_ResetCalibration(ADC1);
    while (ADC_GetResetCalibrationStatus(ADC1) == SET);
    ADC_StartCalibration(ADC1);
    while (ADC_GetCalibrationStatus(ADC1) == SET);

    //开始转换
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);
}

// 直接返回内存中的变量值
uint16_t AD_GetValue(void)
{
    return AD_Value;
}


// 过滤ADC波动：一阶滞后滤波函数
float AD_GetFilteredADC(void)
{
    static float FilteredADC = 0.0; // static变量，用于记忆上一次的值
    static uint8_t is_first_run = 1;   // 标记是否是第一次运行
    
    // // 获取当前瞬间的物理电压
    // float voltage_raw = (float)AD_Value / 4095.0 * 3.3 * 4.0;

    // 如果是第一次运行，直接把当前值赋给平滑值，防止从0开始慢慢爬升
    if (is_first_run)
    {
        FilteredADC = AD_Value;
        is_first_run = 0;
    }
    else
    {
        // 核心算法：一阶低通滤波
        // 0.95 是“惯性”，0.05 是“新值权重”
        FilteredADC = FilteredADC * 0.95 + AD_Value * 0.05;
    }
    return FilteredADC;
}


// 计算电压
float AD_GetVoltage(void)
{
   // 1. 读取 DMA 自动搬运的 ADC 原始值 (0 ~ 4095)
    // 2. 转换成 STM32 引脚电压: (AD_Value / 4095.0) * 3.3
    // 3. 根据分压反推电池电压: Vin=Vout*(2/8),电池电压 * 4.0
    float voltage = (float)AD_GetFilteredADC() / 4095.0 * 3.3 * 4.0;
    return voltage;
}