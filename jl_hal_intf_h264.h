/* 特别说明：hal层不独立成模块，代码附属于各个模块中，因此部分可能被多个模块调用的接口需要做到上下文无关性或者每个上下文对应一个接口 */

/* vedu能力查询接口 */
td_bool jl_hal_h264_sse_support(td_void);

/* vedu内存计算接口 */
jl_hal_h264_get_ai_mask_stride(td_u32 pic_width);

/* vedu寄存器Node内存操作接口 */
td_void jl_hal_h264_init_regs_node(td_s32 chn_id);
td_void jl_hal_h264_update_regs_node(td_s32 chn_id);
td_void jl_hal_h264_read_regs_node(td_s32 chn_id);

/* vedu寄存器直接操作接口 */
