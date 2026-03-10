#include "hi_common.h"
#include "hi_defines.h"
#include "jl_mpi_venc.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

td_s32 jl_mpi_venc_create_chn(ot_venc_chn chn)
{
    return TD_SUCCESS;
}

td_s32 jl_mpi_venc_prepare_chn(ot_venc_chn chn)
{
    return TD_SUCCESS;
}

td_s32 jl_mpi_venc_start_chn(ot_venc_chn chn)
{
    return TD_SUCCESS;
}

td_s32 jl_mpi_venc_stop_chn(ot_venc_chn chn)
{
    return TD_SUCCESS;
}
td_s32 jl_mpi_venc_reset_chn(ot_venc_chn chn)
{
    return TD_SUCCESS;
}

td_s32 jl_mpi_venc_destroy_chn(ot_venc_chn chn)
{
    return TD_SUCCESS;
}

td_s32 jl_mpi_venc_get_fd(ot_venc_chn chn, hi_s32 *fd)
{
    return TD_SUCCESS;
}
td_s32 jl_mpi_venc_query_status(ot_venc_chn chn, jl_venc_chn_status *status)
{
    return TD_SUCCESS;
}
td_s32 jl_mpi_venc_get_stream(ot_venc_chn chn, jl_venc_stream *stream, td_s32 milli_sec)
{
    return TD_SUCCESS;
}
td_s32 jl_mpi_venc_release_stream(ot_venc_chn chn, const jl_venc_stream *stream)
{
    return TD_SUCCESS;
}
td_s32 jl_mpi_venc_send_frame(ot_venc_chn chn, const jl_venc_frame *frame, td_s32 milli_sec)
{
    return TD_SUCCESS;
}

td_s32 jl_mpi_venc_set_static_attr(ot_venc_chn chn, const jl_venc_stat_attr *attr)
{
    return TD_SUCCESS;
}
td_s32 jl_mpi_venc_get_static_attr(ot_venc_chn chn, jl_venc_stat_attr *attr)
{
    return TD_SUCCESS;
}

td_s32 jl_mpi_venc_set_dyn_attr(ot_venc_chn chn, const jl_venc_dyn_attr *attr)
{
    return TD_SUCCESS;
}
td_s32 jl_mpi_venc_get_dyn_attr(ot_venc_chn chn, jl_venc_dyn_attr *attr)
{
    return TD_SUCCESS;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */
