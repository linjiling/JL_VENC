/* 特别说明：hal层独立成模块 形成ot_venc_hal.ko， 这种方法可以使得ot_venc.ko和ot_h264e.ko都能访问一套代码，不必做两套代码 */
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
} jl_vedu_status;

td_s32 jl_hal_create_chn(td_s32 chn_id, jl_hal_attr *attr);
td_s32 jl_hal_reset_chn(td_s32 chn_id);
td_s32 jl_hal_destroy_chn(td_s32 chn_id);

/* vedu能力查询接口 */
td_bool jl_hal_sse_support(td_s32 chn_id);

/* vedu内存计算接口 */
td_u32 jl_hal_get_ai_mask_stride(td_s32 chn_id, td_u32 pic_width);

/* vedu寄存器Node内存操作接口 */
td_void jl_hal_init_regs_node(td_s32 chn_id);
td_void jl_hal_update_regs_node(td_s32 chn_id);
td_void jl_hal_read_regs_node(td_s32 chn_id);

/* vedu寄存器直接操作接口 */
td_u32 jl_hal_start_vedu(td_s32 chn_id, td_u32 vpu_id, td_u32 reg_id);
td_u32 jl_hal_reset_vedu(td_s32 chn_id, td_u32 vpu_id, td_u32 reg_id);
td_u32 jl_hal_read_int_status(td_s32 chn_id, td_u32 vpu_id, td_u32 reg_id, jl_vedu_status *enc_statu);
