#ifndef JL_MPI_VENC_H
#define JL_MPI_VENC_H

#include "ot_common.h"
#include "jl_common_venc.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif
/********************************************/
/* 1. 必须功能：没有开关的功能，编码器创建即开启 */
/* 2. 可选功能：有开关的功能，编码器默认不开启  */
/* 3. 静态参数：运行过程中不可更改的参数，这些参数会导致内部状态或者资源重新申请，用attr */
/* 4. 动态参数：运行过程中可以更改的参数，这些参数不影响内部运行状态和资源，用param */
/* 5. 不允许出现参数会被编码器内部修改的情况，即参数的生命周期是编码器的生命周期，不能是帧级可变的 */
/* 6. 通过动态设置方式实现帧级参数控制 */
/* 7. 参数生效有延时情况，生效前使用最后一次设置的值，proc应展示设置值和生效值 */
/* 8. 如果由于认知不足导致接口或参数设计的不合理，需在主干分支修正，原有的参数或接口应删除，无法删除时注释添加Discarded标记 */
/********************************************/
/* 状态转换接口 必须接口 */
td_s32 jl_mpi_venc_create_chn(ot_venc_chn chn); // 创建通道，分配通道上下文，进入idle状态
td_s32 jl_mpi_venc_prepare_chn(ot_venc_chn chn); // 准备好通道级资源，主要是重构帧VB资源，进入ready状态
td_s32 jl_mpi_venc_start_chn(ot_venc_chn chn); // 进入start状态
td_s32 jl_mpi_venc_stop_chn(ot_venc_chn chn); // 进入stop状态
td_s32 jl_mpi_venc_reset_chn(ot_venc_chn chn); // VENC运行异常时，一键复位内部状态，用户设置的attr/param保留，进入idle状态
td_s32 jl_mpi_venc_destroy_chn(ot_venc_chn chn);

/* START状态下接口 送帧收流接口 必须接口 */
td_s32 jl_mpi_venc_get_stream(ot_venc_chn chn, jl_venc_stream *stream, td_s32 milli_sec);
td_s32 jl_mpi_venc_release_stream(ot_venc_chn chn, const jl_venc_stream *stream);
td_s32 jl_mpi_venc_send_frame(ot_venc_chn chn, const jl_venc_frame *frame, td_s32 milli_sec);
// 可选接口
td_s32 jl_mpi_venc_get_fd(ot_venc_chn chn, hi_s32 *fd);

/* IDLE状态下接口 可选 */
// 设置必须功能的静态参数，是可能会导致内部状态状态重新开始，也可能资源重新申请
td_s32 jl_mpi_venc_set_static_attr(ot_venc_chn chn, const jl_venc_stat_attr *attr);
td_s32 jl_mpi_venc_get_static_attr(ot_venc_chn chn, jl_venc_stat_attr *attr);
// 可选功能静态参数，内部状态重新开始
td_s32 jl_mpi_venc_get_privacy_static_attr(ot_venc_chn chn, jl_venc_stat_attr *attr);
td_s32 jl_mpi_venc_set_privacy_static_attr(ot_venc_chn chn, jl_venc_stat_attr *attr);
td_s32 jl_mpi_venc_get_ssvc_static_attr(ot_venc_chn chn, jl_venc_stat_attr *attr);
td_s32 jl_mpi_venc_set_ssvc_static_attr(ot_venc_chn chn, jl_venc_stat_attr *attr);
td_s32 jl_mpi_venc_set_user_data_attr(ot_venc_chn chn, const jl_venc_user_data_attr *attr);
td_s32 jl_mpi_venc_get_user_data_attr(ot_venc_chn chn, jl_venc_user_data_attr *attr);
td_s32 jl_mpi_venc_set_uservb_attr(ot_venc_chn chn, const jl_venc_usrvb_attr *attr);
td_s32 jl_mpi_venc_get_uservb_attr(ot_venc_chn chn, jl_venc_usrvb_attr *attr);
td_s32 jl_mpi_venc_set_ext_ref_attr(ot_venc_chn chn, const jl_venc_ext_ref_attr *attr);
td_s32 jl_mpi_venc_get_ext_ref_attr(ot_venc_chn chn, jl_venc_ext_ref_attr *attr);
td_s32 jl_mpi_venc_set_aiseg_attr(ot_venc_chn chn, jl_venc_aiseg_attr *attr);
td_s32 jl_mpi_venc_get_aiseg_attr(ot_venc_chn chn, jl_venc_aiseg_attr *attr);

/* START状态下接口 可选 */
// 必须功能的动态参数, 有即可生效的也可以延迟生效，生效前用最后一次设置的值
td_s32 jl_mpi_venc_set_dyn_attr(ot_venc_chn chn, const jl_venc_dyn_attr *attr);
td_s32 jl_mpi_venc_get_dyn_attr(ot_venc_chn chn, jl_venc_dyn_attr *attr);

// 可选功能动态参数
td_s32 jl_mpi_venc_send_user_data(ot_venc_chn chn, td_u8 *data, td_u32 len);
td_s32 jl_mpi_venc_set_mse_param(ot_venc_chn chn, const jl_venc_mse_param *attr);
td_s32 jl_mpi_venc_get_mse_param(ot_venc_chn chn, jl_venc_mse_param *attr);
td_s32 jl_mpi_venc_set_deblur_param(ot_venc_chn chn, const jl_venc_deblur_param *attr);
td_s32 jl_mpi_venc_get_deblu_param(ot_venc_chn chn, jl_venc_deblur_param *attr);
td_s32 jl_mpi_venc_set_md_param(ot_venc_chn chn, const jl_venc_md_param *md_param);
td_s32 jl_mpi_venc_get_md_param(ot_venc_chn chn, jl_venc_md_param *md_param);

/* START状态下接口 必须 */
// 获取当前编码通道内部状态信息
td_s32 jl_mpi_venc_query_status(ot_venc_chn chn, jl_venc_chn_status *status);

/* STOP状态下接口 可选 */

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* JL_MPI_VENC_H */
