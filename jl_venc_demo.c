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
