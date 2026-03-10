static sample_venc_rc_mad_attr g_mad_attr = {
    .i_mad_thres = {{255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255},
                    {  0,   0,   0,   0,   3,   3,   5,   5,   8,   8,   8,  15,  15,  20,  25,  25}},
    .p_mad_thres = {{255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255},
                    {  0,   0,   0,   0,   3,   3,   5,   5,   8,   8,   8,  15,  15,  20,  25,  25}},
    .mad_origin_idx = {8, 6},
};

static sample_venc_rc_attr g_rc_attr = {
    .bit_rate = {1024, 512},
    .rc_mode = {JL_VENC_RC_MODE_CBR, JL_VENC_RC_MODE_VBR},
    .stats_time = {1, 2},
    .ip_qp_delta_base = {2, 5},
    .mad_attr = &g_mad_attr,
    .win_hor_width = {32, 48},
    .win_ver_height = {32, 48},
};

static sample_venc_dyn_attr g_dyn_attr = {
    .pic_width = {1280, 720},
    .pic_hieght = {720, 480},
    .profile = {0, 1},
    .proto_type = {0, 1},
    .pack_mode = {JL_VENC_PACK_MODE_SINGLE, JL_VENC_PACK_MODE_MULTI},
    .stm_mode = {JL_VENC_STM_MODE_NALU, JL_VENC_STM_MODE_FRAME},
    .rc_attr = &g_rc_attr,
};

static sample_venc_chn_config g_chn_config_normal = {
    .chn_id = {0, 1},
    .vpss_chn_id = {0, 1},
    .chn_num = 2,
    .user_send_flag = 1,
    .dyn_chg_attr_flag = 1,
    .dyn_point_start = 20,
    .dyn_set_delta = 30,
    .dyn_set_num = 3,
    .dyn_attr = &g_dyn_attr,
};

static sample_venc_stream_thread_para g_stream_para = {
    .thread_start = HI_FALSE,
    .cnt = 0
};
static sample_venc_frame_thread_para g_frame_para = {
    .thread_start = HI_FALSE,
    .cnt = 0
};
static sample_venc_dynattr_thread_para g_dyn_attr_para = {
    .thread_start = HI_FALSE,
    .cnt = 0
};

static pthread_t g_stream_pid;
static pthread_t g_frame_pid;
static pthread_t g_dyn_attr_pid;
static hi_bool g_sample_venc_exit = HI_FALSE;

static hi_void sample_venc_usage_show_option(hi_void)
{
    printf("option:\n");
    printf("\t  %d) H264.\n", SAMPLE_VENC_OPT_H264);
    printf("\t  %d) H265.\n", SAMPLE_VENC_OPT_H265);
    printf("\t  %d) SVAC3.0.\n", SAMPLE_VENC_OPT_SVAC3);
}

static hi_void sample_venc_usage(hi_char *s_prg_nm)
{
    printf("usage : %s [index] [options]\n", s_prg_nm);
    printf("index:\n");
    printf("\t  %d) normal encode.\n", SAMPLE_VENC_INDEX_NORMAL);
    printf("\t  %d) smart encode.\n", SAMPLE_VENC_INDEX_SMART_ENC);
    printf("\t  %d) aiseg encode.\n", SAMPLE_VENC_INDEX_AISEG);
    sample_venc_usage_show_option();
}

