#ifndef JL_COMMON_VENC_H
#define JL_COMMON_VENC_H

#include "ot_common.h"
#include "ot_common_video.h"
#include "ot_common_vb.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif

#define JL_VENC_MAX_FRAME_NUM 2
#define JL_VENC_MAX_MSE_NUM 8
#define JL_VENC_MD_THRESHOLD_NUM 3
#define JL_VENC_MD_LEVEL_NUM 4

typedef enum {
    JL_VENC_GOP_MODE_NORMAL_P    = 0,
    JL_VENC_GOP_MODE_DUAL_P      = 1,
    JL_VENC_GOP_MODE_SMART_P     = 2,
    JL_VENC_GOP_MODE_ADV_SMART_P = 3,
    JL_VENC_GOP_MODE_BIPRED_B    = 4,
    JL_VENC_GOP_MODE_LOW_DELAY_B = 5,
    JL_VENC_GOP_MODE_SMART_CRR   = 6,
    JL_VENC_GOP_MODE_SINGLE_SP   = 7,
    JL_VENC_GOP_MODE_I           = 8,
    JL_VENC_GOP_MODE_ADV_SMART_CRR = 9,
    JL_VENC_GOP_MODE_BUTT,
} jl_venc_gop_mode;

typedef struct {
    td_u32 sp_interval; /* RW; range:[0, 1) or (1, gop-1], gop is the I frame interval; */
} jl_venc_gop_dual_p;

typedef struct {
    td_bool aiseg_chn_flag; /* RW; range:[0, 1]; */
} jl_venc_svac3_stat_attr;

typedef struct {
    jl_venc_gop_mode gop_mode;
    td_u32 gop_num;
    union {
        jl_venc_gop_dual_p dual_p; /* AUTO:jl_venc_gop_mode:JL_VENC_GOP_MODE_DUAL_P; */
    };
} jl_venc_gop_attr;

typedef enum {
    JL_VENC_PACK_MODE_SINGLE    = 0,
    JL_VENC_PACK_MODE_MULTI     = 1,
    JL_VENC_PACK_MODE_BUTT,
} jl_venc_pack_mode;

typedef enum {
    JL_VENC_STM_MODE_NALU    = 0,
    JL_VENC_STM_MODE_FRAME   = 1,
    JL_VENC_STM_MODE_BUTT,
} jl_venc_stm_mode;

typedef enum {
    JL_VENC_FRAME_MODE_USER   = 0,
    JL_VENC_FRAME_MODE_BIND_SYS = 1,
    JL_VENC_FRAME_MODE_BIND_VPSS = 1,
    JL_VENC_FRAME_MODE_BUTT,
} jl_venc_frame_mode;

typedef enum {
    JL_VENC_RCN_REF_MODE_NONE    = 0,
    JL_VENC_RCN_REF_MODE_SHARE   = 1,
    JL_VENC_RCN_REF_MODE_BUTT,
} jl_venc_rcn_ref_mode;

/* 必须功能的参数不能有使能开关变量 */
typedef struct {
    ot_payload_type type;
    td_u32  profile;
    td_u32  max_pic_width;
    td_u32  max_pic_height;
    td_u32  stm_buf_size;
    jl_venc_pack_mode pack_mode;
    td_u32  pic_width;
    td_u32  pic_height;
    ot_vb_src rcn_vb_mode;
    jl_venc_rcn_ref_mode rcn_ref_mode;
    td_u32 frame_buf_ratio;
    jl_venc_gop_attr gop_attr;
    union {
        jl_venc_svac3_stat_attr svac3_stat_attr; /* AUTO:ot_payload_type:JL_PT_SVAC3; */
    };
} jl_venc_stat_attr;

typedef enum {
    JL_VENC_RC_MODE_CBR = 0,
    JL_VENC_RC_MODE_VBR = 1,
    JL_VENC_RC_MODE_BUTT,
} jl_venc_rc_mode;

typedef struct {
    td_u32 i_mad_thres[16];
    td_u32 p_mad_thres[16];
    td_u32 b_mad_thres[16];
    td_u32 mad_origin_idx;
} jl_venc_rc_mad_attr;

typedef struct {
    td_u32 max_i_qp;
    td_u32 min_i_qp;
    td_u32 max_p_qp;
    td_u32 min_p_qp;
    td_u32 max_vi_qp;
    td_u32 min_vi_qp;
} jl_venc_rc_qp_limit;

typedef struct {
    td_u32 hor_width;
    td_u32 ver_height;
} jl_venc_search_window;

