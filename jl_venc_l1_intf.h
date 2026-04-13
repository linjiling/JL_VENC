#ifndef VENC_L1_INTF_H
#define VENC_L1_INTF_H

/* 提供给给CHNL模块调用的接口 */
td_s32 jl_venc_query_task_ready(td_s32 chn_id, td_void *info);
td_s32 jl_venc_handle_interrupt(td_s32 chn_id, td_void *info);

/* 提供给用户的IOCTL接口 */
#define JL_VENC_CREATE_CHN _IOW(IOC_TYPE_VENC, IOC_VENC_CREATE_CHN, ot_venc_chn_attr)
#define JL_VENC_DESTROY_CHN _IO(IOC_TYPE_VENC, IOC_VENC_DESTROY_CHN)
#define JL_VENC_START_CHN _IOW(IOC_TYPE_VENC, IOC_VENC_START_CHN, ot_venc_start_param)
#define JL_VENC_STOP_CHN _IO(IOC_TYPE_VENC, IOC_VENC_STOP_CHN)
#define JL_VENC_RESET_CHN _IO(IOC_TYPE_VENC, IOC_VENC_RESET_CHN)

#endif
