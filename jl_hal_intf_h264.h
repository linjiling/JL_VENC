/* 特别说明：hal层独立成模块 形成ot_venc_hal.ko， 这种方法可以使得ot_venc.ko和ot_h264e.ko都能访问一套代码，不必做两套代码 */

/* vedu能力查询接口 */
td_bool jl_hal_h264_sse_support(td_s32 chn_id, td_void);

/* vedu内存计算接口 */
td_u32 jl_hal_h264_get_ai_mask_stride(td_s32 chn_id, td_u32 pic_width);

/* vedu寄存器Node内存操作接口 */
td_void jl_hal_h264_init_regs_node(td_s32 chn_id);
td_void jl_hal_h264_update_regs_node(td_s32 chn_id);
td_void jl_hal_h264_read_regs_node(td_s32 chn_id);

/* vedu寄存器直接操作接口 */