static hi_s32 sample_venc_get_and_save_stream(td_s32 chn_id, sample_venc_chn_config* p_chn_config,
    sample_venc_stream_thread_para *stm_thread_ctx)
{
    jl_venc_stream stream;
    jl_venc_chn_status stat;
    hi_s32 ret = HI_SUCCESS;

    if (memset_s(&stream, sizeof(stream), 0, sizeof(stream)) != EOK) {
        printf("call memset_s error\n");
        return HI_FAILURE;
    }

    ret = jl_mpi_venc_query_status(chn_id, &stat);
    if (ret != HI_SUCCESS) {
        printf("query status chn[%d] failed with %#x!\n", chn_id, ret);
        return ret;
    }

    if (stat.cur_packs == 0) {
        return HI_SUCCESS;
    }

    stream.p_pack = (jl_venc_pack *)malloc(sizeof(jl_venc_pack) * stat.cur_packs);
    if (stream.p_pack == HI_NULL) {
        printf("malloc stream pack failed!\n");
        return HI_FAILURE;
    }

    stream.pack_cnt = stat.cur_packs;
    ret = jl_mpi_venc_get_stream(chn_id, &stream, 0);
    if (ret != HI_SUCCESS) {
        printf("get stream failed with %#x!\n", ret);
        goto err_out;
    }

    ret = jl_mpi_venc_release_stream(chn_id, &stream);
    if (ret != HI_SUCCESS) {
        printf("release stream failed with %#x!\n", ret);
    }

err_out:
    free(stream.p_pack);
    return ret;
}

static hi_void *sample_venc_get_stream_thread(hi_void *p)
{
    sample_venc_stream_thread_para *stm_thread_ctx = (sample_venc_stream_thread_para *)p;
    sample_venc_chn_config* p_chn_config = HI_NULL;
    fd_set stream_fds;
    hi_s32 i, j, z;
    hi_s32 ret;
    hi_s32 max_fd = 0;
    struct timeval timeout_val = {0};

    if (stm_thread_ctx->sample_mode == SAMPLE_VENC_INDEX_NORMAL) {
        p_chn_config = &g_chn_config_normal;
    }

    for (i = 0; i < p_chn_config->chn_num; i++) {
        ret = jl_mpi_venc_get_fd(i, &stm_thread_ctx->venc_fd[i]);
        if (ret != HI_SUCCESS) {
            printf("get chn fd with ret %d!\n", ret);
            goto err_out;
        }
        if (stm_thread_ctx->venc_fd[i] > max_fd) {
            max_fd = stm_thread_ctx->venc_fd[i];
        }
    }

    while (stm_thread_ctx->thread_start == HI_TRUE) {
        FD_ZERO(&stream_fds);
        for (z = 0; z < p_chn_config->chn_num; z++) {
            FD_SET(stm_thread_ctx->venc_fd[i], &stream_fds);
        }
        timeout_val.tv_sec = 2; /* 2 is a number */
        timeout_val.tv_usec = 0;
        ret = select(max_fd + 1, &stream_fds, HI_NULL, HI_NULL, &timeout_val);
        if (ret < 0) {
            printf("select failed!\n");
            break;
        } else if (ret == 0) {
            printf("get venc stream time out, try again!\n");
            continue;
        } else {
            for (z = 0; z < p_chn_config->chn_num; z++) {
                if (FD_ISSET(stm_thread_ctx->venc_fd[i], &stream_fds)) {
                    ret = sample_venc_get_and_save_stream(p_chn_config->chn_id[z], p_chn_config, stm_thread_ctx);
                    if (ret != HI_SUCCESS) {
                        printf("get stream with ret %d!\n", ret);
                        goto err_out;
                    }
                }
            }
        }
    }
err_out:
    for (j = 0; j < i; j++) {
        close(stm_thread_ctx->venc_fd[j]);
    }
    return HI_NULL;
}

hi_void *sample_venc_send_frame_thread(hi_void *p)
{
    sample_venc_frame_thread_para *stm_frame_ctx = (sample_venc_frame_thread_para *)p;
    sample_venc_chn_config* p_chn_config = HI_NULL;
    hi_s32 ret;
    hi_s32 z;
    jl_venc_frame frame;

    if (stm_frame_ctx->sample_mode == SAMPLE_VENC_INDEX_NORMAL) {
        p_chn_config = &g_chn_config_normal;
    }

    while (stm_frame_ctx->thread_start == HI_TRUE) {
        for (z = 0; z < p_chn_config->chn_num; z++) {
            ret = jl_mpi_venc_send_frame(p_chn_config->chn_id[z], &frame, 1000); /* 1000 is a number */
            if (ret != HI_SUCCESS) {
                printf("send frame failed with ret %d\n", ret);
            }
        }
    }
    return HI_NULL;
}