typedef struct {
    jl_venc_rc_mode rc_mode;
    td_u32 bit_rate;
    td_u32 stats_time;
    td_u32 ip_qp_delta_base;
    jl_venc_rc_mad_attr mad_attr;
    jl_venc_rc_qp_limit qp_limit;
    jl_venc_search_window sc_window;
} jl_venc_rc_attr;

typedef struct {
    jl_venc_stm_mode stm_mode;
    jl_venc_frame_mode frame_send_mode;
    jl_venc_rc_attr rc_attr;
} jl_venc_dyn_attr;

typedef struct {
    td_phys_addr_t phys_addr;
    td_u8 ATTRIBUTE *addr;
    td_u32 ATTRIBUTE len;
    td_u64 pts;
} jl_venc_pack;

typedef struct {
    td_u32 seq_num;
    jl_venc_pack ATTRIBUTE *p_pack;
    td_u32 ATTRIBUTE pack_cnt;
} jl_venc_stream;

typedef struct {
    td_u32 frame_num;
    ot_video_frame_info frame[JL_VENC_MAX_FRAME_NUM];
} jl_venc_frame;

typedef struct {
    td_u32 cur_packs;
    td_u64 enc_succ_frame_cnt;
    td_u32 mse_total_lcu_cnt;
    td_u32 mse_total_sum;
    td_u64 mse_part_val[JL_VENC_MAX_MSE_NUM];
    td_u32 mse_part_num;
    td_u32 mad_val_by_src;
    td_u32 mad_val_by_ref;
    td_double psnr_val;
} jl_venc_chn_status;

typedef struct {
    td_bool uservb_en;
    ot_vb_pool pic_vb_pool;
    ot_vb_pool pic_info_vb_pool;
} jl_venc_usrvb_attr;

typedef struct {
    td_bool user_data_en;
    td_u32 buf_size;
} jl_venc_user_data_attr;

typedef struct {
    td_bool part_en;
    td_u32 mse_part_num;
    ot_rect mse_part_rect[JL_VENC_MAX_MSE_NUM];
} jl_venc_mse_param;

typedef enum {
    JL_VENC_DEBLUR_MODE_DEFAULT = 0,
    JL_VENC_DEBLUR_MODE_USER = 1,
    JL_VENC_DEBLUR_MODE_BUTT,
} jl_venc_deblur_mode;

typedef struct {
    td_bool deblur_en;
    jl_venc_deblur_mode deblur_mode;
    td_u8 gain;
    td_u8 offset;
    td_u8 near_bg_qp_delta;
    td_u8 near_bg_min_qp;
    td_u8 near_bg_max_qp;
    td_u8 far_bg_qp_delta;
    td_u8 far_bg_min_qp;
    td_u8 far_bg_max_qp;
} jl_venc_deblur_param;

typedef enum {
    JL_VENC_MD_SAD_MODE_ZERO = 0,
    JL_VENC_MD_SAD_MODE_SEARCH = 1,
    JL_VENC_MD_SAD_MODE_BUTT
} jl_venc_md_sad_mode;

typedef struct {
    td_bool move_detect_en;
    td_bool online_en;
    td_bool sad_stats_en;
    td_bool level_stats_en;
    jl_venc_md_sad_mode sad_mode;
    td_s32 threshold_gain[JL_VENC_MD_THRESHOLD_NUM];
    td_s32 threshold_offset[JL_VENC_MD_THRESHOLD_NUM];
    td_s32 qp_delta[JL_VENC_MD_LEVEL_NUM];
    td_s32 md_skip_weight[JL_VENC_MD_LEVEL_NUM];
} jl_venc_md_param;

typedef enum {
    JL_VENC_REF_MODE_OPEN_GRP = 0,
    JL_VENC_REF_MODE_CLOSE_GRP = 1,
    JL_VENC_REF_MODE_MODE_BUTT
} jl_venc_ext_ref_mode;

typedef struct {
    td_bool ext_ref_en;
    td_u32 base_interval;
    td_u32 second_interval;
    jl_venc_ext_ref_mode ref_mode;
} jl_venc_ext_ref_attr;

typedef enum {
    JL_VENC_AISEG_MODE_POINT = 0,
    JL_VENC_AISEG_MODE_RECT = 1,
    JL_VENC_AISEG_MODE_MAP = 2,
    JL_VENC_AISEG_MODE_BUTT,
} jl_venc_aiseg_mode;

typedef struct {
    td_bool aiseg_en;
    td_u32 buf_size;
    jl_venc_aiseg_mode aiseg_mode;
} jl_venc_aiseg_attr;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
