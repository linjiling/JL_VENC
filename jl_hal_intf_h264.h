/*****************************************************************************************************************/ 
/* hal层独立成模块 形成ot_venc_hal.ko， 这种方法可以使得ot_venc.ko和ot_h264e.ko都能访问一套代码，但是配置寄存器所需的输入 */
/* 需要来自各个模块，这样就要定义一套数据结构，用于传递这些输入参数，引入了一个动作和一些内存，解决方法是这些模块都向hal层注 */
/* 册，这样的话hal层可以直接访问这些模块上下文内容，或者hal层代码不独立成模块，依附在各个模块中，这种方法增加代码尺寸       */
/* 这里采用独立模块的方案，定义一套数据结构可以方便调试定位，用一点内存和性能换可维护性是值的的                           */
/******************************************************************************************************************/
typedef struct {
    td_u32 protocol_type;
} jl_hal_attr;

typedef struct {
    td_u16 end_of_pic : 1;
    td_u16 buf_full : 1;
    td_u16 bits_over : 1;
    td_u16 cfg_err : 1;
    td_u16 time_out : 1;
    td_u16 fmu_frame_err : 1;
    td_u16 fmu_malloc_err : 1;
    td_u16 fmu_free_err : 1;
    td_u16 ring_miss_start : 1;
    td_u16 ring_back : 1;
    td_u16 ring_buffull : 1;
    td_u16 otp_cfg_err : 1;
    td_u16 otp_total_perf : 1;
    td_u16 otp_chn_perf : 1;
    td_u16 bus_err : 1;
    td_u16 low_delay_ok : 1;
    td_u16 reserved : 1;
} jl_vedu_int_status;

typedef enum {
    VEDU_WORK_DONE = 0,
    VEDU_WORK_TIMEOUT,
} jl_vedu_work_status;

typedef enum {
    VEDU_INT_NORMAL = 0,
    VEDU_INT_ERROR,
} jl_vedu_int_type;

td_s32 jl_hal_create_chn(td_s32 chn_id, jl_hal_attr *attr);
td_s32 jl_hal_reset_chn(td_s32 chn_id);
td_s32 jl_hal_destroy_chn(td_s32 chn_id);

/* vedu能力查询接口 */
td_bool jl_hal_sse_support(td_s32 chn_id);

/* vedu内存计算接口 */
td_u32 jl_hal_get_ai_mask_stride(td_s32 chn_id, td_u32 pic_width);

/***************************vedu寄存器Node内存操作接口************************************************/
td_void jl_hal_init_regs_node(td_s32 chn_id);
td_void jl_hal_update_regs_node(td_s32 chn_id);
td_void jl_hal_read_regs_node(td_s32 chn_id);
/****************************************************************************************************/

/*****************************vedu寄存器直接操作接口*************************************************/
/* 启动vedu逻辑开始编码 */
td_u32 jl_hal_start_vedu(td_s32 chn_id, td_u32 vpu_id, td_u32 reg_id);
/* 复位vedu寄存器，通过操作时钟控制寄存器方式实现，当出现vedu死机或者其他需要复位的情况时使用 */
td_u32 jl_hal_reset_vedu(td_s32 chn_id, td_u32 vpu_id, td_u32 reg_id);
/* 读取当前中断所匹配的寄存器id号，通过中断状态或者通道状态来判断，当vedu死机时只能通过通道状态判断，vedu_status表示vedu工作状态 */
td_s32 jl_hal_read_reg_id(td_s32 vpu_id, jl_vedu_work_status vedu_status);
/* 读取某vedu某reg_id对应的中断状态寄存器，判断中断类别和相关信息 */
td_u32 jl_hal_read_int_status(td_s32 chn_id, td_u32 vpu_id, td_u32 reg_id, jl_vedu_int_status *int_status);
/* 清除某vedu某reg_id对应的中断, 清正常中断和错误中断清除时间可能不同，因此设置了清除中断类型 */
td_s32 jl_hal_clear_int(td_s32 vpu_id, td_s32 reg_id, jl_vedu_int_type int_type, jl_vedu_int_status int_status);
/* */
/****************************************************************************************************/