hi_s32 sample_venc_set_static_attr(sample_venc_chn_config* p_chn_config, hi_s32 chn_id, hi_s32 dyn_set_num)
{
    jl_venc_stat_attr attr;
    hi_s32 ret;
    ret = jl_mpi_venc_get_static_attr(chn_id, &attr);
    if (ret != HI_SUCCESS) {
        printf("get static attr fail with ret %d!\n", ret);
        return HI_FAILURE;
    }
    attr.pic_width = p_chn_config->dyn_attr->pic_width[dyn_set_num];
    attr.pic_height = p_chn_config->dyn_attr->pic_hieght[dyn_set_num];
    attr.type = p_chn_config->dyn_attr->proto_type[dyn_set_num];
    attr.profile = p_chn_config->dyn_attr->profile[dyn_set_num];
    attr.pack_mode = p_chn_config->dyn_attr->pack_mode[dyn_set_num];
    ret = jl_mpi_venc_set_static_attr(chn_id, &attr);
    if (ret != HI_SUCCESS) {
        printf("set static attr fail with ret %d!\n", ret);
        return HI_FAILURE;
    }
    return HI_SUCCESS;
}

hi_s32 sample_venc_set_dyn_attr(sample_venc_chn_config* p_chn_config, hi_s32 chn_id, hi_s32 dyn_set_num)
{
    jl_venc_dyn_attr attr;
    hi_s32 ret;
    ret = jl_mpi_venc_get_dyn_attr(chn_id, &attr);
    if (ret != HI_SUCCESS) {
        printf("get dyn attr fail with ret %d!\n", ret);
        return HI_FAILURE;
    }
    attr.stm_mode = p_chn_config->dyn_attr->stm_mode[dyn_set_num];
    attr.rc_attr.bit_rate = p_chn_config->dyn_attr->rc_attr->bit_rate[dyn_set_num];
    attr.rc_attr.rc_mode = p_chn_config->dyn_attr->rc_attr->rc_mode[dyn_set_num];
    attr.rc_attr.stats_time = p_chn_config->dyn_attr->rc_attr->stats_time[dyn_set_num];
    attr.rc_attr.ip_qp_delta_base = p_chn_config->dyn_attr->rc_attr->ip_qp_delta_base[dyn_set_num];
    attr.rc_attr.mad_attr.mad_origin_idx = p_chn_config->dyn_attr->rc_attr->mad_attr->mad_origin_idx[dyn_set_num];
    attr.rc_attr.sc_window.hor_width = p_chn_config->dyn_attr->rc_attr->win_hor_width[dyn_set_num];
    attr.rc_attr.sc_window.ver_height = p_chn_config->dyn_attr->rc_attr->win_ver_height[dyn_set_num];
    ret = jl_mpi_venc_set_dyn_attr(chn_id, &attr);
    if (ret != HI_SUCCESS) {
        printf("set dyn attr fail with ret %d!\n", ret);
        return HI_FAILURE;
    }
    return HI_SUCCESS;
}

hi_void *sample_venc_dyn_attr_thread(hi_void *p)
{
    sample_venc_dynattr_thread_para *dyn_attr_ctx = (sample_venc_dynattr_thread_para *)p;
    sample_venc_chn_config* p_chn_config = HI_NULL;
    hi_s32 dyn_set_num = 0;
    hi_s32 ret;
    hi_s32 z;
    hi_u64 dyn_set_point = 0;
    jl_venc_chn_status stat;

    if (dyn_attr_ctx->sample_mode == SAMPLE_VENC_INDEX_NORMAL) {
        p_chn_config = &g_chn_config_normal;
    }

    dyn_set_point = p_chn_config->dyn_point_start;
    while (dyn_attr_ctx->thread_start == HI_TRUE && dyn_set_num < p_chn_config->dyn_set_num) {
        for (z = 0; z < p_chn_config->chn_num; z++) {
            if (p_chn_config->dyn_in_idle_flag) {
                ret = jl_mpi_venc_query_status(p_chn_config->chn_id[z], &stat);
                if (ret != HI_SUCCESS) {
                    printf("query status chn[%d] failed with %#x!\n", p_chn_config->chn_id[z], ret);
                    break;
                }
                if (stat.enc_succ_frame_cnt >= dyn_set_point) {
                    ret = jl_mpi_venc_stop_chn(p_chn_config->chn_id[z]);
                    if (ret != HI_SUCCESS) {
                        printf("stop chn fail with ret %d!\n", ret);
                        break;
                    }
                    ret = jl_mpi_venc_reset_chn(p_chn_config->chn_id[z]);
                    if (ret != HI_SUCCESS) {
                        printf("reset chn fail with ret %d!\n", ret);
                        break;
                    }

                    sample_venc_set_static_attr(p_chn_config, p_chn_config->chn_id[z], dyn_set_num);

                    ret = jl_mpi_venc_prepare_chn(p_chn_config->chn_id[z]);
                    if (ret != HI_SUCCESS) {
                        printf("prepare chn fail with ret %d!\n", ret);
                        break;
                    }

                    ret = jl_mpi_venc_start_chn(p_chn_config->chn_id[z]);
                    if (ret != HI_SUCCESS) {
                        printf("start chn fail with ret %d!\n", ret);
                        break;
                    }
                }
            } else {
                sample_venc_set_dyn_attr(p_chn_config, p_chn_config->chn_id[z], dyn_set_num);
            }
        }
        if (z == p_chn_config->chn_num) {
            dyn_set_num++;
            dyn_set_point += p_chn_config->dyn_set_delta;
        }
    }
    return HI_NULL;
}

static hi_s32 sample_venc_init_other_module()
{
    sample_sns_type sns_type = 0;
    sample_vi_cfg vi_cfg;
    sample_vpss_cfg vpss_cfg;

    sample_comm_vi_get_default_vi_cfg(sns_type, &vi_cfg);
    sample_comm_vpss_get_default_vpss_cfg(&vpss_cfg, HI_FALSE);

    return HI_SUCCESS;
}

static hi_s32 sample_venc_start_main(hi_s32 index)
{
    sample_venc_chn_config* p_chn_config = HI_NULL;
    hi_s32 ret;
    hi_s32 i, j;

    if (index == SAMPLE_VENC_INDEX_NORMAL) {
        p_chn_config = &g_chn_config_normal;
    }

    for (i = 0; i < p_chn_config->chn_num; i++) {
        ret = jl_mpi_venc_create_chn(p_chn_config->chn_id[i]);
        if (ret != HI_SUCCESS) {
            printf("create chn fail with ret %d!\n", ret);
            goto err_out;
        }

        ret = jl_mpi_venc_prepare_chn(p_chn_config->chn_id[i]);
        if (ret != HI_SUCCESS) {
            printf("prepare chn fail with ret %d!\n", ret);
            goto err_out;
        }

        ret = jl_mpi_venc_start_chn(p_chn_config->chn_id[i]);
        if (ret != HI_SUCCESS) {
            printf("start chn fail with ret %d!\n", ret);
            goto err_out;
        }
        ret = sample_venc_init_other_module();
        if (ret != HI_SUCCESS) {
            printf("init other module fail with ret %d!\n", ret);
            goto err_out;
        }
    }
    g_stream_para.sample_mode = index;
    g_stream_para.thread_start = HI_TRUE;
    ret = pthread_create(&g_stream_pid, 0, sample_venc_get_stream_thread, (hi_void *)&g_stream_para);
    if (ret < 0) {
        printf("create stream thread failed with ret %d\n", ret);
        goto err_out;
    }

    if (p_chn_config->user_send_flag) {
        g_frame_para.sample_mode = index;
        g_frame_para.thread_start = HI_TRUE;
        ret = pthread_create(&g_frame_pid, 0, sample_venc_send_frame_thread, (hi_void *)&g_frame_para);
        if (ret < 0) {
            printf("create frame thread failed with ret %d\n", ret);
            goto err_out2;
        }
    }

    if (p_chn_config->dyn_chg_attr_flag) {
        g_dyn_attr_para.sample_mode = index;
        g_dyn_attr_para.thread_start = HI_TRUE;
        ret = pthread_create(&g_dyn_attr_pid, 0, sample_venc_dyn_attr_thread, (hi_void *)&g_dyn_attr_para);
        if (ret < 0) {
            printf("create dyn attr thread failed with ret %d\n", ret);
            goto err_out2;
        }
    }
    return HI_SUCCESS;

err_out2:
    g_stream_para.thread_start = HI_FALSE;
    pthread_join(g_stream_pid, 0);
    g_frame_para.thread_start = HI_FALSE;
    pthread_join(g_frame_pid, 0);
err_out:
    for (j = i - 1; j >= 0; j--) {
        ret = jl_mpi_venc_destroy_chn(p_chn_config->chn_id[j]);
        if (ret != HI_SUCCESS) {
            printf("destory chn %d fail!\n", p_chn_config->chn_id[j]);
        }
    }
    return HI_FAILURE;
}

static hi_s32 sample_venc_wait_exit(hi_s32 index)
{
    sample_venc_chn_config* p_chn_config = HI_NULL;
    hi_s32 ret;
    hi_s32 i;

    if (index == SAMPLE_VENC_INDEX_NORMAL) {
        p_chn_config = &g_chn_config_normal;
    }

    printf("please press twice ENTER to exit this sample\n");
    (hi_void)getchar();

    if (g_sample_venc_exit != HI_TRUE) {
        (hi_void)getchar();
    }
    g_sample_venc_exit = HI_TRUE;

    g_stream_para.thread_start = HI_FALSE;
    pthread_join(g_stream_pid, 0);
    if (p_chn_config->user_send_flag) {
        g_frame_para.thread_start = HI_FALSE;
        pthread_join(g_frame_pid, 0);
    }
    if (p_chn_config->dyn_chg_attr_flag) {
        g_dyn_attr_para.thread_start = HI_FALSE;
        pthread_join(g_dyn_attr_pid, 0);
    }
    for (i = 0; i < p_chn_config->chn_num; i++) {
        ret = jl_mpi_venc_destroy_chn(p_chn_config->chn_id[i]);
        if (ret != HI_SUCCESS) {
            printf("destory chn %d fail!\n", p_chn_config->chn_id[i]);
        }
    }
    return ret;
}

hi_s32 main(hi_s32 argc, hi_char *argv[])
{
    hi_s32 ret;
    hi_u32 index;
    hi_char *end_ptr;

    if (argc < 0x2) { /* 2:arg num */
        sample_venc_usage(argv[0]);
        return HI_FAILURE;
    }

    if (!strncmp(argv[1], "-h", 2)) { /* 2:arg num */
        sample_venc_usage(argv[0]);
        return HI_FAILURE;
    }

    index = strtoul(argv[1], &end_ptr, 10); /* 10: numberbase */
    if (end_ptr == argv[1] || *end_ptr !='\0') {
        sample_venc_usage(argv[0]);
        return HI_FAILURE;
    }

    //sample_sys_signal(sample_venc_handle_sig);

    ret = sample_venc_start_main(index);
    if (ret != HI_SUCCESS) {
        printf("program exit abnormally!\n");
        return HI_FAILURE;
    }

    sample_venc_wait_exit(index);

    printf("program exit normally!\n");

    return ret;
}
